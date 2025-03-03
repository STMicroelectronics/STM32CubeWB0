## __BLE_Privacy_Peripheral Application Description__

Demonstrate STM32WB0 acting as a BLE peripheral and GATT peripheral.

This demo implements a Controller Privacy scenario using Privacy 1.2 available with the STM32WB0 Bluetooth LE stack v4.x.
Two devices, Peripheral and Central, both configured with aci_gap_init (privacy flag = 0x02), should perform these macro steps:

Initially, Central and Peripheral devices have no information in their security database: the two devices should connect and perform pairing and bonding (fixed key: 123456).
After the first connection and the pairing/bonding phase is completed, pressing Push1 on the Peripheral will cause the devices to disconnect. The Peripheral then calls the aci_gap_configure_filter_accept_and_resolving_list() API to add its bonded device address to the controller's filter list and to the list of resolvable addresses.
The Central device enables the Peripheral characteristic notification at pairing complete. The Peripheral sends one notification for every notification enable received, sending a counter as payload data.

The Peripheral enters undirected connectable mode with its own address type set to resolvable address and filter list set to 0x03 as the advertising filter policy.
The Central device performs a direct connection to the detected Peripheral device, which accepts the connection since the Central address is on its filter list: the two devices reconnect and the Peripheral starts a notification to the Central.
NOTES:

If the user presses the button PUSH3 on the Central device, the device disconnects.

Press the button Push1 on the Peripheral when connected: the Peripheral disconnects all connected devices and enters undirected connectable mode with filtering enabled. This implies that the Peripheral device accepts connection requests only from devices on its filter list.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE Controller Privacy

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run BLE_Privacy_Central application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 __You can interact with BLE_Privacy_Central application with another Nucleo board:__

### __Steps to Interact with BLE_Privacy Application__

 - Power up BLE_Privacy_Peripheral device next to a BLE_Privacy_Central device.
 - On BLE_Privacy_Peripheral press button Push2: to clear the security DB.
 - On BLE_Privacy_Central    press button Push2: to clear the security DB.
 - On BLE_Privacy_Peripheral press button Push1: the advertising is initiated.
 - On BLE_Privacy_Central press button Push1: scan is initiated and connection is established to the first BLE_Privacy_Peripheral device that is found.
 - On BLE_Privacy_Peripheral press button Push1: if some bonded devices are found, it will be added to the filter list. The BLE_Privacy_Peripheral disconnects the bonded devices and enters undirected connectable mode with filter policy enabled.
 - On BLE_Privacy_Central press button Push1: scan is initiated and connection is established without passing the password.

When not connected, a click on button Push2 allows clearing the security database.

### __Button Actions Summary__

| Device                 | Button | Action                                                                                    |
|------------------------|--------|-------------------------------------------------------------------------------------------|
| BLE_Privacy_Peripheral | Push1  | Initiates advertising, adds bonded devices to filter list, disconnects bonded devices     |
| BLE_Privacy_Peripheral | Push2  | Clears the security database                                                              |
| BLE_Privacy_Central    | Push1  | Initiates scan, establishes connection, reconnects to Peripheral, scans without password  |
| BLE_Privacy_Central    | Push2  | Clears the security database                                                              |
| BLE_Privacy_Central    | Push3  | Disconnects the device                                                                    |


### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
