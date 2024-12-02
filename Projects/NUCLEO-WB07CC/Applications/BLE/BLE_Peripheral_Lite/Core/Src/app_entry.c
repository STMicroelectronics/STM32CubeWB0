/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_entry.c
  * @author  GPM WBL Application Team
  * @brief   Entry point of the application
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "main.h"
#include "app_ble.h"
#include "hw_rng.h"
#include "hw_aes.h"
#include "hw_pka.h"
#include "stm32wb0x.h"
#include "stm32wb0x_ll_usart.h"
#include "ble_stack.h"
#if (CFG_LPM_SUPPORTED == 1)
#include "stm32_lpm_if.h"
#endif /* CFG_LPM_SUPPORTED */
#include "app_debug.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

#if (CFG_LPM_SUPPORTED == 1)
#define ATOMIC_SECTION_BEGIN() uint32_t uwPRIMASK_Bit = __get_PRIMASK(); \
                                __disable_irq(); \
/* Must be called in the same or in a lower scope of ATOMIC_SECTION_BEGIN */
#define ATOMIC_SECTION_END() __set_PRIMASK(uwPRIMASK_Bit)
#endif /* CFG_LPM_SUPPORTED */

/* USER CODE BEGIN PD */

/* Section specific to button management using UART */
#define C_SIZE_CMD_STRING       256U
                                  
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
#if (CFG_LPM_SUPPORTED == 1)
static void Enter_LowPowerMode(void);
#endif /* CFG_LPM_SUPPORTED */

/* USER CODE BEGIN PFP */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init(void);
#endif
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

uint32_t MX_APPE_Init(void *p_param)
{

  UNUSED(p_param);

  APP_DEBUG_SIGNAL_SET(APP_APPE_INIT);

#if (CFG_DEBUG_APP_ADV_TRACE != 0)
  UTIL_ADV_TRACE_Init();
  UTIL_ADV_TRACE_SetVerboseLevel(VLEVEL_L); /* functional traces*/
  UTIL_ADV_TRACE_SetRegion(~0x0);
#endif

  /* USER CODE BEGIN APPE_Init_1 */
#if (CFG_LED_SUPPORTED == 1)
  Led_Init();
#endif

#if (CFG_DEBUG_APP_TRACE != 0) && (CFG_DEBUG_APP_ADV_TRACE == 0)
  COM_InitTypeDef COM_Init = 
  {
   .BaudRate = 115200,
   .WordLength= COM_WORDLENGTH_8B,
   .StopBits = COM_STOPBITS_1,
   .Parity = COM_PARITY_NONE,
   .HwFlowCtl = COM_HWCONTROL_NONE
  };
  BSP_COM_Init(COM1, &COM_Init);
  
#endif  
    
  /* USER CODE END APPE_Init_1 */

  if (HW_RNG_Init() != HW_RNG_SUCCESS)
  {
    Error_Handler();
  }

  /* Init the AES block */
  HW_AES_Init();
  HW_PKA_Init();
  APP_BLE_Init();

/* USER CODE BEGIN APPE_Init_2 */
  
/* USER CODE END APPE_Init_2 */
  APP_DEBUG_SIGNAL_RESET(APP_APPE_INIT);
  return BLE_STATUS_SUCCESS;
}

/* USER CODE BEGIN FD */
#if (CFG_BUTTON_SUPPORTED == 1)
/**
 * @brief  Action of button 1 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button1Action(void)
{
}

/**
 * @brief  Action of button 2 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button2Action(void)
{
}

/**
 * @brief  Action of button 3 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APPE_Button3Action(void)
{
}
#endif

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
#if (CFG_LPM_SUPPORTED == 1)
static PowerSaveLevels App_PowerSaveLevel_Check(void)
{
  PowerSaveLevels output_level = POWER_SAVE_LEVEL_STOP;
  /* USER CODE BEGIN App_PowerSaveLevel_Check_1 */
  
  /* USER CODE END App_PowerSaveLevel_Check_1 */

  return output_level;
}
#endif

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init( void )
{
  /* Leds Initialization */
  BSP_LED_Init(LED_BLUE); /* LED driven by connected central device */
  BSP_LED_Init(LED_GREEN);/* blinking led at different rate depending on advertsigin or connection */


  BSP_LED_On(LED_GREEN);

  return;
}
#endif

