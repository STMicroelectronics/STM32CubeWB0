## <b>FreeRTOS_Semaphore_LowPower Application Description</b>

This application demonstrates the use of FreeRTOS tickless low power mode and semaphores.

The system relies on wakeup pin PA0 to wake up from its low power state.

The thread blocks on a semaphore which causes the system to enter DEEP STOP mode.

Once the Button is pressed, an Enable wakeup on PA0 is triggered and the CPU returns to RUN mode.
The semaphore is released in PostSleepProcessing. The thread resumes execution and toggles the LED_GREEN.

Two leds LED_GREEN and LED_RED are used to monitor the system state as following:

 - LED_RED ON : system in Deepstop mode
 - LED_GREEN toggling : system in Run mode

#### <b>Expected success behavior</b>
Successful operation is marked by the LED_GREEN toggling every 500ms for 5 seconds and LED_RED OFF indicate system in Run mode
Then, when the system switches in Deepstop mode LED_RED ON and LED_GREEN OFF.
The LED_GREEN toggling each time user presses the user button.

#### <b>Error behaviors</b>
On failure, the LED_RED toggles.

#### <b>Assumptions if any</b>
None

#### <b>Known limitations</b>
None

### <b>Notes</b>
To measure the current consumption remove JP2 jumper and connect an amperemeter to JP2 to measure IDD current.

This example can not be used in DEBUG mode due to the fact that the Cortex-M0+ core is no longer clocked during low power mode so debugging features are disabled.

It is possible to reduce power consumption even further by configuring all pins as analog pins and disabling their clocks.

#### <b>FreeRTOS usage hints</b>
The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

### <b>Keywords</b>

FreeRTOS, RTOS, LowPower, Semaphore, Threading

### <b>Hardware and Software environment</b>

  - This application runs on STM32WB09KE devices.
  - This application has been tested with STMicroelectronics NUCLEO-WB09KE boards Revision MB2032-WB09-B01.
    and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open the project in your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
