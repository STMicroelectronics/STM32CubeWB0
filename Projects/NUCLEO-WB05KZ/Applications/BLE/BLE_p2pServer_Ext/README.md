## __BLE_p2pServer_Ext Application Description__

Demonstrate STM32WB0 acting as BLE peripheral and GATT server and using several advertising sets.

p2pServer_Ext application advertises several extended advertising sets:

- A legacy advertising
- A connectable extended advertising
- A scannable extended advertising
- A non-connectable and non-scannable extended advertising

These sets are configurable in file adv_ext_app.c.

### __Keywords__

Connectivity, BLE, extended, advertising

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run p2pClient_Ext application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 __You can interact with p2pServer_Ext application with a smartphone:__

 - Install and launch ST BLE Toolbox or ST BLE Sensor applications on android or iOS smartphone
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
    - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-sensor/id993670214"> ST BLE Sensor iOS</a>

 __You can interact with p2pServer_Ext application with another Nucleo board running p2pClient_Ext:__

 - Power up p2pClient_Ext device next to a p2pServer_Ext device.
 - On p2pClient_Ext device press B1 button to start scanning: if a p2pServer_Ext connectable extended advertising is discovered, BD address is registered.
 - Then, press B3 button on p2pClient_Ext to connect.

__You can interact with p2pServer_Ext application with another Nucleo board running p2pClient:__

 - Power up p2pClient device next to p2pServer_Ext device.
 - On p2pClient device press B1 button to start scanning: p2pClient will receive legacy advertising from p2pServer_Ext and connect to it.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
