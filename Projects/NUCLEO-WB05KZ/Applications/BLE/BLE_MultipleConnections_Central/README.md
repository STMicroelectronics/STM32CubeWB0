## __BLE_MultipleConnections_Central Application Description__

Demonstrate STM32WB0 acting as GATT client.

The BLE_MultipleConnections_Central application scans for advertising and waits for a connection from either:
- The BLE_MultipleConnections_Central application running on an STM32WB0 device.

Pairing with pass key entry method (with fixed pin) and bonding are used in this example.

Once connected, BLE_MultipleConnections_Central can receive write messages from the Central and it will send notifications periodically to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run BLE_MultipleConnections_Central application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 

 __You can interact with the BLE_MultipleConnections_Central application with another Nucleo board:__

This demo allows targeting a multiple connection scenario with a CentralPeripheral device (the Collector) which can connect to a given number of peer devices as a central and to a given number of peer devices as a peripheral.

The CentralPeripheral device automatically tries to connect to a set of known peer devices (the Nodes) to which it is bonded.

A serial terminal can be opened on the associated COM port to show logs from the application.

### Scenario:
- To bond with a new device, press the PUSH1 button on the Peripheral and use a long press on the PUSH1 button on the CentralPeripheral device.
- Once the two devices are connected, a bond is created. From that moment on, the CentralPeripheral will try to connect to the bonded device even after disconnection, and the Peripheral device will only allow connection from the bonded CentralPeripheral.
- To make the CentralPeripheral connectable as a peripheral to a central device (the Inquirer), press the PUSH1 button on the CentralPeripheral device.
- Use a long press on the PUSH1 button on the Central device to detect the CentralPeripheral device and connect to it (pairing is performed and a bond is created).
- Use a long press on the PUSH2 button on the Central device to write to the characteristic where the central is connected. Use the PUSH2 button on the Peripheral to send a notification to all connected centrals.

### Central Device Actions/Commands:
- **Long Push1:**
  - Start the normal scan in general discovery mode.
  - **Note:** If you want to use the scan in general discovery mode, even if you are previously bonded with a peripheral, you must push Button 2 when not connected to clear the security database.
    (i.e., If Push1 fails to connect to the bonded device and you want to start the scan procedure in general discovery mode, you must first press Button B2 to clear the security database.)
- **Long Push2:**
  - If connected to a peripheral, start the procedure to write to the peripheral's characteristic.
  - If not connected, call the function to clear the security database.
  - **Note:** If you want to force the clearing of the security database, you must disconnect with Push3 before pushing Button 2.
- **Long Push3:**
  - Call the function to perform disconnection.

### Peripheral Device Actions/Commands:
- On device startup and after each disconnection complete event, if bonded devices are found in the list, start advertising in auto connection establishment mode (with filter list active).
- **Push1:**
  - If advertising has been started in auto connection establishment mode, stop this type of advertising.
  - Start advertising in general mode without a filter list to accept new central devices.
- **Push2:**
  - If connected to a central, start the procedure to send a notification to the central.
  - If not connected, call the function to clear the security database.
- **Push3:**
  - Call the function to perform disconnection.

### CentralPeripheral Actions/Commands:
- On device startup and after each disconnection complete event, if bonded devices are found in the list, start advertising in auto connection establishment mode (with filter list active).
- The peripheral component can accept a maximum of **MAX_PERIPHERAL_HANDLES** number of centrals.
- The advertising in auto connection with filter active is started at every disconnection complete event to allow all previously bonded centrals to re-connect.
- The central component can accept a maximum of **MAX_CENTRAL_HANDLES** number of peripherals.
- At every disconnection complete event of some central instance, an auto connection scan will be activated to re-connect automatically to the previously bonded peripheral.
- Normal Push is used for the peripheral side, Long Push for the central.
- **Push1:**
  - If advertising has been started in auto connection establishment mode, stop this type of advertising.
  - Start advertising in general mode without a filter list to accept new central devices.
- **Push2:**
  - If connected to one or more centrals, start the procedure to send a notification to all the connected centrals.
  - If not connected to any central, call the function to clear the security database.
- **Push3:**
  - Call the function to perform disconnection of all connected centrals.
- **Long Push1:**
  - Start the normal scan in general discovery mode to connect to new peripherals.
- **Long Push2:**
  - If connected to at least one peripheral, start the procedure to write to all the characteristics of the previously connected peripherals.
  - If not connected, call the function to clear the security database.
