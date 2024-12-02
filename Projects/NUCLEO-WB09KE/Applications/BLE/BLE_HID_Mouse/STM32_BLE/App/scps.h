/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SCPS.h
  * @author  MCD Application Team
  * @brief   Header for SCPS.c
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
#ifndef SCPS_H
#define SCPS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_status.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  SCPS_SIW,
  SCPS_SCR,

  /* USER CODE BEGIN Service4_CharOpcode_t */

  /* USER CODE END Service4_CharOpcode_t */

  SCPS_CHAROPCODE_LAST
} SCPS_CharOpcode_t;

typedef enum
{
  SCPS_SIW_WRITE_NO_RESP_EVT,
  SCPS_SCR_NOTIFY_ENABLED_EVT,
  SCPS_SCR_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service4_OpcodeEvt_t */

  /* USER CODE END Service4_OpcodeEvt_t */

  SCPS_BOOT_REQUEST_EVT
} SCPS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service4_Data_t */

  /* USER CODE END Service4_Data_t */

} SCPS_Data_t;

typedef struct
{
  SCPS_OpcodeEvt_t       EvtOpcode;
  SCPS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service4_NotificationEvt_t */

  /* USER CODE END Service4_NotificationEvt_t */

} SCPS_NotificationEvt_t;

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
void SCPS_Init(void);
void SCPS_Notification(SCPS_NotificationEvt_t *p_Notification);
tBleStatus SCPS_UpdateValue(SCPS_CharOpcode_t CharOpcode, SCPS_Data_t *pData);
tBleStatus SCPS_NotifyValue(SCPS_CharOpcode_t CharOpcode, SCPS_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*SCPS_H */
