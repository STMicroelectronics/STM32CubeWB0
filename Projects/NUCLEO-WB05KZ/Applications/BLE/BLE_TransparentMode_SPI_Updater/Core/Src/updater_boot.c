/**
  ******************************************************************************
  * @file           : updater_boot.c
  * @brief          : updater_boot file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "stm32wb0x.h"
#include "compiler.h"
#include "transparent_mode_config.h"
#include "updater.h"
#include "system_stm32wb0x.h"

//uint32_t SystemCoreClock  = 64000000U; /*CPU: HSI clock after startup (64MHz)*/

//#define RESET_WAKE_DEEPSLEEP_REASONS 0x05
//#define CRITICAL_PRIORITY 0
//#if ST_OTA_SERVICE_MANAGER_APPLICATION
//#define OTA_VALID_APP_TAG (0xAABBCCDD) /* OTA Service Manager has a special valid tag */
//#else
//#define OTA_VALID_APP_TAG (0xAA5555AA) 
//#endif

#undef BLUE_FLAG_TAG
#define BLUE_FLAG_TAG   (0x00000000)

/* TRIMMING Defines */
#define VALIDITY_TAG 0xFCBCECCC

#define VALIDITY_LOCATION    0x10001EF8
#define TRIMMING_LOCATION    0x10001EE4

#define MAIN_REGULATOR_TRIM_Pos (0)
#define MAIN_REGULATOR_TRIM_Msk (0x0F << MAIN_REGULATOR_TRIM_Pos)
#define SMPS_TRIM_Pos           (4)
#define SMPS_TRIM_Msk           (0x07 << SMPS_TRIM_Pos)
#define LSI_LPMU_TRIM_Pos       (8)
#define LSI_LPMU_TRIM_Msk       (0x0F << LSI_LPMU_TRIM_Pos)
#define LSI_BW_TRIM_Pos         (12)
#define LSI_BW_TRIM_Msk         (0x0F << LSI_BW_TRIM_Pos)
#define HSI_TRIM_Pos            (16)
#define HSI_TRIM_Msk            (0x3F << HSI_TRIM_Pos)

/* Interrupt Vector Table */
#define SYSTEM_CONFIG_SMPS_READY_ERROR 0x01
#define SYSTEM_CONFIG_LSE_READY_ERROR  0x02
#define SYSTEM_CONFIG_LSI_READY_ERROR  0x03
#define SYSTEM_CONFIG_HSE_READY_ERROR  0x04
#define SYSTEM_CONFIG_PLL_READY_ERROR  0x05

NORETURN_FUNCTION(void NMI_IRQHandler(void))
{
  while (1);
}
NORETURN_FUNCTION(void HardFault_IRQHandler(void))
{
  while (1);
}
void SVC_IRQHandler(void)
{
  while (1);
}
void PendSV_IRQHandler(void)
{
  while (1);
}
void SysTick_IRQHandler(void)
{
  while (1);
}

#if defined(__ICCARM__) || defined(__IAR_SYSTEMS_ASM__)
#define RESET_HANDLER                  __iar_program_start
#else
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100))

extern void __main(void);
extern int main(void);
extern unsigned int Image$$ARM_LIB_STACK$$ZI$$Limit;
#define _INITIAL_SP                         (void(*)(void))&Image$$ARM_LIB_STACK$$ZI$$Limit  /* Stack address */
#define VARIABLE_SIZE 1


__attribute__((noreturn)) void RESET_HANDLER(void)
{
  __set_MSP((uint32_t)_INITIAL_SP);
  main();
  while(1);
}

/**
* @brief RADIO MSP Initialization
* This function configures the hardware resources used in this example
* @param hradio: RADIO handle pointer
* @retval None
*/
void HAL_RADIO_MspInit(RADIO_HandleTypeDef* hradio) { }
uint32_t blue_unit_conversion(uint32_t time, uint32_t period_freq,
                              uint32_t thr) { return 0;}
