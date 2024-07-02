;********************************************************************************
;* File Name          : startup_stm32wb09.s
;* Author             : MCD Application Team
;* Description        : STM32WB09 Ultra Low Power Devices vector
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == _iar_program_start,
;*                      - Set the vector table entries with the exceptions ISR
;*                        address.
;*                      - Branches to main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the Cortex-M0+ processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;********************************************************************************
;* @attention
;*
;* Copyright (c) 2024 STMicroelectronics.
;* All rights reserved.
;*
;* This software is licensed under terms that can be found in the LICENSE file
;* in the root directory of this software component.
;* If no LICENSE file comes with this software, it is provided AS-IS.
;*
;*******************************************************************************
;
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

Stack_Size		EQU     0xC00

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size      EQU     0x000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __vector_table
                EXPORT  __vector_table_End
                EXPORT  __vector_table_Size
		
__vector_table  DCD     __initial_sp               ; Top of Stack
				DCD     Reset_Handler             ; Reset Handler
				DCD     NMI_Handler               ; NMI Handler
				DCD     HardFault_Handler         ; Hard Fault Handler
				DCD     0                         ; Reserved
				DCD     0                         ; Reserved
				DCD     0                         ; Reserved
				DCD     0                         ; Reserved
				DCD     0                         ; Reserved
				DCD     0                         ; Reserved
				DCD     0                         ; Reserved
				DCD     SVC_Handler               ; SVCall Handler
				DCD     0                         ; Reserved
				DCD     0                         ; Reserved
				DCD     PendSV_Handler            ; PendSV Handler
				DCD     SysTick_Handler           ; SysTick Handler
	
				; External Interrupts
				DCD FLASH_IRQHandler              ; IRQ0:  FLASH Controller interrupt
				DCD RCC_IRQHandler                ; IRQ1:  RCC interrupt
				DCD PVD_IRQHandler                ; IRQ2:  PVD interrupt
				DCD I2C1_IRQHandler               ; IRQ3:  I2C1 interrupt
				DCD 0x00000000                    ; IRQ4:  Reserved
				DCD 0x00000000                    ; IRQ5:  Reserved
				DCD 0x00000000                    ; IRQ6:  Reserved
				DCD SPI3_IRQHandler               ; IRQ7:  SPI3 interrupt
				DCD USART1_IRQHandler             ; IRQ8:  USART1 interrupt
				DCD LPUART1_IRQHandler            ; IRQ9:  LPUART1 interrupt
				DCD TIM2_IRQHandler               ; IRQ10: TIM2 interrupt
				DCD RTC_IRQHandler                ; IRQ11: RTC interrupt
				DCD ADC_IRQHandler                ; IRQ12: ADC interrupt
				DCD PKA_IRQHandler                ; IRQ13: PKA interrupt
				DCD 0x00000000                    ; IRQ14: Reserved
				DCD GPIOA_IRQHandler              ; IRQ15: GPIOA interrupt
				DCD GPIOB_IRQHandler              ; IRQ16: GPIOB interrupt
				DCD DMA_IRQHandler                ; IRQ17: DMA interrupt
				DCD RADIO_TXRX_IRQHandler         ; IRQ18: RADIO Tx/Rx interrupt
				DCD 0x00000000                    ; IRQ19: Reserved
				DCD RADIO_TIMER_ERROR_IRQHandler  ; IRQ20: RADIO_TIMER Error interrupt
				DCD 0x00000000                    ; IRQ21: Reserved
				DCD 0x00000000                    ; IRQ22: Reserved
				DCD RADIO_TIMER_CPU_WKUP_IRQHandler  ; IRQ23: RADIO_TIMER CPU Wakeup interrupt
				DCD RADIO_TIMER_TXRX_WKUP_IRQHandler ; IRQ24: RADIO_TIMER Wakeup interrupt
				DCD RADIO_TXRX_SEQ_IRQHandler     ; IRQ25: RADIO Tx/Rx Sequence interrupt
				DCD TIM16_IRQHandler              ; IRQ26: TIM16 interrupt
				DCD TIM17_IRQHandler              ; IRQ27: TIM17 interrupt
				DCD TRNG_IRQHandler               ; IRQ28: TRNG interrupt
				DCD 0x00000000                    ; IRQ29: Reserved
				DCD 0x00000000                    ; IRQ30: Reserved
				DCD 0x00000000                    ; IRQ31: Reserved

__vector_table_End

__vector_table_Size  EQU   __vector_table_End - __vector_table
	
				AREA    |.text|, CODE, READONLY
					

; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler              [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler\
                PROC
                EXPORT  NMI_Handler                [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP
SVC_Handler\
                PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler             [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  FLASH_IRQHandler           [WEAK]
				EXPORT  RCC_IRQHandler             [WEAK]
				EXPORT  PVD_IRQHandler             [WEAK]
				EXPORT  I2C1_IRQHandler            [WEAK]
				EXPORT  SPI3_IRQHandler            [WEAK]
				EXPORT  USART1_IRQHandler          [WEAK]
				EXPORT  LPUART1_IRQHandler         [WEAK]
				EXPORT  TIM2_IRQHandler            [WEAK]
				EXPORT  RTC_IRQHandler             [WEAK]
				EXPORT  ADC_IRQHandler             [WEAK]
				EXPORT  PKA_IRQHandler             [WEAK]
				EXPORT  GPIOA_IRQHandler           [WEAK]
				EXPORT  GPIOB_IRQHandler           [WEAK]
				EXPORT  DMA_IRQHandler             [WEAK]
				EXPORT  RADIO_TXRX_IRQHandler      [WEAK]
				EXPORT  RADIO_TIMER_ERROR_IRQHandler     [WEAK]
				EXPORT  RADIO_TIMER_CPU_WKUP_IRQHandler  [WEAK]
				EXPORT  RADIO_TIMER_TXRX_WKUP_IRQHandler [WEAK]
				EXPORT 	RADIO_TXRX_SEQ_IRQHandler  [WEAK]
				EXPORT  TIM16_IRQHandler           [WEAK]
				EXPORT  TIM17_IRQHandler           [WEAK]
				EXPORT  TRNG_IRQHandler            [WEAK]

FLASH_IRQHandler
RCC_IRQHandler
PVD_IRQHandler
I2C1_IRQHandler
SPI3_IRQHandler
USART1_IRQHandler
LPUART1_IRQHandler
TIM2_IRQHandler
RTC_IRQHandler
ADC_IRQHandler
PKA_IRQHandler
GPIOA_IRQHandler
GPIOB_IRQHandler
DMA_IRQHandler
RADIO_TXRX_IRQHandler
RADIO_TIMER_ERROR_IRQHandler
RADIO_TIMER_CPU_WKUP_IRQHandler
RADIO_TIMER_TXRX_WKUP_IRQHandler
RADIO_TXRX_SEQ_IRQHandler
TIM16_IRQHandler
TIM17_IRQHandler
TRNG_IRQHandler
                B       .

                ENDP

				ALIGN
;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

