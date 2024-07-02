/**
  ******************************************************************************
  * @file    system_stm32wb0x.c
  * @author  GPM WBL Application Team
  * @brief   CMSIS Cortex-M0+ Device Peripheral Access Layer System Source File
  *
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
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32wb0x.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *   After each device reset the HSI (64 MHz) is used as system clock source.
  *   Then SystemInit() function is called, in "startup_stm32wb0x.s" file, to
  *   configure the system clock before to branch to main program.
  *
  *   This file configures the system clock as follows:
  *=============================================================================
  *-----------------------------------------------------------------------------
  *        System Clock source                     | HSI
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                              | 16000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                                | 16000000
  *-----------------------------------------------------------------------------
  *=============================================================================
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup STM32WB0x_system
  * @{
  */

/** @addtogroup STM32WB0x_System_Private_Includes
  * @{
  */

#include "stm32wb0x.h"
#include "app_common.h"

/**
  * @}
  */

/** @addtogroup STM32WB0x_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32WB0x_System_Private_Defines
  * @{
  */
#if !defined (HSE_VALUE)
#define HSE_VALUE     (32000000U) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined (HSI_VALUE)
#define HSI_VALUE     (64000000U) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

/* Note: Following vector table addresses must be defined in line with linker
         configuration. */
/*!< Uncomment the following line if you need to relocate the vector table
     anywhere in Flash or Sram, else the vector table is kept at the automatic
     remap of boot address selected */
/* #define USER_VECT_TAB_ADDRESS */

#if defined(USER_VECT_TAB_ADDRESS)
/*!< Uncomment the following line if you need to relocate your vector Table
     in SRAM else user remap will be done in FLASH. */
/* #define VECT_TAB_SRAM */
#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS   SRAM_BASE       /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x100. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x100. */
#else
#define VECT_TAB_BASE_ADDRESS   NVM_BASE        /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x100. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x100. */
#endif /* VECT_TAB_SRAM */
#endif /* USER_VECT_TAB_ADDRESS */

/****************************/
static void JumpFwApp( void );
static void BootModeCheck( void );
static void JumpSelectionOnPowerUp( void );
static void DeleteSlot( uint8_t page_idx );
static void MoveToActiveSlot( uint8_t page_idx );

typedef void (*fct_t)(void);

/******************************************************************************/

/*!< HW TRIMMING Defines */
#define VALIDITY_TAG      0xFCBCECCC  /*!< TAG to validate the content of the 
					   trimming area content. */
#define VALIDITY_LOCATION 0x10001EF8  /*!< ROM address of the the validity trimming values content. */

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

/******************************************************************************/
#define REBOOT_ON_FW_APP          (0x00)
#define REBOOT_ON_BLE_OTA_APP     (0x01)
#define REBOOT_ON_CPU2_UPGRADE    (0x02)

/* Compute start address where the User Configuration shall be located */
#define USER_CFG_SLOT_START_SECTOR_INDEX      (FLASH_PAGE_NUMBER - CFG_NVM_STATIC_NB_SECTORS - CFG_USER_CFG_NB_SECTORS)

/* Compute size in Page of Download and Active slots */
#define DOWNLOAD_ACTIVE_NB_SECTORS            (FLASH_PAGE_NUMBER - CFG_NVM_STATIC_NB_SECTORS - CFG_USER_CFG_NB_SECTORS)

/* Compute size in Page of Download or active slot */
#define APP_SLOT_PAGE_SIZE                    ((DOWNLOAD_ACTIVE_NB_SECTORS - CFG_ACTIVE_SLOT_START_SECTOR_INDEX) / 2)

/* Compute start address where the NEW application shall be downloaded */
#define DOWNLOAD_SLOT_START_SECTOR_INDEX      (APP_SLOT_PAGE_SIZE + CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
/******************************************************************************/

/**
  * @}
  */

/** @addtogroup STM32WB0x_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32WB0x_System_Private_Variables
  * @{
  */
  /* The SystemCoreClock variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
  uint32_t SystemCoreClock = 16000000U; /* The HSI (64MHz) is used as system clock source after startup from reset, configured at 16 MHz. */

  /* The RAM_VR variable is a mirroring in RAM of some registers information.
     It is a sort of virtual register in RAM.
  */
#if defined ( __ICCARM__ )
  #pragma location=".ram_vr"
  __root __no_init RAM_VR_TypeDef RAM_VR;
#else
#if defined ( __ARMCC_VERSION )
  __attribute__((section(".bss" ".ram_vr")))
