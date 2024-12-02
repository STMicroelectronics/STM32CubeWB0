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
#define TX_WAKEUP_TIME          (100000)  /* 100 ms */

#define NETWORK_ACCESS_ADDRESS  (uint32_t)(0x88DF88DF)

#define STARTING_CHANNEL        (uint8_t)(24)   
#define END_CHANNEL             (uint8_t)(26)

#define MIC_FIELD_LEN   MIC_FIELD_LENGTH

#define PRINT_FRAME             0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t channel = STARTING_CHANNEL;
uint8_t sendData[MAX_PACKET_LENGTH];
uint8_t data_val = 0;
uint8_t DataLen;

ActionPacket actPacket;

uint8_t count_tx[5]     = {0x00,0x00,0x00,0x00,0x00};
uint8_t count_rcv[5]    = {0x00,0x00,0x00,0x00,0x00};
uint8_t enc_key[16]     = {0xBF,0x01,0xFB,0x9D,0x4E,0xF3,0xBC,0x36,0xD8,0x74,0xF5,0x39,0x41,0x38,0x68,0x4C};
uint8_t enc_iv[8]       = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RADIO_Init(void);
static void MX_RADIO_TIMER_Init(void);
/* USER CODE BEGIN PFP */

uint8_t conditionRoutine(ActionPacket* p);
uint8_t dataRoutine(ActionPacket* p,  ActionPacket* next);
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

  COM_InitTypeDef COM_Init = {0};

  COM_Init.BaudRate= 115200;
  COM_Init.HwFlowCtl = COM_HWCONTROL_NONE;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.Parity = COM_PARITY_NONE;
  COM_Init.StopBits = COM_STOPBITS_1;
  BSP_COM_Init(COM1, &COM_Init);
  
  DataLen = 20;
  
  /* Build packet */
  sendData[0] = 0x02;                        /* Header */
  sendData[1] = DataLen  + MIC_FIELD_LEN;;   /* Length */
  
  sendData[2] = 0x01; /* Advertising address */
  sendData[3] = 0x02;
  sendData[4] = 0x03;
  sendData[5] = 0x04;
  sendData[6] = 0x05;
  sendData[7] = 0x06;
  
  sendData[8] = 0x08;    /* Length name */
  sendData[9] = channel; /* Shortened local name */
  
  sendData[10] = 'S'; /* Local name */
  sendData[11] = 'T';
  sendData[12] = 'M';
  sendData[13] = '3';
  sendData[14] = '2';
  sendData[15] = 'W';
  sendData[16] = 'B';
  
  for(uint8_t i = 0; i < (DataLen-15); i++) {
    sendData[i+17] = i + data_val;
  }
  data_val++;

  /* Channel map configuration */
  uint8_t map[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};
  HAL_RADIO_SetChannelMap(0, &map[0]);
  
  /* Setting of channel and the channel increment*/
  HAL_RADIO_SetChannel(0, STARTING_CHANNEL, 0);

  /* Sets of the NetworkID and the CRC.*/
  HAL_RADIO_SetTxAttributes(0, NETWORK_ACCESS_ADDRESS, 0x555555);
  
  /* Configures the transmit power level */
  HAL_RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);

  /* Encryption Key and Initial Vector */
  HAL_RADIO_SetEncryptionAttributes( 0, enc_iv, enc_key);
  
  /* Configures the packets encryption. Not used in this application */
  HAL_RADIO_SetEncryptFlags(0, ENABLE, ENABLE);  
  
  /* Build Action Packet */
  actPacket.StateMachineNo = 0;
  actPacket.ActionTag = RELATIVE | TIMER_WAKEUP | TXRX | PLL_TRIG;
  actPacket.WakeupTime = TX_WAKEUP_TIME;                
  actPacket.MaxReceiveLength = 0;                    /* Not applied for TX */
  actPacket.data = sendData;                         /* Data to send */
  actPacket.next_true = &actPacket;                  /* Pointer to the next Action Packet*/
  actPacket.next_false = NULL_0;                     /* Null */   
  actPacket.condRoutine = conditionRoutine;          /* Condition routine */
  actPacket.dataRoutine = dataRoutine;               /* Data routine */
  
  /* Call this function before execute the action packet */
  HAL_RADIO_SetReservedArea(&actPacket);
  
  /* Call this function for the first action packet to be executed */
  if(HAL_RADIO_MakeActionPacketPending(&actPacket) != SUCCESS_0)
  {
      Error_Handler();
  }
  
  printf("Application Beep Start \n");
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
uint8_t conditionRoutine(ActionPacket* p)
{
  
  if( (p->status & BLUE_INTERRUPT1REG_DONE) != 0)
  {
    if(p->status & BLUE_STATUSREG_PREVTRANSMIT)
    {
      BSP_LED_Toggle(LD2);
      channel ++;
      if(channel == (END_CHANNEL+1)) 
      {
        channel = STARTING_CHANNEL;
      }
      HAL_RADIO_SetChannel(0, channel, 0);
    }
  }
  return TRUE; 
}

uint8_t dataRoutine(ActionPacket* p,  ActionPacket* next)
{
  if(PRINT_FRAME == 0)
  {
    printf("Frame: ");
    for(uint16_t i= 0; i<DataLen+2; i++)
    {
      if(i == 9)
      {
        printf("%d:", sendData[i]);
      }
      else if(i>9 && i<17)
      {
        printf("%c:", sendData[i]);
      }
      else
      {
        printf("%02x:", sendData[i]);
      }
    }
    printf("\n");
  }
  sendData[9] = channel;
  for(uint8_t i = 0; i < (DataLen-15); i++)
  {
    sendData[i+17] = i + data_val;
  }
  data_val++;
  

  return TRUE;
  
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
