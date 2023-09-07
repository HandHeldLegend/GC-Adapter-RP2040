#include "xinput.h"

short sign_axis(uint8_t input_axis)
{
    
    int start = (int) (input_axis) - 127;
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

void xinput_hid_report(uint8_t port, joybus_input_s *joybus_data)
{
  static xid_input_s data[4] = {0};
  data[port].stick_left_x     = sign_axis((joybus_data->stick_left_x));
  data[port].stick_left_y     = sign_axis((joybus_data->stick_left_y));
  data[port].stick_right_x    = sign_axis((joybus_data->stick_right_x));
  data[port].stick_right_y    = sign_axis((joybus_data->stick_right_y));

  data[port].dpad_up = joybus_data->dpad_up;
  data[port].dpad_down = joybus_data->dpad_down;
  data[port].dpad_left = joybus_data->dpad_left;
  data[port].dpad_right = joybus_data->dpad_right;

  //data.button_guide = joybus_data->button_minus;
  //data[port].button_back = joybus_data->button_minus;
  data[port].button_menu = joybus_data->button_start;
  data[port].bumper_r = joybus_data->button_z;
  //data[port].bumper_l = joybus_data->button_l;

  //data[port].button_a = joybus_data->button_a;
  data[port].button_a = joybus_data->button_a;
  data[port].button_b = joybus_data->button_b;
  data[port].button_x = joybus_data->button_x;
  data[port].button_y = joybus_data->button_y;

  //data[port].button_stick_l = joybus_data->button_stick_left;
  //data[port].button_stick_r = joybus_data->button_stick_right;

  data[port].analog_trigger_l = joybus_data->analog_trigger_l>45 ? 255 : 0;
  data[port].analog_trigger_r = joybus_data->analog_trigger_r>45 ? 255 : 0;

  tud_xinput_n_report(port, &(data[port]), XID_REPORT_LEN);
}
