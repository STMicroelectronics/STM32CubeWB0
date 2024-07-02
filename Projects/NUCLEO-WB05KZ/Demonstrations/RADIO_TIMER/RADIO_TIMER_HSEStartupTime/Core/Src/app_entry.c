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
#if (CFG_LPM_SUPPORTED == 1)
#include "stm32_lpm.h"
//#include "stm32_lpm_if.h"
#endif /* CFG_LPM_SUPPORTED */

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* Wakeup interval*/
#define WAKEUP_INTERVAL_MS          500

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t old_wakeup_time_mach, wakeup_time_mach, hse_ready_time_mach;
uint32_t max_hse_startup_time_sys = 0;
static VTIMER_HandleType timerHandle;

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/

/* USER CODE BEGIN PFP */
static void timeoutCB(void *param);

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

uint32_t MX_APPE_Init(void *p_param)
{
  UNUSED(p_param);
  
  /* USER CODE BEGIN APPE_Init_1 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  
  COM_InitTypeDef COM_Init = {0};

  COM_Init.BaudRate= 115200;
  COM_Init.HwFlowCtl = COM_HWCONTROL_NONE;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.Parity = COM_PARITY_NONE;
  COM_Init.StopBits = COM_STOPBITS_1;
  BSP_COM_Init(COM1, &COM_Init);
    
#if (CFG_LPM_SUPPORTED == 1)
  /* Low Power Manager Init */
  UTIL_LPM_Init();
#endif /* CFG_LPM_SUPPORTED */
  
  /* Enable HSE Ready interrupt */
  LL_RCC_ClearFlag_RC64MPLLRDY();
  LL_RCC_EnableIT_RC64MPLLRDY();

  timerHandle.callback = timeoutCB;
  HAL_RADIO_TIMER_StartVirtualTimer(&timerHandle, WAKEUP_INTERVAL_MS);

  printf("\nHSE test app started\n");
  BSP_LED_On(LD1);
  /* USER CODE END APPE_Init_1 */
  

  /* USER CODE BEGIN APPE_Init_2 */

  /* USER CODE END APPE_Init_2 */
   return 0;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

#if (CFG_LPM_SUPPORTED == 1)
static PowerSaveLevels App_PowerSaveLevel_Check(void)
{
  PowerSaveLevels output_level = POWER_SAVE_LEVEL_STOP;

  /* USER CODE BEGIN App_PowerSaveLevel_Check_1 */
  
  /* USER CODE END App_PowerSaveLevel_Check_1 */
  
  return output_level;
}
#endif

static void timeoutCB(void *param)
{
  HAL_RADIO_TIMER_StartVirtualTimer(&timerHandle, WAKEUP_INTERVAL_MS);
}

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
  
  /* Perform calibration procedure */
  HAL_RADIO_TIMER_Tick();
  /* USER CODE BEGIN MX_APPE_Process_2 */
  
  if(old_wakeup_time_mach != wakeup_time_mach)
  {
    uint32_t hse_startup_time_sys;
    
    old_wakeup_time_mach = wakeup_time_mach;
    hse_startup_time_sys = HAL_RADIO_TIMER_MachineTimeToSysTime(hse_ready_time_mach - wakeup_time_mach);
    max_hse_startup_time_sys = MAX(hse_startup_time_sys,max_hse_startup_time_sys);
    //printf("%X %X %d\n", wakeup_time_mach, hse_ready_time_mach, hse_ready_time_mach - wakeup_time_mach);
    printf("Last: %u. Worst: %u (%u us)\n", INT(hse_startup_time_sys), INT(max_hse_startup_time_sys), INT(max_hse_startup_time_sys*625/256));
    BSP_LED_Toggle(LD2);
  }
  
  /* USER CODE END MX_APPE_Process_2 */
}

void MX_APPE_Idle(void)
{
  #if (CFG_LPM_SUPPORTED == 1)
  
  PowerSaveLevels app_powerSave_level, vtimer_powerSave_level, final_level;

  /* Need to consume some CSTACK, due to bootloader CSTACK usage. */
  volatile uint32_t dummy[15];

  uint8_t i;
  for (i=0; i<10; i++)
  {
    dummy[i] = 0;
    __NOP();
  }  
  
  app_powerSave_level = App_PowerSaveLevel_Check();
  
  if(app_powerSave_level != POWER_SAVE_LEVEL_RUNNING) 
  {  
    vtimer_powerSave_level = HAL_RADIO_TIMER_PowerSaveLevelCheck();
    final_level = (PowerSaveLevels)MIN(vtimer_powerSave_level, app_powerSave_level);
     
    switch(final_level)
    {
    case POWER_SAVE_LEVEL_RUNNING:
      // Not Power Save device is busy
      return;
      break;
    case POWER_SAVE_LEVEL_CPU_HALT:
      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
      break;
    case POWER_SAVE_LEVEL_STOP_LS_CLOCK_ON:
      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
      break;
    case POWER_SAVE_LEVEL_STOP:
      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
      break;
    }
    
    UTIL_LPM_EnterLowPower();
  }
#endif /* CFG_LPM_SUPPORTED */

}

void RCC_IRQ_Callback(void)
{
  /* USER CODE BEGIN */
  extern uint32_t hse_ready_time_mach, wakeup_time_mach;
  uint32_t wakeup_offset;

  hse_ready_time_mach = WAKEUP->ABSOLUTE_TIME;

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

  wakeup_offset =  WAKEUP->WAKEUP_OFFSET[0];

  wakeup_time_mach = WAKEUP->CM0_WAKEUP_TIME - (wakeup_offset<<4);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  LL_RCC_ClearFlag_RC64MPLLRDY();
  /* USER CODE END */
}
