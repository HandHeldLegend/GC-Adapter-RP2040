#include "gcinput.h"

#define CLAMP_0_255(value) ((value) < 0 ? 0 : ((value) > 255 ? 255 : (value)))

bool _gc_first = false;
bool _gc_enable = false;

void gcinput_enable(bool enable)
{
    _gc_enable = enable;
}

void gcinput_hid_report(joybus_input_s *joybus_data)
{

    static gc_input_s data[4] = {0};
    static uint8_t buffer[37] = {0};

    buffer[0] = 0x21;

    for(uint i = 0; i < 4; i++)
    {

        if(joybus_data[i].port_itf<0) break;
    
        uint idx = (joybus_data[i].port_itf*9)+1;
        uint itf = (uint) joybus_data[i].port_itf;

        buffer[idx] |= 0x14;

        data[itf].button_a = joybus_data[i].button_a;
        data[itf].button_b = joybus_data[i].button_b;
        data[itf].button_x = joybus_data[i].button_x;
        data[itf].button_y = joybus_data[i].button_y;

        data[itf].button_start    = joybus_data[i].button_start;
        data[itf].button_l        = joybus_data[i].button_l;
        data[itf].button_r        = joybus_data[i].button_r;
        data[itf].button_z        = joybus_data[i].button_z;

        data[itf].dpad_down   = joybus_data[i].dpad_down;
        data[itf].dpad_left   = joybus_data[i].dpad_left;
        data[itf].dpad_right  = joybus_data[i].dpad_right;
        data[itf].dpad_up     = joybus_data[i].dpad_up;
    }

    if(!_gc_first)
    {
        buffer[1]   |= 0x04;
        buffer[10]  |= 0x04;
        buffer[19]  |= 0x04;
        buffer[28]  |= 0x04;
        _gc_first = true;
    }
    else
    {
        memcpy(&buffer[2], &data[0], 8);
        memcpy(&buffer[11], &data[1], 8);
        memcpy(&buffer[20], &data[2], 8);
        memcpy(&buffer[28], &data[3], 8);
    }

    tud_ginput_report(0, buffer, 37);
}
