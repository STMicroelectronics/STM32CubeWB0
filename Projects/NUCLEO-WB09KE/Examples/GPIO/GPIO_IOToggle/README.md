## <b>GPIO_IOToggle Example Description</b>

How to configure and use GPIOs through the HAL API.

PB.01 and PB.04 IOs (configured in output pushpull mode) toggle in a forever loop.
On NUCLEO-WB09KE board these IOs are connected to LD1 and LD2.

In this example, HCLK is configured at 64 MHz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)
    
### <b>Keywords</b>

System, GPIO, Input, Output, Alternate function, Push-pull, Toggle

### <b>Directory contents</b>

  - GPIO/GPIO_IOToggle/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - GPIO/GPIO_IOToggle/Inc/stm32wb0x_hal_conf.h    HAL configuration file
  - GPIO/GPIO_IOToggle/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - GPIO/GPIO_IOToggle/Inc/main.h                  Header for main.c module  
  - GPIO/GPIO_IOToggle/Src/stm32wb0x_it.c          Interrupt handlers
  - GPIO/GPIO_IOToggle/Src/stm32wb0x_hal_msp.c     HAL MSP file
  - GPIO/GPIO_IOToggle/Src/main.c                  Main program
  - GPIO/GPIO_IOToggle/Src/system_stm32wb0x.c      STM32WB0x system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WB09KEVx devices.

  - This example has been tested with NUCLEO-WB09KE board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

