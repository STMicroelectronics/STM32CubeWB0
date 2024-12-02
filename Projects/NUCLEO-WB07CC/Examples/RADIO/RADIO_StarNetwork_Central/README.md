## __RADIO_StarNetwork_Central Example Description__

How to implement a Star Network using 2.4 GHz RADIO proprietary driver. At least two device are needed.

The code defines a Central device of a star network. The Central knows how many Peripherals are in the network and their peripheral address.
Then, periodically, asks for data to the Peripherals and waits for their data. 
If data are received, then the Central answers with an ACK packet to the selected Peripheral.

#### __Notes__
                                            
 1. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)


### __Keywords__

Connectivity, 2.4 GHz RADIO IP

### __Hardware and Software environment__

  - This example runs on STM32WB07 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Run the example