- **Long Push3:**
  - Call the function to perform disconnection of all connected peripherals.


| **Device**          | **Button** | **Short Press Action**                                      | **Long Press Action**                                      |
|---------------------|------------|-------------------------------------------------------------|------------------------------------------------------------|
| **Central**         | PUSH1      | N/A                                                         | Start the normal scan in general discovery mode            |
| **Central**         | PUSH2      | N/A                                                         | Write to the peripheral's characteristic / Clear security DB |
| **Central**         | PUSH3      | N/A                                                         | Perform disconnection                                       |
| **Peripheral**      | PUSH1      | Stop auto connection advertising / Start general advertising | N/A                                                        |
| **Peripheral**      | PUSH2      | Send notification to the central                            | N/A                                                        |
| **Peripheral**      | PUSH3      | Perform disconnection                                       | N/A                                                        |
| **CentralPeripheral** | PUSH1    | Stop auto connection advertising / Start general advertising | Start the normal scan in general discovery mode            |
| **CentralPeripheral** | PUSH2    | Send notification to all connected centrals                 | Write to all connected peripherals' characteristics / Clear security DB |
| **CentralPeripheral** | PUSH3    | Perform disconnection of all connected centrals             | Perform disconnection of all connected peripherals         |



### Sequence Diagram for Test Scenario

Participants:3 Central devices, 2 Peripheral devices, and 1 CentralPeripheral device.

#### CentralPeripheral in advertising and Central1 connection
| Step | CentralPeripheral  | Central1  | Central2  | Central3 | Peripheral1 | Peripheral2  |
|------|--------------------|-----------|-----------|----------|-------------|--------------|
|  01  | Push1              |           |           |          |             |              |
|  02  |                    | LongPush1 |           |          |             |              |
|  03  |                    | LongPush2 |           |          |             |              |
|  04  | Push2              |           |           |          |             |              |
|  05  |                    | LongPush3 |           |          |             |              |
|  06  |                    | LongPush1 |           |          |             |              |
|  07  |                    | LongPush2 |           |          |             |              |
|  08  | Push2              |           |           |          |             |              |
|  09  | Push3              |           |           |          |             |              |
|  10  |                    | LongPush1 |           |          |             |              |
|  11  |                    | LongPush2 |           |          |             |              |

#### CentralPeripheral in advertising and Central2 connection
| Step | CentralPeripheral  | Central1  | Central2  | Central3 | Peripheral1 | Peripheral2  |
|------|--------------------|-----------|-----------|----------|-------------|--------------|
|  12  | Push1              |           |           |          |             |              |
|  13  |                    |           | LongPush1 |          |             |              |
|  14  |                    |           | LongPush2 |          |             |              |
|  15  |                    |           | LongPush2 |          |             |              |
|  16  | Push2              |           |           |          |             |              |

#### CentralPeripheral in advertising and Central3 connection          
| Step | CentralPeripheral  | Central1  | Central2 | Central3  | Peripheral1 | Peripheral2  |
|------|--------------------|-----------|----------|-----------|-------------|--------------|
|  17  | Push1              |           |          |           |             |              |
|  18  |                    |           |          | LongPush1 |             |              |
|  19  |                    |           |          | LongPush2 |             |              |
|  20  | Push2              |           |          |           |             |              |

#### Peripheral1 in advertising and first CentralPeripheral connection
| Step | CentralPeripheral  | Central1  | Central2 | Central3  | Peripheral1 | Peripheral2  |
|------|--------------------|-----------|----------|-----------|-------------|--------------|
|  21  |                    |           |          |           | Push1       |              |
|  22  |                    |           |          |           |             |              |
|  21  | LongPush1          |           |          |           |             |              |
|  22  |                    |           |          |           | Push2       |              |
|  23  | LongPush2          |           |          |           |             |              |
|  24  |                    |           |          |           | Push3       |              |
|  25  |                    |           |          |           | Push2       |              |
|  26  | LongPush2          |           |          |           |             |              |

#### Peripheral2 in advertising and second CentralPeripheral connection
| Step | CentralPeripheral  | Central1  | Central2 | Central3  | Peripheral1 | Peripheral2  |
|------|--------------------|-----------|----------|-----------|-------------|--------------|
|  27  |                    |           |          |           |             | Push1        |
|  28  | LongPush1          |           |          |           |             |              |
|  29  |                    |           |          |           |             | Push2        |
|  30  | LongPush2          |           |          |           |             |              |


### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
