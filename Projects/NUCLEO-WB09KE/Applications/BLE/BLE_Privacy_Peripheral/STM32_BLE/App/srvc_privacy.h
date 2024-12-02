/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SRVC_Privacy.h
  * @author  MCD Application Team
  * @brief   Header for SRVC_Privacy.c
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
#ifndef SRVC_PRIVACY_H
#define SRVC_PRIVACY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_status.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */
#define ADVERTISING_HANDLE 0x00
  
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  SRVC_PRIVACY_CHAR_PRIVACY,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  SRVC_PRIVACY_CHAROPCODE_LAST
} SRVC_PRIVACY_CharOpcode_t;

typedef enum
{
  SRVC_PRIVACY_CHAR_PRIVACY_READ_EVT,
  SRVC_PRIVACY_CHAR_PRIVACY_WRITE_EVT,
  SRVC_PRIVACY_CHAR_PRIVACY_NOTIFY_ENABLED_EVT,
  SRVC_PRIVACY_CHAR_PRIVACY_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  SRVC_PRIVACY_BOOT_REQUEST_EVT
} SRVC_PRIVACY_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} SRVC_PRIVACY_Data_t;

typedef struct
{
  SRVC_PRIVACY_OpcodeEvt_t       EvtOpcode;
  SRVC_PRIVACY_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} SRVC_PRIVACY_NotificationEvt_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void SRVC_PRIVACY_Init(void);
void SRVC_PRIVACY_Notification(SRVC_PRIVACY_NotificationEvt_t *p_Notification);
tBleStatus SRVC_PRIVACY_UpdateValue(SRVC_PRIVACY_CharOpcode_t CharOpcode, SRVC_PRIVACY_Data_t *pData);
tBleStatus SRVC_PRIVACY_NotifyValue(SRVC_PRIVACY_CharOpcode_t CharOpcode, SRVC_PRIVACY_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*SRVC_PRIVACY_H */
