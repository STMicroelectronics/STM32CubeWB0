## __BLE_DataThroughput_Server Application Description__

How to demonstrate Point-to-Point communication using BLE component (as GATT server or GATT client).

The Nucleo board flashed with BLE_DataThroughput_Server application acts as GATT Server and the Client could be located on a nucleo board flashed with BLE_DataThroughput_Client or in a phone, using the ST BLE Toolbox application.

Data transfer via notification from server to client or/and write from client to server. Use of a timer that enables to calculate the throughput at application level. 


### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile


### __Hardware and Software environment__

  - This example runs on STM32WB0 devices.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.


### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain 
 - Rebuild all files and flash the board with the executable file.
 - Two ways for using the application: 

__1. Use of two nucleo boards__ 

Two STM32WB0 nucleo boards are used, one central/client (flashed with BLE_DataThroughput_Client) and one peripheral/server (flashed with BLE_DataThroughput_Server). 

The GATT server board support a Data transfer service with a transmission characteristic that supports notification.

The GATT client board support a Data transfer by writing without response to the GATT server board Reception characteristic.

The board flashed with BLE_DataThroughput_Server is defined as GAP peripheral.

Open a serial terminal on Central and Peripheral side (ST Link Com Port, @115200 bauds).

At reset application initialization is done.

 - The peripheral device starts advertising.
 - Push B1 on central: the central device scans and automatically connects to the peripheral (use of CFG_DEV_ID_DT_SERVER). 
 - After connection:
	- the client starts to search the data transfer service and characteristic.
	- the client enables the notification of the transmission characteristic.

On server side when connected:

 - B1 starts (blue LED is ON)/stops (blue LED is OFF) notification.
 - B2 sends L2CAP connection update - toggle 26.25 ms/11.25 ms
 - B3 toggles PHY

On server side when idle:

 - B2 clears the database

On client side when connected:

 - B1 starts (blue LED is ON)/stops (blue LED is OFF) write without response.
 - B2 sends pairing request

On client side when idle:

 - B2 clears the database

On client terminal receiving the current notification, the number of bytes per second is displayed.

On server terminal receiving the current write without response, the number of bytes per second is displayed.


__2. Use of one nucleo board and a smartphone__

 Install the ST BLE Toolbox application on the Android or iOS device:
 
   - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
   - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>

One STM32WB0 Nucleo board is used as GATT Server and GAP peripheral (flashed with BLE_DataThroughput_Server).

This board can be connected with a smartphone supporting ST BLE Toolbox application.

Start a scan and choose DT_XX (XX LSB byte of BD address) board to connect.

Same use of buttons on server side.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
