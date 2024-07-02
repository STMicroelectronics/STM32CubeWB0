## <b>RCC_LSEConfig Example Description</b>

Enabling/disabling of the low-speed external(LSE) RC oscillator (about 32 KHz) at run time, using the RCC HAL API.

   - Board: NUCLEO-WB07CC (embeds a STM32WB07CCVx device)
   - Microcontroller Clock Output LCO Pin: PA.04 (Arduino connector CN6 PIN 3 D11,Morpho connector CN4 pin 17).

In this example, after startup SYSCLK is configured to the max frequency using
PLL as clock source, the USER push-button (connected to B1_PIN)
is used to enable/disable LSE:
Each time the USER push-button is pressed, B1_PIN interrupt is generated and the ISR
enables the LSE when it is off, disables it when it is on.

- when LSE is off, LD1 is toggling (every 300 ms)
- when LSE is on, LD2 is toggling (every 300 ms)

LSE clock waveform is available on the LCO pin PA.04(Arduino connector CN6 PIN 3 D11,Morpho connector CN4 pin 17) and can be captured
on an oscilloscope.


In case of configuration error, LD3 is On.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)
    
### <b>Keywords</b>

System, RCC, PLL, PLLCLK, SYSCLK, LSE, Clock, Oscillator,

### <b>Directory contents</b>

  - RCC/RCC_LSEConfig/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - RCC/RCC_LSEConfig/Inc/stm32wb0x_hal_conf.h    HAL configuration file
  - RCC/RCC_LSEConfig/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - RCC/RCC_LSEConfig/Inc/main.h                        Header for main.c module
  - RCC/RCC_LSEConfig/Src/stm32wb0x_it.c          Interrupt handlers
  - RCC/RCC_LSEConfig/Src/main.c                        Main program
  - RCC/RCC_LSEConfig/Src/system_stm32wb0x.c      STM32WB0x system source file
  - RCC/RCC_LSEConfig/Src/stm32wb0x_hal_msp.c     HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32WB0x devices.

  - This example has been tested with NUCLEO-WB07CC
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

