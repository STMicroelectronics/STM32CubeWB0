/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart_if.c
  * @author  GPM WBL Application Team
  * @brief : Source file for interfacing the stm32_adv_trace to hardware
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

/*
 *	(!) No platform settings set in Cube MX for the MW STM32_WPAN. The Logs will not be used (!)
 *
 *	--------------------------------------------------------------------
 *	If you intend to use the Logs, please follow the procedure below :
 *	--------------------------------------------------------------------
 *
 *	1.	Open your project on Cube MX.
 *
 *	2.	Click on the MW "STM32_BLE".
 *
 *	3.	Click on the "Configuration" panel.
 *
 *	4.	Open the sub-section "Application configuration - Logs".
 *
 *	5.	Enable one of the following according to your needs :
 *		. CFG_LOG_INSERT_TIME_STAMP_INSIDE_THE_TRACE
 *		. CFG_LOG_SUPPORTED
 *
 *	6.	Click on the "Platform Settings" panel.
 *
 *	7.	Select the BSP you'll use for the Logs. It can be one of the following :
 *		In order to select them, you need to activate the corresponding IP beforehand in Cube MX.
 *		. USART1
 *		. LPUART1
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32_adv_trace.h"
#include "usart_if.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RECEIVE_AFTER_TRANSMIT  0 /* Whether the UART should be in RX after a Transmit */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/**
 *  @brief  trace tracer definition.
 *
 *  list all the driver interface used by the trace application.
 */
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver =
{
  UART_Init,
  UART_DeInit,
  UART_StartRx,
  UART_TransmitDMA
};

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private variables ---------------------------------------------------------*/

#if (CFG_DEBUG_APP_ADV_TRACE != 0)

/**
  * @brief buffer to receive 1 character
  */
uint8_t charRx;

/**
  * @brief  TX complete callback
  * @return none
  */
static void (*TxCpltCallback)(void *);
static void (*RxCpltCallback)(uint8_t *pdata, uint16_t size, uint8_t error);

#endif /* (CFG_DEBUG_APP_ADV_TRACE != 0) */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Private function prototypes -----------------------------------------------*/

#if (CFG_DEBUG_APP_ADV_TRACE != 0)

#endif /* (CFG_DEBUG_APP_ADV_TRACE != 0) */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UTIL_ADV_TRACE_Status_t UART_Init(  void (*cb)(void *))
{

#if (CFG_DEBUG_APP_ADV_TRACE != 0)

  /* USER CODE BEGIN UART_Init 1 */

  /* USER CODE END UART_Init 1 */

  /* Init done in main : GPIO */

  /* USER CODE BEGIN UART_Init 2 */

  /* USER CODE END UART_Init 2 */

  TxCpltCallback = cb;

  /* USER CODE BEGIN UART_Init 3 */

  /* USER CODE END UART_Init 3 */

#endif /* (CFG_DEBUG_APP_ADV_TRACE != 0) */

  return UTIL_ADV_TRACE_OK;

  /* USER CODE BEGIN UART_Init 4 */

  /* USER CODE END UART_Init 4 */
}

UTIL_ADV_TRACE_Status_t UART_DeInit( void )
{
#if (CFG_DEBUG_APP_ADV_TRACE != 0)

  /* USER CODE BEGIN UART_DeInit 1 */

  /* USER CODE END UART_DeInit 1 */

  HAL_StatusTypeDef result;

  /* USER CODE BEGIN UART_DeInit 2 */

  /* USER CODE END UART_DeInit 2 */

  if (result != HAL_OK)
  {
    TxCpltCallback = NULL;
    return UTIL_ADV_TRACE_UNKNOWN_ERROR;
  }

  /* USER CODE BEGIN UART_DeInit 3 */

  /* USER CODE END UART_DeInit 3 */

  /* USER CODE BEGIN UART_DeInit 4 */

  /* USER CODE END UART_DeInit 4 */

  /* USER CODE BEGIN UART_DeInit 5 */

  /* USER CODE END UART_DeInit 5 */

#endif /* (CFG_DEBUG_APP_ADV_TRACE != 0) */

  return UTIL_ADV_TRACE_OK;

  /* USER CODE BEGIN UART_DeInit 6 */

  /* USER CODE END UART_DeInit 6 */
}

