/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Cortex/CORTEXM_ModePrivilege/Src/main.c
  * @author  MCD Application Team
  * @brief   Description of the Cortex-M0+ Mode Privilege example.
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
#define SP_PROCESS_SIZE             0x200  /* Process stack size */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint8_t PSPMemAlloc[SP_PROCESS_SIZE];
__IO uint32_t Index = 0, PSPValue = 0, CurrentStack = 0, ThreadMode = 0, OperationComplete = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static __INLINE  void __SVC(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if defined ( __CC_ARM   )
__ASM void __SVC(void)
{
  SVC 0x01
  BX R14
}
#elif defined ( __ICCARM__ )
static __INLINE  void __SVC()
{
  __ASM("svc 0x01");
}
#elif defined   (  __GNUC__  )
static __INLINE void __SVC()
{
  __ASM volatile("svc 0x01");
}
#endif
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init(LD1);

  /* Switch Thread mode Stack from Main to Process -----------------------------*/
  /* Initialize memory reserved for Process Stack */
  for(Index = 0; Index < SP_PROCESS_SIZE; Index++)
  {
    PSPMemAlloc[Index] = 0x00;
  }

  /* Set Process stack value */
  __set_PSP((uint32_t)PSPMemAlloc + SP_PROCESS_SIZE);

  /* Select Process Stack as Thread mode Stack */
  __set_CONTROL(SP_PROCESS);

  /* Execute ISB instruction to flush pipeline as recommended by Arm */
  __ISB(); 

  /* Get the Thread mode stack used */
  if((__get_CONTROL() & 0x02) == SP_MAIN)
  {
    /* Main stack is used as the current stack */
    CurrentStack = SP_MAIN;
  }
  else
  {
    /* Process stack is used as the current stack */
    CurrentStack = SP_PROCESS;

    /* Get process stack pointer value */
    PSPValue = __get_PSP();
  }

  /* Switch Thread mode from privileged to unprivileged ------------------------*/
  /* Thread mode has unprivileged access */
  __set_CONTROL(THREAD_MODE_UNPRIVILEGED | SP_PROCESS);
  
  /* Execute ISB instruction to flush pipeline as recommended by Arm */
  __ISB(); 

  /* Unprivileged access mainly affect ability to:
      - Use or not use certain instructions such as MSR fields
      - Access System Control Space (SCS) registers such as NVIC and SysTick */

  /* Check Thread mode privilege status */
  if((__get_CONTROL() & 0x01) == THREAD_MODE_PRIVILEGED)
  {
    /* Thread mode has privileged access  */
    ThreadMode = THREAD_MODE_PRIVILEGED;
  }
  else
  {
    /* Thread mode has unprivileged access*/
    ThreadMode = THREAD_MODE_UNPRIVILEGED;
  }

  /* Switch back Thread mode from unprivileged to privileged -------------------*/
  /* Try to switch back Thread mode to privileged (Not possible, this can be
     done only in Handler mode) */
  __set_CONTROL(THREAD_MODE_PRIVILEGED | SP_PROCESS);

  /* Execute ISB instruction to flush pipeline as recommended by Arm */
  __ISB(); 

  /* Generate a system call exception, and in the ISR switch back Thread mode
    to privileged */
  __SVC();

  /* Check Thread mode privilege status */
  if((__get_CONTROL() & 0x01) == THREAD_MODE_PRIVILEGED)
  {
    /* Thread mode has privileged access  */
    ThreadMode = THREAD_MODE_PRIVILEGED;
  }
  else
  {
    /* Thread mode has unprivileged access*/
    ThreadMode = THREAD_MODE_UNPRIVILEGED;
  }
  OperationComplete = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Turn ON LED once test finished */
    BSP_LED_On(LD1);

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
  OperationComplete = 2;
  while(1) 
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
