## __RADIO_TIMER_Counter Demonstration Description__

How to configure a radio timer to trigger periodic wakeups.

This code sets a timer and goes into sleep mode. The corresponding timeout triggers a wakeup event; then a new timer is set, and the device returns to sleep mode. 
LD2 blinks at every timeout event.

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)

### __Keywords__

Connectivity, 2.4 GHz radio IP, 2.4 GHz radio IP timer

### __Hardware and Software environment__

  - This example runs on STM32WB0 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Run the example
