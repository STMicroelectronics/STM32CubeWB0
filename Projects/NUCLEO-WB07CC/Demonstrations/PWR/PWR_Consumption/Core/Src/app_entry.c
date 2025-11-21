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
#include "stm32wb0x.h"
#include "stm32wb0x_ll_usart.h"
#if (CFG_LPM_SUPPORTED == 1)
#include "stm32_lpm.h"
#endif /* CFG_LPM_SUPPORTED */
#include "app_debug.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_lpm_if.h"

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

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define WAKEUP_TIMEOUT 5000 // 5 sec

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
static ButtonDesc_t buttonDesc[BUTTON_NB_MAX];
#endif
/* Section specific to button management using UART */
static uint8_t CommandString[C_SIZE_CMD_STRING];
static uint16_t indexReceiveChar = 0;

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
//static void RxCpltCallback(uint8_t * pRxDataBuff, uint16_t nDataSize);
static void UartCmdExecute(void);
static void configRadioWakeupTimer(uint32_t timeout);
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
extern RTC_HandleTypeDef hrtc;

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

uint32_t MX_APPE_Init(void *p_param)
{

  UNUSED(p_param);

//  APP_DEBUG_SIGNAL_SET(APP_APPE_INIT);

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

  UTIL_SEQ_RegTask(1U << CFG_TASK_UART, UTIL_SEQ_RFU, UART_Process);

#if (CFG_LPM_SUPPORTED == 1)
  /* Low Power Manager Init */
  UTIL_LPM_Init();
#endif /* CFG_LPM_SUPPORTED */
/* USER CODE BEGIN APPE_Init_2 */
  
  printf("Power Manager FW demo!\r\nDigit ? for help command\r\n");

#if defined(PWR_SDWN_WUEN_WUEN)
  if (LL_PWR_IsIOWakeupSDN())
  {
    printf("SHUTDOWN Wakeup from PB0\r\n");
    LL_PWR_ClearIOWakeupFlagSDN();
  }
#endif
  
/* USER CODE END APPE_Init_2 */
//  APP_DEBUG_SIGNAL_RESET(APP_APPE_INIT);
  return 0;
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
  PowerSaveLevels output_level = POWER_SAVE_LEVEL_CPU_HALT;
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
  // Loop in case nDataSize > 1 (always 1 in current implementation).
  /* Filling buffer and wait for '\r' char */
  if (indexReceiveChar < C_SIZE_CMD_STRING)
  {
    printf("%c", *pRxDataBuff);
    if (*pRxDataBuff == '\r')
    {
      UTIL_SEQ_SetTask(1U << CFG_TASK_UART, CFG_SEQ_PRIO_0);
    }
    else
    {
      CommandString[indexReceiveChar++] = *pRxDataBuff;
    }
  }
}

void UART_Process(void)
{
  UartCmdExecute();
  
  /* Clear receive buffer and character counter*/
  indexReceiveChar = 0;
  memset(CommandString, 0, C_SIZE_CMD_STRING);
}


