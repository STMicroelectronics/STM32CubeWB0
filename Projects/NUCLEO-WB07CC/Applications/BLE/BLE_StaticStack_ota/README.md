## __BLE_StaticStack_ota Application Description__

Demonstrate how to build a Bluetooth stack for a STM32WB0 device that can be used by an application programmed in a separated area of Flash memory.
This gives the possibility to update only the application, without the need to update the area containing the Bluetooth stack.
This example supports also Firmware Upgrade Over-The-Air (FUOTA) of the application: a new application can be downloaded over Bluetooth and temporarily stored in a dedicated area of Flash memory. BLE_StaticStack_ota will then copy the downloaded application from the download area to the application area.

Binary folder contains the firmware binary and the symbol table required for an application using the BLE Static Stack.
The BLE stack is built using the following BLE stack configuration.

| BLE feature option                               | Value |
| ------------------------------------------------ | ----- |
| CFG_BLE_CONTROLLER_SCAN_ENABLED                  |   0   |
| CFG_BLE_CONTROLLER_PRIVACY_ENABLED               |   0   |
| CFG_BLE_SECURE_CONNECTIONS_ENABLED               |   1   | 
| CFG_BLE_CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED |   1   |
| CFG_BLE_CONTROLLER_2M_CODED_PHY_ENABLED          |   1   |
| CFG_BLE_CONTROLLER_EXT_ADV_SCAN_ENABLED          |   0   |
| CFG_BLE_L2CAP_COS_ENABLED                        |   0   |
| CFG_BLE_CONTROLLER_PERIODIC_ADV_ENABLED          |   0   |
| CFG_BLE_CONTROLLER_PERIODIC_ADV_WR_ENABLED       |   0   |
| CFG_BLE_CONTROLLER_CTE_ENABLED                   |   0   |
| CFG_BLE_CONTROLLER_POWER_CONTROL_ENABLED         |   0   |
| CFG_BLE_CONNECTION_ENABLED                       |   1   |
| CFG_BLE_CONTROLLER_CHAN_CLASS_ENABLED            |   0   |
| CFG_BLE_CONTROLLER_BIS_ENABLED                   |   0   |
| CFG_BLE_CONNECTION_SUBRATING_ENABLED             |   0   |
| CFG_BLE_CONTROLLER_CIS_ENABLED                   |   0   |

Moreover, this is the radio configuration.

| BLE radio configuration                          | Value |
| ------------------------------------------------ | ----- |
| CFG_NUM_RADIO_TASKS                              |   2   |

An application using the BLE Static Stack must enable the same BLE features that have been enabled when the BLE Static Stack has been built and it must configure the same number of radio tasks (CFG_NUM_RADIO_TASKS).
If a different set of features or a different number of radio tasks is required, the BLE Static Stack must be rebuilt with the new configuration.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, Static Stack

### __Hardware and Software environment__

This application runs on STM32WB0 Nucleo board.

To create the library file containing the symbol table used by the application, create_sym_lib.exe utility (inside Utilities folder) is used as a post-build step. This utility requires GNU ARM Embedded Toolchain to be installed and its binary folder to be added to the PATH environment variable.

These are the applications referenced by create_sym_lib.exe:
- arm-none-eabi-readelf
- arm-none-eabi-gcc
- arm-none-eabi-ar  

Open a Windows Command Prompt and try to invoke those commands to check if they are correctly installed.
    
### __How to use it?__

The set of feature supported by the STM32WB0 BLE stack can be customized by changing the BLE Stack modularity options in app_conf.h.
To reduce the footprint, commands can also be removed by removing their references from cmd_call_table array (in ble_cmd_if.c).

These are the steps needed to correctly generate the required files:

- Change MEMORY_FLASH_APP_SIZE in linker options to increase (or possibly reduce) the Flash memory area reserved for the BLE Static Stack.
- If MEMORY_FLASH_APP_SIZE has been changed, change accordingly also RESET_MANAGER_SIZE for C preprocessor (their values must be the same).
- Build.
  - create_sym_lib.exe utility is called as post build step to generate the required library with symbols to be referenced by the
  application.
- Take a note of the first available address in RAM (excluding CSTACK or HEAP) from map file. (e.g. 0x20000270). It has to be taken into account when defining MEMORY_RAM_APP_OFFSET inside application project.

The output files of the projects are:
- BLE_StaticStack_ota.hex: binary image to be loaded in Flash memory
- ble_static_stack_sym.a: library with symbol table to be referenced by application.

_Important_: BLE_p2pServer_StaticStack_ota project by default uses ble_static_stack_sym.a from BLE_StaticStack_ota\Binary directory. After generating the new binary, make sure that its symbol table (generated in BLE_StaticStack_ota folder) is referenced by the application.

### __Notes__

Refer to BLE_p2pServer_StaticStack_ota example for the steps to follow to run the application.
