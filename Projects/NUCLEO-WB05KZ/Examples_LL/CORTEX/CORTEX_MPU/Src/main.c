/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/CORTEX/CORTEX_MPU/Src/main.c
  * @author  MCD Application Team
  * @brief   This example presents the MPU features through
  *          the STM32WB0x CORTEX LL API.
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

/* Address of the privileged read-only area */
#define ARRAY_ADDRESS_START                      (0x20002000UL)
/* Size of privileged read-only area */
#define ARRAY_SIZE                               LL_MPU_REGION_SIZE_512B
/* Region size to contain privileged read-only area */
#define REGION_SIZE                              LL_MPU_REGION_SIZE_256B
#define REGION_NUMBER                            LL_MPU_REGION_NUMBER3
#define EXAMPLE_RAM_ADDRESS_START                (0x20000000UL)
#define EXAMPLE_RAM_SIZE                         LL_MPU_REGION_SIZE_16KB
#define EXAMPLE_PERIPH_ADDRESS_START             (0x40000000)
#define EXAMPLE_PERIPH_SIZE                      LL_MPU_REGION_SIZE_512KB
#define EXAMPLE_FLASH_ADDRESS_START              (0x08000000)
#define EXAMPLE_FLASH_SIZE                       LL_MPU_REGION_SIZE_512KB
#define EXAMPLE_RAM_REGION_NUMBER                LL_MPU_REGION_NUMBER0
#define EXAMPLE_FLASH_REGION_NUMBER              LL_MPU_REGION_NUMBER1
#define EXAMPLE_PERIPH_REGION_NUMBER             LL_MPU_REGION_NUMBER2
#define portMPU_REGION_READ_WRITE                LL_MPU_REGION_FULL_ACCESS
#define portMPU_REGION_PRIVILEGED_READ_ONLY      LL_MPU_REGION_PRIV_RO
#define portMPU_REGION_READ_ONLY                 LL_MPU_REGION_PRIV_RO_URO
#define portMPU_REGION_PRIVILEGED_READ_WRITE     LL_MPU_REGION_PRIV_RW

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

__IO uint8_t ubButtonPress = 0;

#if defined ( __ICCARM__ )
#pragma location=ARRAY_ADDRESS_START
__no_init __IO uint8_t PrivilegedReadOnlyArray[ARRAY_SIZE];
#else
__IO uint8_t PrivilegedReadOnlyArray[ARRAY_SIZE] __attribute__((section(".ROarraySection")));
#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

void     Configure_MPU(void);
void     MPU_AccessPermConfig(void);
void LED_On(void);
void LED_Off(void);
void WaitForUserButtonPress(void);

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
  LL_APB0_GRP1_EnableClock(LL_APB0_GRP1_PERIPH_SYSCFG);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, 3);

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
  /* USER CODE BEGIN 2 */

  /* Set MPU regions */
  Configure_MPU();
  MPU_AccessPermConfig();

  /* Read from PrivilegedReadOnlyArray. This will not generate error */
  (void)PrivilegedReadOnlyArray[ARRAY_SIZE - 1];

  /* Toggle quickly LD1 while waiting for User-button press */
  WaitForUserButtonPress();

  /* Write into PrivilegedReadOnlyArray. This will generate memory fault error */
  PrivilegedReadOnlyArray[ARRAY_SIZE - 1] = 'e';

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

   /* Set HSE Capacitor Tuning */
  LL_RCC_HSE_SetCapacitorTuning(CFG_HW_RCC_HSE_CAPACITOR_TUNE);

   /* Set HSE Current Control */
  LL_RCC_HSE_SetCurrentControl(LL_RCC_HSE_CURRENTMAX_3);

   /* Enable the HSE */
  LL_RCC_HSE_Enable();
/* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() == 0U);

  LL_FLASH_SetLatency(LL_FLASH_WAIT_STATES_1);

   /* Enable the RC64MPLL*/
  LL_RCC_RC64MPLL_Enable();
