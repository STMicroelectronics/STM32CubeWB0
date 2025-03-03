/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wb0x_hal.h"
#include "app_entry.h"
#include "app_common.h"
#include "app_debug.h"
#include "compiler.h"

#include "stm32wb0x_ll_spi.h"
#include "stm32wb0x_ll_bus.h"
#include "stm32wb0x_ll_cortex.h"
#include "stm32wb0x_ll_rcc.h"
#include "stm32wb0x_ll_system.h"
#include "stm32wb0x_ll_utils.h"
#include "stm32wb0x_ll_pwr.h"
#include "stm32wb0x_ll_gpio.h"
#include "stm32wb0x_ll_dma.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wb0x_nucleo.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_SPI1_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MOSI_Pin GPIO_PIN_15
#define MOSI_GPIO_Port GPIOA
#define MISO_Pin GPIO_PIN_14
#define MISO_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_13
#define SCK_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_11
#define CS_GPIO_Port GPIOA
#define IRQ_Pin GPIO_PIN_10
#define IRQ_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define CS_IRQn                 GPIOA_IRQn
#define CS_IRQHandler           GPIOA_IRQHandler
#define CS_EXTI_LINE            LL_EXTI_LINE_PA11
#define CS_WAKEUP_PIN           PWR_WAKEUP_PA11

#define SPI                     SPI1
#define DMA_CH_SPI_TX           LL_DMA_CHANNEL_3
#define DMA_CH_SPI_RX           LL_DMA_CHANNEL_1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
