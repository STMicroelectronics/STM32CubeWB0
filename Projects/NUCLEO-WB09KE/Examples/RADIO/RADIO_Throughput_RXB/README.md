## __RADIO_Throughput_RXB Example Description__

This code implements a throughput test (receiver configuration).
Two devices are necessary to run fully this demo. A device with the Throughput RX Bidirectional configuration and another one with Throughput TX Bidirectional configuration. The RX device will receive the packet and then will send back an ACK packet. The output of the test comes from the TX device. The RX configuration does not need change if the number of bytes of payload changes or the number of packets used for the measurement change. Only the TX configuration contains these parameters.

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

 - Open the projects with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Flash another board with RADIO_Throughput_TXB example
 - Run example