#else
#ifdef __GNUC__

#include <stdint.h>
#include <errno.h>

extern unsigned long _etext;
extern unsigned long _sidata;       /* start address for the initialization values of the .data section. Defined in linker script */
extern unsigned long _sdata;        /* start address for the .data section. Defined in linker script */
extern unsigned long _edata;        /* end address for the .data section. Defined in linker script */
extern unsigned long _sbss;         /* start address for the .bss section. Defined in linker script */
extern unsigned long _ebss;         /* end address for the .bss section. Defined in linker script */
extern unsigned long _sbssblue;     /* start address for the section reserved for the Blue controller. Defined in linker script */
extern unsigned long _ebssblue;     /* end address for the section reserved for the Blue controller. Defined in linker script */
extern unsigned long _estack;       /* init value for the stack pointer. Defined in linker script */
extern unsigned long _sidata2;      /* start address for the initialization values of the special ram_preamble */
extern unsigned long _sdata2;       /* start address the special ram_preamble defined in linker script */
extern unsigned long _edata2;       /* end address the special ram_preamble defined in linker script */
extern uint8_t _sheap;              /* start address for the .heap section. Defined in linker script */
extern uint8_t _eheap;              /* end address for the .heap section. Defined in linker script */

extern int main(void);

void RESET_HANDLER(void)
{
//  if(__low_level_init()==1)
  {
    unsigned long *pulSrc, *pulDest;

    // Copy the data segment initializers from flash to SRAM.
    pulSrc = &_sidata;
    for(pulDest = &_sdata; pulDest < &_edata; )
    {
      *(pulDest++) = *(pulSrc++);
    }

    // Zero fill the bss segment.
    for(pulDest = &_sbssblue; pulDest < &_ebssblue; )
    {
      *(pulDest++) = 0;
    }

    for(pulDest = &_sbss; pulDest < &_ebss; )
    {
      *(pulDest++) = 0;
    }
  }
  // Call the application's entry point.
  __set_MSP((uint32_t)_INITIAL_SP);
  main();
}

//void * _sbrk(int32_t incr)
//{
//  static uint8_t *heap_end;
//  uint8_t *prev_heap_end;
//
//  if (heap_end == 0)
//    heap_end = &_sheap;
//
//  prev_heap_end = heap_end;
//  if ((heap_end + incr) > (&_eheap))
//  {
////    write(1, "Heap and stack collision\n", 25);
////    abort();
//    errno = ENOMEM;
//    return (void *) -1;
//  }
//
//  heap_end += incr;
//
//  return prev_heap_end;
//}

#endif /* __GNUC__ */
#endif
#endif

SECTION(".intvec")
REQUIRED(const intvec_elem __vector_table[]) = {
    {.__ptr = _INITIAL_SP},                   /* Stack address                      */
    {RESET_HANDLER},           		            /* Reset handler is C initialization. */
    {NMI_IRQHandler},                         /* The NMI handler                    */
    {HardFault_IRQHandler},                   /* The hard fault handler             */
    {0},
    {(intfunc) BLUE_FLAG_TAG},                /* Reserved for blue flag DTM updater */ 
};

/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SmpsTrimConfig(void);

/* Exported variables ---------------------------------------------------------*/
//SECTION(".ram_vr")
//NO_INIT(REQUIRED(RAM_VR_TypeDef RAM_VR));


/**
  * @brief  SMPS and Trimming value Configuration 
  */
