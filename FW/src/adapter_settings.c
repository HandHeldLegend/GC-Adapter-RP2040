#include "adapter_settings.h"

hoja_settings_s global_loaded_settings = {0};

// Internal functions for command processing
void _generate_mac()
{
  printf("Generated MAC: ");
  for(uint8_t i = 0; i < 6; i++)
  {
    global_loaded_settings.switch_mac_address[i] = get_rand_32() & 0xFF;
    printf("%X : ", global_loaded_settings.switch_mac_address[i]);
  }
  printf("\n");
}
#define FLASH_TARGET_OFFSET (1200 * 1024)

// Returns true if loaded ok
// returns false if no settings and reset to default
bool settings_load()
{
  static_assert(sizeof(hoja_settings_s) <= FLASH_SECTOR_SIZE);
      const uint8_t *target_read = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET + (FLASH_SECTOR_SIZE));
  memcpy(&global_loaded_settings, target_read, sizeof(hoja_settings_s));

  // Check that the version matches, otherwise reset to default and save.
  if(global_loaded_settings.settings_version != HOJA_SETTINGS_VERSION)
  {
    printf("Settings version does not match. Resetting... \n");
    settings_reset_to_default();
    settings_save(false);
    return false;
  }

  return true;
}

void settings_reset_to_default()
{
  const hoja_settings_s set = {
    .settings_version = HOJA_SETTINGS_VERSION,
    .input_mode = INPUT_MODE_SWPRO,
    .lx_center = 2048,
    .ly_center = 2048,
    .rx_center = 2048,
    .ry_center = 2048,
    .l_angles = {0,0,0,0,0,0,0,0},
    .r_angles = {0,0,0,0,0,0,0,0},
    .l_angle_distances = {
      600, 600, 600, 600, 600, 600, 600, 600,
    },
    .r_angle_distances = {
      600, 600, 600, 600, 600, 600, 600, 600,
    },
    .rgb_colors = {
      COLOR_RED.color,
      COLOR_ORANGE.color,

      COLOR_YELLOW.color,

      COLOR_GREEN.color,
      COLOR_BLUE.color,
      COLOR_CYAN.color,
      COLOR_PURPLE.color,

      COLOR_RED.color,
      COLOR_GREEN.color,
      COLOR_BLUE.color,
      COLOR_YELLOW.color,
      COLOR_BLUE.color,
    },
  };
  memcpy(&global_loaded_settings, &set, sizeof(hoja_settings_s));
  remap_reset_default(INPUT_MODE_SWPRO);
  remap_reset_default(INPUT_MODE_XINPUT);
  remap_reset_default(INPUT_MODE_GAMECUBE);
  remap_reset_default(INPUT_MODE_N64);
  remap_reset_default(INPUT_MODE_SNES);
  for(uint16_t i = 0; i < 26; i++)
  {
    global_loaded_settings.imu_calibration[i] = 0xFF;
  }
  memset(&global_loaded_settings.imu_offsets[0], 0, 12);
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
    static_assert(sizeof(hoja_settings_s) <= FLASH_SECTOR_SIZE);

    // Store interrupts status and disable
    uint32_t ints = save_and_disable_interrupts();

    // Calculate storage bank address via index
    uint32_t memoryAddress = FLASH_TARGET_OFFSET + (FLASH_SECTOR_SIZE);

    // Create blank page data
    uint8_t page[FLASH_SECTOR_SIZE] = {0x00};
    // Copy settings into our page buffer
    memcpy(page, &global_loaded_settings, sizeof(hoja_settings_s));

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

void settings_save_webindicate()
{
  _webusb_indicate = true;
}

void settings_save()
{
  _save_flag = true;
}

void settings_set_centers(int lx, int ly, int rx, int ry)
{
  global_loaded_settings.lx_center = lx;
  global_loaded_settings.ly_center = ly;
  global_loaded_settings.rx_center = rx;
  global_loaded_settings.ry_center = ry;
}

void settings_set_distances(float *l_angle_distances, float *r_angle_distances)
{
  memcpy(global_loaded_settings.l_angle_distances, l_angle_distances, sizeof(float)*8);
  memcpy(global_loaded_settings.r_angle_distances, r_angle_distances, sizeof(float)*8);
}

void settings_set_angles(float *l_angles, float *r_angles)
{
  memcpy(global_loaded_settings.l_angles, l_angles, sizeof(float)*8);
  memcpy(global_loaded_settings.r_angles, r_angles, sizeof(float)*8);
}

void settings_set_mode(input_mode_t mode)
{
  global_loaded_settings.input_mode = mode;
}

void settings_set_snapback(uint8_t axis, uint8_t level)
{
  
}