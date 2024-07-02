## <b>PWR_DEEPSTOP_RTC Example Description</b>

How to enter the Deepstop mode and wake-up from this mode by using an external 
reset or the RTC wakeup timer.


In the associated software, the system clock is set to 64 MHz and the SysTick is
programmed to generate an interrupt each 1 ms.

The Low Speed Internal (LSI) clock is used as RTC clock source by default.
RTC_EXTI is internally connected to the RTC Wakeup event.

The system automatically enters Deepstop mode 5 sec. after start-up. The RTC wake-up
is configured to generate an interrupt on rising edge about 30 sec. afterwards.

Current consumption in Deepstop mode with RTC feature enabled can be measured during that time.
More than half a minute is chosen to ensure current convergence to its lowest operating point.

After wake-up from Deepstop mode, program execution restarts in the same way as after
a software RESET.

Two leds LD1 and LD3 are used to monitor the system state as following:

 - LD3 ON: configuration failed (system will go to an infinite loop)
 - LD1 toggling: system in Run mode
 - LD1 off : system in Deepstop mode

These steps are repeated in an infinite loop.


Note: This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M0+ core is no longer clocked during low power mode 
      so debugging features are disabled.

#### <b>Notes</b>

 1. Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select 
    the RTC clock source; in this case the Backup domain will be reset in  
    order to modify the RTC Clock source, as consequence RTC registers (including 
    the backup registers) and RCC_CSR register are set to their reset values.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 4. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)
    
### <b>Keywords</b>

Power, PWR, Deepstop mode, Interrupt, EXTI, Wakeup, Low Power, RTC, External reset, LSI,

### <b>Directory contents</b>

  - PWR/PWR_Deepstop_RTC/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  -  PWR/PWR_Deepstop_RTC/Inc/stm32wb0x_conf.h         HAL Configuration file
  -  PWR/PWR_Deepstop_RTC/stm32wb0x_it.h           Header for stm32wb0x_it.c
  -  PWR/PWR_Deepstop_RTC/Inc/main.h                         Header file for main.c
  -  PWR/PWR_Deepstop_RTC/Src/system_stm32wb0x.c       STM32WB0x system clock configuration file
  -  PWR/PWR_Deepstop_RTC/Src/stm32wb0x_it.c           Interrupt handlers
  -  PWR/PWR_Deepstop_RTC/Src/main.c                         Main program
  -  PWR/PWR_Deepstop_RTC/Src/stm32wb0x_hal_msp.c      HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32WB09KEVx devices

  - This example has been tested with STMicroelectronics NUCLEO-WB09KE
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-WB09KE Set-up
    - Use LD1 and LD3 connected respectively to PB.01 and PB.02 pins

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

