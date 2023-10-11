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

    rgb_set_all(COLOR_YELLOW.color);
    if (settings_load())
    {
        rgb_set_all(COLOR_RED.color);
    }

    settings_core0_save_check();

    input_mode_t mode = INPUT_MODE_GCADAPTER;

    adapter_usb_start(mode);
    stdio_init_all();

    
    rgb_set_dirty();

    bool did = false;
    bool sent = false;

    for(;;)
    {
        uint32_t t = time_us_32();
        settings_core0_save_check();
        rgb_task(t);
        tud_task();
        
        if (!gpio_get(ADAPTER_BUTTON_1))
        {
            reset_usb_boot(0, 0);
        }

        if(did)
        {
            adapter_comms_task(t);
        }
        else
        {
            adapter_init();
            sleep_ms(100);
            did=true;
        }
    }

}
