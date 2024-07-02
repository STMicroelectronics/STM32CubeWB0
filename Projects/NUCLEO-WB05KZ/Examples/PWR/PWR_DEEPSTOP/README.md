##  <b>PWR_Deepstop Example Description</b>

How to enter the Deepstop mode and wake up from this mode by using an external reset or the WKUP pin.

In the associated software, the system clock is set to 64 MHz, an EXTI line
is connected to the pin PA0  and configured to generate an
interrupt on falling edge.

The SysTick is programmed to generate an interrupt each 1 ms and in the SysTick
interrupt handler, LD1 is toggled in order to indicate whether the MCU is in Deepstop or Run mode.

When a falling edge is detected on the EXTI line, an interrupt is generated and the system wakes up
the program then checks and clears the Deepstop flag.

After clearing the Deepstop flag, the software enables wake-up  PWR_FLAG_WUF12  connected to PA0 , then
the corresponding flag indicating that a wakeup event was received from the PWR_FLAG_WUF12  is cleared.

Finally, the system enters again Deepstop mode causing LD1 to stop toggling.

A falling edge on WKUP pin will wake-up the system from Deepstop.
Alternatively, an external RESET of the board will lead to a system wake-up as well.The application needs to ensure that the SysTick

After wake-up from Deepstop mode, program execution restarts in the same way as after
a RESET and LD1 restarts toggling.

Two leds LD1 and LD3 are used to monitor the system state as follows:

 - LD3 ON: configuration failed (system will go to an infinite loop)
 - LD1 toggling: system in Run mode
 - LD1 off : system in Deepstop mode
 
#### <b>Notes</b>

   1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

   2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

 3. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)
    
### <b>Keywords</b>

Power, PWR, Deepstop mode, Interrupt, Wakeup, Low Power, External reset

### <b>Directory contents</b>

  - PWR/PWR_Deepstop/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - PWR/PWR_Deepstop/Inc/stm32wb0x_conf.h         HAL Configuration file
  - PWR/PWR_Deepstop/Inc/stm32wb0x_it.h           Header for stm32wb0x_it.c
  - PWR/PWR_Deepstop/Inc/main.h                         Header file for main.c
  - PWR/PWR_Deepstop/Src/system_stm32wb0x.c       STM32WB0x system clock configuration file
  - PWR/PWR_Deepstop/Src/stm32wb0x_it.c           Interrupt handlers
  - PWR/PWR_Deepstop/Src/main.c                         Main program
  - PWR/PWR_Deepstop/Src/stm32wb0x_hal_msp.c      HAL MSP module

### <b>Hardware and Software environment</b> 

  - This example runs on STM32WB05KZVx devices

  - This example has been tested with STMicroelectronics NUCLEO-WB05KZ
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WB05KZ Set-up
    - Use LD1 and LD3 connected respectively to PB.01 and PB.02 pins
    - USER push-button connected to pin PA.00 (B1_PIN)
    - WakeUp flag PWR_FLAG_WUF12 connected to PA.00

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