#elif defined (  __GNUC__  )
  __attribute__((section(".ram_vr")))
#endif
  RAM_VR_TypeDef RAM_VR __attribute__((used));
#endif
/**
  * @}
  */

/** @addtogroup STM32WB0x_System_Private_FunctionPrototypes
  * @{
  */

void CPUcontextRestore(void);

/**
  * @}
  */

/** @addtogroup STM32WB0x_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  * @param  None
  * @retval None
  */

void SystemInit(void)
{
  uint32_t mainRegulator, smpsOutVoltage, hsiCalib;
#if defined(STM32WB06) || defined(STM32WB07)
  uint32_t lsiLpmu;
#else
  uint32_t lsiBw;
#endif
  uint8_t i;
  
  /* If the reset reason is a wakeup from power save restore the context */
  if ((RCC->CSR == 0) && ((PWR->SR1 != 0)||(PWR->SR3 != 0))) {
    RAM_VR.WakeupFromSleepFlag = 1; /* A wakeup from power save occurred */
    CPUcontextRestore();            /* Restore the context */
    /* if the context restore worked properly, we should never return here */
    while(1) { 
      NVIC_SystemReset(); 
    }
  }
  
  BootModeCheck();

  /* Configure the Vector Table location */
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; /* Vector Table Relocation */
#else
  SCB->VTOR = (uint32_t) (__vector_table);
#endif /* USER_VECT_TAB_ADDRESS */

  /* Store in RAM the AppBase information */
  RAM_VR.AppBase = (uint32_t) SCB->VTOR;

  /* Enable all the RAM banks in retention during power save */
#if defined(PWR_CR2_RAMRET1)
  SET_BIT(PWR->CR2, PWR_CR2_RAMRET1);
#endif /* PWR_CR2_RAMRET1 */
  
#if defined(PWR_CR2_RAMRET2)
  SET_BIT(PWR->CR2, PWR_CR2_RAMRET2);
#endif /* PWR_CR2_RAMRET2 */
  
#if defined(PWR_CR2_RAMRET2)
  SET_BIT(PWR->CR2, PWR_CR2_RAMRET3);
#endif /* PWR_CR2_RAMRET3 */

  /* Disable the GPIO retention in power save configuration */
#if defined(PWR_CR2_GPIORET)
  CLEAR_BIT(PWR->CR2, PWR_CR2_GPIORET);
#endif /* PWR_CR2_GPIORET */
   
  /* SMPS setup */
  if ((CFG_HW_SMPS == SMPS_ON) || (CFG_HW_SMPS == SMPS_BYPAPSS))
  {
    while(READ_BIT(PWR->SR2, PWR_SR2_SMPSRDY) != PWR_SR2_SMPSRDY); // Wait until SMPS is ready
    MODIFY_REG(PWR->CR5, PWR_CR5_SMPSBOMSEL, (CFG_HW_SMPS_BOM<<PWR_CR5_SMPSBOMSEL_Pos)); // Configure the SMPS BOM
  }
  if ((CFG_HW_SMPS == SMPS_ON) || (CFG_HW_SMPS == SMPS_OFF))
  {
    MODIFY_REG(PWR->CR5, PWR_CR5_NOSMPS, (CFG_HW_SMPS<<PWR_CR5_NOSMPS_Pos)); // SMPS ON/OFF Configuration
  }
  else
  {
    MODIFY_REG(PWR->CR5, PWR_CR5_SMPSFBYP, (1<<PWR_CR5_SMPSFBYP_Pos)); // SMPS BYPASS Configuration
  }
  MODIFY_REG(PWR->CR5, PWR_CR5_SMPSLPOPEN, (CFG_HW_SMPS_LOW_POWER<<PWR_CR5_SMPSLPOPEN_Pos)); // SMPS configuration during power save
  
  /* If Trimming values from engineering in flash locations are not present load default values */
  if (*(volatile uint32_t*)VALIDITY_LOCATION != VALIDITY_TAG)
  {
#if defined(STM32WB06) || defined(STM32WB07)
    hsiCalib       = 0x1E;
    mainRegulator  = 0x08;
    smpsOutVoltage = 0x03;
    lsiLpmu        = 0x08;

    /* Set Low Speed Internal oscillator LPMU trimming value. */
    MODIFY_REG(PWR->ENGTRIM, PWR_ENGTRIM_TRIM_LSI_LPMU, ((lsiLpmu << PWR_ENGTRIM_TRIM_LSI_LPMU_Pos) & PWR_ENGTRIM_TRIM_LSI_LPMU));
    SET_BIT(PWR->ENGTRIM, PWR_ENGTRIM_TRIMLSILPMUEN);
#else
    hsiCalib       = 0x1F;
    lsiBw          = 8;
    mainRegulator  = 0x0A;
    smpsOutVoltage = 0x03;

    /* Low speed internal RC trimming value set by software */
    MODIFY_REG(RCC->CSSWCR, RCC_CSSWCR_LSISWBW, lsiBw << RCC_CSSWCR_LSISWBW_Pos);
    SET_BIT(RCC->CSSWCR, RCC_CSSWCR_LSISWTRIMEN);
#endif
    
    /* Set HSI Calibration Trimming value */
    MODIFY_REG(RCC->CSSWCR, RCC_CSSWCR_HSITRIMSW, hsiCalib << RCC_CSSWCR_HSITRIMSW_Pos);
    SET_BIT(RCC->CSSWCR, RCC_CSSWCR_HSISWTRIMEN);
              
    /* Set Main Regulator voltage Trimming value */ 
    MODIFY_REG(PWR->ENGTRIM, PWR_ENGTRIM_TRIM_MR, ((mainRegulator << PWR_ENGTRIM_TRIM_MR_Pos) & PWR_ENGTRIM_TRIM_MR));
    SET_BIT(PWR->ENGTRIM, PWR_ENGTRIM_TRIMMREN);

    /* Set SMPS output voltage Trimming value */
    MODIFY_REG(PWR->ENGTRIM, PWR_ENGTRIM_SMPS_TRIM, ((smpsOutVoltage << PWR_ENGTRIM_SMPS_TRIM_Pos) & PWR_ENGTRIM_SMPS_TRIM));
    SET_BIT(PWR->ENGTRIM, PWR_ENGTRIM_SMPSTRIMEN);    
  }

  /* Set all the interrupt with low priprity */
  for (i=0; i<32; i++)
  {
    NVIC_SetPriority((IRQn_Type)i, IRQ_LOW_PRIORITY);
  }
  
  /* Enable all the irqs */
  __enable_irq();
}

