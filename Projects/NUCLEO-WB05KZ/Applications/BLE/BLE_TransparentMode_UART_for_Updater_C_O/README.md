## __BLE_TransparentMode_UART_for_Updater_C_O Application Description__

How to use Bluetooth LE stack running on STM32WB0 device configured as a network coprocessor, controller only (UART mode)

### __Keywords__

Connectivity, BLE, BLE protocol

### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
  
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 
    
### __How to use it ?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain 
 - Rebuild all files and flash the board with the executable file.
 - Run the example
 - NOTE: This application assumes that BLE_TransparentMode_UART_Updater has been already loaded on the device at Flash base address 0x10040000.

To test the BLE Transparent Mode application, open the STM32CubeMonitor-RF PC tool:

  - Open the COM port associated to the board running the BLE_TransparentMode application
  - You can send all the ACI commands supported by the Bluetooth LE stack, either standard, such as HCI_RESET, HCI_LE_RECEIVER_TEST, HCI_LE_TRANSMITTER_TEST,
    or proprietary, such as ACI_GAP_* and and ACI_GATT_* commands.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
