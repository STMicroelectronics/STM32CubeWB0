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
typedef struct circular_fifo_s
{
  uint16_t head_index;
  uint16_t tail_index;
  uint8_t curr_size;
  uint8_t max_size;
  uint16_t elem_size;
  uint8_t *buffer;
} circular_fifo_t;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_BUFFER_LEN            (50)
#define PRINT_BUFFER_LEN          (8)

#define N_STATE_MACHINES        CFG_NUM_RADIO_TASKS /* The number of state machines */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define PRINT_INT(x)    ((int)(x))
#define PRINT_FLOAT(x)  (x>0)? ((int) (((x) - PRINT_INT(x)) * 1000)) : (-1*(((int) (((x) - PRINT_INT(x)) * 1000))))

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
circular_fifo_t print_fifo;
uint8_t print_buffer[MAX_PACKET_LENGTH*PRINT_BUFFER_LEN+MAX_PACKET_LENGTH];
uint8_t frameToPrint[MAX_PACKET_LENGTH];

uint8_t receivedData[MAX_PACKET_LENGTH];

uint32_t delay = 10000;
uint32_t rx_timeout = 500000;
int32_t rssi_val = 0;
uint32_t timestamp = 0;
uint32_t actual_ch = 0;
uint8_t channel_map[5] = {0xFF,0xFF,0xFF,0xFF,0xFF};
uint16_t rcv_count;

uint8_t new_packet = TRUE;

uint32_t network_id_values[CFG_NUM_RADIO_TASKS] = {0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6};
uint8_t channel_values[CFG_NUM_RADIO_TASKS] = {21,22,23,25,26,27,28,29};
uint8_t encryption_values[CFG_NUM_RADIO_TASKS] = {0,0,0,1,1,1,1,0};

uint32_t network_id[N_STATE_MACHINES];
uint8_t channel[N_STATE_MACHINES];
uint8_t encryption[N_STATE_MACHINES];
uint8_t No_Packet_Per_StateMachineNo = 5;

static ActionPacket aPacket[N_STATE_MACHINES]; 

uint8_t count_tx[5]     = {0x00,0x00,0x00,0x00,0x00};
uint8_t count_rcv[5]    = {0x00,0x00,0x00,0x00,0x00};
uint8_t enc_key[16]     = {0xBF,0x01,0xFB,0x9D,0x4E,0xF3,0xBC,0x36,0xD8,0x74,0xF5,0x39,0x41,0x38,0x68,0x4C};
uint8_t enc_iv[8]       = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//static uint8_t StateMachine_index = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RADIO_Init(void);
static void MX_RADIO_TIMER_Init(void);
/* USER CODE BEGIN PFP */

void fifo_init(circular_fifo_t *fifo, uint8_t *buffer, uint8_t size, uint16_t elem_size);
uint8_t fifo_size(circular_fifo_t *fifo);
uint8_t fifo_put(circular_fifo_t *fifo, uint8_t elem_size, uint8_t elem_data_offset, uint8_t *elem);
uint8_t fifo_get(circular_fifo_t *fifo, uint8_t *elem);
uint8_t fifo_get_elem_size(circular_fifo_t *fifo, uint8_t size, uint8_t *elem);

