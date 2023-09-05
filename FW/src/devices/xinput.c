#include "xinput.h"

short sign_axis(int input_axis)
{

    int start = (int) (input_axis>>4) - 127;
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
    return (short) start;
}

void xinput_hid_report(uint8_t port, button_data_s *button_data, a_data_s *analog_data)
{
  static xid_input_s data[4] = {0};
  data[port].stick_left_x     = sign_axis((int) (analog_data->lx));
  data[port].stick_left_y     = sign_axis((int) (analog_data->ly));
  data[port].stick_right_x    = sign_axis((int) (analog_data->rx));
  data[port].stick_right_y    = sign_axis((int) (analog_data->ry));

  data[port].dpad_up = button_data->dpad_up;
  data[port].dpad_down = button_data->dpad_down;
  data[port].dpad_left = button_data->dpad_left;
  data[port].dpad_right = button_data->dpad_right;

  //data.button_guide = button_data->button_minus;
  data[port].button_back = button_data->button_minus;
  data[port].button_menu = button_data->button_plus;
  data[port].bumper_r = button_data->trigger_r;
  data[port].bumper_l = button_data->trigger_l;

  //data[port].button_a = button_data->button_a;
  data[port].button_a = !gpio_get(12);
  data[port].button_b = button_data->button_b;
  data[port].button_x = button_data->button_x;
  data[port].button_y = button_data->button_y;

  data[port].button_stick_l = button_data->button_stick_left;
  data[port].button_stick_r = button_data->button_stick_right;

  data[port].analog_trigger_l = button_data->trigger_zl ? 255 : 0;
  data[port].analog_trigger_r = button_data->trigger_zr ? 255 : 0;

  tud_xinput_n_report(port, &(data[port]), XID_REPORT_LEN);
}