static void UartCmdExecute(void)
{
  printf("\r\n");
  
  /* Parse received CommandString */
  if(strcmp((char const*)CommandString, "?") == 0)
  {
    help();
  }
  else if (strcmp((char const*)CommandString, "s") == 0)
  {
    PWR_SHUTDOWNTypeDef configSHD;
    
    printf("SHUTDOWN LEVEL : wakeup on a low pulse on the RSTN pad or a low pulse on PB0\n");
    
    while(((LL_USART_IsActiveFlag_TXE_TXFNF(USART1) == RESET) ||
           (LL_USART_IsActiveFlag_TC(USART1) == RESET) ||
             LL_USART_IsActiveFlag_TXFE(USART1) == RESET));
    
    /* Configure the device to allow the SHUTDOWN mode */
    configSHD.BORStatus = DISABLE;
    configSHD.WakeUpPinStatus = ENABLE;
    configSHD.WakeUpPol = PWR_WUP_FALLEDG;
    HAL_PWR_ConfigSHUTDOWN(&configSHD);
    
    /* Device in SHUTDONW mode */
    HAL_PWREx_EnterSHUTDOWNMode();  

  }
  else if (strcmp((char const*)CommandString, "t") == 0)
  {
    printf("DEEPSTOP with Low Speed clock enabled : wake on UART RX pin or timeout 5 sec or button PUSH1/PUSH2\n");
    
    while(((LL_USART_IsActiveFlag_TXE_TXFNF(USART1) == RESET) ||
           (LL_USART_IsActiveFlag_TC(USART1) == RESET) ||
             LL_USART_IsActiveFlag_TXFE(USART1) == RESET));

  /* Configures the Radio Wakeup Timer to wakeup the device after the Timeout */  
  configRadioWakeupTimer(WAKEUP_TIMEOUT);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PA0, PWR_WUP_RISIEDG);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB5, PWR_WUP_RISIEDG);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB0, PWR_WUP_FALLEDG);

  /* Low Power sequence */
  ATOMIC_SECTION_BEGIN();
  PWR_EnterStopMode();
  PWR_ExitStopMode();
  ATOMIC_SECTION_END();
  
  /* Disable the Radio IP */
  __HAL_RCC_RADIO_CLK_DISABLE();
  
  
  /* Print Wakeup Source */
  printf("Exit DEEPSTOP Mode with low speed clock enabled!\r\n");
  PrintWakeupSource(HAL_PWR_GetClearWakeupSource());
    
  }
  else if (strcmp((char const*)CommandString, "z") == 0)
  {
  uint32_t wakeupPin;
  
    printf("DEEPSTOP with Low Speed clock enabled : wake on UART RX pin or timeout 5 sec (RTC) or button PUSH1/PUSH2\n");
    
    while(((LL_USART_IsActiveFlag_TXE_TXFNF(USART1) == RESET) ||
           (LL_USART_IsActiveFlag_TC(USART1) == RESET) ||
             LL_USART_IsActiveFlag_TXFE(USART1) == RESET));

  /* Enable the RTC IP */
  __HAL_RCC_RTC_CLK_ENABLE();

  /* Configures the RTC Wakeup Timer to wakeup the device after the Timeout */  
  HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, ((WAKEUP_TIMEOUT/1000)*2048), RTC_WAKEUPCLOCK_RTCCLK_DIV16);
  HAL_PWR_EnableWakeUpPin(LL_PWR_WAKEUP_RTC, 0);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PA0, PWR_WUP_RISIEDG);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB5, PWR_WUP_RISIEDG);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB0, PWR_WUP_FALLEDG);

  /* Low Power sequence */
  ATOMIC_SECTION_BEGIN();
  PWR_EnterStopMode();
  PWR_ExitStopMode();
  wakeupPin = HAL_PWR_GetClearWakeupSource();
  ATOMIC_SECTION_END();
  
  /* Disable the RTC IP */
  __HAL_RCC_RTC_CLK_DISABLE();
  
  /* Print Wakeup Source */
  printf("Exit DEEPSTOP Mode with low speed clock enabled!\r\n");
  PrintWakeupSource(wakeupPin);
  
  }
  else if(strcmp((char const*)CommandString, "d") == 0)
  {
    printf("DEEPSTOP : wake on UART RX pin or button PUSH1/PUSH2\n");
      
    while(((LL_USART_IsActiveFlag_TXE_TXFNF(USART1) == RESET) ||
           (LL_USART_IsActiveFlag_TC(USART1) == RESET) ||
             LL_USART_IsActiveFlag_TXFE(USART1) == RESET));
  
  /* Wakeup source configuration */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PA0, PWR_WUP_RISIEDG);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB5, PWR_WUP_RISIEDG);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB0, PWR_WUP_FALLEDG);
  
  /* Low Power sequence */
  ATOMIC_SECTION_BEGIN();
  PWR_EnterOffMode();
  PWR_ExitOffMode();
  ATOMIC_SECTION_END();
  
  /* Print Wakeup Source */
  printf("Exit DEEPSTOP Mode!\r\n");
  PrintWakeupSource(HAL_PWR_GetClearWakeupSource());
  
  }
  else if(strcmp((char const*)CommandString, "w") == 0)
  {
    printf("SLEEP (WFI) : wake on button PUSH1/PUSH2\n");
    
    while(((LL_USART_IsActiveFlag_TXE_TXFNF(USART1) == RESET) ||
           (LL_USART_IsActiveFlag_TC(USART1) == RESET) ||
             LL_USART_IsActiveFlag_TXFE(USART1) == RESET));
  
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PA0, PWR_WUP_RISIEDG);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PB5, PWR_WUP_RISIEDG);
  
  /* Disable the SysTick */
  HAL_SuspendTick();

  /* Device in SLEEP mode (WFI) */
  HAL_PWR_EnterSLEEPMode();
  
  /* Enable the SysTick */
  HAL_ResumeTick();
  
  printf("Exit Sleep (WFI) Mode\r\n");
  
  }
  else if (strcmp((char const*)CommandString, "l") == 0)
  {
    printf("Toggle led LED\n");
    BSP_LED_Toggle(LED_BLUE);
  }
  else if (strcmp((char const*)CommandString, "p") == 0)
  {
    printf("Print Hello World message\n");
  }
  else if (strcmp((char const*)CommandString, "r") == 0)
  {
    printf("Device Reset\n");
    while(((LL_USART_IsActiveFlag_TXE_TXFNF(USART1) == RESET) ||
      (LL_USART_IsActiveFlag_TC(USART1) == RESET) ||
        LL_USART_IsActiveFlag_TXFE(USART1) == RESET));
    for(volatile uint16_t i=0; i<0xFFF; i++);
    NVIC_SystemReset();
  }
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
  UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
  /* USER CODE BEGIN MX_APPE_Process_2 */

  /* USER CODE END MX_APPE_Process_2 */
}
void UTIL_SEQ_PreIdle( void )
{
#if (CFG_LPM_SUPPORTED == 1)
  /* USER CODE BEGIN UTIL_SEQ_PREIDLE */

  /* USER CODE END UTIL_SEQ_PREIDLE */
#endif /* CFG_LPM_SUPPORTED */
  return;
}

