cmake_minimum_required(VERSION 3.13)

# Set the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(gc_adapter_rp2040 C CXX ASM)

pico_sdk_init()

include_directories(
	include
  include/devices
  include/generated
  include/switch
  include/usb
  include/utilities
)

add_executable(${PROJECT_NAME}
  src/main.c 

  src/utilities/rgb.c 

  src/usb/desc_bos.c 
  src/usb/adapter_usb.c 
  src/adapter_defines.c 
  src/adapter_rumble.c 
  src/adapter_settings.c 
  

  src/usb/xinput_device.c 

  src/switch/switch_analog.c 
  src/switch/switch_commands.c 
  src/switch/switch_spi.c 

  src/devices/gcinput.c 
  src/devices/gamecube.c 
  src/devices/swpro.c 
  src/devices/webusb.c 
  src/devices/xinput.c 
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
	PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
	PICO_DEFAULT_UART_TX_PIN=12
	PICO_DEFAULT_UART_RX_PIN=13
	#PICO_DEFAULT_BOOT_STAGE2=boot2_generic_03h
	#PICO_FLASH_SPI_CLKDIV=8
	)

file(MAKE_DIRECTORY /include/generated)
pico_generate_pio_header(${PROJECT_NAME} /pio/ws2812.pio OUTPUT_DIR /include/generated)

#add_custom_command(OUTPUT /generated/cvideo.pio.h
#	DEPENDS /cvideo.pio
#	COMMAND Pioasm /cvideo.pio /generated/cvideo.pio.h
#	)

# Pull in pico libraries that we need
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  pico_stdlib
  pico_bootrom
  pico_multicore
  pico_platform
  pico_rand

  hardware_gpio
  hardware_adc
  hardware_flash
  hardware_pio
  hardware_dma
  hardware_pwm
  hardware_spi
  hardware_sync
  hardware_timer
  hardware_irq

  tinyusb_device
  tinyusb_board
)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})

