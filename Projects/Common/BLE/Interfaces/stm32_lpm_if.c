/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_lpm_if.c
  * @author  GPM WBL Application Team
  * @brief   Low layer function to enter/exit low power modes (stop, sleep)
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
#include "stm32_lpm.h"
#include "stm32_lpm_if.h"
#include "device_context_switch.h"
#include "RTDebug.h"

/** @addtogroup TINY_LPM_IF
  * @{
  */

/* USER CODE BEGIN include */
#include "stm32wb0x_hal.h"
/* USER CODE END include */

/* Exported variables --------------------------------------------------------*/
/** @defgroup TINY_LPM_IF_Exported_variables TINY LPM IF exported variables
  * @{
  */

/**
 * @brief variable to provide all the functions corresponding to the different low power modes.
 */
const struct UTIL_LPM_Driver_s UTIL_PowerDriver =
{
  PWR_EnterSleepMode,
  PWR_ExitSleepMode,

  PWR_EnterStopMode,
  PWR_ExitStopMode,

  PWR_EnterOffMode,
  PWR_ExitOffMode,
};

/**
 * @}
 */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN Private_Function_Prototypes */

/* USER CODE END Private_Function_Prototypes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */

/* USER CODE END Private_Typedef */
/* Private define ------------------------------------------------------------*/
typedef struct clockContextS
{
  uint8_t  directHSEenabled;
  uint8_t  LSEenabled;
  uint8_t  LSIenabled;
} clockContextT;

#define MIN_ACTIVE_TIME_US          200

/* USER CODE BEGIN Private_Define */

/* USER CODE END Private_Define */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Private_Macro */

/* USER CODE END Private_Macro */
/* Private variables ---------------------------------------------------------*/
static apb0PeriphT apb0={0};
static apb1PeriphT apb1={0};
static apb2PeriphT apb2={0};
static ahb0PeriphT ahb0={0};
static cpuPeriphT  cpuPeriph={0};
static uint32_t    cStackPreamble[CSTACK_PREAMBLE_NUMBER];
static clockContextT clockContext;
static uint32_t lastTick;

/* USER CODE BEGIN Private_Variables */
extern void CPUcontextSave(void);

/* USER CODE END Private_Variables */

static uint8_t checkMinWakeUpTime(void)
{
  if(lastTick == HAL_GetTick())
  {
    uint32_t systick_reload_val = SysTick->LOAD;
      
    /* In this case it is likely that we are going to sleep too early.
       Check current value of systick counter.  */
    if(SysTick->VAL > systick_reload_val - MIN_ACTIVE_TIME_US * (HAL_RCC_GetSysClockFreq() / 1000000))
    {
      return 1;
    }
  }
  
  return 0;
}

/** @addtogroup TINY_LPM_IF_Exported_functions
 * @{
 */

void PWR_EnterOffMode( void )
{
  PWR_DEEPSTOPTypeDef configDS;

  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STANDBY_MODE_ENTER);

  /* USER CODE BEGIN PWR_EnterOffMode_1 */

  /* USER CODE END PWR_EnterOffMode_1 */

  /* Save the clock configuration */
  clockContext.directHSEenabled = FALSE;
  clockContext.LSEenabled = FALSE;
  clockContext.LSIenabled = FALSE;
  if (LL_RCC_DIRECT_HSE_IsEnabled())
  {
    clockContext.directHSEenabled = TRUE;
  }
  if (LL_RCC_LSE_IsEnabled())
  {
    clockContext.LSEenabled = TRUE;
    /* Enable pull down for LSE pins */
    HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_B, PWR_GPIO_BIT_12);
    HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_B, PWR_GPIO_BIT_13);
  }
  if (LL_RCC_LSI_IsEnabled())
  {
    clockContext.LSIenabled = TRUE;
  }

  /* This signal cannot be reset later otherwise the GPIO output will be
     automatically restored to high at wakeup. */
  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STANDBY_MODE_ENTER);

  /* Save all the peripheral registers and CPU peripipheral configuration */
  apb0.deepstop_wdg_state = ENABLE;
  prepareDeviceLowPower(&apb0, &apb1, &apb2, &ahb0, &cpuPeriph, cStackPreamble);

  /* DEEPSTOP configuration */
  configDS.deepStopMode = PWR_DEEPSTOP_WITH_SLOW_CLOCK_OFF;
  HAL_PWR_ConfigDEEPSTOP(&configDS);

  /* Clear all the wake-up pin flags */
  LL_PWR_ClearWakeupSource(LL_PWR_WAKEUP_ALL);

  /* Enable the device DEEPSTOP configuration */
  LL_PWR_SetPowerMode(LL_PWR_MODE_DEEPSTOP);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STANDBY_MODE_ACTIVE);
  
  /* Workaround: do not go to sleep if we have been awake for a too short time. */
  if(checkMinWakeUpTime() != 0)
  {
    return;
  }

  /* Save the CPU context & Wait for Interrupt Request to enter in DEEPSTOP */
  CPUcontextSave();

  /* USER CODE BEGIN PWR_EnterOffMode_2 */

  /* USER CODE END PWR_EnterOffMode_2 */
}

