## <b>UTILS_ConfigureSystemClock Example Description</b>

Use of UTILS LL API to configure the system clock using PLL with HSE as source clock. 

The user application just needs to calculate the PLL parameters using STM32CubeMX and call the UTILS LL API.

Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 64MHz.

System starts with clock used after reset.
Then, a system clock switch is done to PLL with HSE as PLL clock source. Automatically, FLASH latency
is tuned according to system constraints described in the reference manual.

User can easily set its own PLL parameters in changing global variable used to store them.

A LD1 toggle of 1sec provides this information that system is well configured to requested frequency.  

Anyway, user can connect on oscilloscope to MCO pin (PA.11) to check requested frequency:
 
 - SYSCLK frequency with frequency value around 32MHz.

#### <b>Notes</b>
                                            
 1. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)

### <b>Keywords</b>

Utils, system, Clock, HSE, PLL, flash latency, SYSCLK, frequency Oscilloscope



### <b>Directory contents</b>

  - UTILS/UTILS_ConfigureSystemClock/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - UTILS/UTILS_ConfigureSystemClock/Inc/main.h                  Header for main.c module
  - UTILS/UTILS_ConfigureSystemClock/Inc/stm32_assert.h          Template file to include assert_failed function
  - UTILS/UTILS_ConfigureSystemClock/Src/stm32wb0x_it.c          Interrupt handlers
  - UTILS/UTILS_ConfigureSystemClock/Src/main.c                  Main program
  - UTILS/UTILS_ConfigureSystemClock/Src/system_stm32wb0x.c      STM32WB0x system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WB07CCVx devices.
    
  - This example has been tested with NUCLEO-WB07CC board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WB07CC Set-up
    - Connect the MCO pin to an oscilloscope to monitor the different waveforms:
      - PA.11: Arduino connector CN8 pin D6, Morpho connector CN4 pin 25 (MB1801D)

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


