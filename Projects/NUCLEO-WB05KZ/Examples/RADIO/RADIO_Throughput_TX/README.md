## __RADIO_Throughput_TX Example Description__

This code implements a throughput test (transmitter, unidirectional configuration: only one device is needed).
The unidirectional example shows the time needed to send a packet, from the transmission command to the TX callback. The time is measured with the TIM peripheral.
The result is shown after a burst of packet number sent. It is not required an RX device for this configuration, 
but could be used the sniffer application if it is necessary to get the packets.

#### <b>Notes</b>
1. The Number of packet sent can be changed by MAX_NUM_PACKET symbol
2. The number of bytes of payload can be changed using the DATA_PACKET_LEN symbol

#### __Notes__
                                            
 1. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)


### __Keywords__

Connectivity, 2.4 GHz RADIO IP

### __Hardware and Software environment__

  - This example runs on STM32WB05 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the projects with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Run example
 
