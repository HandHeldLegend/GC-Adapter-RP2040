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
    INPUT_MODE_GCADAPTER,
    INPUT_MODE_SLIPPI,
    INPUT_MODE_MAX,
    INPUT_MODE_CDC,
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

typedef struct
{
    union
    {
        struct
        {
            uint8_t stick_left_y;
            uint8_t stick_left_x;

            uint8_t dpad_left : 1;
            uint8_t dpad_right : 1;
            uint8_t dpad_down : 1;
            uint8_t dpad_up : 1;
            uint8_t button_z : 1;
            uint8_t button_r : 1;
            uint8_t button_l : 1;
            uint8_t blank_2 : 1;

            uint8_t button_a : 1;
            uint8_t button_b : 1;
            uint8_t button_x : 1;
            uint8_t button_y : 1;
            uint8_t button_start : 1;
            uint8_t blank_1 : 3;
            
        };
        uint32_t byte_1;
    };

    union
    {
        struct
        {
            
            uint8_t analog_trigger_r;
            uint8_t analog_trigger_l;
            uint8_t stick_right_y;
            uint8_t stick_right_x;
        };
        uint32_t byte_2;
    };

    int port_itf;
} joybus_input_s;

typedef struct
{
    union
    {
        struct
        {
            // Y and C-Up (N64)
            uint8_t b_y       : 1;


            // X and C-Left (N64)
            uint8_t b_x       : 1;

            uint8_t b_b       : 1;
            uint8_t b_a       : 1;
            uint8_t t_r_sr    : 1;
            uint8_t t_r_sl    : 1;
            uint8_t t_r       : 1;

            // ZR and C-Down (N64)
            uint8_t t_zr      : 1;
        };
        uint8_t right_buttons;
    };
    union
    {
        struct
        {
            // Minus and C-Right (N64)
            uint8_t b_minus     : 1;

            // Plus and Start
            uint8_t b_plus      : 1;

            uint8_t sb_right    : 1;
            uint8_t sb_left     : 1;
            uint8_t b_home      : 1;
            uint8_t b_capture   : 1;
            uint8_t none        : 1;
            uint8_t charge_grip_active : 1;
        };
        uint8_t shared_buttons;
    };
    union
    {
        struct
        {
            uint8_t d_down    : 1;
            uint8_t d_up      : 1;
            uint8_t d_right   : 1;
            uint8_t d_left    : 1;
            uint8_t t_l_sr    : 1;
            uint8_t t_l_sl    : 1;
            uint8_t t_l       : 1;

            // ZL and Z (N64)
            uint8_t t_zl      : 1;

        };
        uint8_t left_buttons;
    };

    uint16_t ls_x;
    uint16_t ls_y;
    uint16_t rs_x;
    uint16_t rs_y;

} __attribute__ ((packed)) sw_input_s;

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
