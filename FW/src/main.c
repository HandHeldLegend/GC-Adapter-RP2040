#include "adapter_includes.h"
#include "main.h"
#include "interval.h"

void adapter_hardware_setup()
{
    gpio_init(ADAPTER_BUTTON_1);
    gpio_pull_up(ADAPTER_BUTTON_1);
    gpio_set_dir(ADAPTER_BUTTON_1, GPIO_IN);

    gpio_init(ADAPTER_BUTTON_2);
    gpio_pull_up(ADAPTER_BUTTON_2);
    gpio_set_dir(ADAPTER_BUTTON_2, GPIO_IN);

    rgb_init();
}

int main()
{
    
    adapter_hardware_setup();

    // Handle bootloader stuff
    if (!gpio_get(ADAPTER_BUTTON_1))
    {
        reset_usb_boot(0, 0);
    }

    bool inited = adapter_usb_start(INPUT_MODE_XINPUT);
    stdio_init_all();
    //bool inited = stdio_usb_init();

    rgb_set_all(COLOR_BLUE.color);
    if(!inited) rgb_set_all(COLOR_RED.color);
    rgb_set_dirty();

    sleep_ms(1000);

    bool did = false;
    bool sent = false;

    for(;;)
    {
        uint32_t t = time_us_32();
        rgb_task(t);
        tud_task();
        
        if (!gpio_get(ADAPTER_BUTTON_1))
        {
            reset_usb_boot(0, 0);
        }

        if(!did)
        {
            adapter_init_test();
            did=true;
        }
        else
        {
            adapter_comms_task(t);
        }
        
    }

}
