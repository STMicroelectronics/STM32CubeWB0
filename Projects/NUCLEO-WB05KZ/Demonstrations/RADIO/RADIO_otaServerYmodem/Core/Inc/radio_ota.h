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

#define RX_TIMEOUT_DATA         5000
#define RX_TIMEOUT_ACK          500

#define OTA_MANAGER_VERSION_STRING "1.0.0"

#ifdef DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define MIN(a,b)                        (((a) < (b))? (a) : (b))

/* USER CODE END EC */

/* Exported variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/** @brief Get specific application tag value stored at vector table index
  * OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET
  */
#define TAG_VALUE(x)     (* ((volatile uint32_t*) ((x) + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET)))

/** @brief Application base address */
extern volatile uint32_t appAddress;


#define SERVICE_MANAGER_OFFSET     (0x0)
//#define FLASH_PAGE_SIZE LL_FLASH_PAGE_SIZE


/** @brief OTA in progress tag: it is sets on vector table during OTA upgrade */
#define OTA_IN_PROGRESS_TAG      (0xFFFFFFFF)

/** @brief  OTA  invalid, old tag: it tags old application as invalid/old
  * (OTA upgrade done and jump to new application)
  */
#define OTA_INVALID_OLD_TAG      (0x00000000)

/** @brief  OTA valid tag: it tags new application as valid
  * (It is done after a OTA upgrade process is completed with success:
  * as consequence of a SW reset to OTA Reset Manager) */
#define OTA_VALID_TAG            (0xAA5555AA)

/** @brief  OTA Service Manager valid tag: It tags OTA Service manager as valid
  */
#define OTA_SERVICE_MANAGER_TAG  (0xAABBCCDD)


/** @brief  Vector table entry used to register OTA application validity tag*/
#define OTA_TAG_VECTOR_TABLE_ENTRY_INDEX  (4)
/** @brief  Address offset for vector table entry used to register OTA application validity tag */
#define OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET (OTA_TAG_VECTOR_TABLE_ENTRY_INDEX * 4)


#define PAGE_SIZE_ROUND(size) (FLASH_PAGE_SIZE*(((size)+FLASH_PAGE_SIZE-1)/FLASH_PAGE_SIZE))
#define PAGE_SIZE_TRUNC(size) (FLASH_PAGE_SIZE*((size)/FLASH_PAGE_SIZE))


#define NVM_SIZE                   PAGE_SIZE_ROUND(4 * 1024)
#define SERVICE_MANAGER_SIZE       PAGE_SIZE_ROUND(12 * 1024)

/** @brief OTA application with OTA Service manager address: don't change them */
#define SM_APP_OFFSET              (SERVICE_MANAGER_OFFSET + SERVICE_MANAGER_SIZE)
#define APP_WITH_OTA_SERVICE_ADDRESS             (_MEMORY_FLASH_BEGIN_ + SM_APP_OFFSET)

#define APP_OTA_SERVICE_ADDRESS                  (_MEMORY_FLASH_BEGIN_ + SERVICE_MANAGER_OFFSET)

#define SM_APP_SIZE                PAGE_SIZE_TRUNC((_MEMORY_FLASH_SIZE_-SERVICE_MANAGER_SIZE-NVM_SIZE))

#define APP_WITH_OTA_SERVICE_PAGE_NUMBER_START   (SM_APP_OFFSET/FLASH_PAGE_SIZE)
#define APP_WITH_OTA_SERVICE_PAGE_NUMBER_END     (APP_WITH_OTA_SERVICE_PAGE_NUMBER_START + (SM_APP_SIZE/FLASH_PAGE_SIZE) - 1)

#define OTA_NO_OPERATION                                 (0x11)
#define OTA_APP_SWITCH_OP_CODE_NO_OPERATION              (0xb0014211)
//#define OTA_APP_SWITCH_OP_CODE_GO_TO_OTA_SERVICE_MANAGER (OTA_APP_SWITCH_OP_CODE_NO_OPERATION + (OTA_NO_OPERATION*4))
#define OTA_APP_SWITCH_OP_CODE_GO_TO_OTA_SERVICE_MANAGER (0xB0)

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
uint8_t OTA_ConnectionCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_SizeCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_StartCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_DataRequestCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_SendDataCallback(ActionPacket* p, ActionPacket* next);

uint8_t OTA_Init(void);
uint8_t OTA_Tick(void);
int32_t OTA_ymodem_tick(void);

void OTA_Check_ServiceManager_Operation(void);
uint32_t OTA_Check_Application_Tags_Value(void);
void OTA_Set_Application_Tag_Value(uint32_t address,uint32_t Data);
void processInputData(uint8_t* data_buffer, uint16_t Nb_bytes);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*RADIO_OTA_H */
