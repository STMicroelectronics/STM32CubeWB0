## __BLE_PAwR_Broadcaster Application Description__

Demonstrate STM32WB0 acting as a Broadcaster of a Periodic Advertising with Response (PAwR) train.
The device is able to advertise a PAwR train to send and receive data to/from addressed device. The used protocol is derived from standard Electronic Shelf
label profile, even though not fully compliant. It acts as an Access Point (AP).

For the sake of simplicity, the device has a defined list of Peripheral to connect to, the ESLs. Within ESL commands, each ESL is address by a Group ID and an ESL ID. Once connected to the Peripheral, the Periodic Advertising Synchronization Transfer (PAST) procedure is started to let the Peripheral synchronize with the PAwR train, without the need to do an energy-intensive scan. Once the procedure is completed, the link can be disconnected.

To control the AP, an AT-like command interface is used.
Each command ends with \<CR>. For this demo \<group_id> and \<esl_id> are limited to a value between 0 and 2. Only for \<esl_id> there is the possibility to use FF for broadcast messages.
The following commands are supported. 
- Ping an ESL: *AT+PING=\<group_id>,\<esl_id>*
- LED control: *AT+LED=\<group_id>,\<esl_id>,\<led_level>*
  - where \<led_level> can be 0 (off), 1 (on), 2 (blinking)
- Read battery level: *AT+BATT=\<group_id>,\<esl_id>*
  - Response is given asynchronously with *+BATT:\<group_id>,\<esl_id>,\<battery_level>,\<status>*
- Select an image: *AT+IMG=\<group_id>,\<esl_id>,\<img_index>*
- Send a text: *AT+TXT=\<group_id>,\<esl_id>,\<text>*
  - \<text> can be a string with maximum 15 characters
- Send a price: *AT+PRICE=\<group_id>,\<esl_id>,\<price>*

*ATE* can also be typed to enable local echo.

An "OK" or "ERROR" is given after a command is received. When a response is received from the peer, this is notified on the terminal with a string like *+\<RESP>:\<group_id>,\<esl_id>,\<status>*.
Only Ping, Led Control and Read Battery are currently implemented on the PAwR observer running on Nucleo board.
Commands which do not require a response, e.g. LED Control, can be sent in broadcast with all the ESLs in the group, by specifying ESL_ID 0xFF. Broadcast commands are retransmitted to increase reliability.

For each AT command, an ESL-like command is queued and sent as soon as possible.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE PAwR

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board is necessary to run BLE_PAwR_Observer application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Open a serial terminal (with settings 115200-8-N-1 and \<CR> transmission at end of line).
 - Reset the board to run the example.
 - Run BLE_PAwR_Observer on another Nucleo board.
 - Once the PAwR Broadcaster has passed the synchronization information to the Observer, you can send commands on the terminal by specifying the assigned GROUP_ID and ESL_ID (defaults values are 0, 0) to:
   - just ping the board
   - control an LED (on, off, blink)
   - read some sensor data


### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
