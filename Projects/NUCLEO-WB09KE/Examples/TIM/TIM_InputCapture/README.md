## <b>TIM_InputCapture Example Description</b>

How to use the TIM peripheral to measure an external signal frequency.
 
  At the beginning of the main program the HAL_Init() function is called to reset 
  all the peripherals, initialize the Flash interface and the systick.
  The SystemClock_Config() function is used to configure the system clock for STM32WB09KEVx Devices :
  The CPU at 64MHz 

  The TIM2CLK frequency is set to SystemCoreClock (Hz), the Prescaler is 0,
  so the TIM2 counter clock is SystemCoreClock (Hz).

  SystemCoreClock is set to 64MHz for STM32WB09KEVx Devices.

  TIM2 is configured in Input Capture Mode: the external signal is connected to 
  TIM2 Channel2 used as input pin.
  To measure the frequency we use the TIM2 CC interrupt request, so in the 
  TIM2 CC IRQHandler routine, the frequency of the external signal is computed.

  The "uwFrequency" variable contains the external signal frequency:

  uwFrequency = TIM2 counter clock / uwDiffCapture (Hz),
  where "uwDiffCapture" is the difference between two consecutive TIM2 captures.

  The minimum frequency value to measure is TIM2 counter clock / CCR MAX

                                              = (SystemCoreClock) / 65535

  Due to TIM2 CC IRQHandler processing time (around 4ns), the maximum
  frequency value to measure is around 400kHz.

LD3 is ON when there are an error.
  
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

Timer, TIM, DMA, Frequency, Input, Capture, External Signal, Measurement

### <b>Directory contents</b>

  - TIM/TIM_InputCapture/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - TIM/TIM_InputCapture/Inc/stm32wb0x_hal_conf.h    HAL configuration file
  - TIM/TIM_InputCapture/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - TIM/TIM_InputCapture/Inc/main.h                  Header for main.c module
  - TIM/TIM_InputCapture/Src/stm32wb0x_it.c          Interrupt handlers
  - TIM/TIM_InputCapture/Src/main.c                  Main program
  - TIM/TIM_InputCapture/Src/stm32wb0x_hal_msp.c     HAL MSP file
  - TIM/TIM_InputCapture/Src/system_stm32wb0x.c      STM32WB0x system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WB09KEVx devices.
  - In this example, the clock is set to 64MHz.
    
  - This example has been tested with STMicroelectronics NUCLEO-WB09KE
    board and can be easily tailored to any other supported device 
    and development board.      

  - NUCLEO-WB09KE Set-up
    - Connect the external signal to measure to the TIM2 CH2 pin (PB.7 (pin 5 in CN4 connector)).

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

