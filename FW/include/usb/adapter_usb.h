/*
 * Copyright (c) [2023] [Mitch Cairns/Handheldlegend, LLC]
 * All rights reserved.
 *
 * This source code is licensed under the provisions of the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef ADAPTER_USB_H
#define ADAPTER_USB_H

#include "adapter_includes.h"
#include "interval.h"

bool adapter_usb_ready(uint8_t itf);
void adapter_usb_report(uint8_t itf, joybus_input_s *input);

bool adapter_usb_start(input_mode_t mode);
uint8_t dir_to_hat(uint8_t leftRight, uint8_t upDown);
void adapter_usb_task(uint32_t timestamp);

#endif
