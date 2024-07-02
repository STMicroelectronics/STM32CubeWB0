## __RADIO_SerialPort Example Description__

How to implement a two way point to point communication using HAL_RADIO APIs.
Two devices are necessary to run fully this demo.

In this example the device listen for packet and then print content on terminal through COM Port. 
The device also listen for input from terminal and sends a packet, with string typed, 
if carriage return character is detected or maximum length of payload is reached.

To implement this scenario HAL_RADIO_SendPacketWithAck and HAL_RADIO_ReceivePacketWithAck API are used.
These APIs in addition to transmission and reception manage an ACK message, in this way user code may manage packet loss events. 
The default callback HAL_RADIO_Callback is provided to these functions. The following callbacks are implemented, in user code according, to the radio driver's needs: 
HAL_RADIO_CallbackRcvOk, HAL_RADIO_CallbackRcvTimeout, and HAL_RADIO_CallbackTxDone to define the behavior of the radio.

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
 - Rebuild all files and flash two board with the same firmware
 - Run the example
 - Open a serial terminal on the two COM ports and interact with the serial terminal by tiping message, be sure to end the message with the carriage return character.






