## __BLE_HealthThermometer Application Description__

How to use the Health Thermometer profile as specified by the Bluetooth LE SIG.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 


### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - On the android/ios device, enable the Bluetooth communications and if not done before install ST BLE Toolbox application:
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>.
 - Power on the Nucleo board with the BLE_HealthThermometer application
 - Click on the App icon, STM32 BLE Toolbox (android device)
 - Select the HT_xx in the device list
 - Connect the application
 - Open Health Thermometer service
 - The temperature measurement is displayed on the android device,
   temperature min/max valures are also displayed.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
