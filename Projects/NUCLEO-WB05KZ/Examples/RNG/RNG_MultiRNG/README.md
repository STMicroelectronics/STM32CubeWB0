## <b>RNG_MultiRNG Example Description</b>
  
Configuration of the RNG using the HAL API. This example uses the RNG to generate 32-bit long random numbers.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 64MHz.

The RNG peripheral configuration is ensured by the HAL_RNG_Init() function.
The latter is calling the HAL_RNG_MspInit() function which implements
the configuration of the needed RNG resources according to the used hardware (CLOCK, 
GPIO, DMA and NVIC). You may update this function to change RNG configuration.

After startup, user is asked to press USER push-button.
The 8-entry array aRandom32bit[] is filled up by 32-bit long random numbers 
at each USER push-button press.

 
The random numbers can be displayed on the debugger in aRandom32bit variable.

In case of error, LD3 is toggling at a frequency of 1Hz.

#### <b>Notes</b>
                                            
 1. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)

### <b>Keywords</b>

Analog, RNG, Random, FIPS PUB 140-2, Analog Random number generator, Entropy, Period

### <b>Directory contents</b> 

  - RNG/RNG_MultiRNG/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - RNG/RNG_MultiRNG/Inc/stm32wb0x_hal_conf.h              HAL configuration file
  - RNG/RNG_MultiRNG/Inc/stm32wb0x_it.h                    Interrupt handlers header file
  - RNG/RNG_MultiRNG/Inc/main.h                                  Header for main.c module
  - RNG/RNG_MultiRNG/Src/stm32wb0x_it.c                    Interrupt handlers
  - RNG/RNG_MultiRNG/Src/main.c                                  Main program
  - RNG/RNG_MultiRNG/Src/stm32wb0x_hal_msp.c               HAL MSP module 
  - RNG/RNG_MultiRNG/Src/system_stm32wb0x.c                STM32WB0x system source file

     
### <b>Hardware and Software environment</b> 

  - This example runs on STM32WB05KZVx devices.
  
  - This example has been tested with NUCLEO-WB05KZ board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following:

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
 

