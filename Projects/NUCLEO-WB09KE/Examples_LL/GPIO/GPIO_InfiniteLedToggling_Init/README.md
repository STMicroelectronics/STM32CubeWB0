## <b>GPIO_InfiniteLedToggling_Init Example Description</b>

How to configure and use GPIOs to toggle the on-board user LEDs
every 250 ms. This example is based on the STM32WB0x LL API. The peripheral
is initialized with LL initialization function to demonstrate LL init usage.

PB.01 IO (configured in output pushpull mode) toggles in a forever loop.
On NUCLEO-WB09KE board this IO is connected to LD1.

In this example, HCLK is configured at 64 MHz.

#### <b>Notes</b>
                                            
 1. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)

### <b>Keywords</b>

System, GPIO, Output, No pull, Speed, Push-pull, Toggle

### <b>Directory contents</b>

  - GPIO/GPIO_InfiniteLedToggling_Init/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - GPIO/GPIO_InfiniteLedToggling_Init/Inc/main.h                  Header for main.c module
  - GPIO/GPIO_InfiniteLedToggling_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - GPIO/GPIO_InfiniteLedToggling_Init/Src/stm32wb0x_it.c          Interrupt handlers
  - GPIO/GPIO_InfiniteLedToggling_Init/Src/main.c                  Main program
  - GPIO/GPIO_InfiniteLedToggling_Init/Src/system_stm32wb0x.c      STM32WB0x system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WB09KEVx devices.

  - This example has been tested with NUCLEO-WB09KE board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


