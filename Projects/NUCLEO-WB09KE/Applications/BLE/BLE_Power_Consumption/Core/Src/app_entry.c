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
#include "stm32_seq.h"
#include "app_ble.h"
#include "hw_rng.h"
#include "hw_aes.h"
#include "hw_pka.h"
#include "stm32wb0x.h"
#include "stm32wb0x_ll_usart.h"
#include "ble_stack.h"
#if (CFG_LPM_SUPPORTED == 1)
#include "stm32_lpm.h"
#endif /* CFG_LPM_SUPPORTED */
#include "app_debug.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
#if (CFG_BUTTON_SUPPORTED == 1)
typedef struct
{
  Button_TypeDef      button;
  VTIMER_HandleType   longTimerId;
  uint8_t             longPressed;
} ButtonDesc_t;
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */
#if (CFG_BUTTON_SUPPORTED == 1)
#define BUTTON_LONG_PRESS_THRESHOLD_MS   (500u)
#define BUTTON_NB_MAX                    (B3 + 1u)
#endif
/* Section specific to button management using UART */
#define C_SIZE_CMD_STRING       (32U)

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
static ButtonDesc_t buttonDesc[BUTTON_NB_MAX];
#endif
#if (CFG_LPM_SUPPORTED == 0)
/* Section specific to button management using UART */
static uint8_t CommandString[C_SIZE_CMD_STRING];
static uint16_t indexReceiveChar = 0;
#endif

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/

/* USER CODE BEGIN PFP */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init(void);
#endif
#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_Init(void);
static void Button_TriggerActions(void *arg);
#endif
/* Section specific to button management using UART */
static void RxUART_Init(void);
#if (CFG_LPM_SUPPORTED == 0)
static void UartCmdExecute(void);
static void HelpMenu(void);
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
#if (CFG_BUTTON_SUPPORTED == 1)
  Button_Init();
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
  
  RxUART_Init();
  
  /* USER CODE END APPE_Init_1 */

  if (HW_RNG_Init() != HW_RNG_SUCCESS)
  {
    Error_Handler();
  }

  /* Init the AES block */
  HW_AES_Init();
  HW_PKA_Init();
  APP_BLE_Init();

#if (CFG_LPM_SUPPORTED == 1)
  /* Low Power Manager Init */
  UTIL_LPM_Init();
#endif /* CFG_LPM_SUPPORTED */
/* USER CODE BEGIN APPE_Init_2 */
  APP_DBG_MSG("Power Consumption Demo\r\n");
  APP_DBG_MSG("Press button B1 for fast advertising (100 ms).\r\n");
  APP_DBG_MSG("Press button B2 for slow advertising (1 s).\r\n");
  APP_DBG_MSG("Press button B3 to disconnect.\r\n");
  
#if (CFG_LPM_SUPPORTED == 0)
  APP_DBG_MSG("Press ? and <ENTER> for list of commands\r\n");
#endif
/* USER CODE END APPE_Init_2 */
  APP_DEBUG_SIGNAL_RESET(APP_APPE_INIT);
  return BLE_STATUS_SUCCESS;
}

/* USER CODE BEGIN FD */
#if (CFG_BUTTON_SUPPORTED == 1)
/**
 * @brief   Indicate if the selected button was pressedn during a 'long time' or not.
 *
 * @param   btnIdx    Button to test, listed in enum Button_TypeDef
 * @return  '1' if pressed during a 'long time', else '0'.
 */