static void SmpsTrimConfig(void)
{
  uint32_t main_regulator, smps_out_voltage, lsi_bw, hsi_calib;
#if defined(STM32WB06) || defined(STM32WB07)
  uint32_t lsi_lpmu;
#endif
  uint8_t eng_lsi_bw_flag;
  
  /* After reset wait until SMPS is ready */
  while(LL_PWR_IsSMPSReady() == 0);
  
  /* Configure SMPS BOM */
#ifdef CONFIG_HW_SMPS_10uH
  LL_PWR_SetSMPSBOM(LL_PWR_SMPS_BOM3);
  /* SMPS clock 4 MHz configuration */
  LL_RCC_SetSMPSPrescaler(LL_RCC_SMPS_DIV_4);
#else
#ifdef CONFIG_HW_SMPS_2_2uH
  LL_PWR_SetSMPSBOM(LL_PWR_SMPS_BOM2);
  /* SMPS clock 8 MHz configuration  */
  LL_RCC_SetSMPSPrescaler(LL_RCC_SMPS_DIV_2);
#else
#ifdef CONFIG_HW_SMPS_1_5uH
  LL_PWR_SetSMPSBOM(LL_PWR_SMPS_BOM1);
  /* SMPS clock 8 MHz configuration  */
  LL_RCC_SetSMPSPrescaler(LL_RCC_SMPS_DIV_2);
#else
#ifdef CONFIG_HW_SMPS_NONE
  /* SMPS NONE configuration will be done after the trimming configuration values */
#else
#warning "NO SMPS Configuration!!!"
#endif
#endif
#endif
#endif
  
  /* Retrieve Trimming values from engineering flash locations */
  if (*(volatile uint32_t*)VALIDITY_LOCATION == VALIDITY_TAG) {
    main_regulator    = ((*(volatile uint32_t*)TRIMMING_LOCATION) & MAIN_REGULATOR_TRIM_Msk) >> MAIN_REGULATOR_TRIM_Pos;
    smps_out_voltage  = ((*(volatile uint32_t*)TRIMMING_LOCATION) & SMPS_TRIM_Msk) >> SMPS_TRIM_Pos;
#if defined(STM32WB06) || defined(STM32WB07)
    lsi_lpmu          = ((*(volatile uint32_t*)TRIMMING_LOCATION) & LSI_LPMU_TRIM_Msk) >> LSI_LPMU_TRIM_Pos;
#endif /* defined(STM32WB06) || defined(STM32WB07) */
    lsi_bw            = ((*(volatile uint32_t*)TRIMMING_LOCATION) & LSI_BW_TRIM_Msk) >> LSI_BW_TRIM_Pos;
    hsi_calib         = ((*(volatile uint32_t*)TRIMMING_LOCATION) & HSI_TRIM_Msk) >> HSI_TRIM_Pos;
    eng_lsi_bw_flag   = TRUE;
  } else {
#if defined(STM32WB06) || defined(STM32WB07)
    main_regulator    = 0x08;
    lsi_lpmu          = 0x08;
    hsi_calib         = 0x1E;
    eng_lsi_bw_flag   = FALSE;
#endif /* defined(STM32WB06) || defined(STM32WB07) */
#if defined(STM32WB05) || defined(STM32WB09)
    main_regulator    = 0x0A;
    hsi_calib         = 0x1F;
    lsi_bw            = 8;
    eng_lsi_bw_flag   = TRUE;
#endif /* defined(STM32WB05) || defined(STM32WB09) */
    smps_out_voltage  = 0x03;
  }
  
  /* Set HSI Calibration Trimming value */
  LL_RCC_HSI_SetCalibTrimming(hsi_calib);
  
  /* Low speed internal RC trimming value set by software */
  if (eng_lsi_bw_flag)
    LL_RCC_LSI_SetTrimming(lsi_bw);
  
#if defined(STM32WB06) || defined(STM32WB07)
  /* Set LSI LPMU Trimming value */
  LL_PWR_SetLSILPMUTrim(lsi_lpmu);
#endif /* defined(STM32WB06) || defined(STM32WB07) */
  
  /* Set Main Regulator voltage Trimming value */ 
  LL_PWR_SetMRTrim(main_regulator);
  
  /* Set SMPS output voltage Trimming value */
  LL_PWR_SetSMPSTrim(smps_out_voltage);
  
  /* Set SMPS in LP Open */
  LL_PWR_SetSMPSOpenMode(LL_PWR_SMPS_LPOPEN);
  
#ifdef CONFIG_HW_SMPS_NONE
  /* No SMPS configuration */
  LL_PWR_SetSMPSMode(LL_PWR_NO_SMPS);
#endif
  
}

