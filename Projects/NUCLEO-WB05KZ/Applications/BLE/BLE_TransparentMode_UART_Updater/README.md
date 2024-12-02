## __BLE_TransparentMode_UART_Updater Application Description__

Updater application to be used with BLE_TransparentMode_UART_for_Updater.

### __Keywords__

Connectivity, BLE, BLE protocol

### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
  
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 
    
### __How to use it ?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain 
 - Rebuild all files and flash the board with the executable file.
 - Load the BLE_TransparentMode_UART_for_Updater application.
 - Run the example

### BLE_TransparentMode with Updater capability

  - The BLE_TransparentMode_UART_for_Updater application is equivalent to BLE_TransparentMode (UART mode) application. The only difference is that the "with Updater" variant is built with an offset defined by linker configuration symbol MEMORY_FLASH_APP_OFFSET=4096.
 - This allows to use this application in combination with the BLE_TransparentMode_UART_Updater application which offers the capability to update the BLE_TransparentMode_UART_for_Updater application through the supported channel (UART).
 

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
