#ifndef SWPRO_H
#define SWPRO_H

#include "adapter_includes.h"

extern const tusb_desc_device_t swpro_device_descriptor;
extern const uint8_t swpro_hid_report_descriptor[];
extern const uint8_t swpro_configuration_descriptor[];

void swpro_hid_idle(joybus_input_s *joybus_data);
void swpro_hid_report(joybus_input_s *joybus_data);

#endif
