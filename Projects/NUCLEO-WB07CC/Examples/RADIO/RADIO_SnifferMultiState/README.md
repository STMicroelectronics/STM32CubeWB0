## __RADIO_SnifferMultiState Example Description__

How to configure receiving packets on different channels using multiple state machines.

This code implements a sniffer application on different frequency channels, some of which are configured with encryption.
Once the packet is received, the RSSI associated with the packet, the timestamp, and the entire frame are printed.

To implement this scenario, the ActionPacket mechanism is used. Each ActionPacket packet is configured with a different state machine.
Each state machine has its own frequency channel, network address, wakeup time and encryption.
In the DataRoutine, the code to schedule the next ActionPacket is placed.

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
