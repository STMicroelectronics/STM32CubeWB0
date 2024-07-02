## __RADIO_Beep Example Description__

How to configure transmission on different channels using the ActionPacket mechanism exported by the 2.4 GHz radio driver.

In this example, the device sends a packet continuously on three different channels: 24, 25 and 26. The packet sent has 20 bytes of payload.
After each transmission the channel is changed, the next channel setting is done by the application code.
The ActionPacket structure is used to schedule transmission and manage two driver callback: conditionRoutine and dataRoutine.
The conditionRoutine callback is used to program the next transmission channel.
The dataRoutine callback is used to set the content of the next packet payload.

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
