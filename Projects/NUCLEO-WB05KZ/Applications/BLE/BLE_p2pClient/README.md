## __BLE_p2pClient Application Description__

Demonstrate STM32WB0 acting as BLE central and GATT client.

p2pClient application scans and connects to p2pServer device.

Once connected, p2pClient can write a message to the p2pServer and receive notification from it.

### __Keywords__

Connectivity, BLE, BLE protocol

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board is necessary to run p2pServer application.
    
### __How to use it?__

In order to make the program work, you must do the following :

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Place p2pClient device next to a p2pServer device.
 - On p2pClient device, click on button B1 to launch a scan. A connection is initiated if a p2pServer device is detected.
 - Once the p2pClient application is connected to a p2pServer:
    - On p2pClient device, click on button B1 to send a GATT write command toward connected p2pServer device to toggle blue LED.
    - On p2pServer device, click on button B1 to send a notification message toward connected p2pClient device: blue LED will toggle.
  
### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
