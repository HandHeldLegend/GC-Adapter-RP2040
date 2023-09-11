#ifndef ADAPTER_SETTINGS_H
#define ADAPTER_SETTINGS_H

#include "adapter_includes.h"

typedef struct
{
    // We use a settings version to
    // keep settings between updates
    uint16_t    settings_version;
    input_mode_t input_mode;
    uint8_t     switch_mac_address[4][6];

} adapter_settings_s;

extern adapter_settings_s global_loaded_settings;

bool settings_load();
void settings_core0_save_check();
void settings_core1_save_check();
void settings_save_webindicate();
void settings_save();
void settings_reset_to_default();

#endif
