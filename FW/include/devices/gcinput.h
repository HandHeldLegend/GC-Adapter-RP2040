#ifndef GCINPUT_H
#define GCINPUT_H

#include "adapter_includes.h"

extern const tusb_desc_device_t gc_device_descriptor;
extern const uint8_t gc_hid_report_descriptor[];
extern const uint8_t gc_configuration_descriptor[];
extern const uint8_t gc_configuration_descriptor_performance[];

void gcinput_hid_report(button_data_s *button_data, a_data_s *analog_data);

#endif
