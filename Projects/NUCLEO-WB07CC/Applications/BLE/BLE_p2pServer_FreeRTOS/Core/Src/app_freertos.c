/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_freertos.c
  * @author  MCD Application Team
  * @brief   FreeRTOS applicative file
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
#include "FreeRTOS.h"
#include "task.h"
#include "app_conf.h"
#include "ble.h"
#include "hw_pka.h"
#include "stm32_lpm.h"
#include "stm32wb0x_hal.h"
#include "app_freertos.h"
#include "app_ble.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_entry.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static VTIMER_HandleType TimerHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static uint8_t freertos_power_save(PowerSaveLevels level);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

  /**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_FreeRTOS_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  vTaskStartScheduler();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/**
 * @brief  This API returns if the system actually went in stop or off mode.
 * @return 1 (TRUE) if system actually went in stop or off mode, otherwise 0 (FALSE).
 */
static uint8_t freertos_power_save(PowerSaveLevels level)
{
  uint8_t stop_off_mode = FALSE;
  
#if (CFG_LPM_SUPPORTED == 1)
  
  /* Need to consume some CSTACK on WB05, due to bootloader CSTACK usage. */
  volatile uint32_t dummy[15];
  uint8_t i;
  for (i=0; i<10; i++)
  {
    dummy[i] = 0;
    __NOP();
  }
  
  PowerSaveLevels stack_powerSave_level, app_powerSave_level, vtimer_powerSave_level, final_level, pka_level;
  
  stack_powerSave_level = (PowerSaveLevels)BLE_STACK_SleepCheck();
  
  if ((stack_powerSave_level != POWER_SAVE_LEVEL_RUNNING) &&
      ((app_powerSave_level = App_PowerSaveLevel_Check()) != POWER_SAVE_LEVEL_RUNNING)) 
  {  
    vtimer_powerSave_level = HAL_RADIO_TIMER_PowerSaveLevelCheck();
    pka_level = (PowerSaveLevels) HW_PKA_PowerSaveLevelCheck();
    
    final_level = MIN(app_powerSave_level, level);
    final_level = (PowerSaveLevels)MIN(vtimer_powerSave_level, final_level);
    final_level = (PowerSaveLevels)MIN(pka_level, final_level);
     
    switch(final_level)
    {
    case POWER_SAVE_LEVEL_RUNNING:
      /* Not Power Save device is busy */
      return FALSE;
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
    
    if(UTIL_LPM_GetMode() > UTIL_LPM_SLEEPMODE && RAM_VR.WakeupFromSleepFlag)
    {
      stop_off_mode = TRUE;
    }
  }
  else if(stack_powerSave_level == POWER_SAVE_LEVEL_RUNNING)
  {
    BLEStack_Process_Schedule();
  }
#endif /* CFG_LPM_SUPPORTED */
  
  return stop_off_mode;
  
}

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
  eSleepModeStatus eSleepStatus;
  uint64_t timeBeforeSleep, timeAfterSleep;
  uint8_t sleep_without_systick;
  
  timeBeforeSleep = HAL_RADIO_TIMER_GetCurrentSysTime();
  
  eSleepStatus = eTaskConfirmSleepModeStatus();  
  
  /* Start a timer to wake up the system after the given interval if there
    are tasks waiting for a timeout. Since this may take some time, it is not
    called inside the critical section.  */
  if(eSleepStatus == eStandardSleep)
  {
    HAL_RADIO_TIMER_StopVirtualTimer(&TimerHandle);
    HAL_RADIO_TIMER_StartVirtualTimer(&TimerHandle, (xExpectedIdleTime*1000)/configTICK_RATE_HZ);
  }
  
  /* Enter a critical section. */
  __disable_irq();
  
  /* Ensure it is still ok to enter the sleep mode. */
  eSleepStatus = eTaskConfirmSleepModeStatus();
  
  if( eSleepStatus == eAbortSleep )
    {
        /* A task has been moved out of the Blocked state since this macro was
        executed, or a context siwth is being held pending.  Do not enter a
        sleep state.  Exit the critical section. */
        __enable_irq();
        HAL_RADIO_TIMER_StopVirtualTimer(&TimerHandle);
    }
    else
    {
        if( eSleepStatus == eNoTasksWaitingTimeout )
        {
            /* It is not necessary to configure an interrupt to bring the
            microcontroller out of its low power state at a fixed time in the
            future. */
           /* No need to stop the timer since it was started only if eSleepStatus was
            eStandardSleep. It may happen that eSleepStatus becomes eAbortSleep
            after the second call to eTaskConfirmSleepModeStatus(), but it cannot
            happen that eSleepStatus becomes eNoTasksWaitingTimeout if it
            was eStandardSleep. */  
          freertos_power_save(POWER_SAVE_LEVEL_STOP);
        }
        else
        {
            /* Enter the low power state. */ 
           sleep_without_systick = freertos_power_save(POWER_SAVE_LEVEL_STOP_LS_CLOCK_ON);
            
           if(sleep_without_systick)
            {              
              /* Determine how long the microcontroller was actually in a low power
              state for, which will be less than xExpectedIdleTime if the
              microcontroller was brought out of low power mode by an interrupt
              other than that configured by the vSetWakeTimeInterrupt() call.
              Note that the scheduler is suspended before
              portSUPPRESS_TICKS_AND_SLEEP() is called, and resumed when
              portSUPPRESS_TICKS_AND_SLEEP() returns.  Therefore no other tasks will
              execute until this function completes. */
              timeAfterSleep = HAL_RADIO_TIMER_GetCurrentSysTime();
              
              /* Correct the kernels tick count to account for the time the
              microcontroller spent in its low power state. */
              
              /* Account for the rounding to millisecond time units.
                Set the systick LOAD register to the appropriate value.  */
              
              vTaskStepTick( HAL_RADIO_TIMER_DiffSysTimeMs(timeAfterSleep,timeBeforeSleep)/portTICK_PERIOD_MS );
            }
        }

        /* Exit the critical section - it might be possible to do this immediately
        after the prvSleep() calls. */
        __enable_irq();
    }
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
