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
#include "stm32_lpm.h"
#endif /* CFG_LPM_SUPPORTED */
#include "app_debug.h"
#include "FreeRTOS.h"
#include "task.h"

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
#define C_SIZE_CMD_STRING       256U

/* It is important to set same priority for BLE Task and other tasks calling
   BLE stack API's to avoid protecting these functions from concurrent calls. */
#define BUTTON1_TASK_STACK_SIZE         (800)
#define BUTTON1_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define BUTTON2_TASK_STACK_SIZE         (800)
#define BUTTON2_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define BUTTON3_TASK_STACK_SIZE         (800)
#define BUTTON3_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
static ButtonDesc_t buttonDesc[BUTTON_NB_MAX];

TaskHandle_t Button1ActionTask;
TaskHandle_t Button2ActionTask;
TaskHandle_t Button3ActionTask;
#endif
/* Section specific to button management using UART */
static uint8_t CommandString[C_SIZE_CMD_STRING];
static uint16_t indexReceiveChar = 0;

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/
void vApplicationIdleHook(void);
void vApplicationTickHook(void);

/* USER CODE BEGIN PFP */
#if (CFG_LED_SUPPORTED == 1)
static void Led_Init(void);
#endif
#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_Init(void);
static void Button_TriggerActions(void *arg);
void APP_ENTRY_Key_Button1_Task(void *pvParameters);
void APP_ENTRY_Key_Button2_Task(void *pvParameters);
void APP_ENTRY_Key_Button3_Task(void *pvParameters);
#endif
/* Section specific to button management using UART */
static void RxUART_Init(void);
//static void RxCpltCallback(uint8_t * pRxDataBuff, uint16_t nDataSize);
static void UartCmdExecute(void);
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

/* USER CODE END APPE_Init_2 */
  APP_DEBUG_SIGNAL_RESET(APP_APPE_INIT);
  return BLE_STATUS_SUCCESS;
}

/*------------ FreeRTOS Hooks--------------------------------*/

void vApplicationMallocFailedHook( void )
{
  /* vApplicationMallocFailedHook() will only be called if
  configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
  function that will get called if a call to pvPortMalloc() fails.
  pvPortMalloc() is called internally by the kernel whenever a task, queue,
  timer or semaphore is created.  It is also called by various parts of the
  demo application.  If heap_1.c or heap_2.c are used, then the size of the
  heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
  FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
  to query the size of free heap space that remains (although it does not
  provide information on how the remaining heap might be fragmented). */
  taskDISABLE_INTERRUPTS();
  puts("MALLOC FAIL ");
  for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
  /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
  to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
  task.  It is essential that code added to this hook function never attempts
  to block in any way (for example, call xQueueReceive() with a block time
  specified, or call vTaskDelay()).  If the application makes use of the
  vTaskDelete() API function (as this demo application does) then it is also
  important that vApplicationIdleHook() is permitted to return to its calling
  function, because it is the responsibility of the idle task to clean up
  memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
  ( void ) pcTaskName;
  ( void ) pxTask;
  
  /* Run time stack overflow checking is performed if
  configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
  function is called if a stack overflow is detected. */
  taskDISABLE_INTERRUPTS();
  puts("STACK OVERFLOW ");
  puts(pcTaskName);
  for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
  /* This function will be called by each tick interrupt if
  configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
  added here, but the tick hook is called from an interrupt context, so
  code must not attempt to block, and only the interrupt safe FreeRTOS API
  functions can be used (those that end in FromISR()). */
}
/*-----------------------------------------------------------*/

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
PowerSaveLevels App_PowerSaveLevel_Check(void)
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
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  BSP_LED_On(LED_GREEN);

  return;
}
#endif

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
  xTaskCreate(APP_ENTRY_Key_Button1_Task, "BUTTON1", BUTTON1_TASK_STACK_SIZE>>2, NULL, BUTTON1_TASK_PRIORITY, &Button1ActionTask);
  xTaskCreate(APP_ENTRY_Key_Button2_Task, "BUTTON2", BUTTON2_TASK_STACK_SIZE>>2, NULL, BUTTON2_TASK_PRIORITY, &Button2ActionTask);
  xTaskCreate(APP_ENTRY_Key_Button3_Task, "BUTTON3", BUTTON3_TASK_STACK_SIZE>>2, NULL, BUTTON3_TASK_PRIORITY, &Button3ActionTask);

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
      xTaskNotifyGive(Button1ActionTask);
      break;
    case B2:
      xTaskNotifyGive(Button2ActionTask);
      break;
    case B3:
      xTaskNotifyGive(Button3ActionTask);
      break;
    default:
      break;
  }

  return;
}

void APP_ENTRY_Key_Button1_Task(void *pvParameters)
{
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    APPE_Button1Action();
  }
}

void APP_ENTRY_Key_Button2_Task(void *pvParameters)
{
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    APPE_Button2Action();
  }
}

void APP_ENTRY_Key_Button3_Task(void *pvParameters)
{
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    APPE_Button3Action();
  }
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
  // Loop in case nDataSize > 1 (always 1 in current implementation).
  /* Filling buffer and wait for '\r' char */
  if (indexReceiveChar < C_SIZE_CMD_STRING)
  {
    if (*pRxDataBuff == '\r')
    {
      APP_DBG_MSG("received %s\n", CommandString);

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
}

static void UartCmdExecute(void)
{
  /* Parse received CommandString */
  if(strcmp((char const*)CommandString, "SW1") == 0)
  {
    APP_DBG_MSG("SW1 OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B1].longPressed = 0;
    xTaskNotifyGive(Button1ActionTask);
#endif
  }
  else if (strcmp((char const*)CommandString, "SW2") == 0)
  {
    APP_DBG_MSG("SW2 OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B2].longPressed = 0;
    xTaskNotifyGive(Button2ActionTask);
#endif
  }
  else if (strcmp((char const*)CommandString, "SW3") == 0)
  {
    APP_DBG_MSG("SW3 OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B3].longPressed = 0;
    xTaskNotifyGive(Button3ActionTask);
#endif
  }
  else if(strcmp((char const*)CommandString, "SW1_LONG") == 0)
  {
    APP_DBG_MSG("SW1_LONG OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B1].longPressed = 1;
    xTaskNotifyGive(Button1ActionTask);
#endif
  }
  else if (strcmp((char const*)CommandString, "SW2_LONG") == 0)
  {
    APP_DBG_MSG("SW2_LONG OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B2].longPressed = 1;
    xTaskNotifyGive(Button2ActionTask);
#endif
  }
  else if (strcmp((char const*)CommandString, "SW3_LONG") == 0)
  {
    APP_DBG_MSG("SW3_LONG OK\n");
#if (CFG_BUTTON_SUPPORTED == 1)
    buttonDesc[B3].longPressed = 1;
    xTaskNotifyGive(Button3ActionTask);
#endif
  }
  else
  {
    APP_DBG_MSG("NOT RECOGNIZED COMMAND : %s\n", CommandString);
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

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
