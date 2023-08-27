#ifndef SWITCH_ANALOG_H
#define SWITCH_ANALOG_H

#include "adapter_includes.h"

extern uint8_t switch_analog_calibration_data[18];

void switch_analog_calibration_init();

void switch_analog_decode(uint8_t s0, uint8_t s1, uint8_t s2, uint16_t *out_lower, uint16_t *out_upper);

void switch_analog_encode(uint16_t in_lower, uint16_t in_upper, uint8_t *out);

#endif
