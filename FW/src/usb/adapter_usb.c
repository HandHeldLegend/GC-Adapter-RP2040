/*
 * Copyright (c) [2023] [Mitch Cairns/Handheldlegend, LLC]
 * All rights reserved.
 *
 * This source code is licensed under the provisions of the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "adapter_usb.h"
#include "interval.h"

input_mode_t _usb_mode    = INPUT_MODE_XINPUT;
bool _usb_clear = false;

// Default 8ms (8000us)
uint32_t _usb_rate = 0;

typedef void (*usb_cb_t)(button_data_s *, a_data_s *);

usb_cb_t _usb_hid_cb = NULL;

void _hoja_usb_set_interval(usb_rate_t rate)
{
  _usb_rate = rate;
}

bool hoja_usb_start(input_mode_t mode)
{

  imu_set_enabled(false);

  switch(mode)
  {
    default:
    case INPUT_MODE_SWPRO:
      _hoja_usb_set_interval(USBRATE_8);
      _usb_hid_cb = swpro_hid_report;
      break;

    case INPUT_MODE_XINPUT:
      _hoja_usb_set_interval(USBRATE_8);
      _usb_hid_cb = xinput_hid_report;
      break;
  }

  _usb_mode = mode;
  return tusb_init();
}

uint8_t buf = 0;

static inline bool _hoja_usb_ready()
{
  if (_usb_mode == INPUT_MODE_XINPUT)
  {
    return tud_xinput_ready();
  }
  else return tud_hid_ready();
}

void hoja_usb_task(uint32_t timestamp, button_data_s *button_data, a_data_s *analog_data)
{
    if (interval_resettable_run(timestamp, _usb_rate, _usb_clear))
    {
      if(_hoja_usb_ready())
      {
        _usb_hid_cb(button_data, analog_data);
      }
    }
    else
    {
      _usb_clear = false;
    }
}

/********* TinyUSB HID callbacks ***************/

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const* tud_descriptor_device_cb(void) {
  switch(_usb_mode)
  {
    default:
    case INPUT_MODE_SWPRO:
      return (uint8_t const*) &swpro_device_descriptor;
      break;

    case INPUT_MODE_XINPUT:
      return (uint8_t const*) &xid_device_descriptor;
      break;
  }
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
  (void)index;  // for multiple configurations
  switch(_usb_mode)
  {
    default:

    case INPUT_MODE_SWPRO:
      return (uint8_t const*) &swpro_configuration_descriptor;
      break;

    case INPUT_MODE_XINPUT:
      return (uint8_t const*) &xid_configuration_descriptor;
      break;
  }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  (void) instance;
  (void) report_id;
  (void) reqlen;

  return 0;
}

// Invoked when report complete
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
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
            if ( (report[0] == 0x00) && (report[1] == XID_REPORT_LEN))
            {

            }

            break;
    }

}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  switch (_usb_mode)
  {
    default:

    case INPUT_MODE_SWPRO:
      if (!report_id && !report_type)
      {
        if (buffer[0] == SW_OUT_ID_RUMBLE)
        {
          rumble_translate(&buffer[2]);
        }
        else
        {
          switch_commands_future_handle(buffer[0], buffer, bufsize);
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
                  cb_hoja_rumble_enable(true);
              }
              else
              {
                  cb_hoja_rumble_enable(false);
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
    (void) instance;
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
    *driver_count += 1;
    return &tud_xinput_driver;
}

static uint16_t _desc_str[64];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], global_string_descriptor[0], 2);
    chr_count = 1;
  }else
  {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    const char* str = global_string_descriptor[index];

    // Cap at max char... WHY?
    chr_count = strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    // Convert ASCII string into UTF-16
    for(uint8_t i=0; i<chr_count; i++)
    {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
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
uint8_t dir_to_hat(hat_mode_t hat_type, uint8_t leftRight, uint8_t upDown)
{
    uint8_t ret = 0;
    switch(hat_type)
    {
        default:
        case HAT_MODE_NS:
            ret = NS_HAT_CENTER;

        if (leftRight == 2)
        {
            ret = NS_HAT_RIGHT;
            if (upDown == 2)
            {
                ret = NS_HAT_TOP_RIGHT;
            }
            else if (upDown == 0)
            {
                ret = NS_HAT_BOTTOM_RIGHT;
            }
        }
        else if (leftRight == 0)
        {
            ret = NS_HAT_LEFT;
            if (upDown == 2)
            {
                ret = NS_HAT_TOP_LEFT;
            }
            else if (upDown == 0)
            {
                ret = NS_HAT_BOTTOM_LEFT;
            }
        }

        else if (upDown == 2)
        {
            ret = NS_HAT_TOP;
        }
        else if (upDown == 0)
        {
            ret = NS_HAT_BOTTOM;
        }

        return ret;
        break;

        case HAT_MODE_XI:
                ret = XI_HAT_CENTER;

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
            break;
    }
}