/****************************/

/**
 * Return 0 if FW App not valid
 * Return 1 if Fw App valid
 */
static uint8_t  CheckFwAppValidity( uint8_t page_idx );


static uint8_t CheckFwAppValidity( uint8_t page_idx )
{
  uint8_t status;
  uint32_t magic_keyword_address;
  uint32_t last_user_flash_address;

  magic_keyword_address = *(uint32_t*)(FLASH_START_ADDR + (page_idx * FLASH_PAGE_SIZE + 0xC0)); 
  if(page_idx != CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
  { /* magic_keyword_address is in the download slot = active slot + APP_SLOT_PAGE_SIZE */
    magic_keyword_address += ((page_idx - CFG_ACTIVE_SLOT_START_SECTOR_INDEX) * FLASH_PAGE_SIZE);
  }
  last_user_flash_address = (FLASH_SIZE + FLASH_START_ADDR) - 4;
  if( (magic_keyword_address < FLASH_START_ADDR) || (magic_keyword_address > last_user_flash_address) )
  {
    /**
     * The address is not valid
     */
    status = 0;
  }
  else
  {
    if( (*(uint32_t*)magic_keyword_address) != 0x94448A29  )
    {
      /**
       * A firmware update procedure did not complete
       */
      status = 0;
    }
    else
    {
      /**
       * The firmware application is available
       */
      status = 1;
    }
  }

  return status;
}

/**
 * Jump to existing FW App in flash
 * It never returns
 */
static void JumpFwApp( void )
{
  fct_t app_reset_handler;

  SCB->VTOR = FLASH_START_ADDR + (CFG_ACTIVE_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE);
  __set_MSP(*(uint32_t*)(FLASH_START_ADDR + (CFG_ACTIVE_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE)));
  app_reset_handler = (fct_t)(*(uint32_t*)(FLASH_START_ADDR + (CFG_ACTIVE_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE) + 4));
  app_reset_handler();

  /**
   * app_reset_handler() never returns.
   * However, if for any reason a PUSH instruction is added at the entry of  JumpFwApp(),
   * we need to make sure the POP instruction is not there before app_reset_handler() is called
   * The way to ensure this is to add a dummy code after app_reset_handler() is called
   * This prevents app_reset_handler() to be the last code in the function.
   */
  __WFI();


  return;
}

/**
 * Erase active or download slot
 */
static void DeleteSlot( uint8_t page_idx )
{

  /**
   * The number of sectors to erase is read from SRAM1.
   * It shall be checked whether the number of sectors to erase does not overlap on the secured Flash
   * The limit can be read from the SFSA option byte which provides the first secured sector address.
   */

  uint32_t last_page_idx = page_idx + APP_SLOT_PAGE_SIZE - 1;
  FLASH_EraseInitTypeDef p_erase_init;
  uint32_t page_error;
  uint32_t NbrOfPageToBeErased = (uint32_t)APP_SLOT_PAGE_SIZE;

  if(page_idx < CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
  {
    /**
     * Something has been wrong as there is no case we should delete the BLE_BootMngr application
     * Reboot on the active firmware application
     */
    CFG_OTA_REBOOT_VAL_MSG = REBOOT_ON_FW_APP;
    NVIC_SystemReset(); /* it waits until reset */
  }

  if ((page_idx + NbrOfPageToBeErased - 1) > last_page_idx)
  {
    NbrOfPageToBeErased = last_page_idx - page_idx + 1;
  }

  p_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
  p_erase_init.NbPages = NbrOfPageToBeErased;
  p_erase_init.Page = (uint32_t)page_idx;

  HAL_FLASHEx_Erase(&p_erase_init, &page_error);
  
  return;
}

/**
 * Move download slot to active slot
 */
static void MoveToActiveSlot( uint8_t page_idx )
{
  uint32_t dwn_addr, act_addr, last_addr;

  /**
   * Write in Flash the data received in the BLE packet
   */
  dwn_addr = FLASH_START_ADDR + (page_idx * FLASH_PAGE_SIZE);
  act_addr = FLASH_START_ADDR + (CFG_ACTIVE_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE);
  /* Last address of the new binary relative to the start address in the active slot */
  last_addr = *(uint32_t*)(dwn_addr + 0xC0); 
  last_addr &= 0xFFFFFFF0;
  
  /**
   * The flash is written by bunch of 16 bytes
   * Data are written in flash as long as there are at least 16 bytes
   */
  while( act_addr <= last_addr )
  {
    HAL_StatusTypeDef status = HAL_ERROR;
    
    while(status != HAL_OK)
      status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BURST,
                                 act_addr, 
                                 dwn_addr);
    
    if((*(uint64_t*)(act_addr) == *(uint64_t*)(dwn_addr)) &&
       (*(uint64_t*)((act_addr)+8) == *(uint64_t*)((dwn_addr)+8)))
    {
      act_addr += 16;
      dwn_addr += 16;
    }
  }
}

extern FLASH_ProcessTypeDef pFlash;
      
/**
 * Check the Boot mode request
 * Depending on the result, the CPU may either jump to an existing application in the user flash
 * or keep on running the code to start the OTA loader
 */
static void BootModeCheck( void )
{
  /* Fix: global variables are not initialized yet. pFlash is used by Flash driver. */
  memset(&pFlash, 0, sizeof(FLASH_ProcessTypeDef));
  
  if(RAM_VR.ResetReason & LL_RCC_CSR_SFTRSTF)
  {
    /**
     * The SRAM1 content is kept on Software Reset.
     * In the Ble_Ota application, the first address of the SRAM1 indicates which kind of action has been requested
     */

    /**
     * Check Boot Mode from SRAM1
     */
    if((CFG_OTA_REBOOT_VAL_MSG == REBOOT_ON_FW_APP) && (CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) != 0))
    {
      uint8_t download_slot_start_sector = CFG_OTA_START_SECTOR_IDX_VAL_MSG;
      
      if((download_slot_start_sector >= FLASH_PAGE_NUMBER) || 
         ((download_slot_start_sector < CFG_ACTIVE_SLOT_START_SECTOR_INDEX)))
      {
        /* CFG_OTA_START_SECTOR_IDX_VAL_MSG not correctly initialized */
        download_slot_start_sector = DOWNLOAD_SLOT_START_SECTOR_INDEX;
        CFG_OTA_START_SECTOR_IDX_VAL_MSG = DOWNLOAD_SLOT_START_SECTOR_INDEX;
      }
      
      if(CheckFwAppValidity(download_slot_start_sector) != 0)
      {
        /**
         * The user has requested to start on the firmware application and it has been checked
         * a valid application is ready in the download slot
         */
        DeleteSlot( CFG_ACTIVE_SLOT_START_SECTOR_INDEX );     /* Erase active slot */
        MoveToActiveSlot(download_slot_start_sector);                           /* Move download slot to active slot */
        if(CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) != 0)
        {
          DeleteSlot( DOWNLOAD_SLOT_START_SECTOR_INDEX ); /* Erase download slot */
        }
      }
      
      /**
       * Jump now on the application
       */
      JumpFwApp();
    }
    else if((CFG_OTA_REBOOT_VAL_MSG == REBOOT_ON_FW_APP) && (CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) == 0))
    {
      /**
       * The user has requested to start on the firmware application but there is no valid application
       * Erase all sectors specified by byte1 and byte1 in SRAM1 to download a new App.
       */
      CFG_OTA_REBOOT_VAL_MSG = REBOOT_ON_BLE_OTA_APP;     /* Request to reboot on BLE_Ota application */
      CFG_OTA_START_SECTOR_IDX_VAL_MSG = CFG_ACTIVE_SLOT_START_SECTOR_INDEX;
      CFG_OTA_NBR_OF_SECTOR_VAL_MSG = 0xFF;
    }
    else if(CFG_OTA_REBOOT_VAL_MSG == REBOOT_ON_BLE_OTA_APP)
    {
      /**
       * It has been requested to reboot on BLE_Ota application to download data
       * Erase download slot to download a new App.
       */
      DeleteSlot( DOWNLOAD_SLOT_START_SECTOR_INDEX );     /* Erase download slot */
      JumpFwApp();                                            /* Jump to OTA application */
    }
    else if(CFG_OTA_REBOOT_VAL_MSG == REBOOT_ON_CPU2_UPGRADE)
    {
      /**
       * It has been requested to reboot on BLE_Ota application to keep running the firmware upgrade process
       *
       */
    }
    else
    {
      /**
       * There should be no use case to be there because the device already starts from power up
       * and the SRAM1 is then filled with the value define by the user
       * However, it could be that a reset occurs just after a power up and in that case, the Ble_Ota
       * will be running but the sectors to download a new App may not be erased
       */
      JumpSelectionOnPowerUp( );
    }
  }
  else
  {
    /**
     * On Power up, the content of SRAM1 is random
     * The only thing that could be done is to jump on either the firmware application
     * or the Ble_Ota application
     */
    JumpSelectionOnPowerUp( );
  }

  /**
   * Return to the startup file and run the Ble_Ota application
   */
  return;
}