void PWR_ExitOffMode( void )
{
  /* USER CODE BEGIN PWR_ExitOffMode_1 */

  /* USER CODE END PWR_ExitOffMode_1 */

  /* Workaround to avoid going to sleep too early after a wakeup.
     We need to have a time reference. We can save current value of tick.
   */
  if(RAM_VR.WakeupFromSleepFlag)
  {
    lastTick = HAL_GetTick();
  }

  /* Restore low speed clock configuration */
  if (clockContext.LSEenabled == TRUE)
  {
    LL_PWR_SetNoPullB(LL_PWR_GPIO_BIT_12 | LL_PWR_GPIO_BIT_13);
    LL_RCC_LSE_Enable();
  }
  if (clockContext.LSIenabled == TRUE)
  {
    LL_RCC_LSI_Enable();
  }

  /* Clear SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

  /* Restore all the peripheral registers and CPU peripipheral configuration */
  restoreDeviceLowPower(&apb0, &apb1, &apb2, &ahb0, &cpuPeriph, cStackPreamble);

  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STANDBY_MODE_ACTIVE);
  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STANDBY_MODE_EXIT);

#if defined(PWR_CR2_GPIORET)
  /* Disable the GPIO retention at wake DEEPSTOP configuration */
  LL_PWR_DisableGPIORET();
#endif

  /* Wait until the HSE is ready */
  while(LL_RCC_HSE_IsReady() == 0U);

  /* Restore the DIRECT_HSE configuration */
  if (clockContext.directHSEenabled == TRUE)
  {
    LL_RCC_DIRECT_HSE_Enable();
    LL_RCC_RC64MPLL_Disable();
  }
  if (clockContext.LSEenabled == TRUE)
  {
    /* Wait until the LSE is ready */
    while(LL_RCC_LSE_IsReady() == 0U);
  }
  if (clockContext.LSIenabled == TRUE)
  {
    /* Wait until the LSI is ready */
    while(LL_RCC_LSI_IsReady() == 0U);
  }
  if (LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_MRBLE))
  {
    /* Wait untile the ABSOLUTE TIME clock correctly */
    while(WAKEUP->ABSOLUTE_TIME == 0xF);
  }

  if(RAM_VR.WakeupFromSleepFlag)
  {
    /* Handler to manage the IOs IRQ if needed */
    HAL_PWR_WKUP_IRQHandler();
  }

  /* USER CODE BEGIN PWR_ExitOffMode_2 */

  /* USER CODE END PWR_ExitOffMode_2 */

  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STANDBY_MODE_EXIT);
}

