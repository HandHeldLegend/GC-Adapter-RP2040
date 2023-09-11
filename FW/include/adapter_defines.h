#ifndef ADAPTER_DEFINES_H
#define ADAPTER_DEFINES_H

#define TUSB_DESC_TOTAL_LEN      (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

extern const char* global_string_descriptor[];

#define RGB_PIO pio1
#define RGB_SM 0

#define JOYBUS_PIO pio0

#define UTIL_RGB_PIN   10
#define UTIL_RGB_COUNT 4
#define UTIL_RGBW_EN 0

#define JOYBUS_PORT_1 22
#define JOYBUS_PORT_2 23
#define JOYBUS_PORT_3 24
#define JOYBUS_PORT_4 25

#define ADAPTER_MANUFACTURER "HHL"
#define ADAPTER_PRODUCT "GC Pocket+"

#define ADAPTER_FIRMWARE_VERSION 0x0000
#define ADAPTER_SETTINGS_VERSION 0x0002
#define ADAPTER_WEBUSB_URL "handheldlegend.com"


#endif
