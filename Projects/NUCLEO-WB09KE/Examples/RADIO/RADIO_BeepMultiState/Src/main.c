/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct{
  uint8_t activity_flag;
  uint32_t wakeup_time[CFG_NUM_RADIO_TASKS];
}multi_state_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DATA_LEN                (uint8_t)(26)
#define N_STATE_MACHINES        CFG_NUM_RADIO_TASKS /* The number of state machines */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

multi_state_t multi_state = {0, {0,}};

uint8_t channel_map[5] = {0xFF,0xFF,0xFF,0xFF,0x0F};
uint8_t channel_values[CFG_NUM_RADIO_TASKS] = {21,22,23,25,26,27,28,29};
uint32_t period_usec_values[CFG_NUM_RADIO_TASKS] = {100000,100000,100000,100000,100000,100000,100000,100000};
uint32_t network_id_values[CFG_NUM_RADIO_TASKS] = {0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6};
uint8_t encryption_values[CFG_NUM_RADIO_TASKS] = {0,0,0,1,1,1,1,0};

uint8_t channel[N_STATE_MACHINES];
uint32_t period_usec[N_STATE_MACHINES];
uint32_t network_id[N_STATE_MACHINES];
uint8_t encryption[N_STATE_MACHINES];

uint8_t sendData[N_STATE_MACHINES][DATA_LEN+3];
ActionPacket actPacket[N_STATE_MACHINES];

uint8_t count_tx[5] = {0x00,0x00,0x00,0x00,0x00};
uint8_t count_rcv[5] = {0x00,0x00,0x00,0x00,0x00};
uint8_t enc_key[16] = {0xBF,0x01,0xFB,0x9D,0x4E,0xF3,0xBC,0x36,0xD8,0x74,0xF5,0x39,0x41,0x38,0x68,0x4C};
uint8_t enc_iv[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RADIO_Init(void);
static void MX_RADIO_TIMER_Init(void);
/* USER CODE BEGIN PFP */

void multi_state_scheduler(multi_state_t * multi_state);
uint8_t conditionRoutine(ActionPacket* p);
uint8_t dataRoutine(ActionPacket* p,  ActionPacket* next);
void beep_init(uint8_t StateMachineNo);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RADIO_Init();
  MX_RADIO_TIMER_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);
  
  COM_InitTypeDef COM_Init = {0};

  COM_Init.BaudRate= 115200;
  COM_Init.HwFlowCtl = COM_HWCONTROL_NONE;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.Parity = COM_PARITY_NONE;
  COM_Init.StopBits = COM_STOPBITS_1;
  BSP_COM_Init(COM1, &COM_Init);
  
  /* Set the parameters and configure the state machines */
  for(uint8_t i = 0; i < N_STATE_MACHINES; i++) {
    network_id[i] = network_id_values[i];
    channel[i] = channel_values[i];
    encryption[i] = encryption_values[i];
    period_usec[i] = period_usec_values[i];
    
    beep_init(i);
  }
  
  /* Call this function for the first action packet to be executed */
  HAL_RADIO_MakeActionPacketPending(&actPacket[0]);  
  
  BSP_LED_On(LD1);
  printf("\n Beep_Multistate Started \n");
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_RADIO_TIMER_Tick();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource and SYSCLKDivider
  */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_RC64MPLL;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_RC64MPLL_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_WAIT_STATES_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLK_DIV4;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RADIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_RADIO_Init(void)
{

  /* USER CODE BEGIN RADIO_Init 0 */

  /* USER CODE END RADIO_Init 0 */

  RADIO_HandleTypeDef hradio = {0};

  /* USER CODE BEGIN RADIO_Init 1 */

  /* USER CODE END RADIO_Init 1 */
  hradio.Instance = RADIO;
  HAL_RADIO_Init(&hradio);
  /* USER CODE BEGIN RADIO_Init 2 */

  /* USER CODE END RADIO_Init 2 */

}

/**
  * @brief RADIO_TIMER Initialization Function
  * @param None
  * @retval None
  */