uint8_t nullfunction(ActionPacket* p);
uint8_t RxCallback(ActionPacket* p, ActionPacket* next);
int sniffer_init(uint8_t StateMachineNo);
uint8_t condRoutineTrue(ActionPacket* p);
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
  
  fifo_init(&print_fifo, print_buffer, PRINT_BUFFER_LEN, MAX_PACKET_LENGTH);
  
  /* Set the parameters */
  for(uint8_t i = 0; i < N_STATE_MACHINES; i++)
  {
    network_id[i] = network_id_values[i];
    channel[i] = channel_values[i];
    encryption[i] = encryption_values[i];
  }
  
  /* Initialize the sniffer State Machines */
  for(uint8_t i = 0; i<N_STATE_MACHINES; i++) {
    sniffer_init(i);
  }
  HAL_RADIO_SetGlobalReceiveTimeout(rx_timeout);
  /* Call this function for the first action packet to be executed */
  
  printf("\r\n >> Sniffing channel..%d\r\n", (uint8_t)channel[0]);
  HAL_RADIO_MakeActionPacketPending(&aPacket[0]);
  BSP_LED_On(LD1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_RADIO_TIMER_Tick();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    while(fifo_size(&print_fifo) > 0)
    {
      rcv_count = rcv_count + 1;
      fifo_get(&print_fifo, frameToPrint);
      fifo_get_elem_size(&print_fifo, 4, (uint8_t*)&rssi_val);
      fifo_get_elem_size(&print_fifo, 4, (uint8_t*)&timestamp);
      fifo_get_elem_size(&print_fifo, 1, (uint8_t*)&actual_ch);

      printf("\n\r[%d] Timestamp: %d.%03d ms - ", rcv_count, PRINT_INT((timestamp*625/256000.0)), PRINT_FLOAT((timestamp*625/256000.0)));
      printf("RSSI: %d dBm, channel: %d\n\r", rssi_val, actual_ch);
      printf("Frame[%d]: ", frameToPrint[1]);
      for(uint8_t i= 0; i<frameToPrint[1]+1; i++)
      {
        if((i > 0) && (i < 3))
        {
          printf("%d:", frameToPrint[i]);
        }
        else
        {
          printf("%02x:", frameToPrint[i]);
        }
      }
      printf("\n\r");
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

void fifo_init(circular_fifo_t *fifo, uint8_t *buffer, uint8_t size, uint16_t elem_size)
{
  fifo->tail_index = fifo->head_index = 0;
  fifo->curr_size = 0;
  fifo->max_size = size;
  fifo->elem_size = elem_size;
  fifo->buffer = buffer;
  //printf("fifo_init: curr_size[%d],max_size[%d],elem_size[%d] \n", fifo->curr_size, fifo->max_size, fifo->elem_size);
}

uint8_t fifo_size(circular_fifo_t *fifo)
{
  return fifo->curr_size;
}

uint8_t fifo_put(circular_fifo_t *fifo, uint8_t elem_size, uint8_t elem_data_offset, uint8_t *elem)
{
  //printf("fifo_put: curr_size[%d],max_size[%d],elem_size[%d] \n", fifo->curr_size, fifo->max_size, elem_size);
  //printf("PUT: Curr Size [%d], Head Index [%d], Tail Index [%d] \n", fifo->curr_size, fifo->head_index, fifo->tail_index);
  if (fifo->curr_size < fifo->max_size)
  {
    //printf("PUT: Curr Size [%d], Head Index [%d], Tail Index [%d], Elem size [%d] , offset [%d]\n", fifo->curr_size, fifo->head_index, fifo->tail_index, elem_size, elem_data_offset);
    /*copy element*/
    uint16_t fifo_ind = fifo->tail_index * fifo->elem_size;
    for (uint8_t i = 0; i < elem_size; i++)
    {
      fifo->buffer[fifo_ind] = elem[i + elem_data_offset];
      fifo_ind++;
    }

    if (fifo->tail_index < (fifo->max_size - 1))
    {
      fifo->tail_index++;
    }
    else
    {
      fifo->tail_index = 0;
    }
    fifo->curr_size++;
    return 0;
  }
  return 1;
}

uint8_t fifo_get(circular_fifo_t *fifo, uint8_t *elem)
{
  //printf("GET: Curr Size [%d], Head Index [%d], Tail Index [%d] \n", fifo->curr_size, fifo->head_index, fifo->tail_index);
  if (fifo->curr_size > 0)
  {
    uint16_t fifo_ind = fifo->head_index * fifo->elem_size;
    uint8_t c_size = fifo->buffer[fifo_ind+1];
    //printf("fifo_get: c_size [%d]\n", c_size);
    //for (uint8_t i = 0; i < fifo->elem_size; i++)
    for (uint8_t i = 0; i < c_size+1; i++)
    {
      elem[i] = fifo->buffer[fifo_ind];
      fifo_ind++;
    }

    fifo->curr_size--;

    if (fifo->head_index < fifo->max_size - 1)
    {
      fifo->head_index++;
    }
    else
    {
      fifo->head_index = 0;
    }
    return 0;
  }
  return 1;
}

uint8_t fifo_get_elem_size(circular_fifo_t *fifo, uint8_t size, uint8_t *elem)
{
  //printf("GET: Curr Size [%d], Head Index [%d], Tail Index [%d] \n", fifo->curr_size, fifo->head_index, fifo->tail_index);
  if (fifo->curr_size > 0)
  {
    uint16_t fifo_ind = fifo->head_index * fifo->elem_size;
    for (uint8_t i = 0; i < size; i++)
    {
      elem[i] = fifo->buffer[fifo_ind];
      fifo_ind++;
    }

    fifo->curr_size--;

    if (fifo->head_index < fifo->max_size - 1)
    {
      fifo->head_index++;
    }
    else
    {
      fifo->head_index = 0;
    }
    return 0;
  }
  return 1;
}

uint8_t condRoutineTrue(ActionPacket* p)
{
  return TRUE;
}

uint8_t RxCallback(ActionPacket* p, ActionPacket* next)
{
  static uint8_t count_t = 0;
  static uint8_t StateMachine_index = 0;

  /* received a packet */
  if((p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
  {
    /* Number of packet capture per State Machine */
    if(count_t < No_Packet_Per_StateMachineNo)
    {
      count_t++;
    
      if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
      {
        BSP_LED_Toggle(LD2);
        if((PRINT_BUFFER_LEN - fifo_size(&print_fifo)) > 3)
        {
          fifo_put(&print_fifo, p->data[1]+1, 0, (uint8_t*)(&p->data[0]));
          fifo_put(&print_fifo, 4, 0, (uint8_t*)&p->rssi);
          fifo_put(&print_fifo, 4, 0, (uint8_t*)&p->timestamp_receive);
          fifo_put(&print_fifo, 1, 0, (uint8_t*)(&channel[p->StateMachineNo]));
        }
        HAL_RADIO_SetChannel(StateMachine_index,(uint8_t)channel[StateMachine_index], 0);
        HAL_RADIO_MakeActionPacketPending(&aPacket[StateMachine_index]);
        new_packet = TRUE;
      }
      else if((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0)
      {
        //printf("Timeout:%dus\r\n",rx_timeout);
        //printf("%d",count_t);
        HAL_RADIO_SetChannel(StateMachine_index,(uint8_t)channel[StateMachine_index], 0);
        HAL_RADIO_MakeActionPacketPending(&aPacket[StateMachine_index]);
        new_packet = TRUE;
      }
      else if((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)
      {
        //printf("CRC error\r\n");
        //printf("%d",count_t);
        HAL_RADIO_SetChannel(StateMachine_index,(uint8_t)channel[StateMachine_index], 0);
        HAL_RADIO_MakeActionPacketPending(&aPacket[StateMachine_index]);
        new_packet = TRUE;
      }              
    }
    else
    {
      if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
      {
        BSP_LED_Toggle(LD2);
        if((PRINT_BUFFER_LEN - fifo_size(&print_fifo)) > 3)
        {
          fifo_put(&print_fifo, p->data[1]+1, 0, (uint8_t*)(&p->data[0]));
          fifo_put(&print_fifo, 4, 0, (uint8_t*)&p->rssi);
          fifo_put(&print_fifo, 4, 0, (uint8_t*)&p->timestamp_receive);
          fifo_put(&print_fifo, 1, 0, (uint8_t*)(&channel[p->StateMachineNo]));
        }
    
      }
      count_t = 0;
      StateMachine_index++;
      StateMachine_index = StateMachine_index%N_STATE_MACHINES; /* Update the State Machine */
      new_packet = TRUE;
      HAL_RADIO_SetChannel(StateMachine_index,(uint8_t)channel[StateMachine_index], 0);
      HAL_RADIO_MakeActionPacketPending(&aPacket[StateMachine_index]);
      //printf("\r\n >> Sniffing channel..%d StateMachineNo:%d Encryption:%d Network_ID:0x%X\r\n",(uint8_t)channel[StateMachine_index],StateMachine_index,encryption[StateMachine_index],network_id[StateMachine_index]);
      printf("\r\n >> Sniffing channel..%d\r\n", (uint8_t)channel[StateMachine_index]);
    }
  }
  return TRUE;
}


int sniffer_init(uint8_t StateMachineNo)
{
  HAL_RADIO_SetChannelMap(StateMachineNo, channel_map);
  HAL_RADIO_SetChannel(StateMachineNo, (uint8_t)channel[StateMachineNo], 0);
  HAL_RADIO_SetTxAttributes(StateMachineNo, network_id[StateMachineNo], 0x555555);
  
  aPacket[StateMachineNo].StateMachineNo = StateMachineNo;   
  aPacket[StateMachineNo].ActionTag = PLL_TRIG | RELATIVE | TIMER_WAKEUP;
  aPacket[StateMachineNo].WakeupTime = delay;
  aPacket[StateMachineNo].MaxReceiveLength = MAX_LL_PACKET_LENGTH; 
  aPacket[StateMachineNo].data = receivedData; 
  aPacket[StateMachineNo].next_true = NULL_0;
  aPacket[StateMachineNo].next_false = NULL_0;    
  aPacket[StateMachineNo].condRoutine = condRoutineTrue;
  aPacket[StateMachineNo].dataRoutine = RxCallback;
  if(encryption[StateMachineNo])
  {
    HAL_RADIO_SetEncryptionAttributes(StateMachineNo, enc_iv, enc_key);
    HAL_RADIO_SetEncryptionCount(StateMachineNo, &count_tx[0], &count_rcv[0]); 
    HAL_RADIO_SetEncryptFlags(StateMachineNo,(FunctionalState)encryption[StateMachineNo],(FunctionalState)encryption[StateMachineNo]);    
  }
  HAL_RADIO_SetReservedArea(&aPacket[StateMachineNo]);
  
  printf(" >> Sniffing channel..%d StateMachineNo:%d Encryption:%d Network_ID:0x%X\r\n", (uint8_t)channel[StateMachineNo], StateMachineNo, encryption[StateMachineNo], network_id[StateMachineNo]);
  return 0;
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
  BSP_LED_On(LD3);
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
