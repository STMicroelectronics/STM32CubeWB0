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
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CONSUMER_VALUE     10
#define GEN_VALUE          20
#define PRODUCER_MAX_VALUE 25
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart1;

/* Definitions for ProducerThread */
osThreadId_t ProducerThreadHandle;
const osThreadAttr_t ProducerThread_attributes = {
  .name = "ProducerThread",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ConsumerThread */
osThreadId_t ConsumerThreadHandle;
const osThreadAttr_t ConsumerThread_attributes = {
  .name = "ConsumerThread",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GenThread */
osThreadId_t GenThreadHandle;
const osThreadAttr_t GenThread_attributes = {
  .name = "GenThread",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for ReceiveThread */
osThreadId_t ReceiveThreadHandle;
const osThreadAttr_t ReceiveThread_attributes = {
  .name = "ReceiveThread",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for osQueue */
osMessageQueueId_t osQueueHandle;
const osMessageQueueAttr_t osQueue_attributes = {
  .name = "osQueue"
};
/* USER CODE BEGIN PV */
uint32_t ProducerValue = 10;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void Producer_Thread(void *argument);
void Consumer_Thread(void *argument);
void Gen_Thread(void *argument);
void Receive_Thread(void *argument);

/* USER CODE BEGIN PFP */
#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of osQueue */
  osQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &osQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ProducerThread */
  ProducerThreadHandle = osThreadNew(Producer_Thread, NULL, &ProducerThread_attributes);

  /* creation of ConsumerThread */
  ConsumerThreadHandle = osThreadNew(Consumer_Thread, NULL, &ConsumerThread_attributes);

  /* creation of GenThread */
  GenThreadHandle = osThreadNew(Gen_Thread, NULL, &GenThread_attributes);

  /* creation of ReceiveThread */
  ReceiveThreadHandle = osThreadNew(Receive_Thread, NULL, &ReceiveThread_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource and SYSCLKDivider
  */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_RC64MPLL;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_RC64MPLL_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_WAIT_STATES_1) != HAL_OK)
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_RED_Pin|LED_BLUE_Pin|LED_GREEN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LED_RED_Pin LED_BLUE_Pin LED_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin|LED_BLUE_Pin|LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  LL_PWR_EnableGPIOPullUp(LL_PWR_GPIO_B, LL_PWR_GPIO_BIT_2|LL_PWR_GPIO_BIT_1|LL_PWR_GPIO_BIT_4);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_Producer_Thread */
/**
  * @brief  Function implementing the ProducerThread thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Producer_Thread */
void Producer_Thread(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    if (ProducerValue < PRODUCER_MAX_VALUE)
    {
      /* Set the message to the queue */
      printf("ProducerThread: sending ProducerValue msg to the queue\r\n");
      if (osMessageQueuePut(osQueueHandle, &ProducerValue, 100, osWaitForever) != osOK)
      {
        Error_Handler();
      }

      ProducerValue = ProducerValue + 10;
      osDelay(2000);
    }
    else
    {
      /* Terminate ProducerThread execution */
      osThreadTerminate(ProducerThreadHandle);
    }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Consumer_Thread */
/**
* @brief Function implementing the ConsumerThread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Consumer_Thread */
void Consumer_Thread(void *argument)
{
  /* USER CODE BEGIN Consumer_Thread */
  uint32_t osQueueMsg = 0;

  for(;;)
  {
    /* Get the message from the queue */

    if (osMessageQueueGet(osQueueHandle, &osQueueMsg, NULL, osWaitForever) != osOK)
    {
      Error_Handler();
    }
    else
    {
      /* Check if it is the correct message */
      printf("ConsumerThread: getting the msg from queue & checking if it's the correct msg\r\n");
      if(osQueueMsg == CONSUMER_VALUE)
      {
        /* Toggle LD1 (LED_GREEN) */
        printf("ConsumerThread: correct msg verified -> Toggles LED_GREEN\r\n");
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
      }
    }

    /* Terminate ConsumerThread execution */
    osThreadTerminate(ConsumerThreadHandle);
  }
  /* USER CODE END Consumer_Thread */
}

/* USER CODE BEGIN Header_Gen_Thread */
/**
* @brief Function implementing the GenThread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Gen_Thread */
void Gen_Thread(void *argument)
{
  /* USER CODE BEGIN Gen_Thread */
  uint32_t osQueueMsg = 0;

  for(;;)
  {
    /* Get the message from the queue */
    if (osMessageQueueGet(osQueueHandle, &osQueueMsg, NULL, osWaitForever) != osOK)
    {
      Error_Handler();
    }
    else
    {
      /* Check if it's the correct message */
      printf("GenThread: getting the msg from queue & checking if the GEN_VALUE is reached\r\n");
      if(osQueueMsg == GEN_VALUE)
      {
        /* Set ReceiveThread' flag to 1 */
        printf("GenThread: GenerationValue reached -> Set ReceiveThread' flag to 1\r\n");
        osThreadFlagsSet(ReceiveThreadHandle, 0x0001U);
      }
    }

    /* Terminate GenThread execution */
    osThreadTerminate(GenThreadHandle);
   }
  /* USER CODE END Gen_Thread */
}

/* USER CODE BEGIN Header_Receive_Thread */
/**
* @brief Function implementing the ReceiveThread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Receive_Thread */
void Receive_Thread(void *argument)
{
  /* USER CODE BEGIN Receive_Thread */
  for (;;)
  {
    /* Wait forever until thread flag 1 is set */
    printf("ReceiveThread: Wait until thread flag 1 is set\r\n");
    osThreadFlagsWait(0x0001U, osFlagsWaitAny, osWaitForever);

    /* Toggle LD2 (LED_BLUE) */
    printf("ReceiveThread: Flag set to 1 -> Toggles LED_BLUE\r\n");
    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);

    /* Terminate ReceiveThread execution */
    osThreadTerminate(ReceiveThreadHandle);
  }
  /* USER CODE END Receive_Thread */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  printf("Critical Error occurred \r\n");
  while (1)
  {
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    HAL_Delay(500);
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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