static void MX_RADIO_TIMER_Init(void)
{

  /* USER CODE BEGIN RADIO_TIMER_Init 0 */

  /* USER CODE END RADIO_TIMER_Init 0 */

  RADIO_TIMER_InitTypeDef RADIO_TIMER_InitStruct = {0};

  /* USER CODE BEGIN RADIO_TIMER_Init 1 */

  /* USER CODE END RADIO_TIMER_Init 1 */

  if (__HAL_RCC_RADIO_IS_CLK_DISABLED())
  {
    /* Radio Peripheral reset */
    __HAL_RCC_RADIO_FORCE_RESET();
    __HAL_RCC_RADIO_RELEASE_RESET();

    /* Enable Radio peripheral clock */
    __HAL_RCC_RADIO_CLK_ENABLE();
  }
  /* Wait to be sure that the Radio Timer is active */
  while(LL_RADIO_TIMER_GetAbsoluteTime(WAKEUP) < 0x10);
  RADIO_TIMER_InitStruct.XTAL_StartupTime = 320;
  RADIO_TIMER_InitStruct.enableInitialCalibration = FALSE;
  RADIO_TIMER_InitStruct.periodicCalibrationInterval = 0;
  HAL_RADIO_TIMER_Init(&RADIO_TIMER_InitStruct);
  /* USER CODE BEGIN RADIO_TIMER_Init 2 */

  /* USER CODE END RADIO_TIMER_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  multi_state_scheduler routine.
  * @param  multi_state
  * @retval void
  */
void multi_state_scheduler(multi_state_t * multi_state)
{
  uint32_t next_wakeup_time_min = multi_state->wakeup_time[0];
  uint8_t i = 0;
  uint8_t index = 0;
  
  /* Find the minimum wakeup time among the active state machine */
  for(i = 1;i<CFG_NUM_RADIO_TASKS;i++)
  {
    if((next_wakeup_time_min > multi_state->wakeup_time[i]) && ((multi_state->activity_flag >> i)&0x01))
    {
      next_wakeup_time_min = multi_state->wakeup_time[i];
      index  = i;
    }
  }
  printf("Sending Frame on channel [%d]\n", channel[index]);
  HAL_RADIO_SetChannel(actPacket[index].StateMachineNo, channel[index], 0);
  HAL_RADIO_MakeActionPacketPending(&actPacket[index]);
  multi_state->wakeup_time[index] +=(period_usec[index] >> 10);
}

/**
  * @brief  Condition routine.
  * @param  ActionPacket
  * @retval TRUE
  */
uint8_t conditionRoutine(ActionPacket* p)
{
  return TRUE;   
} 

/**
  * @brief  Data routine.
  * @param  ActionPacket: current
  * @param  ActionPacket: next
  * @retval TRUE
  */
uint8_t dataRoutine(ActionPacket* p,  ActionPacket* next)
{
  if(next == NULL)
  {
    BSP_LED_Toggle(LD2);
    multi_state_scheduler(&multi_state);
  }
  return TRUE;
}


/**
  * @brief  Beep Initialize.
  * @param  State Machine No
  * @retval void
  */
void beep_init(uint8_t StateMachineNo)
{
  sendData[StateMachineNo][0] = 0x02; 
  sendData[StateMachineNo][1] = DATA_LEN;       /* Length position is fixed */
  sendData[StateMachineNo][2] = StateMachineNo;
  
  for(uint8_t i = 1; i < DATA_LEN; i++)
  {
    sendData[StateMachineNo][2+i] = i;
  }
  
  if(encryption[StateMachineNo])
  {
    sendData[StateMachineNo][1] += MIC_FIELD_LENGTH;
  }
  
  actPacket[StateMachineNo].StateMachineNo = StateMachineNo;
  actPacket[StateMachineNo].ActionTag = RELATIVE | TIMER_WAKEUP | TXRX | PLL_TRIG ;
  actPacket[StateMachineNo].WakeupTime = period_usec[StateMachineNo];
  actPacket[StateMachineNo].MaxReceiveLength = 0;/* not applied for Tx*/
  actPacket[StateMachineNo].data = (uint8_t *)&sendData[StateMachineNo][0]; 
  actPacket[StateMachineNo].next_true = NULL;   /* -> points to AP[0]  */
  actPacket[StateMachineNo].next_false = NULL;  /* -> points to AP[0]  */   
  actPacket[StateMachineNo].condRoutine = conditionRoutine;
  actPacket[StateMachineNo].dataRoutine = dataRoutine;  
  multi_state.activity_flag = multi_state.activity_flag | 1<<StateMachineNo;
  multi_state.wakeup_time[StateMachineNo] = actPacket[StateMachineNo].WakeupTime;
  HAL_RADIO_SetChannelMap(StateMachineNo, &channel_map[0]);
  HAL_RADIO_SetChannel(StateMachineNo, channel[StateMachineNo],0);
  HAL_RADIO_SetTxAttributes(StateMachineNo, network_id[StateMachineNo] , 0x555555);
  HAL_RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  HAL_RADIO_SetTxPower(0x10);
  
  if(encryption[StateMachineNo])
  {
    HAL_RADIO_SetEncryptionCount(StateMachineNo, &count_tx[0], &count_rcv[0]); 
    HAL_RADIO_SetEncryptionAttributes(StateMachineNo, enc_iv, enc_key);
    HAL_RADIO_SetEncryptFlags(StateMachineNo,(FunctionalState)encryption[StateMachineNo],(FunctionalState)encryption[StateMachineNo]);    
  }
  
  HAL_RADIO_SetReservedArea(&actPacket[StateMachineNo]); 
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