uint8_t APPE_ButtonIsLongPressed(uint16_t btnIdx)
{
  uint8_t pressStatus;

  if ( btnIdx < BUTTON_NB_MAX )
  {
    pressStatus = buttonDesc[btnIdx].longPressed;
  }
  else
  {
    pressStatus = 0;
  }

  return pressStatus;
}

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
  
  /* Enable UART CLOCK for the DEBUG MSG */
  __HAL_RCC_USART1_CLK_ENABLE();
  
  /* Wait until the UART FIFO is empty */
  if ((__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_TXFNF) == 0) ||
      (__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_TC) == 0))

  {
    output_level = POWER_SAVE_LEVEL_RUNNING;
  }
  
  /* Disable UART CLOCK to reduce the power consumption */
  __HAL_RCC_USART1_CLK_DISABLE();
  /* USER CODE END App_PowerSaveLevel_Check_1 */

  return output_level;
}
#endif

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_Init( void )
{
  /* Button Initialization */
  buttonDesc[B1].button = B1;
  buttonDesc[B2].button = B2;
  buttonDesc[B3].button = B3;
  BSP_PB_Init(B1, BUTTON_MODE_EXTI);
  BSP_PB_Init(B2, BUTTON_MODE_EXTI);
  BSP_PB_Init(B3, BUTTON_MODE_EXTI);
  
#if (CFG_LPM_SUPPORTED == 1)
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PA0, PWR_WUP_FALLEDG);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB5, PWR_WUP_FALLEDG);
#if defined(STM32WB06) || defined(STM32WB07)
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB9, PWR_WUP_FALLEDG);
#else
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB14, PWR_WUP_FALLEDG);
#endif  
#endif  
  
  /* Register tasks associated to buttons */
  UTIL_SEQ_RegTask(1U << TASK_BUTTON_1, UTIL_SEQ_RFU, APPE_Button1Action);
  UTIL_SEQ_RegTask(1U << TASK_BUTTON_2, UTIL_SEQ_RFU, APPE_Button2Action);
  UTIL_SEQ_RegTask(1U << TASK_BUTTON_3, UTIL_SEQ_RFU, APPE_Button3Action);

  /* Create timers to detect button long press (one for each button) */
  Button_TypeDef buttonIndex;
  for ( buttonIndex = B1; buttonIndex < BUTTON_NB_MAX; buttonIndex++ )
  {
    buttonDesc[buttonIndex].longTimerId.callback = Button_TriggerActions;
    buttonDesc[buttonIndex].longTimerId.userData = &buttonDesc[buttonIndex];
  }
  
  return;
}

static void Button_TriggerActions(void *arg)
{
  ButtonDesc_t *p_buttonDesc = ((VTIMER_HandleType *)arg)->userData;

  p_buttonDesc->longPressed = BSP_PB_GetState(p_buttonDesc->button);

  APP_DBG_MSG("Button %d pressed\n", (p_buttonDesc->button + 1));
  switch (p_buttonDesc->button)
  {
    case B1:
      UTIL_SEQ_SetTask(1U << TASK_BUTTON_1, CFG_SEQ_PRIO_0);
      break;
    case B2:
      UTIL_SEQ_SetTask(1U << TASK_BUTTON_2, CFG_SEQ_PRIO_0);
      break;
    case B3:
      UTIL_SEQ_SetTask(1U << TASK_BUTTON_3, CFG_SEQ_PRIO_0);
      break;
    default:
      break;
  }

  return;
}

#endif

static void RxUART_Init(void)
{
  /* Enable the RX not empty interrupt */
  LL_USART_EnableIT_RXNE(USART1);
  
  /* Enable the UART IRQ */
  NVIC_SetPriority(USART1_IRQn, IRQ_HIGH_PRIORITY);
  NVIC_EnableIRQ(USART1_IRQn);
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif
}

void UartRxCpltCallback(uint8_t * pRxDataBuff, uint16_t nDataSize)
{
#if (CFG_LPM_SUPPORTED == 0)  
  /* Filling buffer and wait for '\n' char */
  if (indexReceiveChar < C_SIZE_CMD_STRING)
  {
    if (*pRxDataBuff == '\n')
    {
      APP_DBG_MSG("Command received %s\n", CommandString);

      UartCmdExecute();

      /* Clear receive buffer and character counter*/
      indexReceiveChar = 0;
      memset(CommandString, 0, C_SIZE_CMD_STRING);
    }
    else
    {
      CommandString[indexReceiveChar++] = *pRxDataBuff;
    }
  }
#endif  
}

#if (CFG_LPM_SUPPORTED == 0)
static void UartCmdExecute(void)
{
  /* Parse received CommandString */
  if((strcmp((char const*)CommandString, "f\r") == 0) || (strcmp((char const*)CommandString, "f\r") == 0))
  {
    UTIL_SEQ_SetTask(1U << TASK_BUTTON_1, CFG_SEQ_PRIO_0);
  }
  else if ((strcmp((char const*)CommandString, "s") == 0) || (strcmp((char const*)CommandString, "s\r") == 0))
  {
    UTIL_SEQ_SetTask(1U << TASK_BUTTON_2, CFG_SEQ_PRIO_0);
  }
  else if ((strcmp((char const*)CommandString, "d") == 0) || (strcmp((char const*)CommandString, "d\r") == 0))
  {
    UTIL_SEQ_SetTask(1U << TASK_BUTTON_3, CFG_SEQ_PRIO_0);
  }
  else if ((strcmp((char const*)CommandString, "r") == 0) || (strcmp((char const*)CommandString, "r\r") == 0))
  {
    APP_DBG_MSG("Device Reset \n");
    NVIC_SystemReset();
  }
  else if ((strcmp((char const*)CommandString, "?") == 0) || (strcmp((char const*)CommandString, "?\r") == 0))
  {
    APP_DBG_MSG("Help:\n");
    HelpMenu();
  }
  else
  {
    APP_DBG_MSG("NOT RECOGNIZED COMMAND : %s\n", CommandString);
  }
}

