cmake_minimum_required(VERSION 3.22)

project(stm32cubemx)
add_library(stm32cubemx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32cubemx INTERFACE 
	USE_HAL_DRIVER 
	STM32L412xx
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ../../X-CUBE-MEMS1/Target
    ../../X-CUBE-TOF1/Target
    ../../USB_DEVICE/App
    ../../USB_DEVICE/Target
    ../../Core/Inc
    ../../Drivers/STM32L4xx_HAL_Driver/Inc
    ../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
    ../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ../../Drivers/CMSIS/Device/ST/STM32L4xx/Include
    ../../Drivers/CMSIS/Include
    ../../Drivers/BSP/Components/vl53l1cb/modules
    ../../Drivers/BSP/Components/vl53l1cb/porting
    ../../Drivers/BSP/Components/vl53l1cb
    ../../Drivers/BSP/Components/iis2mdc
)

target_sources(stm32cubemx INTERFACE
    ../../Core/Src/main.c
    ../../Core/Src/custom_bus.c
    ../../Core/Src/stm32l4xx_it.c
    ../../Core/Src/stm32l4xx_hal_msp.c
    ../../USB_DEVICE/App/usb_device.c
    ../../USB_DEVICE/App/usbd_desc.c
    ../../USB_DEVICE/App/usbd_cdc_if.c
    ../../USB_DEVICE/Target/usbd_conf.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_api.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_api_calibration.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_api_core.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_api_debug.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_api_preset_modes.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_api_strings.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_core.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_core_support.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_dmax.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_error_strings.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_hist_algos_gen3.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_hist_algos_gen4.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_hist_char.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_hist_core.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_hist_funcs.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_nvm.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_nvm_debug.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_register_funcs.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_sigma_estimate.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_silicon_core.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_wait.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_xtalk.c
    ../../Drivers/BSP/Components/vl53l1cb/modules/vl53l1_zone_presets.c
    ../../Drivers/BSP/Components/vl53l1cb/porting/vl53l1_platform.c
    ../../Drivers/BSP/Components/vl53l1cb/porting/vl53l1_platform_ipp.c
    ../../Drivers/BSP/Components/vl53l1cb/porting/vl53l1_platform_log.c
    ../../Drivers/BSP/Components/vl53l1cb/vl53l1cb.c
    ../../Drivers/BSP/Components/iis2mdc/iis2mdc_reg.c
    ../../Drivers/BSP/Components/iis2mdc/iis2mdc.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c
    ../../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c
    ../../Core/Src/system_stm32l4xx.c
    ../../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ../../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    ../../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
    ../../Core/Src/sysmem.c
    ../../Core/Src/syscalls.c
    ../../startup_stm32l412xx.s
)

target_link_directories(stm32cubemx INTERFACE
)

target_link_libraries(stm32cubemx INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


