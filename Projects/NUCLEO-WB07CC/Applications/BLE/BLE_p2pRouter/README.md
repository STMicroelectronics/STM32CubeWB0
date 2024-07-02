## __BLE_p2pRouter Application Description__

Demonstrate STM32WB0 acting at the same time as both: BLE central and peripheral, GATT server and client.

BLE_p2pRouter application scans to connect to p2pServer devices and accept connection of ST BLE Toolbox smartphone app. It will route BLE messages received from both side.

### __Keywords__

Connectivity, BLE

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - At least another STM32WB0 Nucleo board is necessary to run p2pServer application.
    
### __How to use it?__

In order to make the program work, you must do the following :

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Install and launch ST BLE Toolbox application on android or iOS smartphone
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
 - Connect to STM32WB0 device, select p2pR_xx device in the list.
 - Power up to 7 p2pServer devices next to BLE_p2pRouter device.
 - With a click on B1, BLE_p2pRouter will scan and then connect to a p2pServer device surrounding.
 - On smartphone interface you can see devices appearing.
 - On p2pServer device, a click on B1 sends a notification to BLE_p2pRouter.
    - this GATT notification message is forwarded to the smartphone and displayed on the interface.

 - On smartphone interface a GATT write command can be sent to selected p2pServer devices, changing led status.
    - this write message is sent first to BLE_p2pRouter and then routed to its destination. 
	  The blue led will toggle on selected p2pServer devices.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
