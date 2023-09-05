/*
 * Copyright (c) [2023] [Mitch Cairns/Handheldlegend, LLC]
 * All rights reserved.
 *
 * This source code is licensed under the provisions of the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef XINPUT_DEVICE_H
#define XINPUT_DEVICE_H

#include "adapter_includes.h"
#include "device/usbd_pvt.h"

extern const tusb_desc_device_t xid_device_descriptor;
extern const uint8_t xid_configuration_descriptor[];
extern const char *xid_string_descriptor[];

void xinputd_init(void);
void xinputd_reset(uint8_t rhport);
uint16_t xinputd_open(uint8_t rhport, tusb_desc_interface_t const * desc_itf, uint16_t max_len);
bool xinputd_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
bool xinputd_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes);
bool tud_xinput_n_report(uint8_t instance, void const * report, uint16_t len);
bool tud_xinput_n_ready(uint8_t instance);
extern const usbd_class_driver_t tud_xinput_driver;
uint8_t const *xinput_descriptor_device_cb(void);
uint8_t const *xinput_descriptor_configuration_cb(uint8_t index);
void tud_xinput_getout(void);

#endif