/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/
void MX_APPE_Process(void)
{
  /* USER CODE BEGIN MX_APPE_Process_1 */
  
  /* USER CODE END MX_APPE_Process_1 */
  VTimer_Process();

  BLEStack_Process();

  NVM_Process();

  SERVICE_APP_Process();
#if (CFG_LPM_SUPPORTED == 1)
  Enter_LowPowerMode();
#endif /* CFG_LPM_SUPPORTED */

  /* USER CODE BEGIN MX_APPE_Process_2 */
  
  /* USER CODE END MX_APPE_Process_2 */
}

#if (CFG_LPM_SUPPORTED == 1)
/**
  * @brief  This function configures the device in SLEEP (WFI).
  * @param  None
  * @retval None
  */
static void sleep(void)
{
   /* Disable the SysTick */
   HAL_SuspendTick();

   /* Device in SLEEP mode (WFI) */
   HAL_PWR_EnterSLEEPMode();

    /* Enable the SysTick */
   HAL_ResumeTick();
}

/**
  * @brief  This function configures the device in DEEPSTOP mode with the low
  *         speed oscillator enabled.
  * @note   At waekup the hardware resources located in the VDD12i power domain
  *         are reset and the CPU reboots.
  * @param  None
  * @retval None
  */
static void deepstopTimer(void)
{
   /* To consume additional CSTACK location */
   volatile uint32_t dummy[15];
   uint8_t i;

   for (i=0; i<10; i++)
   {
     dummy[i] = 0;
     __NOP();
   }

   /* Low Power sequence */
   ATOMIC_SECTION_BEGIN();
   PWR_EnterStopMode();
   PWR_ExitStopMode();
   ATOMIC_SECTION_END();
}

/**
  * @brief  This function configures the device in DEEPSTOP mode with the low
  *         speed oscillator disabled.
  * @note   At waekup the hardware resources located in the VDD12i power domain
  *         are reset and the CPU reboots.
  * @param  None
  * @retval None
  */
static void deepstop(void)
{
   /* To consume additional CSTACK location */
   volatile uint32_t dummy[15];
   uint8_t i;

   for (i=0; i<10; i++)
   {
     dummy[i] = 0;
     __NOP();
   }

  /* Low Power sequence */
   ATOMIC_SECTION_BEGIN();
   PWR_EnterOffMode();
   PWR_ExitOffMode();
   ATOMIC_SECTION_END();
}

static void Enter_LowPowerMode(void)
{
  PowerSaveLevels app_powerSave_level, vtimer_powerSave_level, final_level, pka_level;

  if ((BLE_STACK_SleepCheck() != POWER_SAVE_LEVEL_RUNNING) &&
      ((app_powerSave_level = App_PowerSaveLevel_Check()) != POWER_SAVE_LEVEL_RUNNING))
  {
    vtimer_powerSave_level = HAL_RADIO_TIMER_PowerSaveLevelCheck();
    pka_level = (PowerSaveLevels) HW_PKA_PowerSaveLevelCheck();
    final_level = (PowerSaveLevels)MIN(vtimer_powerSave_level, app_powerSave_level);
    final_level = (PowerSaveLevels)MIN(pka_level, final_level);

    switch(final_level)
    {
    case POWER_SAVE_LEVEL_RUNNING:
      /* Not Power Save device is busy */
      return;
      break;
    case POWER_SAVE_LEVEL_CPU_HALT:
      sleep();
      break;
    case POWER_SAVE_LEVEL_STOP_LS_CLOCK_ON:
      deepstopTimer();
      break;
    case POWER_SAVE_LEVEL_STOP:
      deepstop();
      break;
    }
  }
}
#endif /* CFG_LPM_SUPPORTED */

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
#if (CFG_BUTTON_SUPPORTED == 1)
void BSP_PB_Callback(Button_TypeDef Button)
{

}

#if (CFG_LPM_SUPPORTED == 1)
void HAL_PWR_WKUPx_Callback(uint32_t wakeupIOs)
{

}
#endif

void HAL_GPIO_EXTI_Callback(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{

}

#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* USER CODE END FD_WRAP_FUNCTIONS */
