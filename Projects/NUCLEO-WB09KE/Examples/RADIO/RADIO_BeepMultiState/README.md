## __RADIO_BeepMultistate Example Description__

How to configure multiple state machine for transmission on different channels using the ActionPacket mechanism exported by the 2.4 GHz radio driver.

In this example, the device sends a packet continuously on eight different channels. The packet sent has a payload of 26 bytes, some packets are encrypted.

After each transmission, the channel is changed and encryption is configured if activated for the channel. 
The ActionPacket structure is configured to implement several state machines, one for each channel. Each state machine is configured with its own channel and encryption. 
The ConditionRoutine and DataRoutine are bound to all state machines. In the data routine, code for scheduling the next ActionPacket is implemented.

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
