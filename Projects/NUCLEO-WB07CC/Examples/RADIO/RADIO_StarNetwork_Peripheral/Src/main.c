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

#define PERIPHERAL_DEV1              (uint32_t)(0x8888A1A1)
#define PERIPHERAL_DEV2              (uint32_t)(0x8888D3D3)
#define PERIPHERAL_DEV3              (uint32_t)(0x8888C5C5)
#define PERIPHERAL_DEV4              (uint32_t)(0x8888B7B7)

#define PERIPHERAL_NETWORK_ID        PERIPHERAL_DEV1

#define APP_CHANNEL             (uint8_t)(24)
#define MAX_RETRY_TEST          (3)
#define WKP_OFFSET              (300)      /* 300 us */
#define RX_TIMEOUT_DATA         (100000)   /* 100 ms */
#define RX_TIMEOUT_ACK          (1000)     /* 1 ms */
#define DATA_LEN                (62)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t txDataBuffer[DATA_LEN];
uint8_t ackBuffer[2];
uint8_t receiveBuffer[MAX_PACKET_LENGTH];

ActionPacket waitRequestAction;
ActionPacket sendDataAction;
ActionPacket waitAckAction;

uint16_t pkt_counter_test = 0;
uint16_t retry_counter_test = 0;
uint8_t retry = 0;
uint16_t ack_counter_test = 0;

uint16_t print_ack;
uint16_t print_request;

uint32_t n_request = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RADIO_Init(void);
static void MX_RADIO_TIMER_Init(void);
/* USER CODE BEGIN PFP */

static void InitializationActionPackets(void);
uint8_t requestDataCB(ActionPacket* p);
uint8_t waitAckCB(ActionPacket* p);
uint8_t waitRequestDR(ActionPacket* p,  ActionPacket* next);
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
  BSP_LED_Init(LD3);
  
  COM_InitTypeDef COM_Init = {0};

  COM_Init.BaudRate= 115200;
  COM_Init.HwFlowCtl = COM_HWCONTROL_NONE;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.Parity = COM_PARITY_NONE;
  COM_Init.StopBits = COM_STOPBITS_1;
  BSP_COM_Init(COM1, &COM_Init);
  
  /* Build send me packet */
  txDataBuffer[0] = 0x00;  /* First byte */
  txDataBuffer[1] = DATA_LEN-2;   /* Length position is fixed */
  for(uint8_t i=0; i != DATA_LEN-2; i++)
  {
    txDataBuffer[i+2] = i;
  }
  
  /* Build ACK packet */
  ackBuffer[0] = 0x00;
  ackBuffer[1] = 0;
  
  print_ack = 0;
  print_request = 0;

  HAL_RADIO_SetGlobalReceiveTimeout(RX_TIMEOUT_DATA);
  
  /* Channel map configuration */
  uint8_t map[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};
  HAL_RADIO_SetChannelMap(0, &map[0]);
  
  /* Set the channel */
  HAL_RADIO_SetChannel(0, APP_CHANNEL, 0);

  /* Sets of the NetworkID and the CRC */
  HAL_RADIO_SetTxAttributes(0, PERIPHERAL_NETWORK_ID, 0x555555);
  
  HAL_RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  
  /* Initialize the routine for sending data and wait for the ACK */
  InitializationActionPackets();
  
  
  /* Call this function for the first action packet to be executed */
  if(HAL_RADIO_MakeActionPacketPending(&waitRequestAction) != SUCCESS_0)
  {
     Error_Handler();
  }
  
  printf("\n\nPeripheral started\n");
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_RADIO_TIMER_Tick();
    
    if(print_ack != 0)
    {
      printf("Ack Frame: ");
      for(uint16_t i= 0; i<2; i++)
      {
        if(i == 1)
        {
          printf("%d:", ackBuffer[i]);
        }
        else
        {
          printf("%02x:", ackBuffer[i]);
        }
      }
      printf("\n");
      ackBuffer[0] = 0x00;
      ackBuffer[1] = 0;
      print_ack = 0;
    }

    if(print_request != 0)
    {
      n_request++;
      printf("Request Frame [%d]: ", n_request);
      for(uint16_t i= 0; i<2; i++)
      {
        if(i == 1)
        {
          printf("%d:", receiveBuffer[i]);
        }
        else
        {
          printf("%02x:", receiveBuffer[i]);
        }
      }
      printf("\n");
      print_request = 0;
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

/**
  * @brief  Condition routine.
  * @param  ActionPacket
  * @retval TRUE
  */
uint8_t requestDataCB(ActionPacket* p)
{
  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
    {
      print_request = 1;
      return TRUE;
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0))
    {
      BSP_LED_Toggle(LD3);
      return FALSE;
    }
  }
  else if((p->status & BLUE_INTERRUPT1REG_DONE) != 0)
  {
     /* RADIO TX operation done */
     BSP_LED_Toggle(LD2);
  }
  return FALSE;
} 

