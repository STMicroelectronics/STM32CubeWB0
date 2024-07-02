/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32wb0x_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */
 
/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void lowPowerIOSetup(void);
/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* IOs configuration to have the lowest power consumption in DEEPSTOP */
  lowPowerIOSetup();

  /* USER CODE END MspInit 0 */

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA8     ------> USART1_RX
    PA9     ------> USART1_TX
    */
    
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }

}

/**
  * @brief PKA MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hpka: PKA handle pointer
  * @retval None
  */
void HAL_PKA_MspInit(PKA_HandleTypeDef* hpka)
{
  /* USER CODE BEGIN PKA_MspInit 0 */

  /* USER CODE END PKA_MspInit 0 */
  if(hpka->Instance==PKA)
  {
    /* Peripheral clock enable */
    __HAL_RCC_PKA_CLK_ENABLE();

    /* Configure NVIC for PKA interrupts */
    /*   Set priority for PKA_IRQn */
    /*   Enable PKA_IRQn */
    NVIC_SetPriority(PKA_IRQn, IRQ_LOW_PRIORITY );
    NVIC_EnableIRQ(PKA_IRQn);
  }
  /* USER CODE BEGIN PKA_MspInit 1 */

  /* USER CODE END PKA_MspInit 1 */
}

/* USER CODE BEGIN 1 */

/**
  * @brief  Configures the IOs pull resistors to have the optimized power consumption.
  * @param  None
  * @retval None
  */
static void lowPowerIOSetup(void)
{
  
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, 
                       PWR_GPIO_BIT_0|
                       PWR_GPIO_BIT_1|
                       PWR_GPIO_BIT_2|                         
                       PWR_GPIO_BIT_3);
  
  HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, 
                             PWR_GPIO_BIT_8|
                             PWR_GPIO_BIT_9|
                             PWR_GPIO_BIT_10|
                             PWR_GPIO_BIT_11);
  
  HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_B, 
                             PWR_GPIO_BIT_0|
                             PWR_GPIO_BIT_3|
                             PWR_GPIO_BIT_6|
                             PWR_GPIO_BIT_7|
                             PWR_GPIO_BIT_12|
                             PWR_GPIO_BIT_13);
  
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B, 
                       PWR_GPIO_BIT_1|
                       PWR_GPIO_BIT_2|
                       PWR_GPIO_BIT_4|
                       PWR_GPIO_BIT_5|  
                       PWR_GPIO_BIT_14|
                       PWR_GPIO_BIT_15);
}

/* USER CODE END 1 */

