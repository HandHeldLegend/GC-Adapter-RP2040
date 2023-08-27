#ifndef ADAPTER_TYPES_H
#define ADAPTER_TYPES_H

#include <inttypes.h>

typedef enum
{
    GC_SP_MODE_NONE = 0, // No function. LT and RT are output full according to digital button.
    GC_SP_MODE_LT   = 1, // SP buttton inputs light trigger left
    GC_SP_MODE_RT   = 2, // SP buttton inputs light trigger right
    GC_SP_MODE_ADC  = 3, // Controlled fully by analog, SP button is unused
} gc_sp_mode_t;

typedef struct
{
    union
    {
        struct
        {
            uint8_t padding : 8;
            uint8_t b : 8;
            uint8_t r : 8;
            uint8_t g : 8;
        };
        uint32_t color;
    };
} rgb_s;

typedef enum
{
    INPUT_MODE_SWPRO,
    INPUT_MODE_XINPUT,
    INPUT_MODE_GAMECUBE,
} input_mode_t;

typedef enum
{
    USBRATE_8 = 7600,
    USBRATE_1 = 600,
} usb_rate_t;

typedef enum
{
  RUMBLE_OFF,
  RUMBLE_BRAKE,
  RUMBLE_ON,
} rumble_t;

typedef enum
{
  XI_HAT_TOP          = 0x01,
  XI_HAT_TOP_RIGHT    = 0x02,
  XI_HAT_RIGHT        = 0x03,
  XI_HAT_BOTTOM_RIGHT = 0x04,
  XI_HAT_BOTTOM       = 0x05,
  XI_HAT_BOTTOM_LEFT  = 0x06,
  XI_HAT_LEFT         = 0x07,
  XI_HAT_TOP_LEFT     = 0x08,
  XI_HAT_CENTER       = 0x00,
} xi_input_hat_dir_t;

/** @brief This is a struct for containing all of the
 * button input data as bits. This saves space
 * and allows for easier handoff to the various
 * controller cores in the future.
**/
typedef struct
{
    union
    {
        struct
        {
            // D-Pad
            uint8_t dpad_up         : 1;
            uint8_t dpad_down       : 1;
            uint8_t dpad_left       : 1;
            uint8_t dpad_right      : 1;
            // Buttons
            uint8_t button_a       : 1;
            uint8_t button_b     : 1;
            uint8_t button_x     : 1;
            uint8_t button_y    : 1;
            // Triggers
            uint8_t trigger_l       : 1;
            uint8_t trigger_zl      : 1;
            uint8_t trigger_r       : 1;
            uint8_t trigger_zr      : 1;

            // Special Functions
            uint8_t button_plus     : 1;
            uint8_t button_minus    : 1;

            // Stick clicks
            uint8_t button_stick_left   : 1;
            uint8_t button_stick_right  : 1;
        };
        uint16_t buttons_all;
    };

    union
    {
        struct
        {
            // Menu buttons (Not remappable by API)
            uint8_t button_capture  : 1;
            uint8_t button_home     : 1;
            uint8_t button_safemode : 1;
            uint8_t padding         : 5;
        };
        uint8_t buttons_system;
    };
} __attribute__ ((packed)) button_data_s;

// Analog input data structure
typedef struct
{
    int lx;
    int ly;
    int rx;
    int ry;
    int lt;
    int rt;
} a_data_s;



#endif
