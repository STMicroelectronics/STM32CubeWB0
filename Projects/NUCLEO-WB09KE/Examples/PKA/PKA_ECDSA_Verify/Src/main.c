/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PKA/PKA_ECDSA_Verify/Src/main.c
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

PKA_HandleTypeDef hpka;

RNG_HandleTypeDef hrng;

/* USER CODE BEGIN PV */
PKA_ECDSAVerifInTypeDef in = {0};
__IO uint32_t operationComplete = 0;
const uint32_t input2_Size = 20;  
const uint32_t input2_ModSize = 20;
const uint32_t input2_aSign = 0;
const uint8_t input2_1PKA_ECDSA_VERIF_IN_A_COEFF[20] =            {0x34, 0x0E, 0x7B, 0xE2, 0xA2, 0x80, 0xEB, 0x74, 0xE2, 0xBE, 0x61, 0xBA, 0xDA, 0x74, 0x5D, 0x97, 0xE8, 0xF7, 0xC3, 0x00};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_MOD_GF[20] =             {0xE9, 0x5E, 0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF, 0xC7, 0xAD, 0x95, 0xB3, 0xD8, 0x13, 0x95, 0x15, 0x62, 0x0F};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_INITIAL_POINT_X[20] =    {0xBE, 0xD5, 0xAF, 0x16, 0xEA, 0x3F, 0x6A, 0x4F, 0x62, 0x93, 0x8C, 0x46, 0x31, 0xEB, 0x5A, 0xF7, 0xBD, 0xBC, 0xDB, 0xC3};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_INITIAL_POINT_Y[20] =    {0x16, 0x67, 0xCB, 0x47, 0x7A, 0x1A, 0x8E, 0xC3, 0x38, 0xF9, 0x47, 0x41, 0x66, 0x9C, 0x97, 0x63, 0x16, 0xDA, 0x63, 0x21};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_PUBLIC_KEY_POINT_X[20] = {0x2F, 0xE5, 0x29, 0xFA, 0x05, 0x3A, 0x62, 0xC5, 0xE6, 0x38, 0xAE, 0xFA, 0xF7, 0x24, 0x0C, 0x40, 0x74, 0xEC, 0xB5, 0x99};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_PUBLIC_KEY_POINT_Y[20] = {0x3F, 0x6F, 0x83, 0xD7, 0x4B, 0xAA, 0x44, 0x72, 0x6B, 0x7D, 0x11, 0x4D, 0x7D, 0xF3, 0x42, 0x63, 0x6F, 0xC3, 0xB9, 0xCA};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_SIGNATURE_R[20] =        {0x11, 0xA8, 0x7D, 0xF3, 0xC0, 0xAB, 0xEA, 0xE4, 0x94, 0xED, 0x97, 0x0C, 0xA8, 0x30, 0x65, 0x28, 0x46, 0xB8, 0x1F, 0x32};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_SIGNATURE_S[20] =        {0xDC, 0x69, 0x05, 0xAA, 0x7C, 0x66, 0xD4, 0x65, 0xD3, 0x54, 0x92, 0xB9, 0xF5, 0xA0, 0x5D, 0x13, 0xF0, 0xD2, 0x24, 0xDA};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_HASH_E[20] =             {0xFF, 0x20, 0x02, 0xEA, 0x5B, 0xC3, 0x18, 0xA7, 0x3D, 0xAB, 0xC3, 0xB7, 0x80, 0x4E, 0xD8, 0x9B, 0x7D, 0x18, 0xA7, 0xDD};
const uint8_t input2_1PKA_ECDSA_VERIF_IN_ORDER_N[20] =            { 0xE9, 0x5E, 0x4A, 0x5F, 0x73, 0x70, 0x59, 0xDC, 0x60, 0xDF, 0x59, 0x91, 0xD4, 0x50, 0x29, 0x40, 0x9E, 0x60, 0xFC, 0x09}; 
const uint8_t input1_1PKA_ECDSA_SIGN_IN_HASH_E[20] =              {0x78, 0xF7, 0xB9, 0x38, 0xD9, 0x68, 0x76, 0xE0, 0xF6, 0xCA, 0x3B, 0x7C, 0x42, 0x61, 0x2F, 0xD5, 0xC7, 0x31, 0x94, 0xDC};
const uint32_t SigVer_Result = SET;
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
  
  in.primeOrderSize = input2_Size;
  in.modulusSize = input2_ModSize;
  in.coefSign = input2_aSign;
  in.coef = input2_1PKA_ECDSA_VERIF_IN_A_COEFF;
  in.modulus = input2_1PKA_ECDSA_VERIF_IN_MOD_GF;
  in.basePointX = input2_1PKA_ECDSA_VERIF_IN_INITIAL_POINT_X;
  in.basePointY = input2_1PKA_ECDSA_VERIF_IN_INITIAL_POINT_Y;
  in.pPubKeyCurvePtX = input2_1PKA_ECDSA_VERIF_IN_PUBLIC_KEY_POINT_X;
  in.pPubKeyCurvePtY = input2_1PKA_ECDSA_VERIF_IN_PUBLIC_KEY_POINT_Y;
  in.RSign = input2_1PKA_ECDSA_VERIF_IN_SIGNATURE_R;
  in.SSign = input2_1PKA_ECDSA_VERIF_IN_SIGNATURE_S;
  in.hash = input2_1PKA_ECDSA_VERIF_IN_HASH_E;
  in.primeOrder = input2_1PKA_ECDSA_VERIF_IN_ORDER_N;
  
  /* Launch the verification */
  if(HAL_PKA_ECDSAVerif(&hpka, &in, 5000) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare to expected result */
  if(HAL_PKA_ECDSAVerif_IsValidSignature(&hpka) != SigVer_Result)
  {
    Error_Handler();
  }

  /* Simulate a wrong hash message verification */
  in.hash = input1_1PKA_ECDSA_SIGN_IN_HASH_E;
  
  /* Launch the verification */
  if(HAL_PKA_ECDSAVerif(&hpka, &in, 5000) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare to expected result ( must be different from SigVer_Result as the hash has been altered! ) */
  if(HAL_PKA_ECDSAVerif_IsValidSignature(&hpka) == SigVer_Result)
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
