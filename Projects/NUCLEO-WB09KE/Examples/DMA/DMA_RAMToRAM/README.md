## <b>DMA_RAMToRAM Example Description</b>

How to use a DMA to transfer a word data buffer from embedded SRAM to embedded SRAM through the HAL API.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals and the systick.

DMA1_Channel1 is configured to transfer the contents of a 32-word data 
buffer stored in RAM to the reception buffer declared in RAM.

The start of transfer is triggered by software. DMA1_Channel1 memory-to-memory
transfer is enabled. Source and destination addresses incrementing is also enabled.

The transfer is started by setting the channel enable bit for DMA1_Channel1.

At the end of the transfer a Transfer Complete interrupt is generated since it
is enabled and the callback function (customized by user) is called.

STM32WB09KEVx board's LEDs can be used to monitor the transfer status:

 - LD2 is ON when the transfer is complete (into the Transfer Complete interrupt routine).

 - LD3 is ON when a Error_Handler is called

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

System, DMA, Data Transfer, Memory to memory, Stream, RAM

### <b>Directory contents</b>

  - DMA/DMA_RAMToRAM/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - DMA/DMA_RAMToRAM/Src/system_stm32wb0x.c           stm32wb0x system source file
  - DMA/DMA_RAMToRAM/Src/stm32wb0x_it.c               Interrupt handlers
  - DMA/DMA_RAMToRAM/Src/main.c                             Main program
  - DMA/DMA_RAMToRAM/Inc/stm32wb0x_hal_conf.h         HAL Configuration file
  - DMA/DMA_RAMToRAM/Inc/stm32wb0x_it.h               Interrupt handlers header file
  - DMA/DMA_RAMToRAM/Inc/main.h                             Main program header file
  - DMA/DMA_RAMToRAM/Src/stm32wb0x_hal_msp.c          HAL MSP module

     
### <b>Hardware and Software environment</b>

  - This example runs on STM32WB0x Devices.

  - This example has been tested with STMicroelectronics NUCLEO-WB09KE 
    board and can be easily tailored to any other supported device 
    and development board.    

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
