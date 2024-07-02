## <b>LPUART_TwoBoards_ComIT Example Description</b>

LPUART transmission (transmit/receive) in Interrupt mode
between two boards.

    Board NUCLEO-WB09KE (embeds a STM32WB09KEVx device):
    - Tx Pin: PB.06 (pin 10 on CN6)
    - Rx Pin: PB.07 (pin 9 on CN6)

Two identical boards are connected as shown on the picture above.

Board 1: transmitting then receiving board
Board 2: receiving then transmitting board

NB: After plugging boards already loaded with the example, it is better to either:

- reset them using the RESET button;
- start again the example using your preferred IDE.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 64Mhz.

The user presses the USER push-button on board 1.
Then data are transmitted through LPUART1 in IT mode. 2nd board has configured LPUART1
to receive data in IT mode. Once data received is completed, 2nd board also
transmit same characters on LPUART1 which will be received by 1st board.
Both boards compare transmitted data to received ones. If they are same,
LD1 is turned on, else LD1 is turned off.

Warning: As both boards do not behave same way, "TRANSMITTER_BOARD" switch
compilation exists to determine either software is for 1st transmitter board or
2nd receiver (then transmitter) board. In other words, 1st board has to be
flashed with software compiled with switch enable, 2nd board has to be flashed
with software compiled with switch disable.

LD1 can be used to monitor the transfer status:

 - LD1 turns ON on master board waiting USER push-button to be pressed.
 - LD1 turns OFF on master board waiting the transmission process complete.
 - LD1 turns ON when the transmission/reception process is correct.
 - If there is an initialization or transfer error, LD1 is slowly blinking (1 sec. period).

The LPUART is configured as follows:

    - BaudRate = 115200 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - None parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

**Note:** When the parity is enabled, the computed parity is inserted at the MSB
position of the transmitted data.

#### <b>Notes</b>
  1.  Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  2. The application needs to ensure that the SysTick time base is always set to
      1 millisecond to have correct HAL operation.

 3. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)
    
### <b>Keywords</b>

Connectivity, LPUART, baud rate, RS-232, full-duplex, Interrupt
Transmitter, Receiver, Asynchronous, Low Power

### <b>Directory contents</b>

  - UART/LPUART_TwoBoards_ComIT/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - UART/LPUART_TwoBoards_ComIT/Src/main.c                 Main program
  - UART/LPUART_TwoBoards_ComIT/Src/system_stm32wb0x.c     STM32WB0x system clock configuration file
  - UART/LPUART_TwoBoards_ComIT/Src/stm32wb0x_it.c         Interrupt handlers
  - UART/LPUART_TwoBoards_ComIT/Src/stm32wb0x_hal_msp.c    HAL MSP module
  - UART/LPUART_TwoBoards_ComIT/Inc/main.h                 Main program header file
  - UART/LPUART_TwoBoards_ComIT/Inc/stm32wb0x_hal_conf.h   HAL Configuration file
  - UART/LPUART_TwoBoards_ComIT/Inc/stm32wb0x_it.h         Interrupt handlers header file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WB09KEVx devices.

  - This example has been tested with NUCLEO-WB09KE board and can be
    easily tailored to any other supported device and development board.

  - STM32WB09KEVx Set-up
    - Connect a wire between 1st board PB.06 (pin 10 on CN6) pin (LPUART Tx) and 2nd board PB.07 (pin 9 on CN6) pin (LPUART Rx).
    - Connect a wire between 1st board PB.07 (pin 9 on CN6) pin (LPUART Rx) and 2nd board PB.06 (pin 10 on CN6) pin (LPUART Tx).
    - Connect 1st board GND to 2nd Board GND.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
