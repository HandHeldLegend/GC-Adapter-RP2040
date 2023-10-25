#ifndef ADAPTER_H
#define ADAPTER_H
#include "adapter_includes.h"

void adapter_set_interval(uint32_t interval);
void adapter_enable_rumble(uint8_t itf, bool enable);
void adapter_mode_task(uint32_t timestamp);
void adapter_comms_task(uint32_t timestamp);
void adapter_init();

#endif