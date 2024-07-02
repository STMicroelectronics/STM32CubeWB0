## __BLE_ApplicationInstallManager Application Description__

The BLE_ApplicationInstallManager application, associated to a BLE application embedding OTA service, manages the firmware update over the air of a BLE application. 

### __Keywords__

Connectivity, BLE, BLE protocol, BLE firmware update over the air, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory

The BLE_ApplicationInstallManager application must run with BLE applications embedding OTA service like: BLE_HeartRate_ota or BLE_p2pServer_ota applications.

__BLE_ApplicationInstallManager__ is loaded at the memory address __0x10040000__ and __BLE_HeartRate_ota__ or __BLE_p2pServer_ota__ application is loaded at the memory address __0x10041000__.

For more details on OTA process please refers to associated readme.html.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
