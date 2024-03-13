#include "main.h"

#define CLAMP_0_255(value) ((value) < 0 ? 0 : ((value) > 255 ? 255 : (value)))

uint32_t _usb_interval = 7000;

uint _adapter_output_irq;
uint _adapter_input_irq;
uint _gamecube_offset;
pio_sm_config _gamecube_c[4];

const uint8_t _led_defs[4] = {0, 1, 2, 3};

volatile bool _gc_tx_done = false;
bool _gc_running = false;

#define ALIGNED_JOYBUS_8(val) ((val) << 24)

uint8_t _port_phases[4] = {0};
uint32_t _port_probes[4] = {0};
uint32_t _port_inputs[4][4] = {{0}};
joybus_input_s _port_joybus[4] = {0, 0, 0, 0};

bool _port_rumble[4] = {false, false, false, false};

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

void _gc_port_reset(uint port)
{
    _port_joybus[port].port_itf = -1;
    _port_phases[port] = 0;
    _port_joybus[port].port_ready = false;
}

void _gc_port_data(uint port)
{
    if (!_port_phases[port])
    {
        // For this specific circumstance, we must push
        // manually since our data is set to push auto 32 bits
        pio_sm_exec(JOYBUS_PIO, port, pio_encode_push(false, false));
        _port_probes[port] = pio_sm_get(JOYBUS_PIO, port) >> 17;

        if (_port_probes[port] == 0x09)
        {
            _port_phases[port] = 1;
            
            adapter_timer_reset();
        }

        _port_probes[port] = 0;
    }
    else if (_port_phases[port] == 1)
    {

        // Collect data for analog offset creation
        for (uint i = 0; i < 2; i++)
        {
            if (!pio_sm_is_rx_fifo_empty(JOYBUS_PIO, port))
            {
                _port_inputs[port][i] = pio_sm_get(JOYBUS_PIO, port);
            }
            else
            {
                _gc_port_reset(port);
                return;
            }
        }

        _port_joybus[port].byte_1 = _port_inputs[port][0];
        _port_joybus[port].byte_2 = _port_inputs[port][1];

        _port_offsets[port].lx_offset = 128 - (int)_port_joybus[port].stick_left_x;
        _port_offsets[port].rx_offset = 128 - (int)_port_joybus[port].stick_right_x;
        _port_offsets[port].ly_offset = 128 - (int)_port_joybus[port].stick_left_y;
        _port_offsets[port].ry_offset = 128 - (int)_port_joybus[port].stick_right_y;

        _port_offsets[port].lt_offset = -(int)_port_joybus[port].analog_trigger_l;
        _port_offsets[port].rt_offset = -(int)_port_joybus[port].analog_trigger_r;

        // Set the port phase
        _port_phases[port] = 2;
        adapter_timer_reset();
        sleep_ms(10); // Sleep on probe collect to allow voltage stabilization

        // Set the port USB Interface
        int tmp_itf = 0;

        for (uint8_t i = 0; i < 4; i++)
        {
            bool itfInUse = false;

            for (uint8_t j = 0; j < 4; j++)
            {
                if (_port_joybus[j].port_itf == i)
                {
                    itfInUse = true;
                    break;
                }
            }

            if (!itfInUse)
            {
                tmp_itf = i;
                break; // Exit the loop once an available port number is found
            }
        }

        _port_joybus[port].port_itf = tmp_itf;
    }
    else if (_port_phases[port] == 2)
    {
        static uint8_t port_reset_timer[4] = {0};

        for (uint i = 0; i < 2; i++)
        {
            if (!pio_sm_is_rx_fifo_empty(JOYBUS_PIO, port))
            {
                _port_inputs[port][i] = pio_sm_get(JOYBUS_PIO, port);
            }
            else
            {   
                port_reset_timer[port] += 1;
                if(port_reset_timer[port]>=10)
                {
                    _gc_port_reset(port);
                    port_reset_timer[port] = 0;
                }
                
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
        _port_joybus[port].stick_left_x = (uint8_t)lx;
        _port_joybus[port].stick_left_y = (uint8_t)ly;
        _port_joybus[port].stick_right_x = (uint8_t)rx;
        _port_joybus[port].stick_right_y = (uint8_t)ry;

        _port_joybus[port].analog_trigger_l = (uint8_t)lt;
        _port_joybus[port].analog_trigger_r = (uint8_t)rt;
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
            sleep_ms(50);
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

void joybus_itf_enable_rumble(uint8_t interface, bool enable)
{
    for (uint i = 0; i < 4; i++)
    {
        if (_port_joybus[i].port_itf == interface)
        {
            _port_rumble[i] = enable;
            break;
        }
    }
}

void joybus_itf_poll(joybus_input_s **out)
{

    *out = _port_joybus;

    _gamecube_send_probe();
    sleep_us(500);
    _gamecube_get_data();
}

void joybus_itf_init()
{
    _gamecube_offset = pio_add_program(JOYBUS_PIO, &joybus_program);
    for (uint i = 0; i < 4; i++)
    {
        memset(&_port_joybus[i], 0, sizeof(joybus_input_s));
        _port_joybus[i].port_itf = -1;
    }

    joybus_program_init(JOYBUS_PIO, _gamecube_offset + joybus_offset_joybusout, JOYBUS_PORT_1, _gamecube_c);
    sleep_ms(100);
}