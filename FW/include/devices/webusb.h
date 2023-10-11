#ifndef WEBUSB_H
#define WEBUSB_H

#include "adapter_includes.h"

typedef enum
{
  // Set FW update mode
  WEBUSB_CMD_FW_SET = 0x0F,
  // Get firmware version
  WEBUSB_CMD_FW_GET = 0xAF,

  WEBUSB_CMD_SAVEALL = 0xF1,
} webusb_cmd_t;

void webusb_save_confirm();
void webusb_command_processor(uint8_t *data);

#endif