/* Wait till RC64MPLL is ready */
  while(LL_RCC_RC64MPLL_IsReady() == 0);

 /* Configure the RC64MPLL multiplication factor */
  LL_RCC_SetRC64MPLLPrescaler(LL_RCC_RC64MPLL_DIV_1);

 /* Update the SystemCoreClock global variable */
  LL_SetSystemCoreClock(64000000);

 /* Configure the source of time base considering new system clocks settings*/
  LL_Init1msTick(64000000);
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  LL_RCC_SetSMPSPrescaler(LL_RCC_SMPS_DIV_4);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  LL_APB0_GRP1_EnableClock(LL_APB0_GRP1_PERIPH_SYSCFG);

  /**/
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);

  /**/
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

  /**/
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_MODE_INPUT);

  /* EXTI interrupt init*/
  NVIC_SetPriority(GPIOA_IRQn, 0);
  NVIC_EnableIRQ(GPIOA_IRQn);

  LL_EXTI_EnableIT(LL_EXTI_LINE_PA0);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/**
  * @brief  Configures the main MPU regions.
  * @param  None
  * @retval None
  */
void Configure_MPU(void)
{
  /* Disable MPU */
  LL_MPU_Disable();

  /* Configure RAM region as Region Number 0, 16KB of size and R/W region */
  LL_MPU_ConfigRegion(EXAMPLE_RAM_REGION_NUMBER, 0x00, EXAMPLE_RAM_ADDRESS_START,
         EXAMPLE_RAM_SIZE | portMPU_REGION_READ_WRITE | LL_MPU_ACCESS_NOT_BUFFERABLE |
         LL_MPU_ACCESS_NOT_CACHEABLE | LL_MPU_ACCESS_NOT_SHAREABLE | LL_MPU_TEX_LEVEL0 |
         LL_MPU_INSTRUCTION_ACCESS_ENABLE);

  /* Configure FLASH region as REGION Number 1, 512KB of size and R/W region */
  LL_MPU_ConfigRegion(EXAMPLE_FLASH_REGION_NUMBER, 0x00, EXAMPLE_FLASH_ADDRESS_START,
         EXAMPLE_FLASH_SIZE | portMPU_REGION_READ_WRITE | LL_MPU_ACCESS_NOT_BUFFERABLE |
         LL_MPU_ACCESS_NOT_CACHEABLE | LL_MPU_ACCESS_NOT_SHAREABLE | LL_MPU_TEX_LEVEL0 |
         LL_MPU_INSTRUCTION_ACCESS_ENABLE);

  /* Configure Peripheral region as REGION Number 2, 255MB of size, R/W and Execute
  Never region */
  LL_MPU_ConfigRegion(EXAMPLE_PERIPH_REGION_NUMBER, 0x00, EXAMPLE_PERIPH_ADDRESS_START,
         EXAMPLE_PERIPH_SIZE | portMPU_REGION_READ_WRITE | LL_MPU_ACCESS_NOT_BUFFERABLE |
         LL_MPU_ACCESS_NOT_CACHEABLE | LL_MPU_ACCESS_NOT_SHAREABLE | LL_MPU_TEX_LEVEL0 |
         LL_MPU_INSTRUCTION_ACCESS_DISABLE);

  /* Enable MPU (any access not covered by any enabled region will cause a fault) */
  LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);
}

/**
  * @brief  This function configures the access right using Cortex-M0+ MPU regions.
  * @param  None
  * @retval None
  */
void MPU_AccessPermConfig(void)
{
  /* Configure region for PrivilegedReadOnlyArray as REGION Number 3, 32byte and R
     only in privileged mode */
  /* Disable MPU */
  LL_MPU_Disable();

  LL_MPU_ConfigRegion(REGION_NUMBER, 0x00, ARRAY_ADDRESS_START,
         REGION_SIZE | portMPU_REGION_PRIVILEGED_READ_ONLY | LL_MPU_ACCESS_NOT_BUFFERABLE |
         LL_MPU_ACCESS_NOT_CACHEABLE | LL_MPU_ACCESS_NOT_SHAREABLE | LL_MPU_TEX_LEVEL0 |
         LL_MPU_INSTRUCTION_ACCESS_ENABLE);

  /* Enable MPU (any access not covered by any enabled region will cause a fault) */
  LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);
}


/**
  * @brief  Turn on LD1.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LD1 on */
  LL_GPIO_SetOutputPin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  Turn off LD1.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LD1 off */
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  Wait for USER push-button press and toggle LD1.
  * @param  None
  * @retval None
  */
/*  */
void WaitForUserButtonPress(void)
{
  while (ubButtonPress == 0)
  {
    LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    LL_mDelay(LED_BLINK_FAST);
  }
  /* Turn LD1 off */
  LED_Off();
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Function to manage User button IRQ Handler
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Update USER push-button variable : to be checked in waiting loop in main program */
  ubButtonPress = 1;
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

  /* Infinite loop */
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
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
