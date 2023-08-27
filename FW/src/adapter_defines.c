#include "adapter_defines.h"
#include "adapter_includes.h"

const char* global_string_descriptor[] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04},                // 0: is supported language is English (0x0409)
    ADAPTER_MANUFACTURER,              // 1: Manufacturer
    ADAPTER_PRODUCT,        // 2: Product
    "000000",           // 3: Serials, should use chip ID
};
