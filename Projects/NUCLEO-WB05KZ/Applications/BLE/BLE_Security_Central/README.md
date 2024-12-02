## __BLE_Security_Central Application Description__

Demonstrate STM32WB0 acting as BLE central and GATT client.

The BLE_Security_Central application scans for advertising and waits for a connection from either:
- The BLE_Security_Peripheral application running on an STM32WB0 device.

Pairing with pass key entry method (with fixed pin) and bonding are used in this example.

Once connected, BLE_Security_Peripheral can receive write messages from the Central and it will send notifications periodically to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run BLE_Security_Central application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 

 __You can interact with the BLE_Security_Peripheral application with another Nucleo board:__

  - Power up the BLE_Security_Peripheral device next to a BLE_Security_Central device.
  - On the BLE_Security_Peripheral, press the B2 button to clear the security DB.
  - On the BLE_Security_Central, press the B2 button to clear the security DB.
  - On the BLE_Security_Peripheral, press the B1 button to start the advertising procedure with a one-minute timeout. During advertising, LED2 will blink.
  - On the BLE_Security_Central, press the B1 button to initiate a scan and establish a connection to the first BLE_Security_Peripheral device that is found.
  - A GATT notification message is sent periodically to the connected BLE_Security_Central device, and the LED1 on the Central will toggle.
  - On the BLE_Security_Central, press the B1 button to send a GATT write command to the connected BLE_Security_Peripheral device, causing LED1 on the Peripheral to toggle.

On the BLE_Security_Peripheral, advertising stops after 60 seconds. Pressing the B1 button restarts it. If the Peripheral is connected, pressing the B1 button starts the procedure to disconnect from the Central.
When the BLE_Security_Peripheral is not connected, pressing the B2 button clears the security database.
Once connected, pressing the B3 button on the Peripheral updates the connection interval parameters.

When the BLE_Security_Central is not connected, pressing the B2 button clears the security database.
If the Central is connected, pressing the B3 button starts the procedure to disconnect from the Peripheral.
Once connected, pressing the B1 button on the Central sends a GATT write command to the connected BLE_Security_Peripheral device, causing LED1 on the Peripheral to toggle.


### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
