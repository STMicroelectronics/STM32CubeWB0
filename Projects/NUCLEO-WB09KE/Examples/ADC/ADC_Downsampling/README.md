## <b>ADC_Downsampling Example Description</b>

How to use an ADC peripheral with downsampling.
This example is based on the STM32WB0x ADC HAL API.


Description of ADC downsampling:

Perform automatically multiple successive ADC conversions and average
computation, by ADC hardware, and therefore off-load the CPU for the
equivalent task.
This feature can be used for the functions: averaging, data rate reduction,
SNR improvement, basic filtering.

This example uses 3 configurations of downsampling, for comparison
of conversion data and evaluation of downsampling benefits.


Example configuration:

ADC is configured to convert a single channel, in single conversion mode,
from SW trigger.
ADC downsampling feature is used with 3 settings:

- ADC downsampling enabled: ratio 16 and width 12 BIT.
- ADC downsampling enabled: ratio 16 and width 16 BIT.
- ADC downsampling disabled.

Example execution:

From the main program execution, the ADC group regular converts the
selected channel successively with the 3 downsampling settings.

Then, data are evaluated:

- data range validity check
- user can evaluate the expected results of downsampling:
  ADC conversion data with downsampling enabled has less variation
  than with downsampling disabled.
- SW calculation of downsampled conversion data on 16 bits to obtain an accurate
  data equivalent to resolution 12 bits with floating point.

When all ADC conversions are completed, the cycle of 3 conversions restarts.

LD1 is used to monitor program execution status:

- Normal operation: LED toggle at 1Hz (toggle at the end of conversions cycle)
- Error: LED remaining turned on

Debug: variables to monitor with debugger:

- "uhADCxConvertedData_OVS_ratio16_width_12_bit":   ADC group regular conversion data, downsampling ratio 16  (data scale: 12 bits)
- "uhADCxConvertedData_OVS_ratio16_width_16_bit":   ADC group regular conversion data, downsampling ratio 16  (data scale: 16 bits)
- "uhADCxConvertedData_OVS_disabled":  ADC group regular conversion data, downsampling disabled (data scale corresponds to ADC resolution: 12 bits)
- "fConvertedData_OVS_EquivalentValue12bits": Calculation of downsampling raw data on 16 bits (from variable "uhADCxConvertedData_OVS_ratio16_width_16_bit") to the equivalent data on resolution 12 bits with floating point.

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).
Note: If no power supply available, this pin can be let floating (in this case
      ADC conversion data will be undetermined).

Other peripherals used:

  1 GPIO for LD1

  1 GPIO for analog input: ADC channel P0 on pin PB.03 (Arduino connector CN6 pin 6 D13, Morpho connector CN4 pin 11)

#### <b>Notes</b>
                                            
 1. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, downsampling

### <b>Directory contents</b>

  - ADC/ADC_Downsampling/Inc/stm32wb0x_nucleo_conf.h     BSP configuration file
  - ADC/ADC_Downsampling/Inc/stm32wb0x_hal_conf.h    HAL configuration file
  - ADC/ADC_Downsampling/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - ADC/ADC_Downsampling/Inc/main.h                  Header for main.c module
  - ADC/ADC_Downsampling/Src/stm32wb0x_it.c          Interrupt handlers
  - ADC/ADC_Downsampling/Src/stm32wb0x_hal_msp.c     HAL MSP module
  - ADC/ADC_Downsampling/Src/main.c                  Main program
  - ADC/ADC_Downsampling/Src/system_stm32wb0x.c      STM32WB0x system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WB09KEVx devices.

  - This example has been tested with NUCLEO-WB09KE board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example