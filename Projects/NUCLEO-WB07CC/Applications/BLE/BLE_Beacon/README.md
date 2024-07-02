## __BLE_Beacon Application Description__

How to advertise 4 types of beacon ( tlm, uuid, url, iBeacon ).

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This example runs on STM32WB0 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - 4 Beacon types are available and have to be defined in app_con.h file through CFG_BEACON_TYPE macro, that can be set to one of the following values:
     - CFG_IBEACON (default)
     - CFG_EDDYSTONE_UID_BEACON_TYPE
     - CFG_EDDYSTONE_URL_BEACON_TYPE
     - CFG_EDDYSTONE_TLM_BEACON_TYPE
 - Rebuild all files and flash the board with the executable file.
 - On the smartphone, download and run a Beacon Scanner application.
 - For iBeacons, on iOS devices you need to specify the beacon UUID to search for: 00010203-0405-0607-0809-0a0b0c0d0e0f.
 - Start scanning .
 - The Beacon information are displayed on the smartphone.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
