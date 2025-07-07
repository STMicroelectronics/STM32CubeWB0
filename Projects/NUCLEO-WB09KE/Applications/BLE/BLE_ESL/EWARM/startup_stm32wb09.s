;********************************************************************************
;* File Name          : startup_stm32wb09.s
;* Author             : GPM MCD Application Team
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

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA
__vector_table
        DCD     sfe(CSTACK)
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
	DCD RADIO_TIMER_CPU_WKUP_IRQHandler   ; IRQ23: RADIO_TIMER CPU Wakeup interrupt
	DCD RADIO_TIMER_TXRX_WKUP_IRQHandler  ; IRQ24: RADIO_TIMER Wakeup interrupt
	DCD RADIO_TXRX_SEQ_IRQHandler     ; IRQ25: RADIO Tx/Rx Sequence interrupt
	DCD TIM16_IRQHandler              ; IRQ26: TIM16 interrupt
	DCD TIM17_IRQHandler              ; IRQ27: TIM17 interrupt
	DCD TRNG_IRQHandler               ; IRQ28: TRNG interrupt
	DCD 0x00000000                    ; IRQ29: Reserved
	DCD 0x00000000                    ; IRQ30: Reserved
	DCD 0x00000000                    ; IRQ31: Reserved

__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK FLASH_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FLASH_IRQHandler
        B FLASH_IRQHandler

        PUBWEAK RCC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RCC_IRQHandler
        B RCC_IRQHandler

        PUBWEAK PVD_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
PVD_IRQHandler
        B PVD_IRQHandler

        PUBWEAK I2C1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_IRQHandler
        B I2C1_IRQHandler

        PUBWEAK SPI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI3_IRQHandler
        B SPI3_IRQHandler

        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART1_IRQHandler
        B USART1_IRQHandler

	PUBWEAK LPUART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LPUART1_IRQHandler
        B LPUART1_IRQHandler

	PUBWEAK TIM2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM2_IRQHandler
        B TIM2_IRQHandler

	PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RTC_IRQHandler
        B RTC_IRQHandler

	PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ADC_IRQHandler
        B ADC_IRQHandler

	PUBWEAK PKA_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
PKA_IRQHandler
        B PKA_IRQHandler

	PUBWEAK GPIOA_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPIOA_IRQHandler
        B GPIOA_IRQHandler

	PUBWEAK GPIOB_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPIOB_IRQHandler
        B GPIOB_IRQHandler

	PUBWEAK DMA_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA_IRQHandler
        B DMA_IRQHandler

	PUBWEAK RADIO_TXRX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RADIO_TXRX_IRQHandler
        B RADIO_TXRX_IRQHandler

	PUBWEAK RADIO_TIMER_ERROR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RADIO_TIMER_ERROR_IRQHandler
        B RADIO_TIMER_ERROR_IRQHandler

	PUBWEAK RADIO_TIMER_CPU_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RADIO_TIMER_CPU_WKUP_IRQHandler
        B RADIO_TIMER_CPU_WKUP_IRQHandler

	PUBWEAK RADIO_TIMER_TXRX_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RADIO_TIMER_TXRX_WKUP_IRQHandler
        B RADIO_TIMER_TXRX_WKUP_IRQHandler

	PUBWEAK RADIO_TXRX_SEQ_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RADIO_TXRX_SEQ_IRQHandler
        B RADIO_TXRX_SEQ_IRQHandler

	PUBWEAK TIM16_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM16_IRQHandler
        B TIM16_IRQHandler

	PUBWEAK TIM17_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM17_IRQHandler
        B TIM17_IRQHandler
	
	PUBWEAK TRNG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TRNG_IRQHandler
        B TRNG_IRQHandler

        END
