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

    adapter_hardware_setup();

    // Handle bootloader stuff
    if (!gpio_get(ADAPTER_BUTTON_1))
    {
        reset_usb_boot(0, 0);
    }

    rgb_set_all(COLOR_BLUE.color);
    rgb_set_dirty();

    bool red = false;
    bool green = false;
    bool off = false;
    bool final = false;

    adapter_usb_start(INPUT_MODE_XINPUT);

    for(;;)
    {
        uint32_t t = time_us_32();
        rgb_task(t);
        tud_task();
        if (!gpio_get(ADAPTER_BUTTON_1) && !gpio_get(ADAPTER_BUTTON_2))
        {
            reset_usb_boot(0, 0);
            
        }

        if(tud_xinput_n_ready(0))
        {
            static button_data_s b;
            static a_data_s a;
            xinput_hid_report(0, &b, &a);
        }
        
    }

}
