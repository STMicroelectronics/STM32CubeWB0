/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    radio_ota.h
  * @author  GPM WBL Application Team
  * @brief   RADIO ota utility
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
#ifndef RADIO_OTA_H
#define RADIO_OTA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wb0x_hal.h"
#include "main.h"
#include "stm32wb0x_hal_flash.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define MAX_RETRY     (10)

#define HEADER_CONNECTION       0xA0
#define HEADER_SIZE             0xB0
#define HEADER_START            0xC0
#define HEADER_DATAREQ          0xD0
#define HEADER_GETDATA          0xE0
#define HEADER_SENDATA          0xE0
#define HEADER_NOTSTART         0xF0
  

#define OTA_ACCESS_ADDRESS      (uint32_t)(0x88D188DA)
#define OTA_CHANNEL             24
#define RX_WAKEUP_TIME          400
#define TX_WAKEUP_TIME          600

#define RX_TIMEOUT_DATA         50000
#define RX_TIMEOUT_ACK          500
  
#define BYTE_IN_FRAME   16 //64
  
#define RADIO_OTA_MANAGER_VERSION_STRING "1.0.0"
  
#define MIN(a,b)                        (((a) < (b))? (a) : (b))
#include <stdio.h>
#ifdef DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/

uint8_t OTA_ConnectionCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_SizeCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_StartCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_DataRequestCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_SendDataCallback(ActionPacket* p, ActionPacket* next);

uint8_t OTA_Init(void);
uint8_t OTA_Tick(void);

/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*RADIO_OTA_H */
