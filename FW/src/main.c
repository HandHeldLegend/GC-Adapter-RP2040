#include "main.h"

bool cb_adapter_hardware_test()
{
    adapter_ll_hardware_setup();
    rgb_init();

    // Check GPIO levels if they are all HIGH indicating pull-ups are working
    bool gpio_fail = false;
    if(!adapter_ll_gpio_read(JOYBUS_PORT_1))
    {
        gpio_fail=true;
    }

    if(!adapter_ll_gpio_read(JOYBUS_PORT_2))
    {
        gpio_fail=true;
    }

    if(!adapter_ll_gpio_read(JOYBUS_PORT_3))
    {
        gpio_fail=true;
    }

    if(!adapter_ll_gpio_read(JOYBUS_PORT_4))
    {
        gpio_fail=true;
    }

    // If the test has failed, we can return a fail
    if(gpio_fail) return false;

    // Flash all three rgb colors then we can return our test value
    rgb_set_instant(COLOR_RED.color);
    sleep_ms(1000);
    rgb_set_instant(COLOR_GREEN.color);
    sleep_ms(1000);
    rgb_set_instant(COLOR_BLUE.color);
    sleep_ms(1000);
    rgb_set_instant(COLOR_WHITE.color);
    sleep_ms(1000);
    rgb_set_instant(0x00);
    sleep_ms(1000);
    return true;
}

int main()
{
    adapter_main_init();
    adapter_main_loop();
}
