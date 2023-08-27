#ifndef ADAPTER_SETTINGS_H
#define ADAPTER_SETTINGS_H

#include "adapter_includes.h"

typedef struct
{
    // We use a settings version to
    // keep settings between updates
    uint16_t    settings_version;
    input_mode_t input_mode;
    uint8_t     switch_mac_address_p1[6];
    uint8_t     switch_mac_address_p2[6];
    uint8_t     switch_mac_address_p3[6];
    uint8_t     switch_mac_address_p4[6];

    gc_sp_mode_t gc_sp_mode;

} adapter_settings_s;

extern adapter_settings_s global_loaded_settings;

bool settings_load();
void settings_core1_save_check();
void settings_save_webindicate();
void settings_save();
void settings_reset_to_default();

#endif
