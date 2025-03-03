## __BLE_ANCS_CentralPeripheral Application Description__

The ANCS (Apple Notification Center Service) demo configures a STM32WB0x device as a Notification Consumer.
The purpose of the ANCS profile, Bluetooth LE Notification Consumer role is to give Bluetooth accessories a simple and convenient way to access many kinds of notifications that are generated on a Notification Provider.

The BLE_ANCS demo, after the reset, puts the Bluetooth LE device in advertising with device name "ANCS_Demo", and it sets the Bluetooth LE authentication requirements for enabling bonding.
When the device is connected and bonded with a Notification Provider, the demo configures the Bluetooth LE Notification Consumer device to discover the service and the characteristics of the Notification Provider. 
When the setup phase is completed, the Bluetooth LE device is configured as a Notification Consumer and is able to receive every notification sent from the Notification Provider.

The Apple Notification Center Service (ANCS) is a Bluetooth Low Energy (BLE) profile that allows Bluetooth accessories to access notifications generated on an iOS device.
The ANCS profile defines the roles of Notification Provider (NP) and Notification Consumer (NC).

Roles:
- Notification Provider (NP): Typically an iOS device (iPhone, iPad, etc.). Generates and sends notifications to the Notification Consumer.
- Notification Consumer (NC): STM32WB0x emulate a Bluetooth accessory (e.g., smartwatch, fitness tracker). Receives and processes notifications from the Notification Provider.

### Demo Application Workflow

#### Advertising and Connection

The NC starts advertising with a specific device name ("ANCS_Demo").
The NP scans for advertising devices and initiates a connection.

#### Bonding and Authentication

The NC sets authentication requirements to enable bonding.
The NP and NC complete the bonding process.

#### Service Discovery

The NC discovers the ANCS service and its characteristics on the NP.

#### Notification Handling

The NP sends notifications to the NC via the Notification Source characteristic.
The NC can request detailed information about notifications using the Control Point characteristic.
The NC receives detailed notification data via the Data Source characteristic.
The NC can perform actions on notifications (e.g., dismiss, respond) using the Control Point characteristic.

#### Example Use Case

A smartwatch (NC) receives a notification about an incoming call from an iPhone (NP).
The smartwatch displays the caller's name and allows the user to accept or reject the call directly from the watch.

#### Conclusion

The ANCS profile provides a standardized way for Bluetooth accessories to interact with notifications generated on iOS devices.
It enhances the user experience by allowing seamless access to notifications and enabling actions directly from the accessory.

### Peripheral Device Initialization

- The Peripheral device starts by entering discoverable mode.
- LED2 blinks, indicating that the board is in advertising mode.
- LED3 is on, indicating that the NC board is connected to the NP.
- Every time PUSH1 is pressed, each notification stored on the device is shown on the terminal one by one, and the user can perform a positive or negative action on the selected notification.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE ANCS

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run BLE_RC_LongRange_Peripheral application.
    
### __How to use it?__

The demo requires two boards configured with the Central and Peripheral project configurations, respectively.

  - Power Supply: Boards can be powered from a USB cable.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
