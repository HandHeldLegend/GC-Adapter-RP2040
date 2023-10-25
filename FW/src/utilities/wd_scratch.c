#include "wd_scratch.h"
#include "interval.h"

#define SCRATCH_OFFSET 0xC
#define MAX_INDEX     7

uint32_t scratch_get(uint8_t index) {
    if (index > MAX_INDEX) {
        // Handle the error, maybe by returning an error code or logging a message.
        // Here we just return 0 as a simple example.
        return 0;
    }
    return *((volatile uint32_t *) (WATCHDOG_BASE + SCRATCH_OFFSET + (index * 4)));
}

void scratch_set(uint8_t index, uint32_t value) {
    if (index > MAX_INDEX) {
        // Handle the error here. For simplicity, we'll just return in this example.
        return;
    }
    *((volatile uint32_t *) (WATCHDOG_BASE + SCRATCH_OFFSET + (index * 4))) = value;
}

void wd_mode_task(uint32_t timestamp)
{
    static bool fwd_press = false;
    static bool back_press = false;

    if(interval_run(timestamp, 16000))
    {
        
        if(!gpio_get(ADAPTER_BUTTON_1) && !back_press)
        {
            back_press = true;
        }
        else if(gpio_get(ADAPTER_BUTTON_1) && back_press)
        {
            back_press = false;
            adapter_usb_mode_cycle(false);
        }

        if(!gpio_get(ADAPTER_BUTTON_2) && !fwd_press)
        {
            fwd_press = true;
        }
        else if (gpio_get(ADAPTER_BUTTON_2) && fwd_press)
        {
            fwd_press = false;
            adapter_usb_mode_cycle(true);
        }
    }
}