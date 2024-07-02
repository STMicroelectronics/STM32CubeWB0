## <b>PKA_ECDSA_Verify Example Description</b>

How to determine if a given signature is valid regarding the Elliptic curve digital signature algorithm
(ECDSA).

For this example, a test vector have been extracted from National Institute of Standards and Technology (NIST)

 - Cryptographic Algorithm Validation Program (CAVP) in order to demonstrate the usage of the hal.
This reference files can be found under: "http://csrc.nist.gov/groups/STM/cavp/documents/dss/186-3ecdsatestvectors.zip (ZIP SigGen.rsp)"

This test vector has been chosen to demonstrate the behavior in a case where the input signature
is valid. A second input is provided where one element of the hash message has been modified to 
demonstrate the behavior in a case where the signature is invalid. 


In case of success, the LD1 (BLUE) is ON.
In case of any error, the LD1 (BLUE) is toggling slowly.

#### <b>Notes</b>
1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

 3. On Keil framework, the following compilation setting are applied :
    
        --diag_suppress L6312W          (Hide “Empty <type> region description for region <region>” warning)
        --diag_suppress L6314W          (Hide “No section matches pattern <module>(<section>” warning)
        --diag_suppress L6329W          (Hide “Pattern <module>(<section>) only matches removed unused sections” warning)
    
### <b>Keywords</b>
Security, PKA, NIST, CAVP, ECDSA verification

### <b>Directory contents</b>
  
  - PKA/PKA_ECDSA_Verify/Inc/stm32wb0x_hal_conf.h    HAL configuration file
  - PKA/PKA_ECDSA_Verify/Inc/stm32wb0x_it.h          Interrupt handlers header file
  - PKA/PKA_ECDSA_Verify/Inc/main.h                        Header for main.c module
  - PKA/PKA_ECDSA_Verify/Src/stm32wb0x_it.c          Interrupt handlers
  - PKA/PKA_ECDSA_Verify/Src/main.c                        Main program
  - PKA/PKA_ECDSA_Verify/Src/stm32wb0x_hal_msp.c     HAL MSP module 
  - PKA/PKA_ECDSA_Verify/Src/system_stm32wb0x.c      STM32WB0x system source file
  - PKA/PKA_ECDSA_Verify/Src/prime256v1.c                  Description of P-256 (ECDSA-256)
  - PKA/PKA_ECDSA_Verify/Inc/prime256v1.h                  Header for prime256v1.c
  - PKA/PKA_ECDSA_Verify/Src/SigVer.c                      Reflect the content of the test vector from SigVer.rsp
  - PKA/PKA_ECDSA_Verify/Src/SigVer.rsp                    Extract from NIST CAVP
  - PKA/PKA_ECDSA_Verify/Inc/SigVer.h                      Header of SigVer.c

### <b>Hardware and Software environment</b> 

  - This example runs on STM32WB09KEVx devices.
  
  - This example has been tested with an STMicroelectronics NUCLEO-WB09KE
    board and can be easily tailored to any other supported device 
    and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following:

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
 
