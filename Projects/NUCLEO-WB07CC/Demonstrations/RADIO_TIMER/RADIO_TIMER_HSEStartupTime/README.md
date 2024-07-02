## __RADIO_TIMER_HSEStartupTime Demonstration Description__

How to calculate HSE startup time.

This code measures the time that the HSE Clock needs to become ready after waking up from sleep status. 
A pulse on a configurable GPIO, PB6, is generated when the HSE is ready. The startup time must be measured from when the voltage begins to rise on the VCAP pin to when the voltage on the test GPIO is high. 
The example also measures the startup time using the internal clock as a reference. In this case no instrument is needed to measure the time.

### __Notes__
 - The measurement must be taken at the minimum operating voltage, since start-up time increases at low voltage.

 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)

### __Keywords__

Connectivity, 2.4 GHz radio IP, 2.4 GHz radio IP timer

### __Hardware and Software environment__

  - This example runs on STM32WB07 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the project with your preferred toolchain
 - Rebuild all files and flash the board with the executable file
 - Run the example
