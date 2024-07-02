## __RADIO_otaClient Demonstration Description__

How to configure client application using proprietary 2.4 GHz radio driver for over-the-air firmware upgrade.

This code runs OTA Client application: receives a new binary image of the firmware containing user application from OTA server, saves it to flash memory and runs the received application. A proprietary protocol implemented at the radio's low-level driver is used for communication between client and server.
The user app code is located in flash region after OTA_client code, a specific TAG in the startup file indicates if received firmware is a valid user application and which application should run, if OTA client or user application. 
The PB2 button allows users to activate the OTA Client and get new image from an OTA Server.

Two devices are necessary to fully run this demo.
Both otaServerYmodem and otaServerFixed can be used as the Server application.
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
