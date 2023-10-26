#ifndef RGB_F_H
#define RGB_F_H

#include "adapter_includes.h"
#include "interval.h"

//#define RGB_S_COLOR(r, g, b) ((rgb_s) { .color = ((r << 16) | (g << 8) | (b))})

#define COLOR_RED    (rgb_s) {.r = 0xFF, .g = 0x00, .b = 0x00}
#define COLOR_ORANGE (rgb_s) {.r = 0xFF, .g = 0x4d, .b=0x00}
#define COLOR_YELLOW (rgb_s) {.r = 0xFF, .g = 0xE6, .b=0x00}
#define COLOR_GREEN  (rgb_s) {.r = 0x00, .g = 0xff, .b=0x00}
#define COLOR_BLUE   (rgb_s) {.r = 0x00, .g = 0x00, .b=0xFF}
#define COLOR_CYAN   (rgb_s) {.r = 0x15, .g = 0xFF, .b=0xF1}
#define COLOR_PURPLE (rgb_s) {.r = 0x61, .g = 0x00, .b=0xff}
#define COLOR_PINK   (rgb_s) {.r = 0xff, .g = 0x2B, .b=0xD0}
#define COLOR_WHITE  (rgb_s) {.r = 0xa1, .g = 0xa1, .b=0xa1}

typedef enum{
    RGB_GROUP_RS = 0,
    RGB_GROUP_LS = 1,
    RGB_GROUP_DPAD = 2,
    RGB_GROUP_MINUS = 3,
    RGB_GROUP_CAPTURE = 4,
    RGB_GROUP_HOME = 5,
    RGB_GROUP_PLUS = 6,
    RGB_GROUP_Y = 7,
    RGB_GROUP_X = 8,
    RGB_GROUP_A = 9,
    RGB_GROUP_B = 10,
    RGB_GROUP_MAX,
} rgb_group_t;

void rgb_set_dirty();

void rgb_load_preset();

void rgb_set_group(rgb_group_t group, uint32_t color);

void rgb_set_instant(uint32_t color);

void rgb_set_single(uint32_t color, uint8_t idx);

void rgb_set_all(uint32_t color);

void rgb_init();

void rgb_task(uint32_t timestamp);

#endif
