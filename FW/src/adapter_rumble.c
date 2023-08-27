#include "adapter_includes.h"
#include "interval.h"
#include "main.h"

bool rumble_on = false;
bool ramp_up = false;
bool ramp_down = false;
const uint32_t _rumble_interval = 8000;
#define RUMBLE_MAX 110
#define HOLD_WAIT 8

static bool _rumble = false;

void app_rumble_task(uint32_t timestamp)
{
    static int lvl = 0;

    if(interval_run(timestamp, _rumble_interval))
    {
        if (_rumble)
        {
            lvl += 10;
            if (lvl >= RUMBLE_MAX)
            {
                lvl = RUMBLE_MAX;
            }

            /*
            pwm_set_gpio_level(PGPIO_RUMBLE_BRAKE, 0);
            pwm_set_gpio_level(PGPIO_RUMBLE_MAIN, lvl);
            */
        }
        else
        {
            lvl -= 20;
            if (lvl <= 0)
            {
                lvl = 0;
                /*
                pwm_set_gpio_level(PGPIO_RUMBLE_MAIN, 0);
                pwm_set_gpio_level(PGPIO_RUMBLE_BRAKE, 255);
                */
            }
            else
            {
                //pwm_set_gpio_level(PGPIO_RUMBLE_MAIN, lvl);
            } 
        }
    }
}

void cb_hoja_rumble_enable(bool rumble)
{
    _rumble = rumble;
}
