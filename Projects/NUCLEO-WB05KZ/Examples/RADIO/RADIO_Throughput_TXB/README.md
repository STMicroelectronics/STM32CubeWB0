## __RADIO_Throughput_TXB Example Description__

This code implements a throughput test (transmitter, bidirectional).
Two devices are needed, one with the RX bidirectional configuration.
In the bidirectional configuration, the TX device will wait for the ACK packet. The usage is the same as the unidirectional configuration, but it is necessary to have another device configured with the RxThroughput Bidirectional as well.

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
 - Flash another board with RADIO_Throughput_TXB example
 - Run example

