#include "adapter.h"
#include "interval.h"

uint _adapter_output_irq;
uint _adapter_input_irq;
uint _gamecube_offset;
pio_sm_config _gamecube_c[4];

volatile bool _gc_tx_done = false;
bool _gc_running = false;

#define ALIGNED_JOYBUS_8(val) ((val) << 24)

uint8_t _port_phases[4] = {0};
uint32_t _port_probes[4] = {0};
uint32_t _port_inputs[4][4] = {{0}};
joybus_input_s _port_joybus[4] = {0};
bool _port_ready[4] = {false};

bool _port_rumble[4] = {false, false, false, false};

uint read_count = 0;

void _gc_port_data(uint port)
{
    if(!_port_phases[port])
    {
        // For this specific circumstance, we must push
        // manually since our data is set to push auto 32 bits
        pio_sm_exec_wait_blocking(JOYBUS_PIO, port, pio_encode_push(false, false));
        _port_probes[port] = pio_sm_get(JOYBUS_PIO, port)>>17;

        if (_port_probes[port] == 0x09)
        {
            _port_phases[port] = 1;
        }
    }
    else if (_port_phases[port]==1)
    {
        pio_sm_clear_fifos(JOYBUS_PIO, port);
        _port_phases[port] = 2;
    }
    else if (_port_phases[port]==2)
    {
        for(uint i = 0; i < 2; i++)
        {
            if(!pio_sm_is_rx_fifo_empty(JOYBUS_PIO, port))
            {
                _port_inputs[port][i] = pio_sm_get(JOYBUS_PIO, port);
            }
            else 
            {
                _port_phases[port] = 0;
                return;
            }
        }
        _port_joybus[port].byte_1 = _port_inputs[port][0];
        _port_joybus[port].byte_2 = _port_inputs[port][1];
    }
}

void _gamecube_get_data()
{
    _gc_port_data(0);
    _gc_port_data(1);
    _gc_port_data(2);
    _gc_port_data(3);
}

void _gamecube_send_probe()
{
    pio_sm_clear_fifos(JOYBUS_PIO, 0);
    pio_set_sm_mask_enabled(JOYBUS_PIO, 0b1111, false);
    for (uint i = 0; i < 4; i++)
    {
        switch (_port_phases[i])
        {
            default:
            case 0:
            {
                pio_sm_exec_wait_blocking(JOYBUS_PIO, i, pio_encode_set(pio_y, 0));
                pio_sm_exec_wait_blocking(JOYBUS_PIO, i, pio_encode_jmp(_gamecube_offset));
                pio_sm_put_blocking(JOYBUS_PIO, i, ALIGNED_JOYBUS_8(0x00));
                pio_sm_exec_wait_blocking(JOYBUS_PIO, i, pio_encode_set(pio_y, 7));
            }
            break;

            case 1:
                pio_sm_exec_wait_blocking(JOYBUS_PIO, i, pio_encode_set(pio_y, 0));
                pio_sm_exec_wait_blocking(JOYBUS_PIO, i, pio_encode_jmp(_gamecube_offset));
                pio_sm_put_blocking(JOYBUS_PIO, i, ALIGNED_JOYBUS_8(0x41));
                pio_sm_exec_wait_blocking(JOYBUS_PIO, i, pio_encode_set(pio_y, 7));
                break;

            case 2:
            {
                pio_sm_exec_wait_blocking(JOYBUS_PIO, i, pio_encode_jmp(_gamecube_offset + joybus_offset_joybusout));
                pio_sm_put_blocking(JOYBUS_PIO, i, ALIGNED_JOYBUS_8(0x40));
                pio_sm_put_blocking(JOYBUS_PIO, i, ALIGNED_JOYBUS_8(0x03));
                pio_sm_put_blocking(JOYBUS_PIO, i, ALIGNED_JOYBUS_8(_port_rumble[i]));
            }
            break;
        }
    }
    pio_set_sm_mask_enabled(JOYBUS_PIO, 0b1111, true);
}

void adapter_comms_task(uint32_t timestamp)
{
    if (interval_run(timestamp, 7000))
    {
        _gamecube_send_probe();
        sleep_us(500);
        _gamecube_get_data();

        if(_port_joybus[0].button_z)
        {
            _port_rumble[0] = true;
            _port_rumble[1] = true;
        }
        else
        {
            _port_rumble[0] = false;
            _port_rumble[1] = false;
            _port_rumble[2] = false;
            _port_rumble[3] = false;
        }

        if(_port_ready[0])  xinput_hid_report(0, &_port_joybus[0]);
        if(_port_ready[1])  xinput_hid_report(1, &_port_joybus[1]);
        if(_port_ready[2])  xinput_hid_report(2, &_port_joybus[2]);
        if(_port_ready[3])  xinput_hid_report(3, &_port_joybus[3]);

        _port_ready[0] = 0;
        _port_ready[1] = 0;
        _port_ready[2] = 0;
        _port_ready[3] = 0;
    }
    else
    {
        _port_ready[0] = tud_xinput_n_ready(0);
        _port_ready[1] = tud_xinput_n_ready(1);
        _port_ready[2] = tud_xinput_n_ready(2);
        _port_ready[3] = tud_xinput_n_ready(3);
    }
}

void adapter_init_test()
{
    _gamecube_offset = pio_add_program(JOYBUS_PIO, &joybus_program);

    joybus_program_init(JOYBUS_PIO, _gamecube_offset + joybus_offset_joybusout, JOYBUS_PORT_1, _gamecube_c);
    sleep_ms(100);
}