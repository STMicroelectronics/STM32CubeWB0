## __RADIO_Skeleton Example Description__

Code demonstrating the basic project structure template with initialization framework to be used for building a 2.4GHz RADIO example application.

#### __Notes__
                                            
 1. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)


### __Keywords__

Connectivity, 2.4 GHz RADIO IP

### __Hardware and Software environment__

  - This example runs on STM32WB09 device.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector. 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open the projects with your preferred toolchain
 - Customize the project for implementing the user specific example scenario using the 2.4 GHz radio proprietary driver
 - Rebuild all files and flash the board with the executable file.

