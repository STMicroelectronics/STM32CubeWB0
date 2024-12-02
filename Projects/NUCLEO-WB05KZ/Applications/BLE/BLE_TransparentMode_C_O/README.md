## __BLE_TransparentMode_C_O Application Description__

How to use Bluetooth LE stack running on STM32WB0 device configured as a network coprocessor controller only (UART mode).

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

To test the BLE Transparent_C_O Mode application, open the STM32CubeMonitor-RF PC tool:

  - Open the COM port associated to the board running the BLE_TransparentMode_C_O application
  - You can send all the ACI commands supported by the Bluetooth LE stack, either standard, such as HCI_RESET,
    or proprietary, such as ACI_GAP_* and and ACI_GATT_* commands.

### BLE_TransparentMode_C_O Bluetooth LE configuration

  - BLE_TransparentMode_C_O application is configured in order to support the controller only mode for interacting with an external microcontroller providing the Bluetooth LE Host stack features. The BLESTACK_CONTROLLER_ONLY=1 option is added on project preproprocessor options and refer to stm32wb0x_ble_stack_controller_only.a library.
  - Refer to the BLE_TransparentMode_C_O\Core\Inc\app_conf.h file, BLE Stack modularity options section for getting the specific Bluetooth LE features supported by the  BLE_TransparentMode_C_O application.
  - Refer to the BLE_TransparentMode_C_O\Core\Inc\app_conf.h file,  BLE Stack initialization parameters section for getting the specific Bluetooth LE stack initialization parameters used by the BLE_TransparentMode_C_O application.
  - Refer to the BLE_TransparentMode_C_O\Core\Inc\stm32wb0x_hal_conf.h file, CFG_NUM_RADIO_TASKS define for getting the Maximum number of simultaneous radio tasks supported  by the BLE_TransparentMode_C_O application.
  - NOTE: BLE Stack modularity options, Bluetooth LE stack initialization parameters and  CFG_NUM_RADIO_TASKS  values can be configured through the STM32CubeMX tool when defining an STM32_BLE application (STM32WB0 series Bluetooth LE middleware)

### BLE_TransparentMode_C_O UART configuration

  - Baudrate: 921600
  - Word Length: 8
  - Stop bits: 1
  - Parity: None
  - Mode: TX, RX
  - Flow Control: None
  - NOTE: These parameters are defined on MX_USART1_UART_Init(void) function on main.c file. 


### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
