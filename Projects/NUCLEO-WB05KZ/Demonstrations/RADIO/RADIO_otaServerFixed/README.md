## __RADIO_otaServerFixed Demonstration Description__

How to configure a server application using a proprietary 2.4 GHz radio driver for over-the-air (OTA) firmware upgrades.

This code runs the OTA Server application: it establishes a connection with the OTA client and sends the new firmware. 
A fixed binary image of the firmware to be sent is hardcoded into the server application in the binary_image_txrx_example.c file. 
A proprietary protocol, implemented at the radio's low-level driver, is used for communication between the client and server.

Two devices are necessary to fully run this demo. OtaClient can be used as the Client application.
For example, the Radio_otaTx and Radio_otaRx projects allow building the required images to be transferred to the OTA Client device.

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
