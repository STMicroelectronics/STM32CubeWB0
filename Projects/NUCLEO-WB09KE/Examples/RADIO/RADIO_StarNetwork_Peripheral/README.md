## __RADIO_StarNetwork_Peripheral Description__

How to implement a Star Network using a 2.4 GHz RADIO proprietary driver. 

At least two device are needed. The code defines a Peripheral device in a star network. The Peripheral waits for a data request from the Central device. 
Once the request is received, the Peripheral sends the data packet and waits for an ACK packet from the central device that confirms data reception.
If the ACK packet is not received, the Peripheral resends the data packet to the Central device. 
The Peripheral can send up to three data packets to the Central device if the ACK packet is not received.

#### __Notes__
                                            
 1. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)


### __Keywords__

Connectivity, 2.4 GHz RADIO IP

### __Hardware and Software environment__

  - This example runs on STM32WB09 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Run the example