//void SystemCoreClockUpdate(void)
//{
//  uint8_t directHSE_enabled;
//  uint8_t divPrescaler;
//
//  /* Get SYSCLK source HSE or HSI+PLL64MHz */
//  directHSE_enabled = (RCC->CFGR & RCC_CFGR_HSESEL) >> RCC_CFGR_HSESEL_Pos;
//
//#if defined(STM32WB06) || defined(STM32WB07)
//  /* Get the clock divider */
//    divPrescaler = (RCC->CFGR & RCC_CFGR_CLKSYSDIV) >> RCC_CFGR_CLKSYSDIV_Pos;
//#else
//  /* Get the clock divider */
//  divPrescaler = (RCC->CFGR & RCC_CFGR_CLKSYSDIV_STATUS) >> RCC_CFGR_CLKSYSDIV_STATUS_Pos;
//#endif /* defined(STM32WB06) || defined(STM32WB07) */
//
//  if (directHSE_enabled)
//  {
//    SystemCoreClock = HSE_VALUE >> (divPrescaler - 1U);
//  }
//  else
//  {
//    SystemCoreClock = HSI_VALUE >> divPrescaler;
//  }  
//}

/*!< SMPS Configuration Defines */
#if !defined(CFG_HW_SMPS)
#define CFG_HW_SMPS SMPS_ON
#endif
                                             
#if !defined(CFG_HW_SMPS_BOM)
#define CFG_HW_SMPS_BOM SMPS_BOM3 /*!< SMPS Inductor 10uH */
#endif

#if !defined(CFG_HW_SMPS_LOW_POWER)
#define CFG_HW_SMPS_LOW_POWER SMPS_LOW_POWER_OPEN
#endif

void DTM_SystemInit(void)
{
  /* Remap the vector table */
  LL_FLASH_SetVectTable(FLASH, FLASH_CONFIG_IRQ_VECT_TABLE_FLASH);
  
  /* Vector Table Offset Register */
  SCB->VTOR = (uint32_t) (__vector_table);
  //RAM_VR.AppBase = (uint32_t) SCB->VTOR;
  
  /* Enable all the RAM banks in retention during DEEPSTOP */
  SET_BIT(PWR->CR2, PWR_CR2_RAMRET1);
#if defined(LL_PWR_RAMRET_2)
  SET_BIT(PWR->CR2, PWR_CR2_RAMRET2);
#endif
#if defined(LL_PWR_RAMRET_3)
  SET_BIT(PWR->CR2, PWR_CR2_RAMRET3);
#endif
  
  /* HW SMPS and HW Trimming value Configuration */
  SmpsTrimConfig();
  
  /* Low Speed Crystal Configuration */
  LL_RCC_LSCO_SetSource(LL_RCC_LSCO_CLKSOURCE_LSI);
  LL_RCC_LSI_Enable();
  while (LL_RCC_LSI_IsReady() == 0U);
  
  LL_RCC_HSE_SetCurrentControl(LL_RCC_HSE_CURRENTMAX_3);

  /* System Clock Configuration */
  LL_RCC_HSE_Enable();
  while (LL_RCC_HSE_IsReady() == 0U);
  
  LL_RCC_SetRC64MPLLPrescaler(LL_RCC_RC64MPLL_DIV_2);
  
  LL_RCC_RC64MPLL_Enable();
  while(LL_RCC_RC64MPLL_IsReady() == 0U);
  
//  LL_FLASH_SetWaitStates(FLASH, LL_FLASH_WAIT_STATES_0);
}
