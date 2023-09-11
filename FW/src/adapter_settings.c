#include "adapter_settings.h"

adapter_settings_s global_loaded_settings = {0};

// Internal functions for command processing
void _generate_mac()
{
  printf("Generated MAC: ");
  for(uint8_t i = 0; i < 6; i++)
  {
    global_loaded_settings.switch_mac_address[0][i] = get_rand_32() & 0xFF;
    global_loaded_settings.switch_mac_address[1][i] = get_rand_32() & 0xFF;
    global_loaded_settings.switch_mac_address[2][i] = get_rand_32() & 0xFF;
    global_loaded_settings.switch_mac_address[3][i] = get_rand_32() & 0xFF;
  }
  printf("\n");
}
#define FLASH_TARGET_OFFSET (1200 * 1024)

// Returns true if loaded ok
// returns false if no settings and reset to default
bool settings_load()
{
  static_assert(sizeof(adapter_settings_s) <= FLASH_SECTOR_SIZE);
      const uint8_t *target_read = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET + (FLASH_SECTOR_SIZE));
  memcpy(&global_loaded_settings, target_read, sizeof(adapter_settings_s));

  // Check that the version matches, otherwise reset to default and save.
  if(global_loaded_settings.settings_version != ADAPTER_SETTINGS_VERSION)
  {
    printf("Settings version does not match. Resetting... \n");
    settings_reset_to_default();
    settings_save();
    return false;
  }

  return true;
}

void settings_reset_to_default()
{
  const adapter_settings_s set = {
    .settings_version = ADAPTER_SETTINGS_VERSION,
    .input_mode = INPUT_MODE_SWPRO,
  };
  memcpy(&global_loaded_settings, &set, sizeof(adapter_settings_s));
  _generate_mac();
}

volatile bool _save_flag = false;
volatile bool _webusb_indicate = false;

void settings_core1_save_check()
{
  if (_save_flag)
  {
    multicore_lockout_start_blocking();
    // Check that we are less than our flash sector size
    static_assert(sizeof(adapter_settings_s) <= FLASH_SECTOR_SIZE);

    // Store interrupts status and disable
    uint32_t ints = save_and_disable_interrupts();

    // Calculate storage bank address via index
    uint32_t memoryAddress = FLASH_TARGET_OFFSET + (FLASH_SECTOR_SIZE);

    // Create blank page data
    uint8_t page[FLASH_SECTOR_SIZE] = {0x00};
    // Copy settings into our page buffer
    memcpy(page, &global_loaded_settings, sizeof(adapter_settings_s));

    // Erase the settings flash sector
    flash_range_erase(memoryAddress, FLASH_SECTOR_SIZE);

    // Program the flash sector with our page
    flash_range_program(memoryAddress, page, FLASH_SECTOR_SIZE);

    // Restore interrups
    restore_interrupts(ints);
    multicore_lockout_end_blocking();

    // Indicate change
    if (_webusb_indicate)
    {
      webusb_save_confirm();
      _webusb_indicate = false;
    }
    _save_flag = false;
  }
}

void settings_core0_save_check()
{
  if (_save_flag)
  {
    //multicore_lockout_start_blocking();
    // Check that we are less than our flash sector size
    static_assert(sizeof(adapter_settings_s) <= FLASH_SECTOR_SIZE);

    // Store interrupts status and disable
    uint32_t ints = save_and_disable_interrupts();

    // Calculate storage bank address via index
    uint32_t memoryAddress = FLASH_TARGET_OFFSET + (FLASH_SECTOR_SIZE);

    // Create blank page data
    uint8_t page[FLASH_SECTOR_SIZE] = {0x00};
    // Copy settings into our page buffer
    memcpy(page, &global_loaded_settings, sizeof(adapter_settings_s));

    // Erase the settings flash sector
    flash_range_erase(memoryAddress, FLASH_SECTOR_SIZE);

    // Program the flash sector with our page
    flash_range_program(memoryAddress, page, FLASH_SECTOR_SIZE);

    // Restore interrups
    restore_interrupts(ints);
    //multicore_lockout_end_blocking();

    // Indicate change
    if (_webusb_indicate)
    {
      webusb_save_confirm();
      _webusb_indicate = false;
    }
    _save_flag = false;
  }
}

void settings_save_webindicate()
{
  _webusb_indicate = true;
}

void settings_save()
{
  _save_flag = true;
}

void settings_set_mode(input_mode_t mode)
{
  global_loaded_settings.input_mode = mode;
}
