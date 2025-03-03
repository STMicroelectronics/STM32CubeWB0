## __BLE_RC_LongRange_Peripheral Application Description__

This demo shows how to control a remote device (e.g., to drive an actuator) using the STM32WB07 on Coded PHY (Bluetooth LE specifications v5.0) to reach longer distances.

### Demo Application Workflow

### Peripheral Device Initialization
- The Peripheral device starts by entering discoverable mode with a fixed address (0x0280E10000E1).
- LED2 blinks, indicating that the board is in advertising mode.
- LED3 is off, indicating that the board is using 1 Mbps PHY.
- Pressing PUSH1 starts a new advertisement process. Please note that if a Coded PHY advertisement is running, it will be terminated. If a connection has been successfully established before, it will also be terminated. If an advertisement with 1M PHY is running, no action will be performed.

### Central Device Initialization
- Pressing the push button PUSH1 on the Central device (address 0x0280E10000E0) starts the attempt to connect to the Peripheral board.
- LED2 blinks, indicating that the Central is trying to connect.
- LED3 is off, indicating that the board is using 1 Mbps PHY.

### Advertising and Scanning
- The Peripheral starts BLE advertising for 60 seconds initially. After this period, advertising can be restarted by pressing PUSH1 on the Peripheral.
- The Central starts the BLE scanning procedure by pressing PUSH1.

### Connection and Communication
- Once connected (LED2 remains on for both devices), the Central writes to a BLE characteristic to switch LED1 on and off on the Peripheral.
- The state of LED1 changes every 300 ms if all packets are received from the Central.
- The state of LED1 also changes on the Central side when the write to the characteristic is successful.

### Measuring Communication Range
- To measure the communication range, move the boards away from each other until LED1 on the Peripheral blinks at a constant rate. When the blinking rate of LED1 decreases, it indicates that several packets are being retransmitted by the Central, suggesting the boards are at the limit of a stable communication range.

### Switching PHY
- While connected, press button PUSH2 on one of the boards to start the procedure to change the PHY to Coded PHY.
- When the procedure completes (i.e., Coded PHY activated), a message is visible on the terminal.
- Pressing the PUSH2 button again on the same device switches the PHY back to 1 Mbps.
- LED3 is on, indicating that the board is using the Coded PHY.
- LED3 is off, indicating that the board is using 1 Mbps PHY.
- **Note**: When the PHY is switched by one device, the other device will not be able to switch the PHY. Once the first device sets the preferred PHY (either 1 Mbps or Coded), only that device will be able to continue switching between 1 Mbps and Coded PHY.

### Measuring Range with Coded PHY
- Users can measure the stable communication range using Coded PHY by repeating the test described above: move the boards away from each other until LED1 on the Peripheral blinks at a constant rate.
- An increased stable communication range compared to 1 Mbps PHY should be visible.

### Switching Advertising or Scanning PHY
- The PUSH3 button can also be used to start BLE advertising on the Peripheral device or BLE scanning on the Central device using the Coded PHY. If a connection or an advertisement process is ongoing, these will terminate before starting the new procedures.

### Additional Information
- On the Central device, some statistics on the link quality are printed.

### Terminal Output
- The Central and the Peripheral show information for each action status, whether success or fail.
- During the connection, the Central device shows the statistics of the radio activity every 10 seconds and will print one symbol * to the terminal for each packet transmitted.

#### Central Output when connected near the Peripheral

```
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        CRC errs         0%
        Missed packets   1%
        PER              1%
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        CRC errs         0%
        Missed packets   0%
        PER              0%
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
```

#### Central Output when it is connected far away from the Peripheral (about 20 meters), wait 30 Seconds

```
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        CRC errs         0%
        Missed packets  15%
        PER             15%
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        CRC errs         1%
        Missed packets  14%
        PER             14%
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        CRC errs         1%
        Missed packets  13%
        PER             14%
```

#### Central Output When the push button PHUS2 on the Peripheral is pressed while it is away from the Central, Wait 30 Seconds

```
* * * * * * * * * * * HCI_LE_META_EVT: 0x07
 gap_cmd_resp_release() HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE
  PHY changed, tx:3 rx:3 Coded Phy Led3 On
hci_le_phy_update_complete_event - HAL_RADIO_TIMER_StartVirtualTimer(&bleAppContext.Statistics_timer_Id, STATS_INTERVAL_MS);
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        CRC errs         0%
        Missed packets  13%
        PER             13%
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        CRC errs         0%
        Missed packets   8%
        PER              8%
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        CRC errs         0%
        Missed packets   7%
        PER              7%
```

### LED and Push Button Behavior

| Behavior   | Central Device | Peripheral Device |
|------------|----------------|-------------------|
| **LED1**   | Indicates a successful write to the BLE characteristic on the Peripheral. | Indicates successful packets are received from the Central. |
| **LED2**   | Blinks: BLE scanning ongoing. On: device connected. Off: device disconnected. | Blinks: BLE advertising ongoing. On: device connected. Off: device disconnected. |
| **LED3**   | On: Coded PHY. Off: 1M PHY | On: Coded PHY. Off: 1M PHY |
| **PUSH1**  | Starts the BLE scanning procedure 1M PHY. | If connected, disconnects and starts BLE advertising in 1M PHY. |
| **PUSH2**  | Switches PHY to Coded PHY when connected. | Switches PHY to Coded PHY when connected. |
| **PUSH3**  | Starts the BLE scanning procedure Coded PHY. | Starts BLE advertising in Coded PHY. |



### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE Controller Privacy

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board may be necessary to run BLE_RC_LongRange_Peripheral application.
    
### __How to use it?__

The demo requires two boards configured with the Central and Peripheral project configurations, respectively.

  - Power Supply: Boards can be powered from a USB cable.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
