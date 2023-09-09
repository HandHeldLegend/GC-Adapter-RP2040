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

    input_mode_t mode = INPUT_MODE_SWPRO;

    adapter_usb_start(mode);
    stdio_init_all();

    rgb_set_all(COLOR_BLUE.color);
    rgb_set_dirty();

    sleep_ms(1000);
    adapter_init(mode);

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

        adapter_comms_task(t);
        
    }

}
