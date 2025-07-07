## __BLE_ESL Application Description__

This example demonstrates how to use an STM32WB0 as Electronic Shelf Label device using Periodic Advertising with Responses (PAwR) feature. In particular, 
it implements the ESL role of the standard profile (Electronic Shelf Label, aka ESL).

The device is addressed by using a Group ID and an ESL ID, which are assigned by the Access Point.

The application starts as a simple Peripheral advertising.
Once a Central gets connected to the Peripheral, the Peripheral can receive the Periodic Advertising Synchronization Info so that it can synchronize with
the periodic advertising train and disconnect the link. In particular, the device will only synchronize with the PAwR subevent corresponding to the group.

Once synchronized with the PAwR train, the device can receive and execute ESL commands.

The following commands can be used for tests (working only if CFG_LPM_SUPPORTED is 0):
 - *ABSTIME*: Get Current Absolute Time
 - *SRVNEEDED*: Set Service Needed bit to True
 - *UNSYNC*: Set The ESL state to Unsynchronized

### __Keywords__

Connectivity, BLE, BLE protocol, BLE PAwR, ESL

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board is necessary to run BLE_ESL_AP application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory (erase all the Flash before downloading, to initialize sectors used to store images)
 - Run the example.
 - Optionally, a serial terminal (with settings 921600-8-N-1), can be opened to see some info.
 - Use another Nucleo board running BLE_ESL_AP. The centralized AP will try to connect to the ESL to send configuration settings
   to an ESL and to pass synchronization information.
 - Once the BLE_ESL has been synchronized, AT commands can be sent from the AP specifying a GROUP_ID and a ESL_ID. 

For more info on how to send commands from the AP, see the related documentation.

ATTENTION: ESL removes any bonding information after a reset is performed.

### __Notes__

When acting as a PAwR Observer, the Sleep Clock Accuracy must be under 50 ppm. The accuracy of the LSE on Nucleo-WB09KE meets this requirement in the range from 0 to 50 degree. If application needs to operate at a wider temperature range, it is suggested to enable the periodic calibration. LSI cannot be used.
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
