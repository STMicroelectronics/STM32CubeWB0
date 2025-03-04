## __BLE_p2pServer_StaticStack_ota Application Description__

Demonstrate how to build an application referencing a Bluetooth stack located in a separate area of Flash Memory. In this way, it is possible to update the application without the need to program the Bluetooth stack. Firmware Upgrade Over-The-Air (FUOTA) is also supported to update the application over Bluetooth. The new application is temporarily stored in a dedicated area of Flash memory (download area) and then copied by BLE_StaticStack_ota from the download area to the application area.

This example is derived from BLE_p2pServer_ota and it keeps the original functionality. Refer to the documentation of BLE_p2pServer_ota for more information.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, Static Stack

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - There is also the need of either:
    - a smartphone (Android or iOS) for ST BLE Toolbox or ST BLE Sensor apps, or
    - a second STM32WB0 Nucleo board to run p2pClient or BLE_p2pRouter application.
    
### __How to use it?__

- To be able to run the demo, first program BLE_StaticStack_ota firmware (BLE_StaticStack_ota.hex from BLE_StaticStack_ota\\Binary folder) with STM32CubeProgrammer.
- Build BLE_p2pServer_StaticStack project with your preferred toolchain and load the image into target memory.
- Reset the board to start the application.

Refer to BLE_p2pServer_ota application for more info.
A raw binary file of the application is needed to test Firmware upgrade with the smartphone app. If the used toolchain does not generate a raw binary file, it can be obtained by converting an Intel HEX file with the help of an external tool, like the arm-none-eabi-objcopy (e.g. arm-none-eabi-objcopy -I ihex -O binary BLE_p2pServer_StaticStack_ota.hex BLE_p2pServer_StaticStack_ota.bin).

### __Notes__

## Converting existing application with FUOTA support to use BLE Static Stack

 This section describes the changes that are needed to convert an existing application embedding the traditional STM32WB0 Bluetooth stack library into one using a STM32WB0 Bluetooth stack located in a separated area of Flash memory. This steps are valid if application is already using FUOTA. If not, refer to documentation related to BLE_p2pServer_StaticStack.  
 BLE Static Stack and application must share the same stack modularity configuration and the same number of radio tasks. See BLE_StaticStack_ota to know the configuration used by BLE Static Stack with FUOTA support. If the application needs a different BLE configuration (e.g. a different set of features or a greater number or radio tasks), the BLE Static Stack must be rebuilt with the new configuration. See README inside BLE_StaticStack_ota for the steps to follow to correctly build a BLE_StaticStack_ota project.

- Remove stm32wb0x_ble_stack.a and ble_stack_user_cfg.c from build list.
- Add ble_static_stack_sym.a as a library. This is a file generated after BLE_StaticStack_ota is built.
  - BLE_p2pServer_StaticStack_ota uses ble_static_stack_sym.a from BLE_StaticStack_ota\\Binary folder. If BLE_StaticStack_ota is rebuilt, make sure to point to the new ble_static_stack_sym.a in BLE_StaticStack_ota folder.
- Add a copy of ble_static_stack_calls.c to the project. It can be found in BLE_p2pServer_StaticStack_ota\\STM32_BLE\\App.
- Replace original linker script with the one used by BLE_p2pServer_StaticStack_ota, which contains instructions for the placement of STATIC_STACK_APP_CALL_TABLE section.
- Take note of the following information from BLE_StaticStack_ota project:
  - RESET_MANAGER_SIZE and MEMORY_FLASH_APP_SIZE definitions (they have the same value)
  - first available location in RAM (excluding CSTACK or HEAP)
- Set CFG_ACTIVE_SLOT_START_SECTOR_INDEX in app_conf.h to the index of the first available sector in Flash. This can be easily found by using _RESET_MANAGER_SIZE/FLASH_PAGE_SIZE_, where the value of _RESET_MANAGER_SIZE_ is equal to the value of RESET_MANAGER_SIZE used in BLE_StaticStack_ota.
- Add the following definitions of symbols for the linker:
  - MEMORY_FLASH_APP_OFFSET=X, where X is equal to MEMORY_FLASH_APP_SIZE and RESET_MANAGER_SIZE in BLE Static Stack (e.g. if MEMORY_FLASH_APP_SIZE=0x2C000, then MEMORY_FLASH_APP_OFFSET=0x2C000).
  - MEMORY_RAM_APP_OFFSET=Y, where Y is an offset in RAM (compared to the base address 0x20000000) pointing to an area not used by BLE Static Stack. E.g., if R/W section in BLE Static Stack is allocated till address 0x20000270 excluded, set MEMORY_RAM_APP_OFFSET at least to 0x270.
- To protect the user from generating an image that does not fit inside the application area, the MEMORY_FLASH_APP_SIZE symbol can be defined for the linker. Its value can be calculated as follows:
  - _floor_((number_of_pages - CFG_NVM_STATIC_NB_SECTORS - CFG_USER_CFG_NB_SECTORS - CFG_ACTIVE_SLOT_START_SECTOR_INDEX) / 2) * flash_page_size,  
   where number_of_pages is the total number of Flash pages and flash_page_size is the size of one page (2048 bytes).
