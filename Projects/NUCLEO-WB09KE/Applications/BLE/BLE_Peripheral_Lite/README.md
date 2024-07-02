## __BLE_Peripheral_Lite Application Description__

Demonstrate how to communicate with simple Bluetooth LE peripheral with minimum activated features.
This application shows simple Bluetooth LE peripheral operation with bidirectional communication with minimalistic device configuration flow
and Lite service support with minimum needed features activation (not including e.g. Tasks Sequencer, Timer Server, Low-Power Manager).

BLE_Peripheral_Lite application advertises and waits for a connection from either:

- ST BLE Toolbox smartphone application
- ST BLE Sensor smartphone application

Once connected, BLE_Peripheral_Lite can receive write messages from the Client and send notifications to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 __You can interact with BLE_Peripheral_Lite application with a smartphone:__

 - Install and launch ST BLE Toolbox or ST BLE Sensor applications on android or iOS smartphone
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
    - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-sensor/id993670214"> ST BLE Sensor iOS</a>
 - Once the code (BLE_Peripheral_Lite) is downloaded into the STM32WB0 board and executed, the module is initialized.
 - The Peripheral device (BLE_Peripheral_Lite) starts advertising with data using local name "HELLO!".
 - The green led blinks for each advertising event slowly each ~0.5sec.
 - The Smart phone application starts scanning.
 - Select the "HELLO!" device (containing My Very Own service as P2PServer profile) in the list of Smart phone application and connect to it. The green led toggles faster. 
 - Select the P2P Server interface and click on the LED icon to switch On/Off LED1 (blue LED of the Nucleo board0).
 - Notification is sent from the server (Peripheral device - Nucleo board)  to the client (Smart phone) every ~1sec.
 - When the Peripheral device (Nucleo board) is disconnected, advertising is restarted and it is possible to connect to it again.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
