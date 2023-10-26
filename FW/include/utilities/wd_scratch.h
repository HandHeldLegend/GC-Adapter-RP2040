#ifndef WD_SCRATCH_H
#define WD_SCRATCH_H

#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"
#include "adapter_includes.h"

#define WD_READOUT_IDX 5

typedef enum
{
    WD_REBOOT_REASON_NULL = 0,
    WD_REBOOT_REASON_MODECHANGE = 1,
} wd_scratch_reboot_reason_t;

typedef union
{
    struct
    {
        uint8_t reboot_reason : 8;
        uint8_t adapter_mode : 8;
        uint8_t padding_1 : 8;
        uint8_t padding_2 : 8;
    };
    uint32_t value;
} wd_scratch_readout_u;

uint32_t scratch_get(uint8_t index);
void scratch_set(uint8_t index, uint32_t value);

void wd_mode_task(uint32_t timestamp);

#endif