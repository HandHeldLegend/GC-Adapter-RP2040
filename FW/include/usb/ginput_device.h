#ifndef GINPUT_DEVICE_H
#define GINPUT_DEVICE_H

#include "adapter_includes.h"
#include "device/usbd_pvt.h"

extern bool gc_connected;

extern const tusb_desc_device_t ginput_device_descriptor;
extern const uint8_t ginputslippi_configuration_descriptor[];
extern const uint8_t ginput_configuration_descriptor[];
extern const char *ginput_string_descriptor[];
extern const uint8_t gc_hid_report_descriptor[];

extern const usbd_class_driver_t tud_ginput_driver;

//--------------------------------------------------------------------+
// Application API (Multiple Instances)
// CFG_TUD_GC > 1
//--------------------------------------------------------------------+

// Check if the interface is ready to use
bool tud_ginput_n_ready(uint8_t instance);

// Get interface supported protocol (bInterfaceProtocol) check out ginput_interface_protocol_enum_t for possible values
uint8_t tud_ginput_n_interface_protocol(uint8_t instance);

// Get current active protocol: ginput_PROTOCOL_BOOT (0) or ginput_PROTOCOL_REPORT (1)
uint8_t tud_ginput_n_get_protocol(uint8_t instance);

// Send report to host
bool tud_ginput_n_report(uint8_t instance, uint8_t report_id, void const* report, uint16_t len);

// KEYBOARD: convenient helper to send keyboard report if application
// use template layout report as defined by ginput_keyboard_report_t
bool tud_ginput_n_keyboard_report(uint8_t instance, uint8_t report_id, uint8_t modifier, uint8_t keycode[6]);

// MOUSE: convenient helper to send mouse report if application
// use template layout report as defined by ginput_mouse_report_t
bool tud_ginput_n_mouse_report(uint8_t instance, uint8_t report_id, uint8_t buttons, int8_t x, int8_t y, int8_t vertical, int8_t horizontal);

// Gamepad: convenient helper to send gamepad report if application
// use template layout report TUD_ginput_REPORT_DESC_GAMEPAD
bool tud_ginput_n_gamepad_report(uint8_t instance, uint8_t report_id, int8_t x, int8_t y, int8_t z, int8_t rz, int8_t rx, int8_t ry, uint8_t hat, uint32_t buttons);

//--------------------------------------------------------------------+
// Application API (Single Port)
//--------------------------------------------------------------------+
bool    tud_ginput_ready(void);
static inline uint8_t tud_ginput_interface_protocol(void);
static inline uint8_t tud_ginput_get_protocol(void);
bool    tud_ginput_report(uint8_t report_id, void const* report, uint16_t len);

//--------------------------------------------------------------------+
// Internal Class Driver API
//--------------------------------------------------------------------+
void     ginputd_init            (void);
void     ginputd_reset           (uint8_t rhport);
uint16_t ginputd_open            (uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len);
bool     ginputd_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
bool     ginputd_xfer_cb         (uint8_t rhport, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes);


#endif