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
    if (!gpio_get(ADAPTER_BUTTON_1) && !gpio_get(ADAPTER_BUTTON_2))
    {
        reset_usb_boot(0, 0);
    }

    input_mode_t mode = INPUT_MODE_SWPRO;

    if (settings_load())
    {
        wd_scratch_readout_u wd = {0};

        wd.value = scratch_get(WD_READOUT_IDX);

        if (wd.reboot_reason == WD_REBOOT_REASON_MODECHANGE)
        {
            mode = wd.adapter_mode;
        }
        else
        {
            mode = global_loaded_settings.input_mode;
        }

        switch (mode)
        {
        default:
        case INPUT_MODE_SWPRO:
            rgb_set_all(COLOR_YELLOW.color);
            break;

        case INPUT_MODE_SLIPPI:
            rgb_set_all(COLOR_PINK.color);
            break;

        case INPUT_MODE_XINPUT:
            rgb_set_all(COLOR_GREEN.color);
            break;

        case INPUT_MODE_GCADAPTER:
            rgb_set_all(COLOR_PURPLE.color);
            break;
        }
    }

    settings_core0_save_check();

    adapter_usb_start(mode);
    stdio_init_all();

    rgb_set_dirty();

    bool did = false;
    bool sent = false;

    for (;;)
    {
        uint32_t t = time_us_32();
        settings_core0_save_check();
        rgb_task(t);
        tud_task();

        if (did)
        {
            adapter_comms_task(t);
            wd_mode_task(t);
        }
        else
        {
            adapter_init();
            sleep_ms(100);
            did = true;
        }
    }
}
