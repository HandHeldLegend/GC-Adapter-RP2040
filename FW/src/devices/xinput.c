#include "xinput.h"

short sign_axis(uint8_t input_axis)
{

    float start = (float)(input_axis)-127;
    start *= 1.28;
    if ((start * 256) > 32765)
    {
        start = 32765;
    }
    else if ((start * 256) < -32765)
    {
        start = -32765;
    }
    else
    {
        start *= 256;
    }
    return (short)start;
}

void xinput_hid_idle(joybus_input_s *joybus_data)
{
    if (!joybus_data[0].port_ready && (joybus_data[0].port_itf > -1))
        joybus_data[0].port_ready = tud_xinput_n_ready(joybus_data[0].port_itf);

    if (!joybus_data[1].port_ready && (joybus_data[1].port_itf > -1))
        joybus_data[1].port_ready = tud_xinput_n_ready(joybus_data[1].port_itf);

    if (!joybus_data[2].port_ready && (joybus_data[2].port_itf > -1))
        joybus_data[2].port_ready = tud_xinput_n_ready(joybus_data[2].port_itf);

    if (!joybus_data[3].port_ready && (joybus_data[3].port_itf > -1))
        joybus_data[3].port_ready = tud_xinput_n_ready(joybus_data[3].port_itf);
}

void xinput_hid_report(joybus_input_s *joybus_data)
{
    static xid_input_s data[4] = {0};

    for (uint i = 0; i < 4; i++)
    {
        if(joybus_data[i].port_itf<0) continue;

        uint8_t itf = (uint8_t) joybus_data[i].port_itf;

        data[itf].stick_left_x  = sign_axis((joybus_data[i].stick_left_x));
        data[itf].stick_left_y  = sign_axis((joybus_data[i].stick_left_y));
        data[itf].stick_right_x = sign_axis((joybus_data[i].stick_right_x));
        data[itf].stick_right_y = sign_axis((joybus_data[i].stick_right_y));

        data[itf].dpad_up       = joybus_data[i].dpad_up;
        data[itf].dpad_down     = joybus_data[i].dpad_down;
        data[itf].dpad_left     = joybus_data[i].dpad_left;
        data[itf].dpad_right    = joybus_data[i].dpad_right;

        // data.button_guide = joybus_data[i].button_minus;
        // data[port].button_back = joybus_data[i].button_minus;
        data[itf].button_menu   = joybus_data[i].button_start;
        data[itf].bumper_r      = joybus_data[i].button_z;
        // data[port].bumper_l = joybus_data[i].button_l;

        // data[port].button_a = joybus_data[i].button_a;
        data[itf].button_a = joybus_data[i].button_a;
        data[itf].button_b = joybus_data[i].button_b;
        data[itf].button_x = joybus_data[i].button_x;
        data[itf].button_y = joybus_data[i].button_y;

        // data[port].button_stick_l = joybus_data[i].button_stick_left;
        // data[port].button_stick_r = joybus_data[i].button_stick_right;

        data[itf].analog_trigger_l = (joybus_data[i].button_l) ? 255 : joybus_data[i].analog_trigger_l;
        data[itf].analog_trigger_r = (joybus_data[i].button_r) ? 255 : joybus_data[i].analog_trigger_r;

        if(joybus_data[i].port_ready)
        {
            tud_xinput_n_report(itf, &data[itf], XID_REPORT_LEN);
            joybus_data[i].port_ready = false;
        }
    }
}
