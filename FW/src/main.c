#include "adapter_includes.h"
#include "main.h"

uint main_slice_num = 0;
uint brake_slice_num = 0;

void cb_hoja_hardware_setup()
{
    // Set up GPIO for input buttons
    hoja_setup_gpio_button(PGPIO_BUTTON_RS);
    hoja_setup_gpio_button(PGPIO_BUTTON_LS);

    hoja_setup_gpio_push(PGPIO_PUSH_A);
    hoja_setup_gpio_push(PGPIO_PUSH_B);
    hoja_setup_gpio_push(PGPIO_PUSH_C);
    hoja_setup_gpio_push(PGPIO_PUSH_D);

    hoja_setup_gpio_scan(PGPIO_SCAN_A);
    hoja_setup_gpio_scan(PGPIO_SCAN_B);
    hoja_setup_gpio_scan(PGPIO_SCAN_C);
    hoja_setup_gpio_scan(PGPIO_SCAN_D);

    // Set up Rumble GPIO
    gpio_init(PGPIO_RUMBLE_MAIN);
    gpio_init(PGPIO_RUMBLE_BRAKE);

    gpio_set_dir(PGPIO_RUMBLE_MAIN, GPIO_OUT);
    gpio_set_dir(PGPIO_RUMBLE_BRAKE, GPIO_OUT);

    gpio_set_function(PGPIO_RUMBLE_MAIN, GPIO_FUNC_PWM);
    gpio_set_function(PGPIO_RUMBLE_BRAKE, GPIO_FUNC_PWM);

    main_slice_num = pwm_gpio_to_slice_num(PGPIO_RUMBLE_MAIN);
    brake_slice_num = pwm_gpio_to_slice_num(PGPIO_RUMBLE_BRAKE);

    pwm_set_wrap(main_slice_num, 255);
    pwm_set_wrap(brake_slice_num, 255);

    pwm_set_chan_level(main_slice_num, PWM_CHAN_B, 0);    // B for odd pins
    pwm_set_chan_level(brake_slice_num, PWM_CHAN_B, 255); // B for odd pins

    pwm_set_enabled(main_slice_num, true);
    pwm_set_enabled(brake_slice_num, true);

    pwm_set_gpio_level(PGPIO_RUMBLE_BRAKE, 255);
    pwm_set_gpio_level(PGPIO_RUMBLE_MAIN, 0);

    sleep_us(150); // Stabilize voltages

    // initialize SPI at 1 MHz
    // initialize SPI at 3 MHz just to test
    spi_init(spi0, 3000 * 1000);
    gpio_set_function(PGPIO_SPI_CLK, GPIO_FUNC_SPI);
    gpio_set_function(PGPIO_SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(PGPIO_SPI_RX, GPIO_FUNC_SPI);

    // Left stick initialize
    gpio_init(PGPIO_LS_CS);
    gpio_set_dir(PGPIO_LS_CS, GPIO_OUT);
    gpio_put(PGPIO_LS_CS, true); // active low

    // Right stick initialize
    gpio_init(PGPIO_RS_CS);
    gpio_set_dir(PGPIO_RS_CS, GPIO_OUT);
    gpio_put(PGPIO_RS_CS, true); // active low

    // IMU 0 initialize
    gpio_init(PGPIO_IMU0_CS);
    gpio_set_dir(PGPIO_IMU0_CS, GPIO_OUT);
    gpio_put(PGPIO_IMU0_CS, true); // active low

    // IMU 1 initialize
    gpio_init(PGPIO_IMU1_CS);
    gpio_set_dir(PGPIO_IMU1_CS, GPIO_OUT);
    gpio_put(PGPIO_IMU1_CS, true); // active low

    app_imu_init();
}

void cb_hoja_read_buttons(button_data_s *data)
{
    // Keypad version
    gpio_put(PGPIO_SCAN_A, false);
    sleep_us(5);
    data->button_a  = !gpio_get(PGPIO_PUSH_C);
    data->button_b  = !gpio_get(PGPIO_PUSH_D);
    data->button_x  = !gpio_get(PGPIO_PUSH_A);
    data->button_y  = !gpio_get(PGPIO_PUSH_B);
    gpio_put(PGPIO_SCAN_A, true);

    gpio_put(PGPIO_SCAN_B, false);
    sleep_us(5);
    data->dpad_left     = !gpio_get(PGPIO_PUSH_D);
    data->dpad_right    = !gpio_get(PGPIO_PUSH_C);
    data->dpad_down     = !gpio_get(PGPIO_PUSH_B);
    data->dpad_up       = !gpio_get(PGPIO_PUSH_A);
    gpio_put(PGPIO_SCAN_B, true);

    gpio_put(PGPIO_SCAN_C, false);
    sleep_us(5);
    data->button_plus       = !gpio_get(PGPIO_PUSH_A);
    data->button_home       = !gpio_get(PGPIO_PUSH_B);
    data->button_capture    = !gpio_get(PGPIO_PUSH_D);
    data->button_minus      = !gpio_get(PGPIO_PUSH_C);
    gpio_put(PGPIO_SCAN_C, true);

    /*
    if (data->button_capture)
    {
        reset_usb_boot(0, 0);
    }
    if (data->button_home)
    {
        watchdog_reboot(0, 0, 0);
    }*/

    gpio_put(PGPIO_SCAN_D, false);
    sleep_us(5);
    data->trigger_r     = !gpio_get(PGPIO_PUSH_B);
    data->trigger_l     = !gpio_get(PGPIO_PUSH_D);
    data->trigger_zl    = !gpio_get(PGPIO_PUSH_A);
    data->trigger_zr    = !gpio_get(PGPIO_PUSH_C);
    gpio_put(PGPIO_SCAN_D, true);

    data->button_stick_right = !gpio_get(PGPIO_BUTTON_RS);
    data->button_stick_left = !gpio_get(PGPIO_BUTTON_LS);

    data->button_safemode = !gpio_get(PGPIO_BUTTON_MODE);
}

void cb_hoja_read_analog(a_data_s *data)
{
    // Set up buffers for each axis
    uint8_t buffer_lx[3] = {0};
    uint8_t buffer_ly[3] = {0};
    uint8_t buffer_rx[3] = {0};
    uint8_t buffer_ry[3] = {0};

    // CS left stick ADC
    gpio_put(PGPIO_LS_CS, false);
    // Read first axis for left stick
    spi_read_blocking(spi0, X_AXIS_CONFIG, buffer_lx, 3);

    // CS left stick ADC reset
    gpio_put(PGPIO_LS_CS, true);
    gpio_put(PGPIO_LS_CS, false);

    // Set up and read axis for left stick Y  axis
    spi_read_blocking(spi0, Y_AXIS_CONFIG, buffer_ly, 3);

    // CS right stick ADC
    gpio_put(PGPIO_LS_CS, true);
    gpio_put(PGPIO_RS_CS, false);

    spi_read_blocking(spi0, Y_AXIS_CONFIG, buffer_ry, 3);

    // CS right stick ADC reset
    gpio_put(PGPIO_RS_CS, true);
    gpio_put(PGPIO_RS_CS, false);

    spi_read_blocking(spi0, X_AXIS_CONFIG, buffer_rx, 3);

    // Release right stick CS ADC
    gpio_put(PGPIO_RS_CS, true);

    // Convert data
    data->lx = BUFFER_TO_UINT16(buffer_lx);
    data->ly = BUFFER_TO_UINT16(buffer_ly);
    data->rx = BUFFER_TO_UINT16(buffer_rx);
    data->ry = BUFFER_TO_UINT16(buffer_ry);
}

void cb_hoja_task_1_hook(uint32_t timestamp)
{
    app_rumble_task(timestamp);
}

int main()
{
    stdio_init_all();
    sleep_ms(100);

    printf("ProGCC Started.\n");

    hoja_setup_gpio_button(PGPIO_BUTTON_MODE);
    // Handle bootloader stuff
    if (!gpio_get(PGPIO_BUTTON_MODE))
    {
        reset_usb_boot(0, 0);
    }

    hoja_init();
}
