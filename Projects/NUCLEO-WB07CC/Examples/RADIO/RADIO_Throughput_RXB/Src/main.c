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
#define FREQUENCY_CHANNEL       (uint8_t)(21)            /* RF channel 21 */

#define TX_WAKEUP_TIME          (300)          /* 320 us */
#define RX_WAKEUP_TIME          (280)          /* 320 us */
#define RX_TIMEOUT              (100000)       /* 100 ms */
#define RX_TIMEOUT_ACK          (150)          /* 150 us */

#define PRINT_FRAME             (1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static ActionPacket aPacket[3];
uint8_t channel = FREQUENCY_CHANNEL;
uint8_t sendAckData[2];
uint8_t receivedData[MAX_PACKET_LENGTH];

uint8_t rx_done = FALSE, button_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RADIO_Init(void);
static void MX_RADIO_TIMER_Init(void);
/* USER CODE BEGIN PFP */
static uint8_t CondRoutineTrue(ActionPacket* p);
static uint8_t CondRoutineRx(ActionPacket* p);

uint8_t RxCallback(ActionPacket* p, ActionPacket* next);
uint8_t BIDIRECTIONAL_RX_Sequence(uint8_t channel, 
                                  uint8_t* rxBuffer, 
                                  uint8_t* txBuffer,
                                  uint8_t receive_length, 
                                  uint8_t (*Callback)(ActionPacket*, ActionPacket*));
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

  HAL_RADIO_SetNetworkID(TP_NETWORK_ADDRESS);
  
  /* Channel map configuration */
  uint8_t map[5]= {0xFFU,0xFFU,0xFFU,0xFFU,0xFFU};
  HAL_RADIO_SetChannelMap(STATE_MACHINE_0, &map[0]);
  
  /* Set the channel */
  HAL_RADIO_SetChannel(STATE_MACHINE_0, channel, 0);

  /* Sets of the NetworkID and the CRC */
  HAL_RADIO_SetTxAttributes(STATE_MACHINE_0, TP_NETWORK_ADDRESS, 0x555555);
  
  HAL_RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  HAL_RADIO_SetBackToBackTime(100);  
  
  HAL_RADIO_SetGlobalReceiveTimeout(RX_TIMEOUT);
  
  /* Build packet */
  sendAckData[0] = 0xAE;
  sendAckData[1] = 0;
    
  printf("STM32WB07 Radio Driver Throughput Application\r\n");
  ret = BIDIRECTIONAL_RX_Sequence(channel, receivedData, sendAckData, MAX_LL_PACKET_LENGTH, RxCallback);
  
  if(ret != SUCCESS_0) {
    printf("Sequence start error [%d]\r\n", ret);
  }
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_RADIO_TIMER_Tick();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(rx_done == TRUE && (PRINT_FRAME == 1)) 
    {
      printf("Packet Received: ");
      for(volatile uint16_t i = 0; i < (receivedData[1] + 2); i++) 
      {
        printf("%02X ", receivedData[i]);
      }
      printf("\r\n");
      rx_done = FALSE;
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
static uint8_t CondRoutineTrue(ActionPacket* p)
{
  return TRUE;
}

static uint8_t CondRoutineRx(ActionPacket* p)
{
  /* received a packet */
  if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0){
    /* packet received without CRC error */ 
    return TRUE;
  }
  return FALSE; 
}

/**
* @brief  This routine is called when a receive event is complete. 
* @param  p: Current action packet which its transaction has been completed.
* @param  next: Next action packet which is going to be scheduled.
* @retval return value: TRUE
*/
uint8_t RxCallback(ActionPacket* p, ActionPacket* next)
{ 
 /* received a packet */
 if((p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
 {

   if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
   {
     BSP_LED_Toggle(LD1);
     rx_done = TRUE;
   }
   else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0))
   {
     BSP_LED_Toggle(LD3);
   }
  } 
  else if((p->status & BLUE_INTERRUPT1REG_DONE) != 0)    /* Transmit complete */
  {
    BSP_LED_Toggle(LD2);
  }
  return TRUE;   
}

uint8_t BIDIRECTIONAL_RX_Sequence(uint8_t channel, 
                                  uint8_t* rxBuffer, 
                                  uint8_t* txBuffer,
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
    
  if(returnValue == SUCCESS_0) {
        
    aPacket[0].StateMachineNo = STATE_MACHINE_0;
    aPacket[0].ActionTag =  RELATIVE | TIMER_WAKEUP | PLL_TRIG;
    aPacket[0].WakeupTime = RX_WAKEUP_TIME;
    aPacket[0].MaxReceiveLength = receive_length;
    aPacket[0].data = rxBuffer;
    aPacket[0].next_true = &aPacket[1];
    aPacket[0].next_false = &aPacket[2];;
    aPacket[0].condRoutine = CondRoutineRx;
    aPacket[0].dataRoutine = Callback;
        
    aPacket[1].StateMachineNo = STATE_MACHINE_0;
    aPacket[1].ActionTag = TXRX;
    aPacket[1].WakeupTime = TX_WAKEUP_TIME;
    aPacket[1].MaxReceiveLength = 0; /* does not affect for Tx */
    aPacket[1].data = txBuffer;
    aPacket[1].next_true = &aPacket[2];
    aPacket[1].next_false = &aPacket[2];
    aPacket[1].condRoutine = CondRoutineTrue;
    aPacket[1].dataRoutine = Callback;
    
    aPacket[2].StateMachineNo = STATE_MACHINE_0;
    aPacket[2].ActionTag =  RELATIVE | TIMER_WAKEUP;
    aPacket[2].WakeupTime = RX_WAKEUP_TIME;
    aPacket[2].MaxReceiveLength = receive_length;
    aPacket[2].data = rxBuffer;
    aPacket[2].next_true = &aPacket[1];
    aPacket[2].next_false = &aPacket[2];
    aPacket[2].condRoutine = CondRoutineRx;
    aPacket[2].dataRoutine = Callback;
    
    HAL_RADIO_SetReservedArea(&aPacket[0]);
    HAL_RADIO_SetReservedArea(&aPacket[1]);
    HAL_RADIO_SetReservedArea(&aPacket[2]);
    HAL_RADIO_MakeActionPacketPending(&aPacket[0]);
    HAL_RADIO_TIMER_SetRadioCloseTimeout();
    BLUEGLOB->TIMER12INITDELAYCAL = 66;
    BLUEGLOB->TIMER2INITDELAYNOCAL = 4;
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
