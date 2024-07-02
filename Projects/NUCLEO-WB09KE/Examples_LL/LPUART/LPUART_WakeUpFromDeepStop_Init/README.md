## <b>LPUART_WakeUpFromDeepStop_Init Example Description</b>

Configuration of GPIO and LPUART peripherals to allow characters
received on LPUART_RX pin to wake up the MCU from low-power mode. This example is based
on the LPUART LL API. The peripheral initialization uses LL
initialization function to demonstrate LL init usage.

    Board: NUCLEO-WB09KE (embeds a STM32WB09KEVx device)
    Tx Pin: PB.06 (pin 10 on CN6)
    Rx Pin: PB.07 (pin 9 on CN6)

LPUART Peripheral is configured in asynchronous mode (9600 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
LPUART Clock is based on LSE.

Example execution:

After startup from reset and system configuration, LD1 is blinking quickly during 3 sec,
then MCU  enters "Deep Stop" mode (LD1 off).
On first  character reception by the LPUART from PC Com port (ex: using HyperTerminal)
after "Deep Stop" Mode period, MCU wakes up from "Deep Stop" Mode.

Received character value is checked :

- On a specific value ('S' or 's'), LD1 is turned On and program ends.
- If different from 'S' or 's', program performs a quick LD1 blinks during 3 sec and 
  enters again "Deep Stop" mode, waiting for next character to wake up.

In case of errors, LD1 is slowly blinking (1 sec period).

#### <b>Notes</b>

1. In case of non soldered pins of Virtual Com port , you may use "USB-TTL Converter" .

Set-Up :
  - Tx converter connected with Rx LPUART .
  - Rx converter connected with Tx LPUART .
  - GND converter connected with GND Board .
                                          
2. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)

### <b>Keywords</b>

Connectivity, LPUART, baud rate, RS-232, HyperTerminal, full-duplex,
Transmitter, Receiver, Asynchronous, Low Power

### <b>Directory contents</b>

  - LPUART/LPUART_WakeUpFromDeepStop_Init/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - LPUART/LPUART_WakeUpFromDeepStop_Init/Inc/main.h                  Header for main.c module
  - LPUART/LPUART_WakeUpFromDeepStop_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - LPUART/LPUART_WakeUpFromDeepStop_Init/Src/stm32wb0x_it.c          Interrupt handlers
  - LPUART/LPUART_WakeUpFromDeepStop_Init/Src/main.c                  Main program
  - LPUART/LPUART_WakeUpFromDeepStop_Init/Src/system_stm32wb0x.c      STM32WB0x system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WB09KEVx devices.

  - This example has been tested with NUCLEO-WB09KE board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WB09KE Set-up
    Example is delivered for using Virtual Com port feature of STLINK for connection between NUCLEO-WB09KE and PC,
    GPIOs connected to LPUART1 TX/RX (PB6 and PB7) are automatically mapped
    on RX and TX pins of PC UART Com port selected on PC side (please ensure VCP com port is selected).

    - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration
      (9600 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control).


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
