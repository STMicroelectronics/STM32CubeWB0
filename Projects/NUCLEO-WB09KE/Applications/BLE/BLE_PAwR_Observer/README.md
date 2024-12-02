## __BLE_PAwR_Observer Application Description__

Demonstrate STM32WB0 acting as an Observer of a Periodic Advertising with Response (PAwR) train.
The device is able to receive commands from a periodic advertiser and send responses back. The used protocol is derived from standard Electronic Shelf
label profile but it is not fully compliant.

The device is addressed by using a Group ID and an ESL ID. They are statically assigned, for the sake of simplicity (see GROUP_ID and ESL_ID in
esl_profile.h). GROUP_ID and ESL_ID can be defined at project level to change their value (for this demo GROUP_ID and ESL_ID must be between 0 and 2).

The application starts as a simple Peripheral advertising (it uses extended advertising, even though legacy advertising is still possible).
Once a Central gets connected to the Peripheral, the Peripheral can receive the Periodic Advertising Synchronization Info so that it can synchronize with
the periodic advertising train and disconnect the link. In particular, the device will only synchronize with the PAwR subevent corresponding to the group.

Once synchronized with the PAwR train, the device can receive and execute some ESL-like commands.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE PAwR

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board is necessary to run BLE_PAwR_Broadcaster application.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example.
 - Optionally, a serial terminal (with settings 921600-8-N-1), can be opened to see some info.
 - Use another Nucleo board running BLE_PAwR_Broadcaster. The Broadcaster will try to connect to the Observer just to pass synchronization information.
 - Once the BLE_PAwR_Observer has been synchronized, AT commands can be sent from the PAwR Broadcaster specifying a GROUP_ID and a ESL_ID (defaults values for BLE_PAwR_Observer are 0, 0)

For more info on how to send commands from the PAwR broadcaster, see the related documentation.

When not synchronized with Access Point, after reset/power-up or after a loss of synchronization, the board is in advertising state for a limited amount of time (15 minutes). After exiting advertising state, press button 1 to start advertising again.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
