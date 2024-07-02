## __RADIO_TestRx Example Description__

How to configure simple RX with ack scenario using HAL_RADIO API

This code runs RX scenario sending back an acknowledgment (ACK). The payload of the received frame is printed on the terminal.
To implement this scenario the API HAL_RADIO_ReceivePacketWithAck is used. The default callback HAL_RADIO_Callback is provided to this function.
The following callbacks are implemented, in user code, according to the radio driver's needs: 
HAL_RADIO_CallbackRcvOk, HAL_RADIO_CallbackRcvTimeout and HAL_RADIO_CallbackTxDone to define the behavior of the radio.

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

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file 
 - Program two devices: one with the RADIO_TestTx image and the other with the RADIO_TestRx image. On the RADIO_TestTx device, open a serial terminal to get the result of the communication.
 