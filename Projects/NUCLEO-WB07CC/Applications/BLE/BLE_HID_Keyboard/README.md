## __BLE_HID_Keyboard Application Description__

How to use Human Interface Device over GATT profile for a keyboard as specified by the Bluetooth LE SIG.

The Nucleo board flashed with BLE_HID_Keyboard application acts as GAP peripheral.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
  - Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file

 Open a serial terminal on Peripheral side (ST Link Com Port, @115200 bauds).
 At reset, application initialization is done.
  - The peripheral device starts advertising.
  -	You can connect and pair the board with the BLE of a computer or a phone (android or IOS) that acts as GAP central. 
  - Enter data using keyboard on serial terminal: every chars are sent to peer device
		
 When central device disconnects, the BLE_HID_Keyboard device restart advertising and a reconnection is made.
 
### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
