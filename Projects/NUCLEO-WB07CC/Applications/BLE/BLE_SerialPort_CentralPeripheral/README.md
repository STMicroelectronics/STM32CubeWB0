## __BLE_SerialPort_CentralPeripheral Application Description__

Demonstrating Point-to-Point Communication with Bluetooth Low Energy (BLE) GATT.

The Nucleo board, when flashed with the BLE_SerialPort_CentralPeripheral application, operates as a GAP central and peripheral.
This setup allows for the demonstration of simple, two-way Bluetooth LE communication between two STM32WB0 Nucleo boards, emulating serial communication.
The application showcases the use of the BLE GATT component to establish and manage the connection, enabling data exchange between the devices.
The BLE_SerialPort_CentralPeripheral application acts as a router, meaning it can function as both a central and a peripheral.
To connect to each other, two devices with the same firmware are loaded onto STM32WB0 devices. The application starts in discovery mode, scanning for peripherals. Since the same application can also be a peripheral, at the end of the GAP discovery procedure, the application decides to enable advertising to be found by a central that is scanning. The devices connect each other automatically.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing


### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.


### __How to use it?__

To make the program work, you must do the following:

1. Open the project with your preferred toolchain.
2. Rebuild all files and flash the board with the executable file.

This demo shows how to implement simple 2-way Bluetooth LE communication between two STM32WB0 Nucleo boards emulating serial communication.

Two boards must be configured as Central and Peripheral with BLE_SerialCom_CentralPeripheral.

Open a serial terminal on both devices (ST Link Com Port, @115200 bauds).

### __Data Entry and Transmission__

Enter data using the keyboard (on either Central or Peripheral): when LF or CR is encountered, or the data buffer is full (20 bytes), the data is sent to the peer device.

### __LED States and Descriptions__

| __LED__ | __State__ | __Description__                      |
|---------|-----------|--------------------------------------|
| LED1    | On        | Device is connected as a Central.    |
| LED2    | On        | Device is connected as a Peripheral. |

### __Device Roles and Button Actions__

| __Device Role__ | __Button__ | __State__         | __Action__               |
|-----------------|------------|-------------------|--------------------------|
| __Peripheral__  | B1         | Idle              | Start advertising.       |
| __Peripheral__  | B2         | Not connected     | Clear bonding database.  |
| __Peripheral__  | B2         | Connected         | Send security request.   |
| __Central__     | B2         | Not connected     | Clear bonding database.  |
| __Central__     | B2         | Connected         | Send security request.   |
| __Central__     | B3         | Idle              | Start scanning.          |



### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