void PWR_EnterStopMode( void )
{
  PWR_DEEPSTOPTypeDef configDS;

  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STOP_MODE_ENTER);

  /* USER CODE BEGIN PWR_EnterStopMode_1 */

  /* USER CODE END PWR_EnterStopMode_1 */

  /* Save the clock configuration */
  clockContext.directHSEenabled = FALSE;
  clockContext.LSEenabled = FALSE;
  clockContext.LSIenabled = FALSE;
  if (LL_RCC_DIRECT_HSE_IsEnabled())
  {
    clockContext.directHSEenabled = TRUE;
  }

  /* Setup the wakeup sources */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_BLEHOST|PWR_WAKEUP_BLE, 0);

  /* This signal cannot be reset later otherwise the GPIO output will be
     automatically restored to high at wakeup. */
  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STOP_MODE_ENTER);

  /* Save all the peripheral registers and CPU peripipheral configuration */
  apb0.deepstop_wdg_state = ENABLE;
  prepareDeviceLowPower(&apb0, &apb1, &apb2, &ahb0, &cpuPeriph, cStackPreamble);

  /* Clear all the wake-up pin flags */
  LL_PWR_ClearWakeupSource(LL_PWR_WAKEUP_ALL);

  /* DEEPSTOP configuration */
  configDS.deepStopMode = PWR_DEEPSTOP_WITH_SLOW_CLOCK_ON;
  HAL_PWR_ConfigDEEPSTOP(&configDS);

  /* Enable the device DEEPSTOP configuration */
  LL_PWR_SetPowerMode(LL_PWR_MODE_DEEPSTOP);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
  
  /* Workaround: do not go to sleep if we have been awake for a too short time. */
  if(checkMinWakeUpTime() != 0)
  {
    return;
  }

  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STOP_MODE_ACTIVE);
  
#if (CFG_LPM_EMULATED == 1)
#if defined(PWR_CR2_GPIORET)
  LL_PWR_DisableGPIORET();
#endif
  LL_PWR_EnableDEEPSTOP2();
#endif

  /* Save the CPU context & Wait for Interrupt Request to enter in DEEPSTOP */
  CPUcontextSave();

  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STOP_MODE_ACTIVE);

  /* USER CODE BEGIN PWR_EnterStopMode_2 */

  /* USER CODE END PWR_EnterStopMode_2 */
}

void PWR_ExitStopMode( void )
{
  /* USER CODE BEGIN PWR_ExitStopMode_1 */

  /* USER CODE END PWR_ExitStopMode_1 */

  /* Workaround to avoid going to sleep too early after a wakeup.
     We need to have a time reference. We can save current value of tick.
   */
  if(RAM_VR.WakeupFromSleepFlag)
  {
    lastTick = HAL_GetTick();
  }
  
  /* Clear SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

  /* Restore all the peripheral registers and CPU peripipheral configuration */
  restoreDeviceLowPower(&apb0, &apb1, &apb2, &ahb0, &cpuPeriph, cStackPreamble);

  SYSTEM_DEBUG_SIGNAL_SET(LOW_POWER_STOP_MODE_EXIT);

#if defined(PWR_CR2_GPIORET)
  /* Disable the GPIO retention at wake DEEPSTOP configuration */
  LL_PWR_DisableGPIORET();
#endif

  /* Wait until the HSE is ready */
  while(LL_RCC_HSE_IsReady() == 0U);

  /* Restore the DIRECT_HSE configuration */
  if (clockContext.directHSEenabled == TRUE)
  {
    LL_RCC_DIRECT_HSE_Enable();
    LL_RCC_RC64MPLL_Disable();
  }

  if(RAM_VR.WakeupFromSleepFlag)
  {
    /* Handler to manage the IOs IRQ if needed */
    HAL_PWR_WKUP_IRQHandler();
  }

  /* USER CODE BEGIN PWR_ExitStopMode_2 */

  /* USER CODE END PWR_ExitStopMode_2 */

  SYSTEM_DEBUG_SIGNAL_RESET(LOW_POWER_STOP_MODE_EXIT);
}

void PWR_EnterSleepMode( void )
{
  /* USER CODE BEGIN PWR_EnterSleepMode */
  HAL_SuspendTick();
  HAL_PWR_EnterSLEEPMode();
  /* USER CODE END PWR_EnterSleepMode */
}

void PWR_ExitSleepMode( void )
{
  /* USER CODE BEGIN PWR_ExitSleepMode */
  HAL_ResumeTick();
  /* USER CODE END PWR_ExitSleepMode */
}

/* USER CODE BEGIN Private_Functions */

/* USER CODE END Private_Functions */

/**
 * @}
 */

/**
 * @}
 */

