/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWR/PWR_DEEPSTOP/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32WB0x PWR HAL API to enter
  *          and exit the standby mode with a wakeup pin or external reset.
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
#define LED_TOGGLE_DELAY         100UL
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
PWR_DEEPSTOPTypeDef sConfigDEEPSTOP;
static uint32_t TimingDelay = LED_TOGGLE_DELAY;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
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
  /* STM32WB0x HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
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

  /* Uncomment to be able to debug after wake-up from Standby. Consumption will be increased */
  //HAL_DBGMCU_EnableDBGStandbyMode();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* Configure LD1 */
  BSP_LED_Init(LD1);
  /* Configure LD3 for error handler */
  BSP_LED_Init(LD3);


  /* Check if the system was resumed from Standby mode */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_DEEPSTOPF) != RESET)
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_DEEPSTOPF);
        /* Check and Clear the Wakeup flag */
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF12) != RESET)
    {
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF12);
    }
    /* Wait that user release the USER push-button */
    BSP_PB_Init(B1, BUTTON_MODE_GPIO);
    while (BSP_PB_GetState(B1) == GPIO_PIN_RESET) {}
  }
    /* Turn on LD1 */
  BSP_LED_On(LD1);
    /* Insert 5 seconds delay */
  HAL_Delay(5000);
    /* The Following Wakeup sequence is highly recommended prior to Standby mode entry
     - Enable wakeup
     - Clear wake up pin flag depending in edge detection & pin level.
     - Enter the Standby mode.
  */

  /* Enable WakeUp Pin PWR_WAKEUP_PIN0 connected to GPIO_PIN_0 */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PA0,PWR_WUP_RISIEDG);

  /* Clear all related wakeup flags*/
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF12);
  sConfigDEEPSTOP.deepStopMode = PWR_DEEPSTOP_WITH_SLOW_CLOCK_ON;
  /* Enter the Standby mode */
  HAL_PWR_ConfigDEEPSTOP(&sConfigDEEPSTOP);
  HAL_PWR_EnterDEEPSTOPMode();
  /* USER CODE END 2 */

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/**
  * @brief SYSTICK callback
  * @param None
  * @retval None
  */
void HAL_SYSTICK_Callback(void)
{
  if (TimingDelay != 0)
  {
    TimingDelay--;
  }
  else
  {
    /* Toggle LD1 */
    BSP_LED_Toggle(LD1);
    TimingDelay = LED_TOGGLE_DELAY;
  }
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

  /* Turn LD2 on */
  BSP_LED_On(LD2);
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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
