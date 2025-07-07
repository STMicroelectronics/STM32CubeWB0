## __BLE_MultipleConnections_Peripheral Application Description__

This demo allows targeting a multiple connection scenario with a CentralPeripheral device (the Collector) which can connect to a given number of peer devices as a central and to a given number of peer devices as a peripheral.


### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - To test this scenario completely, at least three STM32WB0 Nucleo boards may be necessary to run the BLE_MultipleConnections_CP, BLE_MultipleConnections_Central, and BLE_MultipleConnections_Peripheral applications.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 

 __You can interact with the BLE_MultipleConnections_Central, BLE_MultipleConnections_CP and BLE_MultipleConnections_Peripheral application with another Nucleo boards:__

A serial terminal can be opened on the associated COM port to show logs from the application.

| **Device**            | **Button** | **Short Press Action**                           | **Long Press Action**                               |
|-----------------------|------------|--------------------------------------------------|-----------------------------------------------------|
| **Central**           | PUSH1      | N/A                                              | Start the scan in general discovery mode            |
| **Central**           | PUSH2      | N/A                                              | Clear security DB                                   |
| **Central**           | PUSH3      | N/A                                              | Perform disconnection                               |
| **Peripheral**        | PUSH1      | Stop non disc. adv / Start general disc. adv     | N/A                                                 |
| **Peripheral**        | PUSH2      | Clear security DB                                | N/A                                                 |
| **Peripheral**        | PUSH3      | Perform disconnection                            | N/A                                                 |
| **CentralPeripheral** | PUSH1      | Stop non disc. adv / Start general disc. adv     | Start the scan in general discovery mode            |
| **CentralPeripheral** | PUSH2      | Clear security DB                                | Clear security DB                                   |
| **CentralPeripheral** | PUSH3      | Perform disconnection of all connected centrals  | Perform disconnection of all connected peripherals  |


### Scenario:
- to connect CentralPeripheral with Peripheral : press the PUSH1 button on the Peripheral and use a long press on the PUSH1 button on the CentralPeripheral device.
- to connect Central with CentralPeripheral : press the PUSH1 button on the CentralPeripheral and use a long press on the PUSH1 button on the Central device.


### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
