#include "webusb.h"

uint8_t _webusb_out_buffer[64] = {0x00};

void webusb_save_confirm()
{
    printf("Sending Save receipt...\n");
    memset(_webusb_out_buffer, 0, 64);
    _webusb_out_buffer[0] = 0xF1;
    tud_vendor_n_write(0, _webusb_out_buffer, 64);
    tud_vendor_n_flush(0);
}

void webusb_command_processor(uint8_t *data)
{
    switch(data[0])
    {
        default:

            break;

        case WEBUSB_CMD_FW_SET:
            {
                reset_usb_boot(0, 0);
            }
            break;

        case WEBUSB_CMD_FW_GET:
            {
                _webusb_out_buffer[0] = WEBUSB_CMD_FW_GET;
                _webusb_out_buffer[1] = (HOJA_FW_VERSION & 0xFF00)>>8;
                _webusb_out_buffer[2] = HOJA_FW_VERSION & 0xFF;
                tud_vendor_n_write(0, _webusb_out_buffer, 64);
                tud_vendor_n_flush(0);
            }
            break;

        case WEBUSB_CMD_CALIBRATION_START:
            {
                printf("WebUSB: Got calibration START command.\n");
                analog_calibrate_start();
                _webusb_out_buffer[0] = WEBUSB_CMD_CALIBRATION_START;
                tud_vendor_n_write(0, _webusb_out_buffer, 64);
                tud_vendor_n_flush(0);
            }
            break;

        case WEBUSB_CMD_CALIBRATION_STOP:
            {
                printf("WebUSB: Got calibration STOP command.\n");
                analog_calibrate_stop();
                _webusb_out_buffer[0] = WEBUSB_CMD_CALIBRATION_STOP;
                tud_vendor_n_write(0, _webusb_out_buffer, 64);
                tud_vendor_n_flush(0);
            }
            break;

        case WEBUSB_CMD_OCTAGON_SET:
            {
                printf("WebUSB: Got angle capture command.\n");
                analog_calibrate_angle();
                stick_scaling_set_settings();
                stick_scaling_init();
            }
            break;

        // Set RGB Group
        case WEBUSB_CMD_RGB_SET:
            {
                printf("WebUSB: Got RGB SET command.\n");
                rgb_s col = 
                {
                    .r = data[2],
                    .g = data[3],
                    .b = data[4],
                };
                global_loaded_settings.rgb_colors[data[1]] = col.color;
                rgb_set_group(data[1], col.color);
                rgb_set_dirty();
            }
            break;

        case WEBUSB_CMD_RGB_GET:
            {
                printf("WebUSB: Got RGB GET command.\n");
                memset(_webusb_out_buffer, 0, 64);
                _webusb_out_buffer[0] = WEBUSB_CMD_RGB_GET;

                for(uint8_t i = 0; i < 12; i++)
                {
                    rgb_s c = {.color = global_loaded_settings.rgb_colors[i]};
                    uint8_t t = (i*3)+1;
                    _webusb_out_buffer[t] = c.r;
                    _webusb_out_buffer[t+1] = c.g; 
                    _webusb_out_buffer[t+2] = c.b;
                }
                tud_vendor_n_write(0, _webusb_out_buffer, 64);
                tud_vendor_n_flush(0);
            }
            break;

        case WEBUSB_CMD_SNAPBACK_SET:
            {
                printf("WebUSB: Got Snapback SET command.\n");
                settings_set_snapback(data[1], data[2]);
            }
            break;

        case WEBUSB_CMD_SNAPBACK_GET:
            {
                printf("DEPRECIATED: WebUSB: Got Snapback GET command.\n");
                _webusb_out_buffer[0] = WEBUSB_CMD_SNAPBACK_GET;
                _webusb_out_buffer[1] = 0;
                _webusb_out_buffer[2] = 0;
                _webusb_out_buffer[3] = 0;
                _webusb_out_buffer[4] = 0;
                tud_vendor_n_write(0, _webusb_out_buffer, 64);
                tud_vendor_n_flush(0);
            }
            break;

        case WEBUSB_CMD_REMAP_SET:
            {
                printf("WebUSB: Got Remap SET command.\n");
                remap_listen_enable(data[1], data[2]);
            }
            break;

        case WEBUSB_CMD_REMAP_GET:
            {
                printf("WebUSB: Got Remap GET command.\n");
                remap_send_data_webusb(data[1]);
            }
            break;

        case WEBUSB_CMD_GCSP_SET:
            {
                printf("WebUSB: Got GCSP SET command.\n");
                remap_set_gc_sp(data[1]);
            }
            break;

        case WEBUSB_CMD_REMAP_DEFAULT:
            {
                printf("WebUSB: Got Remap SET default command.\n");
                remap_reset_default(data[1]);
                remap_send_data_webusb(data[1]);
            }
            break;

        case WEBUSB_CMD_SAVEALL:
            {
                printf("WebUSB: Got SAVE command.\n");
                settings_save_webindicate();
                settings_save();
            }
            break;
    }
}