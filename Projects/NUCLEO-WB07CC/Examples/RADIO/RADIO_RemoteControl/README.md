## __RADIO_RemoteControl Example Description__

How to configure a basic remote control scenario between two devices using the HAL_RADIO API.

In this example, the device is programmed to listen for packet. When the packet is received the LD2 is toggled.
If the push button PB1 is pressed, a transmission of a packet is scheduled.  The transmission is continuously done until the button is released.

To implement this scenario, two HAL APIs are used: HAL_RADIO_SendPacket for transmission and HAL_RADIO_ReceivePacket for reception.
The default callback HAL_RADIO_Callback is provided to these functions.The following callbacks are implemented according to the radio driver's needs: HAL_RADIO_CallbackRcvOk, HAL_RADIO_CallbackRcvTimeout, and HAL_RADIO_CallbackTxDone to define the behavior of the radio.

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
 - Rebuild all files and flash the two board with the same firmware
 - Run the example
 - Press the B1 button on a board, then the other board will turn on or turn off the LD2 on reception. And vice-versa.