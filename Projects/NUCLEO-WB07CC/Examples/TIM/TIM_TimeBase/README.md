## <b>TIM_TimeBase Example Description</b>

This example shows how to configure the TIM peripheral to generate a time base of 
one second with the corresponding Interrupt request.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32WB07CCVx Devices :
The CPU at 64MHz 

    In this example TIM1 input clock (TIM1CLK)  is set to APB0 clock (PCLK1),
    since APB0 prescaler is equal to 1.
      TIM1CLK = PCLK1
      PCLK1 = HCLK
      => TIM1CLK = HCLK = SystemCoreClock
    To get TIM1 counter clock at 10 KHz, the Prescaler is computed as following:
    Prescaler = (TIM1CLK / TIM1 counter clock) - 1
    Prescaler = (SystemCoreClock /10 KHz) - 1

SystemCoreClock is set to 64MHz for STM32WB0x Devices.

The TIM1 ARR register value is equal to 10000 - 1

Update rate = TIM1 counter clock / (Period + 1) = 1 Hz

So the TIM1 generates an interrupt each 1 s

When the counter value reaches the auto-reload register value, the TIM update 
interrupt is generated and, in the handler routine, pin PB.0 (connected to LD1 on board NUCLEO-WB07CC)
is toggled. So led blinks at the following frequency: 0.5Hz.

In case of error, LD3 is turned ON.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)
    
### <b>Keywords</b>

Timer, TIM, time base , Interrupt, Clock source

### <b>Directory contents</b>

  - TIM/TIM_TimeBase/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - TIM/TIM_TimeBase/Inc/stm32wb0x_hal_conf.h    HAL configuration file
  - TIM/TIM_TimeBase/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - TIM/TIM_TimeBase/Inc/main.h                  Header for main.c module  
  - TIM/TIM_TimeBase/Src/stm32wb0x_it.c          Interrupt handlers
  - TIM/TIM_TimeBase/Src/main.c                  Main program
  - TIM/TIM_TimeBase/Src/stm32wb0x_hal_msp.c     HAL MSP file
  - TIM/TIM_TimeBase/Src/system_stm32wb0x.c      STM32WB0x system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WB07CCVx devices.
  - In this example, the clock is set to 64MHz.
    
  - This example has been tested with STMicroelectronics NUCLEO-WB07CC
    board and can be easily tailored to any other supported device
    and development board.      

  - NUCLEO-WB07CC Set-up
    - Use LD1 connected to PB.0 pin (pin 6 in CN4 connector) and connect it on an oscilloscope 
      to show the time base signal.  


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


