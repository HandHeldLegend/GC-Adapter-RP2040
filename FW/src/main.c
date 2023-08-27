#include "adapter_includes.h"
#include "main.h"

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
    stdio_init_all();

    printf("GC Adapter Started.\n");

    // Handle bootloader stuff
    if (!gpio_get(ADAPTER_BUTTON_1))
    {
        //reset_usb_boot(0, 0);
    }
    
    adapter_hardware_setup();

    rgb_set_all(COLOR_BLUE.color);
    rgb_set_dirty();

    bool red = false;
    bool green = false;
    bool off = false;
    bool final = false;

    for(;;)
    {
        uint32_t timestamp = time_us_32();
        rgb_task(timestamp);

        if(( timestamp> (1*1000000)) && !red)
        {
            rgb_set_all(COLOR_RED.color);
            rgb_set_dirty();
            red=true;
        }

        else if(( timestamp>(2*1000000)) && !green)
        {
            rgb_set_all(COLOR_GREEN.color);
            rgb_set_dirty();
            green=true;
        }

        else if(( timestamp>(3*1000000) ) && !off)
        {
            rgb_set_all(0x00);
            rgb_set_dirty();
            off=true;
        }

        else if (( timestamp>(4*1000000) ) && !final)
        {
            reset_usb_boot(0, 0);
        }
    }

}
