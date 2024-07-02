## __BLE_p2pServer_ota Application Description__

Demonstrate STM32WB0 acting as BLE peripheral and GATT server and offering OTA firmware update service.

BLE_p2pServer_ota application advertises and wait for a connection from either:

- BLE_p2pClient application running on STM32WB0 device
- BLE_p2pRouter application running on STM32WB0 device
- ST BLE Toolbox smartphone application
- ST BLE Sensor smartphone application

Once connected, p2pServer can receive write messages from the Client and send notifications to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE firmware update over the air, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run p2pClient or BLE_p2pRouter application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory:
    - BLE_ApplicationInstallManager binary is downloaded at the memory address 0x10040000
    - BLE_p2pServer_ota binary is downloaded at the memory address 0x10041000 
 - Run the example

 __You can interact with p2pServer_ota application with a smartphone:__

 - Install and launch ST BLE Toolbox or ST BLE Sensor applications on Android or iOS smartphone
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
    - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-sensor/id993670214"> ST BLE Sensor iOS</a>

 - Connect to STM32WB0 device, select p2pS_xx device in the list, where xx is the 2 last digits of the BD ADDRESS.
 - Select the P2P Server interface and click on the LED icon to switch On/Off LED1 of the Nucleo board.
 - Press the Button B1 of the board to notify the smartphone application.

 __You can interact with p2pServer_ota application with another Nucleo board:__

 - Power up p2pClient devices next to p2pServer device.
 - On p2pClient device press B1 button: scan is initiated and connection is established to the first p2pServer device that is found.
    - On p2pServer device press B1 button: a GATT notification message is sent toward connected p2pClient device.
    - On p2pClient device press B1 button: a GATT write command is sent toward connected p2pServer device.
 
On p2pServer, advertising is stopped after 60s. A click on button B1 allows to restart it.

When not connected, a click on button B2 allows to clear the security database.

Once connected, a click on button B3 allows to update the connection interval parameters.

Multi connection support:

 - Pressing button B2 while the device is already connected starts a new advertising to allow multi-connection. 

 __You can update the application from a smartphone:__

 - Power on the Nucleo board with the BLE_ApplicationInstallManager and BLE_p2pServer_ota applications.
 - Open ST BLE Toolbox application:
   select the p2pS_xx in the device list, where xx is the 2 last digits of the BD ADDRESS.
 - Select the Over The Air Update Server interface and the STM32WB0 device type
 - The STM32WB0 interface offers the choice to download an Application Binary or a User Conf Binary
 - Memory mapping is defined as following:
    - 4 KB (sectors 0 and 1) are reserved for the BLE_ApplicationInstallManager application
    - 250 KB (from sector 2 to sector 126) are dedicated to BLE_p2pServer_ota application
    - the second 250 KB of Flash (from sector 127 to sector 251) are dedicated for the new application
    - sectors 252 and 253 are dedicated to User Configuration Data
    - sectors 254 and 255 are reserved for NVM data used by Bluetooth Stack.
 - When selecting Application Binary the default Address offset 0x03F800 is displayed. The download address of the new application is then 0x1007F800.
 - When selecting the User Conf Binary, the Address offset to choose is the address offset of the configured User Configuration Data
   area defined in the app_conf.h header file (default is 0x07E000).
 - With _SELECT FILE_ you can choose the binary of the new application or the binary of the User Configuration Data to download
 - After this binary choice, a calculated Number of sectors to erase is displayed resulting of the size in sectors of the application or the user configuration data to download.
 - The Number of sectors to erase can be forced with a specified number by checking _Force it_ checkbox
 - After selecting download icon, the download will start.
 - In case of new application download, a reboot on this new application is done at the end of the procedure.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
