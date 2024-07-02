## <b>CORTEXM_ModePrivilege Example Description</b>

How to modify the Thread mode privilege access and stack. Thread mode is entered
on reset or when returning from an exception.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 64MHz.

The associated program is used to:

1. Switch the Thread mode stack from Main stack to Process stack

2. Switch the Thread mode from Privileged to Unprivileged

3. Switch the Thread mode from Unprivileged back to Privileged

To monitor the stack used and the privileged or unprivileged access level of code
in Thread mode, a set of variables is available within the program. It is also
possible to use the 'Cortex register' window of the debugger.

LD1 Turns ON when the test is finished successfully.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2.  The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

 3. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)
    
### <b>Keywords</b>

System, Cortex, Mode Privilege, Privileged access, thread mode, main stack, process stack

### <b>Directory contents</b>

  - CORTEX/CORTEXM_ModePrivilege/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - CORTEX/CORTEXM_ModePrivilege/Inc/stm32wb0x_hal_conf.h    HAL configuration file
  - CORTEX/CORTEXM_ModePrivilege/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - CORTEX/CORTEXM_ModePrivilege/Inc/main.h                  Header for main.c module
  - CORTEX/CORTEXM_ModePrivilege/Src/stm32wb0x_it.c          Interrupt handlers
  - CORTEX/CORTEXM_ModePrivilege/Src//stm32wb0x_hal_msp.c     HAL MSP file
  - CORTEX/CORTEXM_ModePrivilege/Src/main.c                  Main program
  - CORTEX/CORTEXM_ModePrivilege/Src/system_stm32wb0x.c      STM32WB0x system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WB09KEVx devices.
    
  - This example has been tested with NUCLEO-WB09KE board and can be
    easily tailored to any other supported device and development board.      

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Open your preferred toolchain 
- Rebuild all files and load your image into target memory
- Run the example

