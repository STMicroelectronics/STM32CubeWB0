## __BLE_Power_Consumption Application Description__

Basic Bluetooth LE demo to demonstrate the device power consumption in specific scenarios (ADV 100 ms, ADV 1000 ms and CONNECTION).

### __Keywords__

Connectivity, BLE, BLE protocol, Power Consumption in ADV and CONN

### __Hardware and Software environment__

  - This example runs on STM32WB0 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.
    Connect the Nucleo Board to an amperometer in the JP2 connector.

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Run the example
 - To start the advertising:
   - Press B1 button Nucleo board to start the fast Advertising with interval 100 ms and take the power consumption figures in this scenario.
   - Press B2 button on the Nucleo board to start the fast Advertising with interval 1000 ms and take the power consumption figures in this scenario.
 - On the smartphone, download a BLE Scanner application
 - Enable Bluetooth communications
 - Run BLE Scanner application and start scanning
 - The device name is "Power Consunption Test"
 - Connect the BLE scanner application with the STM32WB0 and take the power consumption figures.
 - To disconnect the device press the button B3 on the Nucleo Board.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
