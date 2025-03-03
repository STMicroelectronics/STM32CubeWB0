## __BLE_PowerControl_Peripheral Application Description__

This example shows the LE Power Control features:
- Dynamic adjustment of TX power level on a connection, based on RSSI
- TX power reporting
- Path loss monitoring.

### __Keywords__

Connectivity, BLE, BLE protocol

### __Hardware and Software environment__

  - This application runs on STM32WB0 Nucleo board.
  - Another STM32WB0 Nucleo board is necessary to run BLE_PowerControl_Central application.
    
### __How to use it?__

In order to make the program work, you must do the following :

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Place BLE_PowerControl_Peripheral device next to a BLE_PowerControl_Central device.
 - A connection is initiated if a BLE_PowerControl_Central device is detected.
 - Once the BLE_PowerControl_Peripheral application is connected to a BLE_PowerControl_Central:
    - On BLE_PowerControl_Peripheral device, LED2 is turned On.
    - On BLE_PowerControl_Central device, LED2 is turned On.
  
### __Notes__
  

### Procedure

The Bluetooth stack on each device uses power control procedures to adjust the power of the peer in order to have the signal always in the golden range. The target RSSI and the hysteresis are specified inside the `app_ble.c` file.

```C
/* Values for power control */
#define RSSI_TARGET_1M              -70  // dBm
#define RSSI_TARGET_2M              -67  // dBm
#define RSSI_TARGET_CODED_S8        -77  // dBm
#define RSSI_TARGET_CODED_S2        -73  // dBm
#define RSSI_HYSTERESIS               8  // dB

/* Values for path loss monitoring */
#define HIGH_THRESHOLD      74  // dB
#define HIGH_HYSTERESIS     6   // dB
#define LOW_THRESHOLD       55  // dB
#define LOW_HYSTERESIS      6   // dB
#define MIN_TIME            4   // Connection events
```

Each time there is a power change on the local or on the remote device, an `hci_le_transmit_power_reporting_event` is received and it is printed on the UART interface.

### Path Loss Monitoring

Path loss monitoring is also enabled: three path loss zones are defined (low, medium, and high). The LED1 blinks at different rates in each zone:
- High rate in low path loss zone (boards close to each other)
- Medium rate in medium path loss zone
- Low rate in high path loss zone (boards far from each other)

LED1 always ON means that no `hci_le_transmit_power_reporting_event` has been received. When a new zone is entered, this is also printed through UART.


### Terminal Output
The Central and the Peripheral show information for each action status, whether success or fail.

#### Central Output when connected near the Peripheral

```

  hci_le_path_loss_threshold_event - PATH LOSS: 47 dB (zone 0)
  hci_le_transmit_power_reporting_event - Local: -12 dBm (9 dB)
  hci_le_transmit_power_reporting_event - Remote: -16 dBm (-1 dB)

```

#### Central Output when it is connected far away from the Peripheral (about 5 meters), wait 30 Seconds

```

  hci_le_path_loss_threshold_event - PATH LOSS: 63 dB (zone 1)
  hci_le_transmit_power_reporting_event - Local: -3 dBm (10 dB)
  hci_le_transmit_power_reporting_event - Remote: 0 dBm (12 dB)

```

#### Central Output when it is connected far away from the Peripheral (about 20 meters), wait 30 Seconds

```

  hci_le_path_loss_threshold_event - PATH LOSS: 82 dB (zone 2)
  hci_le_transmit_power_reporting_event - Local: 6 dBm (9 dB)
  hci_le_transmit_power_reporting_event - Remote: 6 dBm (7 dB)

```

### LED and Push Button Behavior

| Behavior   | Central Device       | Peripheral Device    |
|------------|----------------------|----------------------|
| **LED1**   | Path loss monitoring | Path loss monitoring |
| **LED2**   | On: connected. Blinking: initiating state | On: connected. Blinking: advertising |
                                          
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
