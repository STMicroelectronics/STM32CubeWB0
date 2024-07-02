/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_WriteProtection/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to set write protection on
  *          STM32WB0x FLASH.
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
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FLASH_USER_START_ADDR   (FLASH_END_ADDR - FLASH_PAGE_SIZE + 1)
#define FLASH_USER_END_ADDR     (FLASH_END_ADDR + 1)
#define DATA_32                 ((uint32_t)0x12345678)

/* Uncomment this line to program the Flash pages */
#define FLASH_PAGE_PROGRAM

/* Uncomment this line to Enable Write Protection */
/* #define WRITE_PROTECTION_ENABLE */

/* Uncomment this line to Disable Write Protection */
/* #define WRITE_PROTECTION_DISABLE */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t FirstPage = 0, NbOfPages = 0;
uint32_t Address = 0, PageError = 0;
__IO uint32_t MemoryProgramStatus = 0;

#ifdef FLASH_PAGE_PROGRAM
static FLASH_EraseInitTypeDef EraseInitStruct;
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t GetPage(uint32_t Address);
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
  /* USER CODE BEGIN 2 */
  /* Initialize LD1, LD2 and LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);
  
   /* Get the 1st page */
  FirstPage = GetPage(FLASH_USER_START_ADDR);

  /* Get the number of pages from 1st page */
  NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage;

#ifdef WRITE_PROTECTION_DISABLE

  /* Disable the page write protection for group 15 containing Flash pages 120 to 127 */
  FLASH_PageProtectionTypeDef pageProtectionStruct = {0};

  uint32_t seg0_offset = (FirstPage << 0x8) & 0xFF00;
  uint32_t seg0_size  = NbOfPages & 0x00FF;
  pageProtectionStruct.pageProt_0 =  seg0_offset | seg0_size;
  pageProtectionStruct.state = DISABLE;

  /* It allows protecting from write and page erase. */
  
  HAL_FLASHEx_PageProtection(&pageProtectionStruct);
  

#elif defined WRITE_PROTECTION_ENABLE
  
    /* Enable the page write protection for group 15 containing Flash pages 120 to 127 */
  FLASH_PageProtectionTypeDef pageProtectionStruct = {0};

  uint32_t seg0_offset = (FirstPage << 0x8) & 0xFF00;
  uint32_t seg0_size  = NbOfPages & 0x00FF;
  pageProtectionStruct.pageProt_0 =  seg0_offset | seg0_size;
  pageProtectionStruct.state = ENABLE;

  /* It allows protecting from write and page erase. */
  
  HAL_FLASHEx_PageProtection(&pageProtectionStruct);
  
#endif /* WRITE_PROTECTION_DISABLE */


#ifdef FLASH_PAGE_PROGRAM

  /* The desired pages are not write protected */
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
      /*
        Error occurred while page erase.
        User can add here some code to deal with this error.
        PageError will contain the faulty page and then to know the code error on this page,
        user can call function 'HAL_FLASH_GetError()'
      */
      while (1)
      {
        Error_Handler();
      }
    }
  
#endif /* FLASH_PAGE_PROGRAM */
  /* Program the user Flash area word by word
  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
  
  Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Address, DATA_32) == HAL_OK)
    {
      Address = Address + 4;
      MemoryProgramStatus++;
    }
    if (MemoryProgramStatus != 0)
    {
      /* No error detected. Switch on LED1*/

      while (1)
      {
        BSP_LED_On(LD1);
      }
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
        User can add here some code to deal with this error */

      BSP_LED_On(LD2);
    }
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */
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
/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  if(!IS_ADDR_ALIGNED_32BITS(Addr)){
    while(1);
  }
  uint32_t page = (Addr - FLASH_START_ADDR) / FLASH_PAGE_SIZE;
  if(!IS_FLASH_PAGE(page)){
    while(1);
  }
  return page;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  while(1)
  {
    BSP_LED_On(LD3);
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
