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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STATE_MACHINE_0         (0)

#define TP_NETWORK_ADDRESS      (uint32_t)(0x888888DF)
#define FREQUENCY_CHANNEL       (uint8_t)(21)             /* RF channel 21 */

#define TX_WAKEUP_TIME          (360)         /* 360 us */
#define RX_WAKEUP_TIME          (360)         /* 360 us */
#define RX_TIMEOUT_ACK          (150)         /* 150 us */
#define RX_TIMEOUT              (100000)      /* 100 ms */

#define MAX_NUM_PACKET          (1000)    /* Number of packets used for the test */
#define DATA_PACKET_LEN         (20) //20 //255 //31    /* PDU length in bytes  */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define PRINT_INT(x)    ((int)(x))
#define PRINT_FLOAT(x)  (x>0)? ((int) (((x) - PRINT_INT(x)) * 1000)) : (-1*(((int) (((x) - PRINT_INT(x)) * 1000))))
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
uint8_t channel = FREQUENCY_CHANNEL;
uint8_t sendData[MAX_PACKET_LENGTH];
uint16_t packet_counter = 0;
uint16_t timeout_error_counter = 0;

static uint32_t time_cumulate = 0;
static uint32_t timer_reload;
static ActionPacket aPacket[3];

uint8_t sendData[MAX_PACKET_LENGTH];
uint8_t receivedData[MAX_PACKET_LENGTH];
uint8_t sendNewPacket = FALSE;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RADIO_Init(void);
static void MX_RADIO_TIMER_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
static uint8_t CondRoutineTrue(ActionPacket* p);
static uint8_t CondRoutineAck(ActionPacket* p);
static uint8_t dataRoutineNull(ActionPacket* current_action_packet, ActionPacket* next);

uint8_t TxCallback(ActionPacket* p, ActionPacket* next);
uint8_t BIDIRECTIONAL_TX_Sequence(uint8_t channel, 
                                   uint8_t* txBuffer, 
                                   uint8_t* rxBuffer,
                                   uint8_t receive_length,
                                   uint8_t (*Callback)(ActionPacket*, ActionPacket*));

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
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
  uint8_t ret;
  sendNewPacket = TRUE;
  packet_counter = 0;
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
  MX_TIM2_Init();
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
  
  if(DATA_PACKET_LEN > (MAX_PACKET_LENGTH-HEADER_LENGTH)) {
    printf("DATA_PACKET_LEN too big %d\r\n", DATA_PACKET_LEN);
    while(1);
  }
     
  /* Build packet */
  sendData[0] = 0x02;
  sendData[1] = DATA_PACKET_LEN;   /* Length position is fixed */
  for(volatile uint16_t j=0; j<DATA_PACKET_LEN; j++) {
    sendData[2+j] = 0xAE;
  }

  /* Channel map configuration */
  uint8_t map[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};
  HAL_RADIO_SetChannelMap(STATE_MACHINE_0, &map[0]);
  
  /* Set the channel */
  HAL_RADIO_SetChannel(STATE_MACHINE_0, channel, 0);

  /* Sets of the NetworkID and the CRC */
  HAL_RADIO_SetTxAttributes(STATE_MACHINE_0, TP_NETWORK_ADDRESS, 0x555555);
  
  HAL_RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  HAL_RADIO_SetBackToBackTime(100);  
  
  HAL_RADIO_SetGlobalReceiveTimeout(RX_TIMEOUT);
  
  /* Start Timer */
  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }
  
  printf("STM32WB09 Radio Driver Throughput Bidirectional Application\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_RADIO_TIMER_Tick();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(packet_counter == MAX_NUM_PACKET) {
      BSP_LED_Off(LD1);
      if(time_cumulate!=0) {
        printf("%d TX packets. %d PCKT LEN. Average time: %d.%02d ms. Data throughput: %d.%01d kbps.\r\n", packet_counter, DATA_PACKET_LEN,PRINT_INT(time_cumulate/1000.0/packet_counter),PRINT_FLOAT(time_cumulate/1000.0/packet_counter),PRINT_INT((packet_counter*DATA_PACKET_LEN*8)*1000.0/time_cumulate),PRINT_FLOAT((packet_counter*DATA_PACKET_LEN*8)*1000.0/time_cumulate));
      }
      packet_counter = 0;
      timeout_error_counter = 0;
      //crc_error_counter = 0;
      sendData[6] = 0;
      sendNewPacket = TRUE;
      time_cumulate = 0;
      for(volatile uint32_t i = 0; i<0xFFFFF; i++);
    }
    
    
    if(sendNewPacket == TRUE) {
      sendNewPacket = FALSE;    
      ret = BIDIRECTIONAL_TX_Sequence(channel, sendData, receivedData, MAX_LL_PACKET_LENGTH, TxCallback);
      timer_reload = 0;
      htim2.Instance->CNT = PERIOD_VALUE;
      if(ret != SUCCESS_0) {
        printf("2) ERROR %02X (%d)\r\n",ret, packet_counter);
      }
      BSP_LED_On(LD1);
    }
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = PRESCALER_VALUE;
  htim2.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim2.Init.Period = 0xffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  timer_reload++;
  BSP_LED_Toggle(LD2);
}

