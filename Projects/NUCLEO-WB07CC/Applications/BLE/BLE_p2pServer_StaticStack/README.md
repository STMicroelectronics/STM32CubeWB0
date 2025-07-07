## __BLE_p2pServer_StaticStack Application Description__

Demonstrate how to build an application referencing a Bluetooth stack located in a separate area of Flash Memory. In this way, it is possible to update the application without the need to program the Bluetooth stack.

This example is derived from BLE_p2pServer and it keeps the original functionality. Refer to the documentation of BLE_p2pServer for more information.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, Static Stack

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - There is also the need of either:
    - a smartphone (Android or iOS) for ST BLE Toolbox or ST BLE Sensor apps, or
    - a second STM32WB0 Nucleo board to run p2pClient or BLE_p2pRouter application.
    
### __How to use it?__

- To be able to run the demo, first program BLE_StaticStack firmware (BLE_StaticStack.hex from BLE_StaticStack\\Binary folder) with STM32CubeProgrammer.
- Build BLE_p2pServer_StaticStack project with your preferred toolchain and load the image into target memory.
- Reset the board to start the application.

Refer to BLE_p2pServer application for more info.

_Important_: BLE_p2pServer_StaticStack project by default uses ble_static_stack_sym.a from BLE_StaticStack\\Binary directory, which contains the symbol table needed to correctly link the application to the BLE_StaticStack.hex firmware present in Binary folder. If application does not use the precompiled BLE_StaticStack.hex in Binary folder, make sure to use the correct ble_static_stack_sym.a file.

### __Converting existing application to use BLE Static Stack__

 This section describes the changes that are needed to convert an existing application embedding the traditional STM32WB0 Bluetooth stack library into one using a STM32WB0 Bluetooth stack located in a separated area of Flash memory.  
 BLE Static Stack and application must share the same stack modularity configuration and the same number of radio tasks. See BLE_StaticStack to know the configuration used by BLE Static Stack. If the application needs a different BLE configuration (e.g. a different set of features or a greater number or radio tasks), the BLE Static Stack must be rebuilt with the new configuration. See README inside BLE_StaticStack for the steps to follow to correctly build a BLE_StaticStack project.

- Remove stm32wb0x_ble_stack.a and ble_stack_user_cfg.c from build list.
- Add ble_static_stack_sym.a as a library. This is a file generated after BLE_StaticStack is built.
  - BLE_p2pServer_StaticStack uses ble_static_stack_sym.a from BLE_StaticStack\\Binary folder. If BLE_StaticStack is rebuilt, make sure to point to the new ble_static_stack_sym.a in BLE_StaticStack folder.
- Add a copy of ble_static_stack_calls.c to the project. It can be found in BLE_p2pServer_StaticStack\\STM32_BLE\\App.
- Replace original linker script with the one used by BLE_p2pServer_StaticStack, which contains instructions for the placement of STATIC_STACK_APP_CALL_TABLE section.
- Take note of the following information from BLE_StaticStack project:
  - RESET_MANAGER_SIZE and MEMORY_FLASH_APP_SIZE definitions (they have the same value)
  - first available location in RAM (excluding CSTACK or HEAP)
- Add the following definitions of symbols for the linker:
  - MEMORY_FLASH_APP_OFFSET=X, where X is equal to MEMORY_FLASH_APP_SIZE and RESET_MANAGER_SIZE in BLE Static Stack (e.g. if MEMORY_FLASH_APP_SIZE=0x2C000, then MEMORY_FLASH_APP_OFFSET=0x2C000).
  - MEMORY_RAM_APP_OFFSET=Y, where Y is an offset in RAM (compared to the base address 0x20000000) pointing to an area not used by BLE Static Stack. E.g., if R/W section in BLE Static Stack is allocated till address 0x20000270 excluded, set MEMORY_RAM_APP_OFFSET at least to 0x270.