static void JumpSelectionOnPowerUp( void )
{
  /**
   * Check if there is a FW App
   */
  if(CheckFwAppValidity(CFG_ACTIVE_SLOT_START_SECTOR_INDEX) != 0)
  {
    /**
     * The SRAM1 is random
     * Initialize SRAM1 to indicate we requested to reboot of firmware application
     */
    CFG_OTA_REBOOT_VAL_MSG = REBOOT_ON_FW_APP;

    /**
     * A valid application is available
     * Jump now on the application
     */
    JumpFwApp();
  }
  else
  {
    /**
     * The SRAM1 is random
     * Initialize SRAM1 to indicate we requested to reboot of BLE_Ota application
     */
    CFG_OTA_REBOOT_VAL_MSG = REBOOT_ON_BLE_OTA_APP;

    /**
     * There is no valid application available
     * Erase all sectors specified by byte1 and byte1 in SRAM1 to download a new App.
     */
    CFG_OTA_START_SECTOR_IDX_VAL_MSG = CFG_ACTIVE_SLOT_START_SECTOR_INDEX;
    CFG_OTA_NBR_OF_SECTOR_VAL_MSG = 0xFF;
  }
  return;
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint8_t directHSE_enabled;
  uint8_t divPrescaler;

  /* Get SYSCLK source HSE or HSI+PLL64MHz */
  directHSE_enabled = (RCC->CFGR & RCC_CFGR_HSESEL) >> RCC_CFGR_HSESEL_Pos;

#if defined(STM32WB06) || defined(STM32WB07)
  /* Get the clock divider */
    divPrescaler = (RCC->CFGR & RCC_CFGR_CLKSYSDIV) >> RCC_CFGR_CLKSYSDIV_Pos;
#else
  /* Get the clock divider */
  divPrescaler = (RCC->CFGR & RCC_CFGR_CLKSYSDIV_STATUS) >> RCC_CFGR_CLKSYSDIV_STATUS_Pos;
#endif

  if (directHSE_enabled)
  {
    SystemCoreClock = HSE_VALUE >> (divPrescaler - 1U);
  }
  else
  {
    SystemCoreClock = HSI_VALUE >> divPrescaler;
  }  
}

/**
  * @brief  Restores the saved CPU state before to enter in power save 
  *         by popping it from the stack 
  * @param  None
  * @retval None
  */
__WEAK void CPUcontextRestore(void)
{
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