void UTIL_SEQ_Idle( void )
{
#if (CFG_LPM_SUPPORTED == 1)

//  /* Need to consume some CSTACK on WB05, due to bootloader CSTACK usage. */
//  volatile uint32_t dummy[15];
//  uint8_t i;
//  for (i=0; i<10; i++)
//  {
//    dummy[i] = 0;
//    __NOP();
//  }
//
//  PowerSaveLevels app_powerSave_level, vtimer_powerSave_level, final_level, pka_level;
//
//  if ((BLE_STACK_SleepCheck() != POWER_SAVE_LEVEL_RUNNING) &&
//      ((app_powerSave_level = App_PowerSaveLevel_Check()) != POWER_SAVE_LEVEL_RUNNING))
//  {
//    vtimer_powerSave_level = HAL_RADIO_TIMER_PowerSaveLevelCheck();
//    pka_level = (PowerSaveLevels) HW_PKA_PowerSaveLevelCheck();
//    final_level = (PowerSaveLevels)MIN(vtimer_powerSave_level, app_powerSave_level);
//    final_level = (PowerSaveLevels)MIN(pka_level, final_level);
//
//    switch(final_level)
//    {
//    case POWER_SAVE_LEVEL_RUNNING:
//      /* Not Power Save device is busy */
//      return;
//      break;
//    case POWER_SAVE_LEVEL_CPU_HALT:
//      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
//      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
//      break;
//    case POWER_SAVE_LEVEL_STOP_LS_CLOCK_ON:
//      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
//      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
//      break;
//    case POWER_SAVE_LEVEL_STOP:
//      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
//      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
//      break;
//    }
//
//  /* USER CODE BEGIN UTIL_SEQ_IDLE_BEGIN */
//
//  /* USER CODE END UTIL_SEQ_IDLE_BEGIN */
//
//    UTIL_LPM_EnterLowPower();
//
//  /* USER CODE BEGIN UTIL_SEQ_IDLE_END */
//
//  /* USER CODE END UTIL_SEQ_IDLE_END */
//  }
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


/**
  * @brief  Display the help functions.
  * @param  None
  * @retval None
  */
void help(void)
{
  printf("POWER MANAGER help commands:\r\n");
  printf("s:  SHUTDOWN LEVEL : wakeup on a low pulse on the RSTN pad or a low pulse on PB0\r\n");
  printf("t:  DEEPSTOP with Low Speed clock enabled : wake on UART RX pin or timeout 5 sec or button PUSH1/PUSH2\r\n");
  printf("z:  DEEPSTOP with Low Speed clock enabled : wake on UART RX pin or timeout 5 sec (RTC) or button PUSH1/PUSH2\r\n");
  printf("d:  DEEPSTOP : wake on UART RX pin or button PUSH1/PUSH2\r\n");
  printf("w:  SLEEP (WFI) : wake on button PUSH1/PUSH2\r\n");
  printf("l:  Toggle led LED2\r\n");
  printf("p:  Print Hello World message\r\n");
  printf("r:  Device Reset\r\n");
  printf("?:  Display this help menu\r\n");
  printf("\r\n> ");
}

/**
  * @brief  Display the Wakeup Source.
  * @param  wakeupSource Wakeup Sources
  * @retval None
  */
void PrintWakeupSource(uint32_t wakeupSources)
{  
  uint8_t wakeup_reasons[][30] = {"WAKEUP_PB0", "WAKEUP_PB1", "WAKEUP_PB2", "WAKEUP_PB3",  "WAKEUP_PB4",
                                  "WAKEUP_PB5", "WAKEUP_PB6", "WAKEUP_PB7", "WAKEUP_PA8",  "WAKEUP_PA9",
                                  "WAKEUP_PA10", "WAKEUP_PA11", "WAKEUP_BLE", "WAKEUP_BLE_HOST_TIMER",
                                  "WAKEUP_LPU", "WAKEUP_RTC",
                                  "WAKEUP_PA0", "WAKEUP_PA1", "WAKEUP_PA2", "WAKEUP_PA3",
                                  "WAKEUP_PB12", "WAKEUP_PB13", "WAKEUP_PB14", "WAKEUP_PB15"};
                                    
  
  if (wakeupSources != 0)
  {
    printf("Wakeup Sources : ");
    for (int i=0; i<32; i++)
    {
      if ((wakeupSources >> i) & 1)
      {
        printf("%s ", wakeup_reasons[i]);
      }
    }
    printf("\r\n");
  }
}

/**
  * @brief  Configures the radio wakeup timer, to wakeup the device from DEEPSTOP
  *         at specified timeout
  * @param  timeout wakeup timeout expressed in ms
  */
static void configRadioWakeupTimer(uint32_t timeout)
{
  uint32_t freq, timeoutTick;
  uint32_t wakeupOffset;
  
  /* Enable the BLUE Radio Peripheral */
  if (__HAL_RCC_RADIO_IS_CLK_ENABLED() == 0)
  {
    __HAL_RCC_RF_CLK_SET_CONFIG(RCC_RF_CLK_16M);
    __HAL_RCC_RADIO_CLK_ENABLE();
    
  }
  
  /* Get the correct low speed frequency */
  freq = 32768;
  if (__HAL_RCC_GET_LSI_READYFLAG())
  {
    LL_RADIO_TIMER_GetLSIFrequency(RADIO_CTRL);
    freq = LL_RADIO_TIMER_GetLSIFrequency(RADIO_CTRL);
    printf("LSI frequency = %d Hz\r\n", freq);
  }
  
  /* Wait until UART is not busy */
  while(((LL_USART_IsActiveFlag_TXE_TXFNF(USART1) == RESET) ||
         (LL_USART_IsActiveFlag_TC(USART1) == RESET) ||
           LL_USART_IsActiveFlag_TXFE(USART1) == RESET));
  
  /* Calculate Wakeup OFFSET */
  wakeupOffset = ((780*freq)/1000000)+1;
  WAKEUP->WAKEUP_OFFSET[0] = wakeupOffset;
  
  /* Calculate the Radio Wakeup Timer Tick */
  timeoutTick = (timeout*freq)/1000;
  
  /* Configure the BLE CPU Host wakeup block */
  SET_BIT(WAKEUP->CM0_SLEEP_REQUEST_MODE, WAKEUP_CM0_SLEEP_REQUEST_MODE_CPU_WAKEUP_EN);
  SET_BIT(WAKEUP->BLUE_SLEEP_REQUEST_MODE, WAKEUP_BLUE_SLEEP_REQUEST_MODE_SLEEP_EN);
  CLEAR_BIT(WAKEUP->BLUE_SLEEP_REQUEST_MODE, WAKEUP_BLUE_SLEEP_REQUEST_MODE_FORCE_SLEEPING);
  CLEAR_BIT(WAKEUP->CM0_SLEEP_REQUEST_MODE, WAKEUP_CM0_SLEEP_REQUEST_MODE_FORCE_SLEEPING);
  WAKEUP->CM0_WAKEUP_TIME = (uint32_t)(WAKEUP->ABSOLUTE_TIME + (timeoutTick<<4));
}

/* USER CODE END FD_WRAP_FUNCTIONS */
