#include "adapter.h"
#include "interval.h"

#define CLAMP_0_255(value) ((value) < 0 ? 0 : ((value) > 255 ? 255 : (value)))

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
joybus_input_s _port_joybus[4] = {0, 0, 0, 0};

bool _itf_ready[4] = {false};
bool _port_rumble[4] = {false, false, false, false};
int _port_interface[4] = {-1,-1,-1,-1};

typedef struct
{
    int lx_offset;
    int ly_offset;
    int rx_offset;
    int ry_offset;
    int lt_offset;
    int rt_offset;
} analog_offset_s;

analog_offset_s _port_offsets[4] = {0};

uint read_count = 0;

void _gc_port_data(uint port)
{
    if(!_port_phases[port])
    {
        // For this specific circumstance, we must push
        // manually since our data is set to push auto 32 bits
        pio_sm_exec(JOYBUS_PIO, port, pio_encode_push(false, false));
        _port_probes[port] = pio_sm_get(JOYBUS_PIO, port)>>17;

        if (_port_probes[port] == 0x09)
        {
            
            _port_phases[port] = 1;
        }

        _port_probes[port] = 0;
    }
    else if (_port_phases[port]==1)
    {
        // Collect data for analog offset creation
        for(uint i = 0; i < 2; i++)
        {
            if(!pio_sm_is_rx_fifo_empty(JOYBUS_PIO, port))
            {
                _port_inputs[port][i] = pio_sm_get(JOYBUS_PIO, port);
            }
            else 
            {
                _port_interface[port] = -1;
                _port_phases[port] = 0;
                return;
            }
        }
        _port_joybus[port].byte_1 = _port_inputs[port][0];
        _port_joybus[port].byte_2 = _port_inputs[port][1];

        _port_offsets[port].lx_offset = 128 - (int) _port_joybus[port].stick_left_x;
        _port_offsets[port].rx_offset = 128 - (int) _port_joybus[port].stick_right_x;
        _port_offsets[port].ly_offset = 128 - (int) _port_joybus[port].stick_left_y;
        _port_offsets[port].ry_offset = 128 - (int) _port_joybus[port].stick_right_y;

        _port_offsets[port].lt_offset = -(int) _port_joybus[port].analog_trigger_l;
        _port_offsets[port].rt_offset = -(int) _port_joybus[port].analog_trigger_r;

        // Set the port phase
        _port_phases[port] = 2;
        
        // Set the port USB Interface
        uint8_t tmp_itf = 0;
        for(uint8_t i = 0; i < 4; i++)
        {
            if (_port_interface[i] == tmp_itf)
            {
                tmp_itf += 1;
            }
        }
        _port_interface[port] = tmp_itf;
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
                _port_interface[port] = -1;
                _port_phases[port] = 0;
                return;
            }
        }
        _port_joybus[port].byte_1 = _port_inputs[port][0];
        _port_joybus[port].byte_2 = _port_inputs[port][1];

        int lx = CLAMP_0_255(_port_joybus[port].stick_left_x + _port_offsets[port].lx_offset);
        int ly = CLAMP_0_255(_port_joybus[port].stick_left_y + _port_offsets[port].ly_offset);
        int rx = CLAMP_0_255(_port_joybus[port].stick_right_x + _port_offsets[port].rx_offset);
        int ry = CLAMP_0_255(_port_joybus[port].stick_right_y + _port_offsets[port].ry_offset);

        int lt = CLAMP_0_255(_port_joybus[port].analog_trigger_l + _port_offsets[port].lt_offset);
        int rt = CLAMP_0_255(_port_joybus[port].analog_trigger_r + _port_offsets[port].rt_offset);

        // Apply offsets
        _port_joybus[port].stick_left_x =   (uint8_t) lx;
        _port_joybus[port].stick_left_y =   (uint8_t) ly;
        _port_joybus[port].stick_right_x =  (uint8_t) rx;
        _port_joybus[port].stick_right_y =  (uint8_t) ry;

        _port_joybus[port].analog_trigger_l = (uint8_t) lt;
        _port_joybus[port].analog_trigger_r = (uint8_t) rt;
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

void _adapter_report(uint8_t itf)
{
    joybus_input_s zero_report = {
        .stick_left_x = 128,
        .stick_right_x = 128,
        .stick_left_y = 128,
        .stick_right_y = 128,
    };

    bool reported = false;

    for(uint i = 0; i < 4; i++)
    {
        if(_port_interface[i] == itf)
        {
            if(_itf_ready[itf])
            {
                adapter_usb_report(itf, &(_port_joybus[i]));
                _itf_ready[itf] = false;
                reported = true;
                break;
            }
        }
    }
}

void adapter_enable_rumble(uint8_t itf, bool enable)
{
    for(uint i = 0; i < 4; i++)
    {
        if(_port_interface[i] == itf)
        {
            _port_rumble[i] = enable;
            break;
        }
    }
}

void adapter_comms_task(uint32_t timestamp)
{
    if (interval_run(timestamp, 7000))
    {
        
        _gamecube_send_probe();
        sleep_us(500);
        _gamecube_get_data();

        _adapter_report(0);
        _adapter_report(1);
        _adapter_report(2);
        _adapter_report(3);
    }
    else
    {
        _itf_ready[0] = adapter_usb_ready(0);
        _itf_ready[1] = adapter_usb_ready(1);
        _itf_ready[2] = adapter_usb_ready(2);
        _itf_ready[3] = adapter_usb_ready(3);
    }
}

void adapter_init()
{
    _gamecube_offset = pio_add_program(JOYBUS_PIO, &joybus_program);
    for(uint i = 0; i < 4; i++)
    {
        memset(&_port_joybus[i], 0, sizeof(joybus_input_s));
        _port_joybus[i].stick_left_x = 128;
        _port_joybus[i].stick_left_y = 128;
        _port_joybus[i].stick_right_x = 128;
        _port_joybus[i].stick_right_y = 128;
    }

    joybus_program_init(JOYBUS_PIO, _gamecube_offset + joybus_offset_joybusout, JOYBUS_PORT_1, _gamecube_c);
    sleep_ms(100);
}