UTIL_ADV_TRACE_Status_t UART_StartRx(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error))
{
#if (CFG_DEBUG_APP_ADV_TRACE != 0)

  /* USER CODE BEGIN UART_StartRx 1 */

  /* USER CODE END UART_StartRx 1 */

  if (cb != NULL)
  {
    RxCpltCallback = cb;
  }

  /* USER CODE BEGIN UART_StartRx 2 */

  /* USER CODE END UART_StartRx 2 */

#endif /* (CFG_DEBUG_APP_ADV_TRACE != 0) */

  return UTIL_ADV_TRACE_OK;

  /* USER CODE BEGIN UART_StartRx 3 */

  /* USER CODE END UART_StartRx 3 */
}

UTIL_ADV_TRACE_Status_t UART_TransmitDMA ( uint8_t *pdata, uint16_t size )
{
  /* USER CODE BEGIN UART_TransmitDMA 1 */

  /* USER CODE END UART_TransmitDMA 1 */

  UTIL_ADV_TRACE_Status_t status = UTIL_ADV_TRACE_OK;

#if (CFG_DEBUG_APP_ADV_TRACE != 0)

  /* USER CODE BEGIN UART_TransmitDMA 2 */

  /* USER CODE END UART_TransmitDMA 2 */

  HAL_StatusTypeDef result;

  if (result != HAL_OK)
  {
    status = UTIL_ADV_TRACE_HW_ERROR;
  }

#if RECEIVE_AFTER_TRANSMIT
#endif

  /* USER CODE BEGIN UART_TransmitDMA 3 */

  /* USER CODE END UART_TransmitDMA 3 */

#endif /* (CFG_DEBUG_APP_ADV_TRACE != 0) */

  return status;

  /* USER CODE BEGIN UART_TransmitDMA 4 */

  /* USER CODE END UART_TransmitDMA 4 */
}

#if (CFG_DEBUG_APP_ADV_TRACE != 0)

static void USART1_DMA_MspDeInit(void)
{
  /* USER CODE BEGIN USART1_DMA_MspDeInit 1 */

  /* USER CODE END USART1_DMA_MspDeInit 1 */

  /* Disable USART1 clock */
  __HAL_RCC_USART1_CLK_DISABLE();

  /* Disable interrupts for USART1 */
  HAL_NVIC_DisableIRQ(USART1_IRQn);

  /* GPDMA1 controller clock disable */
  __HAL_RCC_DMA_CLK_DISABLE();

  /* DMA interrupt init */
  HAL_NVIC_DisableIRQ(DMA_IRQn);

  /* USER CODE BEGIN USART1_DMA_MspDeInit 2 */

  /* USER CODE END USART1_DMA_MspDeInit 2 */
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* USER CODE BEGIN UsartIf_TxCpltCallback 1 */

  /* USER CODE END UsartIf_TxCpltCallback 1 */

  /* ADV Trace callback */
  if(TxCpltCallback)
    TxCpltCallback(NULL);

  /* USER CODE BEGIN UsartIf_TxCpltCallback 2 */

  /* USER CODE END UsartIf_TxCpltCallback 2 */

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* USER CODE BEGIN UsartIf_RxCpltCallback 1 */

  /* USER CODE END UsartIf_RxCpltCallback 1 */

  RxCpltCallback(&charRx, 1, 0);
  HAL_UART_Receive_IT(&huart1, &charRx, 1);

  /* USER CODE BEGIN UsartIf_RxCpltCallback 2 */

  /* USER CODE END UsartIf_RxCpltCallback 2 */
}

#endif /* (CFG_DEBUG_APP_ADV_TRACE != 0) */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

