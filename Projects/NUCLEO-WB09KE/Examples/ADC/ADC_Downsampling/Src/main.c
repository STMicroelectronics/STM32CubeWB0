/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples/ADC/ADC_Downsampling/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use a ADC peripheral with
  *          ADC downsampling to perform automatically multiple
  *          ADC conversions and average computation, by ADC hardware,
  *          and therefore off-load the CPU for the equivalent task.
  *          This example is based on the STM32WB0x ADC HAL API.
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
/* Definitions of environment analog values */
/* Definitions of data related to this example */
  /* Init variable out of expected ADC conversion data range */
  #define VAR_CONVERTED_DATA_INIT_VALUE    (__LL_ADC_DIGITAL_SCALE(LL_ADC_DS_DATA_WIDTH_12_BIT) + 1)
  /* Init variable out of ADC expected conversion data range for data         */
  /* on 16 bits (downsampling enabled).                                       */
  #define VAR_CONVERTED_DATA_INIT_VALUE_16BITS    (0xFFFF + 1UL)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN PV */
/* Variables for ADC conversion data */
__IO uint16_t uhADCxConvertedData_OVS_ratio16_width_12_bit   = VAR_CONVERTED_DATA_INIT_VALUE;        /* ADC group regular conversion data, downsampling ratio 16  (data scale: 12 bits) */
__IO uint32_t uhADCxConvertedData_OVS_ratio16_width_16_bit   = VAR_CONVERTED_DATA_INIT_VALUE_16BITS; /* ADC group regular conversion data, downsampling ratio 12  (data scale: 16 bits) */
__IO uint16_t uhADCxConvertedData_OVS_disabled  = VAR_CONVERTED_DATA_INIT_VALUE;        /* ADC group regular conversion data, downsampling disabled (data scale corresponds to ADC resolution: 12 bits) */
__IO float    fConvertedData_OVS_EquivalentValue12bits = 4.4f; /* Calculation of downsampling raw data to the equivalent data (from variable "uhADCxConvertedData_OVS_ratio16_width_16_bit") to the equivalent data on 12 bits with floating point */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize LED on board */
  BSP_LED_Init(LD1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /*## Step 1: ADC downsampling initial settings  ##########################*/
    /* Start ADC group regular conversion */
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
      /* ADC conversion start error */
      Error_Handler();
    }
    /* Wait for ADC conversion completed */
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
      /* End Of Conversion flag not set on time */
      Error_Handler();
    }
    /* Retrieve ADC conversion data */
    /* (data scale with downsampling ratio 16 corresponds         */
    /* to ADC resolution: 12 bits)                                            */
    uhADCxConvertedData_OVS_ratio16_width_12_bit = HAL_ADC_GetValue(&hadc1);

    /*## Step 2: ADC downsampling modified settings  #########################*/
    /* Modify ADC downsampling settings:                                      */
    /* - ratio: 16                                                            */
    /* Set ADC downsampling parameters */
    HAL_ADC_Stop(&hadc1);
    hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_16_BIT;
    hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_16;
    hadc1.Init.SamplingMode = ENABLE;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
      Error_Handler();
    }
    /* Start ADC group regular conversion */
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
      /* ADC conversion start error */
      Error_Handler();
    }
    /* Wait for ADC conversion completed */
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
      /* End Of Conversion flag not set on time */
      Error_Handler();
    }
    /* Retrieve ADC conversion data */
    /* (data scale with downsampling ratio 16 exceed             */
    /* ADC resolution 12 bits, data scale expected: 16 bits)                  */
    uhADCxConvertedData_OVS_ratio16_width_16_bit = HAL_ADC_GetValue(&hadc1);

    /*## Step 3: ADC downsampling disabled  ##################################*/
    /* Modify ADC downsampling settings:                                      */
    /* - scope: none (downsampling disabled)                                  */
    /* Set ADC downsampling scope */
    HAL_ADC_Stop(&hadc1);
    hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_12_BIT;
    hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_16;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
      Error_Handler();
    }
    /* Start ADC group regular conversion */
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
      /* ADC conversion start error */
      Error_Handler();
    }
    /* Wait for ADC conversion completed */
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
      /* End Of Conversion flag not set on time */
      Error_Handler();
    }
    /* Retrieve ADC conversion data */
    /* (data scale with downsampling disabled corresponds                     */
    /* to ADC resolution: 12 bits)                                            */
    uhADCxConvertedData_OVS_disabled = HAL_ADC_GetValue(&hadc1);
    /* Restore ADC downsampling initial settings for next loop:               */
    /* - scope: ADC group regular                                             */
    /* - ratio: 16                                                            */
    /* Set ADC downsampling scope */
    HAL_ADC_Stop(&hadc1);
    hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_12_BIT;
    hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_16;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
      Error_Handler();
    }

    /*## Step 4: ADC conversion data evaluation  #############################*/
    /* Expected raw data: */
    /*  - Data of initial downsampling configuration (ratio 16)      */
    /*    should be on the same scale as ADC resolution: 12 bits.             */
    /*  - Data of modified downsampling configuration (ratio 16)     */
    /*    should exceed ADC resolution 12 bits and be on scale: 16 bits       */
    /*  - Data of downsampling disabled should be on the same scale as        */
    /*    ADC resolution: 12 bits.                                            */
    /* Expected data comparison: */
    /*  - Data of initial downsampling configuration (ratio 16)      */
    /*    and modified downsampling configuration (ratio 16) with    */
    /*    SW calculation of equivalent data on 12 bits with floating point    */
    /*    should be similar.                                                  */
    /*  - ADC conversion data with downsampling enabled should have less      */
    /*    variation than with downsampling disabled: downsampling is          */
    /*    equivalent to an averaging (average on 16 ADC conversions with      */
    /*    settings of this example).                                          */
    /* Note: Optionally, for this example purpose, check ADC conversion       */
    /*       data validity.                                                   */
    /* Data value should not exceed range resolution 12 bits */
    if ((uhADCxConvertedData_OVS_ratio16_width_12_bit > __LL_ADC_DIGITAL_SCALE(LL_ADC_DS_DATA_WIDTH_12_BIT)) ||
        (uhADCxConvertedData_OVS_disabled > __LL_ADC_DIGITAL_SCALE(LL_ADC_DS_DATA_WIDTH_12_BIT)))
    {
      /* Error: Data not valid */
      Error_Handler();
    }
    /* Data value should not exceed range of resolution 16 bits */
    if (uhADCxConvertedData_OVS_ratio16_width_16_bit > 0xFFFF)
    {
      /* Error: Data not valid */
      Error_Handler();
    }
    /* For this example purpose, calculation of downsampling raw data         */
    /* (from variable "uhADCxConvertedData_OVS_ratio16_WIDTH_16_BIT_16_BIT")               */
    /* to the equivalent data on 12 bits with floating point                  */
    fConvertedData_OVS_EquivalentValue12bits = (((float)uhADCxConvertedData_OVS_ratio16_width_16_bit) / 16);
    /* Data value should not exceed range resolution 12 bits */
    if (fConvertedData_OVS_EquivalentValue12bits > __LL_ADC_DIGITAL_SCALE(LL_ADC_DS_DATA_WIDTH_12_BIT))
    {
      /* Error: Data not valid */
      Error_Handler();
    }
    /* Toggle LED on at the end of all ADC conversions */
    BSP_LED_Toggle(LD1);
    HAL_Delay(LED_BLINK_SLOW);

    /* Note: ADC conversion data is stored into variables:                                     */
    /*       - "uhADCxConvertedData_OVS_ratio16_width_12_bit"                                  */
    /*       - "uhADCxConvertedData_OVS_ratio16_width_16_bit"                                  */
    /*       - "uhADCxConvertedData_OVS_disabled"                                              */
    /*       Computed data with floating point:                                                */
    /*       - "fConvertedData_OVS_EquivalentValue12bits"                                      */
    /*       (for debug: see variable content into watch window).                              */
    /* Note: ADC conversion data can be computed to physical values using ADC LL driver        */
    /*       helper macro:                                                                     */
    /* uhADCxConvertedData_Voltage_mVolt                                                       */
    /*           = __LL_ADC_CALC_DATA_TO_VOLTAGE(LL_ADC_VIN_RANGE_3V6,                         */
    /*                                           uhADCxConvertedData_OVS_ratio16_width_16_bit, */
    /*                                           LL_ADC_DS_DATA_WIDTH_16_BIT);                 */
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */
  uint32_t uADCxCalibrationPoint1_Gain;
  uint32_t uADCxCalibrationPoint1_Offset;
  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef ConfigChannel = {0};

  /* USER CODE BEGIN ADC1_Init 1 */
  uADCxCalibrationPoint1_Gain   = LL_ADC_GET_CALIB_GAIN_FOR_VINPX_3V6();
  uADCxCalibrationPoint1_Offset = LL_ADC_GET_CALIB_OFFSET_FOR_VINPX_3V6();
  if(uADCxCalibrationPoint1_Gain == 0xFFF)
  {
    uADCxCalibrationPoint1_Gain = LL_ADC_DEFAULT_RANGE_VALUE_3V6;
    uADCxCalibrationPoint1_Offset = 0UL;
  }
  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ConversionType = ADC_CONVERSION_WITH_DS;
  hadc1.Init.SequenceLength = 1;
  hadc1.Init.SamplingMode = ADC_SAMPLING_AT_START;
  hadc1.Init.SampleRate = ADC_SAMPLE_RATE_16;
  hadc1.Init.InvertOutputMode = ADC_DATA_INVERT_NONE;
  hadc1.Init.Overrun = ADC_NEW_DATA_IS_LOST;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_12_BIT;
  hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_16;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  ConfigChannel.Channel = ADC_CHANNEL_VINP0;
  ConfigChannel.Rank = ADC_RANK_1;
  ConfigChannel.VoltRange = ADC_VIN_RANGE_3V6;
  ConfigChannel.CalibrationPoint.Number = ADC_CALIB_POINT_1;
  ConfigChannel.CalibrationPoint.Gain = uADCxCalibrationPoint1_Gain;
  ConfigChannel.CalibrationPoint.Offset = uADCxCalibrationPoint1_Offset;
  if (HAL_ADC_ConfigChannel(&hadc1, &ConfigChannel) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* Turn on LED and remain in infinite loop */
  while (1)
  {
    BSP_LED_On(LD1);
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
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
