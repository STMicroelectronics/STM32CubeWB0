## __RADIO_Sniffer Example Description__

How to configure receiving packet on specific channel using HAL_RADIO API.

This code implements a sniffer application on specific frequency channel.
Once the packet is received, the RSSI associated with the packet, the timestamp, and the entire frame are printed.

To implement this scenario the API HAL_RADIO_ReceivePacket is used. The default callback HAL_RADIO_Callback is provided to this function.
The following callbacks are implemented, in user code, according to the radio driver's needs: 
HAL_RADIO_CallbackRcvOk, HAL_RADIO_CallbackRcvTimeout and HAL_RADIO_CallbackRcvError to define the behavior of the radio.

#### __Notes__
 1. Network ID to sniff can be specified in CFG_RF_NETWORK_ID define                                            
 2. On Keil framework, the following compilation setting are applied :
    
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

 - Open the projects with your preferred toolchain
 - Rebuild all files and flash the board with the executable file 
 - Run example
