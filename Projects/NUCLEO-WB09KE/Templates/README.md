## <b>Templates Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application.
- This project is targeted to run on STM32WB09 device on NUCLEO-WB09KE board from STMicroelectronics.  
- The reference template project configures the maximum system clock frequency at 64Mhz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

Reference, Template

### <b>Directory contents</b>

  - Templates/Src/main.c                   Main program
  - Templates/Src/system_stm32wb0x.c       STM32WB0x system clock configuration file
  - Templates/Src/stm32wb0x_it.c           Interrupt handlers
  - Templates/Src/stm32wb0x_hal_msp.c      HAL MSP module
  - Templates/Inc/main.h                   Main program header file
  - Templates/Inc/stm32wb0x_nucleo_conf.h  BSP Configuration file
  - Templates/Inc/stm32wb0x_hal_conf.h     HAL Configuration file
  - Templates/Inc/stm32wb0x_it.h           Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This template runs on STM32WB09 devices.
  - This template has been tested with STMicroelectronics NUCLEO-WB09KE board and can be easily tailored
    to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


:::
:::

