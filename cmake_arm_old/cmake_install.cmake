# Install script for directory: C:/GitProjects/STM32F103Micro/Cellar

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Cellar")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/cmsis_os.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/FreeRTOS.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/FreeRTOSConfig.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/StackMacros.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/croutine.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/deprecated_definitions.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/event_groups.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/list.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/message_buffer.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/mpu_prototypes.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/mpu_wrappers.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/portable.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/portmacro.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/projdefs.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/queue.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/semphr.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/stack_macros.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/stream_buffer.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/task.h;C:/GitProjects/STM32F103Micro/Cellar/lib/freertos/timers.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/GitProjects/STM32F103Micro/Cellar/lib/freertos" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/cmsis_os/cmsis_os.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/FreeRTOS.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/FreeRTOSConfig.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/StackMacros.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/croutine.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/deprecated_definitions.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/event_groups.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/list.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/message_buffer.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/mpu_prototypes.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/mpu_wrappers.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/portable.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/portable/portmacro.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/projdefs.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/queue.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/semphr.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/stack_macros.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/stream_buffer.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/task.h"
    "C:/GitProjects/STM32F103Micro/Cellar/freertos/inc/timers.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f100xb.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f100xe.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f101x6.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f101xb.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f101xe.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f101xg.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f102x6.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f102xb.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f103x6.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f103xb.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f103xe.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f103xg.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f105xc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f107xc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/system_stm32f1xx.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/arm_common_tables.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/arm_const_structs.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/arm_math.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/cmsis_armcc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/cmsis_armcc_V6.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/cmsis_gcc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_cm0.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_cm0plus.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_cm3.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_cm4.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_cm7.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_cmFunc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_cmInstr.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_cmSimd.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_sc000.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/core_sc300.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32_assert_template.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_adc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_adc_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_can.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_can_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_cec.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_conf_template.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_cortex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_crc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_dac.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_dac_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_def.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_dma.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_dma_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_eth.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_flash.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_flash_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_gpio.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_gpio_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_hcd.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_i2c.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_i2s.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_irda.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_iwdg.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_mmc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_nand.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_nor.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_pccard.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_pcd.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_pcd_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_pwr.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_rcc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_rcc_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_rtc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_rtc_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_sd.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_smartcard.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_spi.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_sram.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_tim.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_tim_ex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_uart.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_usart.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_wwdg.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_adc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_bus.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_cortex.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_crc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_dac.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_dma.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_exti.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_fsmc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_gpio.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_i2c.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_iwdg.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_pwr.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_rcc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_rtc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_sdmmc.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_spi.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_system.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_tim.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_usart.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_usb.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_utils.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_ll_wwdg.h;C:/GitProjects/STM32F103Micro/Cellar/lib/stm32/stm32f1xx_hal_conf.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/GitProjects/STM32F103Micro/Cellar/lib/stm32" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f100xb.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f100xe.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f101x6.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f101xb.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f101xe.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f101xg.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f102x6.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f102xb.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103x6.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103xb.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103xe.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103xg.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f105xc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f107xc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Device/ST/STM32F1xx/Include/system_stm32f1xx.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/arm_common_tables.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/arm_const_structs.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/arm_math.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/cmsis_armcc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/cmsis_armcc_V6.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/cmsis_gcc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_cm0.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_cm0plus.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_cm3.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_cm4.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_cm7.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_cmFunc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_cmInstr.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_cmSimd.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_sc000.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/CMSIS/Include/core_sc300.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32_assert_template.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_adc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_adc_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_can.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_can_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_cec.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_conf_template.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_cortex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_crc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_dac.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_dac_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_def.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_dma.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_dma_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_eth.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_flash.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_flash_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_gpio.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_gpio_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_hcd.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_i2c.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_i2s.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_irda.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_iwdg.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_mmc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_nand.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_nor.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_pccard.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_pcd.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_pcd_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_pwr.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_rcc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_rcc_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_rtc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_rtc_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_sd.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_smartcard.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_spi.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_sram.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_tim.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_tim_ex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_uart.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_usart.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_wwdg.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_adc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_bus.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_cortex.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_crc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_dac.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_dma.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_exti.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_fsmc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_gpio.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_i2c.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_iwdg.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_pwr.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_rcc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_rtc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_sdmmc.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_spi.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_system.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_tim.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_usart.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_usb.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_utils.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_ll_wwdg.h"
    "C:/GitProjects/STM32F103Micro/Cellar/Drivers/stm32f1xx_hal_conf.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/GitProjects/STM32F103Micro/Cellar/lib/libfreertos_lib.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/GitProjects/STM32F103Micro/Cellar/lib" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES "C:/GitProjects/STM32F103Micro/Cellar/cmake_arm/libfreertos_lib.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/GitProjects/STM32F103Micro/Cellar/lib/libstm32f1_hal_lib.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/GitProjects/STM32F103Micro/Cellar/lib" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES "C:/GitProjects/STM32F103Micro/Cellar/cmake_arm/libstm32f1_hal_lib.a")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/GitProjects/STM32F103Micro/Cellar/cmake_arm/freertos/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/GitProjects/STM32F103Micro/Cellar/cmake_arm/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
