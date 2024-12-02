## __BLE_Privacy_Central Application Description__

Demonstrate STM32WB0 acting as BLE peripheral and GATT server.

This demo implements a Controller Privacy scenario using Privacy 1.2 available with STM32WB0 Bluetooth LE stack v4.x. 
Two devices Peripheral, Central, both configured with aci_gap_init(privacy flag = 0x02), which should perform these macro steps:

Initially, Central and Peripheral devices have no info on their security database: the two devices should connect and make a paring and bonding (fixed key: 123456).
After the first connection and the pairing/bonding phase is completed, pressing Push1 on the Peripheral, devices disconnect and the Peripheral calls the aci_gap_configure_filter_accept_and_resolving_list() API to add its bonded device address to the controller's filter list and to the list of resolvable addresses.
The Central device enables the Peripheral characteristic notification at pairing complete. The Peripheral send one notification per every notification enable received, sending a counter as payload data.

The Peripheral enters undirected connectable mode with its own address type = resolvable address and filter list = 0x03 as advertising filter policy.
The Central device performs a direct connection to the detected Peripheral device, which accepts the connection since the Central address is on its filter list: the two devices reconnect and the Peripheral starts a notification to the Peripheral.
NOTES:

If the user presses the button PUSH3 on the Central device, the device disconnect

Press the button Push1 on Peripheral when connected : the Peripheral disconnects all devices connected and enters in undirected connectable mode with filtering enabled. This implies that Peripheral device accepts connection requests only from devices on its filter list.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE Controller Privacy

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run BLE_Privacy_Peripheral application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 __You can interact with BLE_Privacy_Peripheral application with another Nucleo board:__

 - Power up BLE_Privacy_Peripheral device next to a BLE_Privacy_Central device.
 - On BLE_Privacy_Peripheral press button Push2: to clear the security DB.
 - On BLE_Privacy_Central    press button Push2: to clear the security DB.
 - On BLE_Privacy_Peripheral press button Push1: the advertising is initiated.
 - On BLE_Privacy_Central    press button Push1: scan is initiated and connection is established to the first BLE_Privacy_Peripheral device that is found.
 - On BLE_Privacy_Peripheral press button Push1: if some bonded devices are found, it will added to the filter list. The BLE_Privacy_Peripheral disconnect the bonded devices and enters undirected connectable mode with filter policy enabled.
 - On BLE_Privacy_Central    press button Push1: scan is initiated and connection is established without passing the password.

When not connected, a click on button Push2 allows to clear the security database.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
