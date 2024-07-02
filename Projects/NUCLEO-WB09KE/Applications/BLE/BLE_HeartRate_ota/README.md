## __BLE_HeartRate_ota Application Description__

How to use the Heart Rate and OTA profile as specified by the Bluetooth LE SIG.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE firmware update over the air, BLE profile


### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory:
   - BLE_ApplicationInstallManager binary is downloaded at the memory address 0x10040000
   - BLE_HeartRate_ota binary is downloaded at the memory address 0x10041000 
 - Run the example


 - Install and launch ST BLE Toolbox or ST BLE Sensor applications on Android or iOS smartphone
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
    - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-sensor/id993670214"> ST BLE Sensor iOS</a>

 - Power on the Nucleo board with the BLE_ApplicationInstallManager and BLE_HeartRate_ota applications.
 - You can open ST BLE Toolbox application (android device):
   select the HR_xx in the device list, where xx is the 2 last digits of the BD ADDRESS.
 - Connect to the selected STM32WB09 device.

 - Scroll right and select either the Heart Rate interface or the Over The Air Update Server interface

 - In the Heart Rate interface, HeartRate and energy measurement are launched and displayed in graphs,
   you can reset the energy measurement.
     - Pairing is supported: Button B2 clears the security database when the device is not connected. 
     When connected, Button B2 send the slave pairing request, here a popup asks you to associate your device.
     You can either bond from the smartphone by clicking on Bond Button.
     - This example supports switch to 2Mbits PHY, pressing Button B1 while connected allows to switch between 1Mbits PHY and 2Mbits PHY.
     - After 60s of advertising, the application switch from fast advertising to low power advertising, pressing Button B1 while advertising allows to restart fast advertising.
     - Pressing button B3 while connected allows to update the connection interval. 

 - In the Over The Air Update Server interface, select the STM32WB09 device type
   - The STM32WB0 interface offers the choice to download an Application Binary or a User Conf Binary

   - Memory mapping is defined as following:
		- 4 KB (sectors 0 and 1) are reserved for the BLE_ApplicationInstallManager application
		- 250 KB (from sector 2 to sector 126) are dedicated to BLE_HeartRate_ota application
		- the second 250 KB of Flash (from sector 127 to sector 251) are dedicated for the new application
		- sectors 252 and 253 are dedicated to User Configuration Data
		- sectors 254 and 255 are reserved for NVM data used by Bluetooth Stack.
   - When selecting Application Binary the default Address offset 0x03F800 is displayed. The download address of the new application is then 0x1007F800.
   - When selecting the User Conf Binary, the Address offset to choose is the address offset of the configured User Configuration Data
     area defined in the app_conf.h header file (default is 0x07E000).
   - With _SELECT FILE_ you can choose the binary of the new application or the binary of the User Configuration Data to download.
   - After this binary choice, a calculated Number of sectors to erase is displayed resulting of the size in sectors of the application or the user configuration data to download.
   - The Number of sectors to erase can be forced with a specified number by checking _Force_ checkbox,
   - After selecting download icon, the download will start.
   - In case of new application download, a reboot on this new application is done at the end of the procedure.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
