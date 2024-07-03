## <b>PWR_PVD Example Description</b>

How to configure the programmable voltage detector by using an external interrupt 
line. External DC supply must be used to supply Vdd.

In this example, EXTI line 0 is configured to generate an interrupt on each rising
or falling edge of the PVD output signal (which indicates that the Vdd voltage is
moving below or above the PVD threshold).

As long as the voltage is above the target threshold (2.5V), LD2 is blinking with a 200 ms-period; when the voltage drops
below the threshold, LD2 stops blinking and remains constantly on (or appears to be turned off if the voltage is getting really low);
when the voltage moves back above the target threshold, LD2 is on.

#### <b>Notes</b> 

1. PVD thresholds have an hysteresis between rising and falling edges
      of approximately 100mV. For Min/Typ/Max values of thresholds and hysteresis,
      refer to device datasheet.

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

Power, PWR, EXTI, PVD, Interrupt, Wakeup, External reset

### <b>Directory contents</b> 

  - PWR/PWR_PVD PWR Programmable Voltage Detector (PVD)/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - PWR/PWR_PVD/Inc/stm32wb0x_hal_conf.h     HAL Configuration file
  - PWR/PWR_PVD/Inc/stm32wb0x_it.h           Header for stm32wb0x_it.c
  - PWR/PWR_PVD/Inc/main.h                         Header file for main.c
  - PWR/PWR_PVD/Src/system_stm32wb0x.c       STM32WB0x system clock configuration file
  - PWR/PWR_PVD/Src/stm32wb0x_it.c           Interrupt handlers
  - PWR/PWR_PVD/Src/stm32wb0x_hal_msp.c      HAL MSP module
  - PWR/PWR_PVD/Src/main.c                         Main program

### <b>Hardware and Software environment</b>

  - This example runs on STM32WB0xx devices
    
  - This example has been tested with STMicroelectronics NUCLEO-WB09KE
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WB09KE Set-up :
  
    - Jumper JP1 has to be on VEXT position.
    - Connect V+ to Morpho connector CN3 pins 5 "VDD".
    - Connect GND to Arduino connector CN5 pin 6 or 7.
    - LD2 (GREEN) connected to PB.04 pin indicates the behavior of the test software as explained above.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 
 - Rebuild all files and load your image into target memory
 
 - Run the example