/**
  * @brief  Condition routine.
  * @param  ActionPacket: current
  * @retval TRUE
  */
uint8_t waitAckCB(ActionPacket* p)
{
  /* received a packet */
  if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
  {
    print_ack = 1;
    return TRUE;
  }
  else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0))
  {
    if(retry < MAX_RETRY_TEST)
    {
      retry++;
      return FALSE;
    }
    else
    {
      retry = 0;
      return TRUE;
    }
  }
  return TRUE;
}

/**
  * @brief  Data routine.
  * @param  ActionPacket: current
  * @param  ActionPacket: next
  * @retval TRUE
  */
uint8_t waitRequestDR(ActionPacket* p,  ActionPacket* next)
{
  if( ((p->status & BLUE_INTERRUPT1REG_DONE) != 0) && ((p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)) 
  {
    /* received a packet */
      if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0) 
      {
         txDataBuffer[0] = 0x00;  /* First byte */
         txDataBuffer[1] = DATA_LEN-2;   /* Length position is fixed */
         for(uint8_t i=0; i != DATA_LEN-2; i++)
         {
            txDataBuffer[i+2] = txDataBuffer[i+2] + 1;
         }
      }
  }
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
  if( (p->status & BLUE_INTERRUPT1REG_DONE) != 0)
  {  
    /* received a packet */
    if((p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
    {
      if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
      {    
      }
      else if((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0)
      {
      }
      else if((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0) 
      {
      }
    }
  }
  return TRUE;  
}

void InitializationActionPackets(void)
{
  /* Build Action Packets */
  waitRequestAction.StateMachineNo = 0;
  waitRequestAction.ActionTag = RELATIVE | TIMER_WAKEUP | PLL_TRIG;
  waitRequestAction.WakeupTime = 30 + WKP_OFFSET;           /* 30 us minimum of internal delay before start action */
  waitRequestAction.MaxReceiveLength = MAX_LL_PACKET_LENGTH;
  waitRequestAction.data = receiveBuffer;                 /* Data to send */
  waitRequestAction.next_true = &sendDataAction;           /* If condRoutine returns TRUE => Go to next TX the 2nd part of the payload */
  waitRequestAction.next_false = &waitRequestAction;       /* Not Used */   
  waitRequestAction.condRoutine = requestDataCB;          /* Condition routine */
  waitRequestAction.dataRoutine = dataRoutine;            /* Data routine */

  sendDataAction.StateMachineNo = 0;
  sendDataAction.ActionTag = RELATIVE | TIMER_WAKEUP | TXRX;
  sendDataAction.WakeupTime = 40 + WKP_OFFSET;              /* 40 us minimum of internal delay before start action */
  sendDataAction.MaxReceiveLength = 0;                    
  sendDataAction.data = txDataBuffer;                     /* Buffer for ACK packet reception */
  sendDataAction.next_true = &waitAckAction;               /* Reschedule the RX */   
  sendDataAction.next_false = &waitAckAction;              /* Reschedule the RX */   
  sendDataAction.condRoutine = requestDataCB;                /* Condition routine */
  sendDataAction.dataRoutine = dataRoutine;               /* Data routine */
  
  waitAckAction.StateMachineNo = 0;
  waitAckAction.ActionTag = RELATIVE | TIMER_WAKEUP;
  waitAckAction.WakeupTime = 30 + WKP_OFFSET;              /* 30 us minimum of internal delay before start action */
  waitAckAction.MaxReceiveLength = MAX_LL_PACKET_LENGTH;  
  waitAckAction.data = ackBuffer;                    /* Buffer for ACK packet reception */
  waitAckAction.next_true = &waitRequestAction;           /* Reschedule the RX */   
  waitAckAction.next_false = &sendDataAction;             /* Reschedule the RX */   
  waitAckAction.condRoutine = waitAckCB;                 /* Condition routine */
  waitAckAction.dataRoutine = dataRoutine;               /* Data routine */

  /* Call these functions before execute the action packets */
  HAL_RADIO_SetReservedArea(&waitRequestAction);
  HAL_RADIO_SetReservedArea(&sendDataAction);
  HAL_RADIO_SetReservedArea(&waitAckAction);
  
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
