/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PKA/PKA_ModularExponentiation/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use PKA through
  *          the STM32WB0x HAL API.
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
  const uint32_t input1_Size = 24;
  const uint32_t input1_ExpSize = 24;
  const uint8_t input1_1[24] = {0x55, 0xE8, 0x82, 0x73, 0x0E, 0xBC, 0x52, 0xEC, 0x42, 0xBE, 0x0C, 0x7B, 0x6F, 0xFF, 0x0B, 0xFC, 0x07, 0xB0, 0x5A, 0x1C, 0x79, 0x36, 0x03, 0x7D};
  const uint8_t input1_2[24] = {0x17, 0x24, 0x70, 0xF1, 0x2D, 0x60, 0x9A, 0x8B, 0x5F, 0xDC, 0xF8, 0x12, 0x7C, 0xBE, 0xE9, 0xAD, 0xB9, 0x3C, 0xB7, 0x63, 0x59, 0x9D, 0xAE, 0x40};
  const uint8_t input1_3[24] = {0xAA, 0x7F, 0xE9, 0x0B, 0xDB, 0xFD, 0x3B, 0xD7, 0xDE, 0xA1, 0x84, 0x22, 0xA7, 0xBF, 0x12, 0xE2, 0x7F, 0x00, 0x1A, 0x46, 0xBA, 0x64, 0x49, 0x11};
  const uint8_t output1[24]  = {0x48, 0x19, 0x19, 0xB6, 0xA3, 0x2E, 0xFD, 0x38, 0xCC, 0xB7, 0x6B, 0xA7, 0x35, 0xC2, 0x69, 0xA9, 0xD3, 0x8F, 0xE1, 0xAC, 0xEF, 0x76, 0x56, 0x35};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

PKA_HandleTypeDef hpka;

RNG_HandleTypeDef hrng;

/* USER CODE BEGIN PV */
PKA_ModExpInTypeDef in = {0};
uint8_t decipheredBuffer[256];
uint8_t cipheredBuffer[24];
__IO uint32_t operationComplete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RNG_Init(void);
static void MX_PKA_Init(void);
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
  /* Configure LD1 */
  BSP_LED_Init(LD1);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RNG_Init();
  MX_PKA_Init();
  /* USER CODE BEGIN 2 */
  /* Set input parameters */
      in.expSize = input1_ExpSize;
      in.OpSize = input1_Size;
      in.pOp1 = input1_1;
      in.pExp = input1_2;
      in.pMod = input1_3;
  
  /* Start the modular exponentiation (ciphering step) */
  HAL_PKA_RAMReset(&hpka);
  if(HAL_PKA_ModExp(&hpka, &in, 5000) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Copy the results to user specified space */
  HAL_PKA_ModExp_GetResult(&hpka , cipheredBuffer);
  
  /* (Facultative) Compare the computed and expected result */
  if (memcmp(cipheredBuffer, output1, ciphertext_bin_len) != 0)
  {
    Error_Handler();
  }
  
  /* Set input parameters */
  in.expSize = rsa_priv_2048_privateExponent_len;
  in.OpSize  = rsa_pub_2048_modulus_len;
  in.pOp1    = ciphertext_bin;
  in.pExp    = rsa_priv_2048_privateExponent;
  in.pMod    = rsa_priv_2048_modulus;

  /* Start the modular exponentiation (deciphering step) */
  if(HAL_PKA_ModExp(&hpka, &in, 5000) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Copy the results to user specified space */
  HAL_PKA_ModExp_GetResult(&hpka , decipheredBuffer);

  /* (Facultative) Compare the computed and expected result */
  if (memcmp(decipheredBuffer, plaintext_bin, plaintext_bin_len) != 0)
  {
    Error_Handler();
  }
  
  /* Deinitialize the PKA */
  if(HAL_PKA_DeInit(&hpka) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Success */
  operationComplete = 1;
  BSP_LED_On(LD1);
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
  * @brief PKA Initialization Function
  * @param None
  * @retval None
  */
static void MX_PKA_Init(void)
{

  /* USER CODE BEGIN PKA_Init 0 */

  /* USER CODE END PKA_Init 0 */

  /* USER CODE BEGIN PKA_Init 1 */

  /* USER CODE END PKA_Init 1 */
  hpka.Instance = PKA;
  if (HAL_PKA_Init(&hpka) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN PKA_Init 2 */

  /* USER CODE END PKA_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

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
  operationComplete = 2;
  while (1)
  {
    /* Error if LD1 is slowly blinking (1 sec. period) */
    BSP_LED_Toggle(LD1); 
    HAL_Delay(1000);   
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
