## __RADIO_otaServerYmodem Demonstration Description__

How to configure server application using proprietary 2.4 GHz radio driver for over-the-air firmware upgrade. 

This code runs OTA Server application: wait for a binary image to transfer, establish connection with OTA client and send new firmware. The YMODEM serial protocol is used to acquire the new firmware image for transfer to the client. A proprietary protocol, implemented at the radio's low-level driver, is used for communication between the client and server.

Two devices are necessary to fully run this demo.
OtaClient can be used as Client application.
For example, Radio_otaTx and Radio_otaRx projects allow to build the required images to be transferred on the OTA Client device.

#### __Notes__

 1. On Keil framework, the following compilation setting are applied :

        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)

### __Keywords__

Connectivity, 2.4 GHz RADIO IP

### __Hardware and Software environment__

  - This example runs on STM32WB0 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Run the example