static uint8_t CondRoutineTrue(ActionPacket* p)
{
  return TRUE;
}

static uint8_t dataRoutineNull(ActionPacket* current_action_packet, ActionPacket* next)
{
  return TRUE;
}

/**
* @brief  This routine is called when a transmit event is complete. 
* @param  p: Current action packet which its transaction has been completed.
* @param  next: Next action packet which is going to be scheduled.
* @retval return value: TRUE
*/
uint8_t TxCallback(ActionPacket* p, ActionPacket* next)
{ 
  /* received a packet */
  if((p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0){

    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0) {
      
    }
    else if((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) {
      timeout_error_counter = timeout_error_counter + 1;
      BSP_LED_Toggle(LD3);
    }
    else if((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0) {
    }
    
    uint32_t time2 = htim2.Instance->CNT;
    uint32_t diff = (PERIOD_VALUE - time2) + timer_reload*PERIOD_VALUE;
    time_cumulate += diff;
    
    if(packet_counter != MAX_NUM_PACKET)
    {
      htim2.Instance->CNT = PERIOD_VALUE;
      timer_reload = 0;
    }
    
  }
  /* Transmit complete */
  else if((p->status & BLUE_INTERRUPT1REG_DONE) != 0)
  {

  }
  return TRUE;
}


static uint8_t CondRoutineAck(ActionPacket* p)
{
  packet_counter++;  
  if(packet_counter == MAX_NUM_PACKET)
  {
    return FALSE;
  }
  
  return TRUE;
}

uint8_t BIDIRECTIONAL_TX_Sequence(uint8_t channel, 
                                    uint8_t* txBuffer, 
                                    uint8_t* rxBuffer,
                                    uint8_t receive_length,
                                    uint8_t (*Callback)(ActionPacket*, ActionPacket*))
{
  uint8_t returnValue = SUCCESS_0;
  uint32_t dummy;
  
  if(channel > 39) {
    returnValue = INVALID_PARAMETER_C0;      
  }

  if(HAL_RADIO_GetStatus(&dummy) != BLUE_IDLE_0) {
    returnValue = RADIO_BUSY_C4;
  }
  
  HAL_RADIO_SetGlobalReceiveTimeout(RX_TIMEOUT);
  
  if(returnValue == SUCCESS_0) {
    aPacket[0].StateMachineNo = STATE_MACHINE_0;
    aPacket[0].ActionTag = RELATIVE | TIMER_WAKEUP | TXRX | PLL_TRIG;
    aPacket[0].WakeupTime = TX_WAKEUP_TIME;
    aPacket[0].MaxReceiveLength = 0; /* does not affect for Tx */
    aPacket[0].data = txBuffer;
    aPacket[0].next_true = &aPacket[1];
    aPacket[0].next_false = &aPacket[1];
    aPacket[0].condRoutine = CondRoutineTrue;
    aPacket[0].dataRoutine = dataRoutineNull;
    
    aPacket[1].StateMachineNo = STATE_MACHINE_0;   
    aPacket[1].ActionTag = 0;   
    aPacket[1].WakeupTime = RX_TIMEOUT_ACK;
    aPacket[1].MaxReceiveLength = receive_length; 
    aPacket[1].data = rxBuffer; 
    aPacket[1].next_true = &aPacket[2];
    aPacket[1].next_false = NULL_0;
    aPacket[1].condRoutine = CondRoutineAck;
    aPacket[1].dataRoutine = Callback;
    
    aPacket[2].StateMachineNo = STATE_MACHINE_0;
    aPacket[2].ActionTag = RELATIVE | TIMER_WAKEUP | TXRX;
    aPacket[2].WakeupTime = TX_WAKEUP_TIME;
    aPacket[2].MaxReceiveLength = 0; /* does not affect for Tx */
    aPacket[2].data = txBuffer;
    aPacket[2].next_true = &aPacket[1];
    aPacket[2].next_false = &aPacket[1];
    aPacket[2].condRoutine = CondRoutineTrue;
    aPacket[2].dataRoutine = dataRoutineNull;
    
    BLUEGLOB->TIMER12INITDELAYCAL = 46;
    BLUEGLOB->TIMER2INITDELAYNOCAL = 4;
    
    HAL_RADIO_SetReservedArea(&aPacket[0]);
    HAL_RADIO_SetReservedArea(&aPacket[1]);
    HAL_RADIO_SetReservedArea(&aPacket[2]);
    returnValue = HAL_RADIO_MakeActionPacketPending(&aPacket[0]); 

    HAL_RADIO_TIMER_SetRadioCloseTimeout();
  }
    
  return returnValue; 
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
     ex: printf("Wrong parameters value: file %s on line %d\n", file, line) */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
