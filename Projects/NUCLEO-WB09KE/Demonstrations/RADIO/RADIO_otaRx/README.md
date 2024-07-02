## __RADIO_otaRx Demonstration Description__

How to configure a simple RX application with over-the-air (OTA) upgrade capability.

This code runs an RX scenario that sends back an acknowledgment (ACK), and the payload of the received frame is printed on the terminal. 
Sleep management is integrated to conserve power. 
The PB2 button is used to activate OTA_Client mode. 
The firmware image is built with the correct offset to ensure it is compatible with the OTA Client, preventing overwriting of the otaClient during the firmware upgrade process.

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
