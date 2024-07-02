## __BLE_Skeleton Application Description__

Bluetooth Low Energy application skeleton (Bluetooth LE stack modular configuration and initialization parameters).

### __Keywords__

Connectivity, BLE, BLE protocol

### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
  
  - Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.
    
### __How to use it ?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain 
 - Customize the project for implementing the user specific demonstration scenario using the Bluetooth LE stack v3.x
 - Rebuild all files and flash the board with the executable file.
 - Run the example

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
