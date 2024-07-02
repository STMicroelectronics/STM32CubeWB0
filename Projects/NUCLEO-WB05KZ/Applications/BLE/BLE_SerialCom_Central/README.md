## __BLE_SerialCom_Central Application Description__

How to demonstrate Point-to-Point communication using Bluetooth Low Energy L2CAP component. 

The Nucleo board flashed with BLE_SerialCom_Central application acts as GAP central.


### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, L2CAP


### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.


### __How to use it?__

In order to make the program work, you must do the following:

  - Open the project with your preferred toolchain 
  - Rebuild all files and flash the board with the executable file.
 
Two STM32WB0 nucleo boards are used. One board must be configured as a Central with BLE_SerialCom_Central application and the other as a Peripheral with BLE_SerialCom_Peripheral.

Data are transferred using L2CAP connection oriented channels (no additional GATT service is added).

Open a serial terminal on Central and Peripheral side (ST Link Com Port, @115200 bauds).

At reset, application initialization is done.

  - The peripheral device starts advertising.
  - Push B1 on central: the central device scans (searching for CFG_DEV_ID_BLE_COC in advertising data) and automatically  connects to the peripheral. 
  - After Bluetooth connection is established, the client requests to create an L2CAP Connection Oriented Channel. 
  - Enter data using keyboard (on either Central or Peripheral): when LF or CR is encountered or data buffer is full (20 bytes), data are sent to the peer in L2CAP SDUs (Service Data Units).
 
On server side when connected:

  - B2 sends slave security request

On server side when idle:

  - B2 clear database
    
On client side when connected:

  - B2 sends pairing request
  - B3 changes connection interval update
    
On client side when idle:

  - B2 clear database

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
