## __BLE_TransparentMode_SPI_for_Updater Application Description__

How to use Bluetooth LE stack running on STM32WB0 device configured as a network coprocessor with Updater (SPI mode).

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
 - NOTE: This application assumes that BLE_TransparentMode_SPI_Updater has been already loaded on the device at Flash base address 0x10040000. 

To test the BLE Transparent_SPI_for_Updater Mode application, open the STM32CubeMonitor-RF PC tool:

  - Open the COM port associated to the board running the BLE_TransparentMode_SPI_for_Updater application
  - You can send all the ACI commands supported by the Bluetooth LE stack, either standard, such as HCI_RESET,
    or proprietary, such as ACI_GAP_* and and ACI_GATT_* commands.

### BLE_TransparentMode_SPI_for_Updater Bluetooth LE configuration

  - BLE_TransparentMode_SPI_for_Updater application can be also configured in order to support the controller only mode for interacting with an external microcontroller providing the Bluetooth LE Host stack features. User is requested to rebuild the application by adding the BLESTACK_CONTROLLER_ONLY=1 on project preproprocessor options and refer to stm32wb0x_ble_stack_controller_only.a library.
  - Refer to the BLE_TransparentMode_SPI_for_Updater\Core\Inc\app_conf.h file, BLE Stack modularity options section for getting the specific Bluetooth LE features supported by the  BLE_TransparentMode_SPI_for_Updater application.
  - Refer to the BLE_TransparentMode_SPI_for_Updater\Core\Inc\app_conf.h file,  BLE Stack initialization parameters section for getting the specific Bluetooth LE stack initialization parameters used by the BLE_TransparentMode application.
  - Refer to the BLE_TransparentMode_SPI_for_Updater\Core\Inc\stm32wb0x_hal_conf.h file, CFG_NUM_RADIO_TASKS define for getting the Maximum number of simultaneous radio tasks supported  by the BLE_TransparentMode_SPI_for_Updater application.
  - NOTE: BLE Stack modularity options, Bluetooth LE stack initialization parameters and  CFG_NUM_RADIO_TASKS  values can be configured through the STM32CubeMX tool when defining an STM32_BLE application (STM32WB0 series Bluetooth LE middleware)


### BLE_TransparentMode_SPI_for_Updater SPI configuration

  - SPI configuration parameters are defined on MX_SPI3_Init(void) function on main.c file.

### BLE_TransparentMode with Updater capability

  - The BLE_TransparentMode_SPI_for_Updater applications is equivalent to BLE_TransparentMode_SPI (SPI mode) application. The only difference is that the "with Updater" variant is built with an offset defined by linker configuration symbol MEMORY_FLASH_APP_OFFSET=4096.
 - This allows to use this application in combination with the BLE_TransparentMode_SPI_Updater applications which offers the capability to update theBLE_TransparentMode_SPI_for_Updater applications through the supported channel (SPI). 


### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
