## __RADIO_SleepTX Demonstration Description__

How to configure a simple TX scenario with sleep management.

This code runs a TX scenario, sending a packet and waiting for an acknowledgment (ACK). A burst of packets equal to MAX_NUM_PACKET are sent, the last byte of each packet contains a sequential number associated to packet number. If RX timeout occurs while waiting ACK a timeout counter is incremented, if the CRC error occurs, the counter crc_error_counter is incremented. The device goes into sleep mode during periods of inactivity. At the end of every burst the Packet Error Rate (PER) is computed and is printed on the terminal.
To implement this scenario, the API HAL_RADIO_SendPacketWithAck is used. The default callback, HAL_RADIO_Callback, provided to this function, defines the radio's behavior according to the IRQ flags. The following callbacks are implemented in user code according to the radio driver's needs: HAL_RADIO_CallbackRcvOk, HAL_RADIO_CallbackRcvTimeout and HAL_RADIO_CallbackRcvError. The App_PowerSaveLevel_Check callback is used to prevent the device from entering sleep mode during UART communication.

Two devices are necessary to fully run this demo: one with the SleepTX image and the other with the SleepRX image.

#### __Notes__

 1. On Keil framework, the following compilation setting are applied :

        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)

 2. Two devices are necessary to fully run this demo: one with the SleepTX image and the other with the SleepRX image.

### __Keywords__

Connectivity, 2.4 GHz RADIO IP

### __Hardware and Software environment__

  - This example runs on STM32WB0 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Run the example
