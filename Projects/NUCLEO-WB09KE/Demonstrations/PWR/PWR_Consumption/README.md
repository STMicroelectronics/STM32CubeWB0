## __PWR_Consumption Application Description__

This is a demo that shows how to configure the STM32WB0 power modes in order to save power.

This test provides an example for the STM32WB0 series power modes. The power modes tested are SHUTDOWN LEVEL: with wake up on a low pulse on the RSTN pad or a low pulse on PB0, DEEPSTOP: with Low Speed clock enabled with wake on UART RX pin or timeout 5 sec or button PUSH1/PUSH2, DEEPSTOP: with wake on UART RX pin or timeout 5 sec (RTC) or button PUSH1/PUSH2, DEEPSTOP: with wake on UART RX pin or button PUSH1/PUSH2 and SLEEP (WFI): wake on button PUSH1/PUSH2</b>.

### __Keywords__

Connectivity, 2.4 GHz RADIO IP, Power consumption, Power modes

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - A serial terminal must be opened on the associated COM port to show the command menu of the demonstration.

#### __Notes__

 1. On Keil framework, the following compilation setting are applied :

        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)