#include "switch_analog.h"

uint8_t switch_analog_calibration_data[18];

// Initializes the analog calibration data
// for pro controller mode
void switch_analog_calibration_init()
{
  uint16_t min = 128<<4;
  uint16_t center = 128<<4;
  uint16_t max = 128<<4;

  // Left stick X max/Y max
  switch_analog_encode(max, max, &switch_analog_calibration_data[0]);
  // Left stick X/Y center
  switch_analog_encode(center, center, &switch_analog_calibration_data[3]);
  // Left stick X/Y min
  switch_analog_encode(min, min, &switch_analog_calibration_data[6]);

  // Right stick X/Y center
  switch_analog_encode(center, center, &switch_analog_calibration_data[9]);
  // Right stick X/Y min
  switch_analog_encode(min, min, &switch_analog_calibration_data[12]);
  // Right stick X max/Y max
  switch_analog_encode(max, max, &switch_analog_calibration_data[15]);
}

void switch_analog_decode(uint8_t s0, uint8_t s1, uint8_t s2, uint16_t *out_lower, uint16_t *out_upper)
{
  out_lower[0] = (s1 << 8) & 0xF00 | s0;
  out_upper[0] = ( s2 << 4) | (s1 >> 4);
}

void switch_analog_encode(uint16_t in_lower, uint16_t in_upper, uint8_t *out)
{
  out[0] = in_lower & 0xFF;
  out[1] = ((in_lower & 0xF00) >> 8);
  out[1] |= (in_upper & 0xF) << 4;
  out[2] = (in_upper & 0xFF0) >> 4;
}
