#include "switch_spi.h"

/**
 * @brief Reads a chunk of memory from SPI (emulated)
 * @param[in] offset_address The address segment to read from.
 * @param[in] address The actual memory address to read from the segment.
 * @param[in] length The amount of bytes to pull from SPI emulated
 */
void sw_spi_readfromaddress(uint8_t offset_address, uint8_t address, uint8_t length)
{

  uint8_t read_info[5] = {address, offset_address, 0x00, 0x00, length};
  switch_commands_bulkset(14, read_info, 5);

  uint8_t output_spi_data[30] = {};

  for (int i = 0; i < length; i++)
  {
      output_spi_data[i] = sw_spi_getaddressdata(offset_address, address+i);
  }

  // Do a bulk set for the input report
  switch_commands_bulkset(SPI_READ_OUTPUT_IDX, output_spi_data, length);
}

/**
 * @brief Handles SPI reads of misc data
 * or unknown addresses :)
 */
uint8_t sw_spi_getaddressdata(uint8_t offset_address, uint8_t address)
{
    switch (offset_address)
    {
        // Patch ROM not needed
        case 0x00:

            return 0x00;
            break;

        // Failsafe mechanism not needed
        case 0x10:

            return 0x00;
            break;

        // Pairing data
        case 0x20 ... 0x40:
            switch (address)
            {
                // Magic number determines if Pairing info is used
                case 0x00:
                    return 0x00; // 0x95 if used
                    break;

                // Size of pairing data
                case 0x01:
                    return 0x22;
                    break;

                // Checksum
                case 0x02:
                case 0x03:
                    return 0x00;
                    break;

                // Host BT address (Big-endian)
                case 0x04 ... 0x09:
                    return 0x00; //loaded_settings.ns_host_bt_address[address-4];
                    break;

                // Bluetooth LTK (Little-endian) NOT IMPLEMENTED YET
                case 0x0A ... 0x19:
                    return 0x00;
                    break;

                // Host capability 0x68 is Nintendo Switch. 0x08 is PC
                case 0x24:
                    return 0x68;
                    break;

                // Pairing section 2. Respond all 0xFF.
                case 0x26 ... 0x4B:
                    return 0xFF;
                    break;

                // No match... Return 0
                default:
                    return 0x00;
                    break;
            }
            break;

        // Shipment info. ALWAYS return 0.
        case 0x50:
            return 0x00;
            break;

        // Factory configuration and calibration
        case 0x60:
            switch (address)
            {
                // STAGE 1 READ
                // 0x6000 - Serial number stage | Length 16 bytes
                // Serial number disabled. First byte >= 0x80 means NO serial.
                case 0x00 ... 0x0F:
                    return 0xFF; // Real pro controller... 255 for all this data.
                    break;

                // END OF STAGE 1 //
                // --------------------- //

                // Return factory controller type
                // Set as pro controller
                case 0x12:
                    return 0x03;
                    break;

                // Unknown constant
                case 0x13:
                    return 0x02;
                    break;

                // Returns a bool indicating if color is set.
                // 0x02 if the controller is SNES
                case 0x1B:
                    return 0x01;
                    break;

                // TO-DO - Implement factory 6-Axis calibration.

                // X XL
                case 0x20:
                    return 35;
                    break;
                // X XL
                case 0x21:
                    return 0;
                    break;

                // Y XL
                case 0x22:
                    return 185;
                    break;

                // Y XL
                case 0x23:
                    return 255;
                    break;

                // Z XL
                case 0x24:
                    return 26;
                    break;

                // Z XL
                case 0x25:
                    return 1;
                    break;

                // X COEFF
                case 0x26:
                    return 0;
                    break;

                // X COEFF
                case 0x27:
                    return 64;
                    break;

                // Y COEFF
                case 0x28:
                    return 0;
                    break;

                // Y COEFF
                case 0x29:
                    return 64;
                    break;

                // Z COEFF
                case 0x2A:
                    return 0;
                    break;

                // Z COEFF
                case 0x2B:
                    return 64;
                    break;

                // GYRO X - default 7
                case 0x2C:
                    return 1;
                    break;

                case 0x2D:
                    return 0;
                    break;

                // GYRO Y - default 65533
                case 0x2E:
                    return 1;
                    break;

                case 0x2F:
                    return 0;
                    break;

                // GYRO Z - default 65526
                case 0x30:
                    return 1;
                    break;

                case 0x31:
                    return 0;
                    break;

                // GYRO COEFF
                case 0x32:
                    return 0x3B;
                    break;

                case 0x33:
                    return 0x34;
                    break;

                case 0x34:
                    return 0x3B;
                    break;

                case 0x35:
                    return 0x34;
                    break;

                case 0x36:
                    return 0x3B;
                    break;

                case 0x37:
                    return 0x34;
                    break;

                case 0x3D ... 0x45:
                    return switch_analog_calibration_data[address-0x3D];
                    break;

                case 0x46 ... 0x4E:
                    return switch_analog_calibration_data[address-0x3D];
                    break;

                case 0x4F:
                    return 0xFF; // Not sure
                    break;

                // STAGE 2 READ
                // 0x6050 | Length 13 bytes
                // TO-DO - Implement factory body color.
                case 0x50:
                    return 26; // Red
                    break;
                case 0x51:
                    return 26; // Green
                    break;
                case 0x52:
                    return 26; // Blue
                    break;

                // TO-DO - Implement factory buttons color.
                case 0x53 ... 0x55:
                    return 94; // Default real ProCon
                    break;

                // TO-DO - Implement factory left grip color.
                case 0x56:
                    return 255; // R default smash procon
                    break;

                case 0x57:
                    return 255; // G
                    break;

                case 0x58:
                    return 255; // B
                    break;

                // TO-DO - Implement factory grip (left and right) color.
                case 0x59 ... 0x5B:
                    return 255; // Default procon
                    break;

                case 0x5C:
                    return 0x01; // Default smash procon
                    break;

                    // END OF STAGE 2 //
                    // --------------------- //

                // Stage 3 configuration 0x6080, length 24
                // Covers factory sensor and stick device params

                // Start accelerometer offsets
                case 0x80:
                    return 80;
                    break;
                case 0x81:
                    return 253;
                    break;
                case 0x82:
                    return 0;
                    break;
                case 0x83:
                    return 0;
                    break;
                case 0x84:
                    return 198;
                    break;
                case 0x85:
                    return 15;
                    break;

                // Start stick device parameters
                // STAGE 4 is mirrored. Redundancy?
                case 0x98:
                case 0x86:
                    return 15;
                    break;

                case 0x99:
                case 0x87:
                    return 48;
                    break;

                case 0x9A:
                case 0x88:
                    return 97;
                    break;

                case 0x9B:
                case 0x89:
                    return 174;
                    break;

                case 0x9C:
                case 0x8A:
                    return 144;
                    break;

                case 0x9D:
                case 0x8B:
                    return 217;
                    break;

                case 0x9E:
                case 0x8C:
                    return 212;
                    break;

                case 0x9F:
                case 0x8D:
                    return 20;
                    break;

                case 0xA0:
                case 0x8E:
                    return 84;
                    break;

                case 0xA1:
                case 0x8F:
                    return 65;
                    break;

                case 0xA2:
                case 0x90:
                    return 21;
                    break;

                case 0xA3:
                case 0x91:
                    return 84;
                    break;

                case 0xA4:
                case 0x92:
                    return 199;
                    break;

                case 0xA5:
                case 0x93:
                    return 121;
                    break;

                case 0xA6:
                case 0x94:
                    return 156;
                    break;

                case 0xA7:
                case 0x95:
                    return 51;
                    break;

                case 0xA8:
                case 0x96:
                    return 54;
                    break;

                case 0xA9:
                case 0x97:
                    return 99;
                    break;

                    // END OF STAGE 3 //
                    // --------------------- //

                default:
                    return 0x00;
                    break;
            }
            break;


        // User calibration values
        case 0x80:
            switch (address)
            {
                // STAGE 5
                // Stick user calibration
                // Magic bytes

                case 0x10 ... 0x1A:
                    return 0xFF; // No left stick config
                    break;
                case 0x1B ... 0x25:
                    return 0xFF; // No user config... maybe this helps :,)
                    break;

                // Gyro Calibration
                case 0x26 ... 0x3F:
                    return 0xFF; //global_loaded_settings.imu_calibration[address-0x26];
                    break;

                default:
                    return 0xFF;
                    break;
            }
            break;

        default:
            return 0xFF;
            break;
    }

    return 0xFF;
}
