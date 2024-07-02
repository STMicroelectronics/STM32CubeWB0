## __BLE_p2pServer_FreeRTOS Application Description__

Demonstrate STM32WB0 acting as BLE peripheral and GATT server with FreeRTOS.

BLE_p2pServer_FreeRTOS application advertises and waits for a connection from either:

- BLE_p2pClient application running on STM32WB0 device
- BLE_p2pRouter application running on STM32WB0 device
- ST BLE Toolbox smartphone application
- ST BLE Sensor smartphone application

Once connected, p2pServer can receive write messages from the Client and send notifications to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, FreeRTOS

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run p2pClient or BLE_p2pRouter application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 __You can interact with BLE_p2pServer_FreeRTOS application with a smartphone:__

 - Install and launch ST BLE Toolbox or ST BLE Sensor applications on android or iOS smartphone
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
    - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-sensor/id993670214"> ST BLE Sensor iOS</a>
 - Connect to STM32WB0 device, select p2pS_xx device in the list, where xx is the 2 last digits of the BD ADDRESS.
 - Select the P2P Server interface and click on the LED icon to switch On/Off LED1 of the Nucleo board.
 - Press the Button B1 of the board to notify the smartphone application.

 __You can interact with BLE_p2pServer_FreeRTOS application with another Nucleo board:__

 - Power up p2pClient device next to a p2pServer device.
 - On p2pClient device press B1 button: scan is initiated and connection is established to the first p2pServer device that is found.
    - On p2pServer device press B1 button: a GATT notification message is sent toward connected p2pClient device.
    - On p2pClient device press B1 button: a GATT write command is sent toward connected p2pServer device.
 
Advertising is stopped after 60s. A click on button B1 allows to restart it.

When not connected, a click on button B2 allows to clear the security database.

Once connected, a click on button B3 allows to update the connection interval parameters.

__Multi connection support:__

 - Pressing button B2 while the device is already connected starts a new advertising to allow multi-connection. 

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
