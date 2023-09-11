/*
 * Copyright (c) [2023] [Mitch Cairns/Handheldlegend, LLC]
 * All rights reserved.
 *
 * This source code is licensed under the provisions of the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "adapter_usb.h"
#include "interval.h"

input_mode_t _usb_mode = INPUT_MODE_XINPUT;
bool _usb_clear = false;

// Default 8ms (8000us)
uint32_t _usb_rate = 0;

typedef void (*usb_cb_t)(uint8_t, joybus_input_s *);
typedef bool (*usb_ready_cb_t)(uint8_t);

usb_cb_t _usb_hid_cb = NULL;
usb_ready_cb_t _usb_ready_cb = NULL;

void _adapter_usb_set_interval(usb_rate_t rate)
{
  _usb_rate = rate;
}

bool adapter_usb_start(input_mode_t mode)
{
  switch (mode)
  {
  default:
    _usb_hid_cb = NULL;
    _usb_ready_cb = NULL;
    break;
  case INPUT_MODE_SWPRO:
    //_adapter_usb_set_interval(USBRATE_8);
    _usb_hid_cb = swpro_hid_report;
    _usb_ready_cb = tud_hid_n_ready;
    break;

  case INPUT_MODE_XINPUT:
    //_adapter_usb_set_interval(USBRATE_8);
    _usb_hid_cb = xinput_hid_report;
    _usb_ready_cb = tud_xinput_n_ready;
    break;
  }

  _usb_mode = mode;
  
  return tusb_init();
}

uint8_t buf = 0;

bool adapter_usb_ready(uint8_t port)
{
  if(_usb_ready_cb == NULL) return false;
  return _usb_ready_cb(port);
}

void adapter_usb_report(uint8_t port, joybus_input_s *input)
{
  _usb_hid_cb(port, input);
}

void adapter_usb_task(uint32_t timestamp)
{
  
}

/********* TinyUSB HID callbacks ***************/

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void)
{
  switch (_usb_mode)
  {
  default:
  case INPUT_MODE_SWPRO:
    return (uint8_t const *)&swpro_device_descriptor;
    break;

  case INPUT_MODE_XINPUT:
    return (uint8_t const *)&xid_device_descriptor;
    break;

  case INPUT_MODE_CDC:
    return (uint8_t const *)&serial_device_descriptor;
  }
}


// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
  (void)index; // for multiple configurations
  switch (_usb_mode)
  {
  default:

  case INPUT_MODE_SWPRO:
    return (uint8_t const *)&swpro_configuration_descriptor;
    break;

  case INPUT_MODE_XINPUT:
    return (uint8_t const *)&xid_configuration_descriptor;
    break;

  case INPUT_MODE_CDC:
    return (uint8_t const *) &serial_configuration_descriptor;
    break;
  }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
  (void)instance;
  (void)report_id;
  (void)reqlen;

  return 0;
}

// Invoked when report complete
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
  _usb_clear = true;
  switch (_usb_mode)
  {
  case INPUT_MODE_SWPRO:
    if ((report[0] == 0x30))
    {
    }
    break;

  default:

  case INPUT_MODE_XINPUT:
    if ((report[0] == 0x00) && (report[1] == XID_REPORT_LEN))
    {
    }

    break;
  }
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
  switch (_usb_mode)
  {
  default:

  case INPUT_MODE_SWPRO:
    if (!report_id && !report_type)
    {
      if (buffer[0] == SW_OUT_ID_RUMBLE)
      {
        rumble_translate(instance, &buffer[2]);
      }
      else
      {
        switch_commands_future_handle(instance, buffer[0], buffer, bufsize);
      }
    }
    break;

  case INPUT_MODE_XINPUT:
    if (!report_id && !report_type)
    {
      if ((buffer[0] == 0x00) && (buffer[1] == 0x08))
      {
        if ((buffer[3] > 0) || (buffer[4] > 0))
        {
          // cb_hoja_rumble_enable(true);
        }
        else
        {
          // cb_hoja_rumble_enable(false);
        }
      }
    }
    break;
  }
}

// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
  (void)instance;
  switch (_usb_mode)
  {
  default:
  case INPUT_MODE_SWPRO:
    return swpro_hid_report_descriptor;
    break;
  }
  return NULL;
}

// Set up custom TinyUSB XInput Driver
// Sets up custom TinyUSB Device Driver
usbd_class_driver_t const *usbd_app_driver_get_cb(uint8_t *driver_count)
{
  if(_usb_mode == INPUT_MODE_XINPUT)
  {
    *driver_count += 1;
  }
  
  return &tud_xinput_driver;
}

// String Descriptor Index
enum
{
  STRID_LANGID = 0,
  STRID_MANUFACTURER,
  STRID_PRODUCT,
  STRID_SERIAL,
};

static uint16_t _desc_str[64];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void)langid;

  uint8_t chr_count;
  const char *str = global_string_descriptor[index];

  switch (index)
  {
  case STRID_LANGID:
    memcpy(&_desc_str[1], global_string_descriptor[0], 2);
    chr_count = 1;
    break;

  default:
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    // Cap at max char... WHY?
    chr_count = strlen(str);
    if (chr_count > 31)
      chr_count = 31;

    // Convert ASCII string into UTF-16
    for (uint8_t i = 0; i < chr_count; i++)
    {
      _desc_str[1 + i] = str[i];
    }
    break;
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
  return _desc_str;
}

// Vendor Device Class CB for receiving data
void tud_vendor_rx_cb(uint8_t itf)
{
  printf("WebUSB Data Received.\n");
  uint8_t buffer[64] = {0};
  uint32_t size = 0;
  tud_vendor_n_read(itf, buffer, 64);
  webusb_command_processor(buffer);
}

/********* USB Data Handling Utilities ***************/
/**
 * @brief Takes in directions and outputs a byte output appropriate for
 * HID Hat usage.
 * @param hat_type hat_mode_t type - The type of controller you're converting for.
 * @param leftRight 0 through 2 (2 is right) to indicate the direction left/right
 * @param upDown 0 through 2 (2 is up) to indicate the direction up/down
 */
uint8_t dir_to_hat(uint8_t leftRight, uint8_t upDown)
{
  uint8_t ret = XI_HAT_CENTER;

  if (leftRight == 2)
  {
    ret = XI_HAT_RIGHT;
    if (upDown == 2)
    {
      ret = XI_HAT_TOP_RIGHT;
    }
    else if (upDown == 0)
    {
      ret = XI_HAT_BOTTOM_RIGHT;
    }
  }
  else if (leftRight == 0)
  {
    ret = XI_HAT_LEFT;
    if (upDown == 2)
    {
      ret = XI_HAT_TOP_LEFT;
    }
    else if (upDown == 0)
    {
      ret = XI_HAT_BOTTOM_LEFT;
    }
  }

  else if (upDown == 2)
  {
    ret = XI_HAT_TOP;
  }
  else if (upDown == 0)
  {
    ret = XI_HAT_BOTTOM;
  }

  return ret;
}
