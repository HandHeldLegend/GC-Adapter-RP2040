#include "gamecube.h"

#define CLAMP_0_255(value) ((value) < 0 ? 0 : ((value) > 255 ? 255 : (value)))

void gamecube_init()
{

  // joybus_set_in(false, GAMEPAD_PIO, GAMEPAD_SM, _gamecube_offset, &_gamecube_c);
}