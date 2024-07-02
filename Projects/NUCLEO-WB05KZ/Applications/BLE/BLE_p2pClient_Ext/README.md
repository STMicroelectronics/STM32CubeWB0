## __BLE_p2pClient_Ext Application Description__

Demontrates a BLE scanner with connections from an extended and a legacy advertising.

### __Keywords__

Connectivity, BLE, BLE protocol, extended

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
    Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK). 
  - Another STM32WB0 Nucleo board is necessary to run p2pServer_Ext application.
    
### __How to use it?__

In order to make the program work, you must do the following :

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
Open a terminal on Central and Peripheral side (ST Link Com Port, @115200 bauds).

At startup on BLE_p2pServer_Ext side, depending on the chosen configuration in app_conf.h, traces show the started advertising sets. 

The central device (BLE_p2pClient_Ext) scans and receives HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT from BLE_p2pServer_Ext device.

Result is printed over the UART and presented in a tab format with following information:

  - BD address
  - Advertising PDU Type (Legacy/Extended)
  - RSSI
  - Advertising event type (connectable, scannable, scan response)
  - Data length
  - SID
  - Data flags (more data, or data truncated).
 
Pressing the buttons will perform the following actions:

  - B1: Start/stop the scan (General Discovery procedure) when not connected. 
        Send a GATT write command toward connected p2pServer device to toggle blue LED,when connected, .
  - B2: Toggle a filter on extended advertising report only.
  - B3: Initiate connection request with latest detected p2pServer.
  
### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
