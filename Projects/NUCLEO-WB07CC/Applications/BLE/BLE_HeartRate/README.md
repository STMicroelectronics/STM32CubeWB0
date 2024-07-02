## __BLE_HeartRate Application Description__

How to use the Heart Rate profile as specified by the Bluetooth LE SIG.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
  - Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 
  - This application is by default configured to support low power mode ( No traces - No debugger ) 
  - Low Power configuration can be modified in app_conf.h (CFG_FULL_LOW_POWER, CFG_DEBUG_APP_TRACE)

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file

 On the Android/iOS device, enable the Bluetooth communications, and if not done before:

   - Install the ST BLE Sensor application on the Android/iOS device:
	   - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
	   - <a href="https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8"> ST BLE Sensor iOS</a>

   - You can also install the ST BLE Toolbox application on the Android/iOS device:
     - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
     - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
	
 Power on the Nucleo board with the BLE_HeartRate application.

 - Then, click open ST BLE Sensor app anf select the HR_xx in the device list.
	 - The Heart Rate is displayed each second on the smartphone.
	 - Pairing is supported:
	   - Button B2 clears the security database when the device is not connected,
	   - When connected with a client, button B2 send the slave pairing request, here a popup asks you to associate your device.
	 - This example supports switch to 2Mbits PHY, pressing button B1 while connected allows to switch between 1Mbits PHY and 2Mbits PHY.
	 - After 60s of advertising, the application switch from fast advertising to low power advertising, pressing button B1 while advertising allows to restart fast advertising.
	 - Pressing button B3 while connected allows to update the connection interval. 

 - You can either open ST BLE Toolbox application (Android/iOS device).

	 - Select the HR_xx in the device list and connect to the device.
	 - Scroll right and select Heart Rate interface,
	 - HearRate and energy measurement are launched and displayed in graphs. You can reset the energy measurement.
	 - Pairing is supported:   
	   - Button B2 clears the security database when the device is not connected,
	   - When connected with a client, button B2 send the slave pairing request, here a popup asks you to associate your device.
	 - You can either bond from the smartphone by clicking on Bond Button.
	 - This example supports switch to 2Mbits PHY, pressing button B1 while connected allows to switch between 1Mbits PHY and 2Mbits PHY.
	 - After 60s of advertising, the application switch from fast advertising to low power advertising, pressing button B1 while advertising allows to restart fast advertising.
	 - Pressing button B3 while connected allows to update the connection interval. 

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