static void HelpMenu(void)
{
  APP_DBG_MSG("\r\n");
  APP_DBG_MSG("f (or Press B1):   Device in Discoverable mode with fast interval 100 ms \r\n");
  APP_DBG_MSG("s (or press B2):   Device in discoverable mode with slow interval 1000 ms \r\n");
  APP_DBG_MSG("d (or press B3):   Disconnection Procedure \r\n");
  APP_DBG_MSG("r:   Reset the device\r\n");
  APP_DBG_MSG("?:   Display this help menu\r\n");
  APP_DBG_MSG("\r\n> ");
}
#endif /*  (CFG_LPM_SUPPORTED == 0) */

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
  UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
  /* USER CODE BEGIN MX_APPE_Process_2 */

  /* USER CODE END MX_APPE_Process_2 */
}
void UTIL_SEQ_PreIdle( void )
{
#if (CFG_LPM_SUPPORTED == 1)
  /* USER CODE BEGIN UTIL_SEQ_PREIDLE */
  /* Wait until the UART FIFO is empty */
  while ((__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_TXFNF) == 0) ||
         (__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_TC) == 0));
  /* USER CODE END UTIL_SEQ_PREIDLE */
#endif /* CFG_LPM_SUPPORTED */
  return;
}

void UTIL_SEQ_Idle( void )
{
#if (CFG_LPM_SUPPORTED == 1)

  /* Need to consume some CSTACK on WB05, due to bootloader CSTACK usage. */
  volatile uint32_t dummy[15];
  uint8_t i;
  for (i=0; i<10; i++)
  {
    dummy[i] = 0;
    __NOP();
  }

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

  /* USER CODE BEGIN UTIL_SEQ_IDLE_BEGIN */
  /* Enable UART CLOCK to save the register settings */
  __HAL_RCC_USART1_CLK_ENABLE();
  /* USER CODE END UTIL_SEQ_IDLE_BEGIN */

    UTIL_LPM_EnterLowPower();

  /* USER CODE BEGIN UTIL_SEQ_IDLE_END */
  /* Disable UART CLOCK to reduce the power consumption */
  __HAL_RCC_USART1_CLK_DISABLE();
  /* USER CODE END UTIL_SEQ_IDLE_END */
  }
#endif /* CFG_LPM_SUPPORTED */
}

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
#if (CFG_BUTTON_SUPPORTED == 1)
void BSP_PB_Callback(Button_TypeDef Button)
{
  buttonDesc[Button].longPressed = 0;
  HAL_RADIO_TIMER_StartVirtualTimer(&buttonDesc[Button].longTimerId, BUTTON_LONG_PRESS_THRESHOLD_MS);

  return;
}

#if (CFG_LPM_SUPPORTED == 1)
void HAL_PWR_WKUPx_Callback(uint32_t wakeupIOs)
{
  if (wakeupIOs & PWR_WAKEUP_PA0)
  {
    BSP_PB_Callback(B1);
  }
  if (wakeupIOs & PWR_WAKEUP_PB5)
  {
    BSP_PB_Callback(B2);
  }

#if defined(STM32WB06) || defined(STM32WB07)
  if (wakeupIOs & PWR_WAKEUP_PB9)
  {
    BSP_PB_Callback(B3);
  }
#else
  if (wakeupIOs & PWR_WAKEUP_PB14)
  {
    BSP_PB_Callback(B3);
  }
#endif

}
#endif

void HAL_GPIO_EXTI_Callback(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  if (GPIO_Pin == B1_PIN)
  {
    BSP_PB_Callback(B1);
  }
  else if (GPIO_Pin == B2_PIN)
  {
    BSP_PB_Callback(B2);
  }
  else if (GPIO_Pin == B3_PIN)
  {
    BSP_PB_Callback(B3);
  }

  return;
}

#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* USER CODE END FD_WRAP_FUNCTIONS */
