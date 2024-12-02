/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Peripheral_Lite_Server.h
  * @author  MCD Application Team
  * @brief   Header for Peripheral_Lite_Server.c
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
#ifndef PERIPHERAL_LITE_SERVER_H
#define PERIPHERAL_LITE_SERVER_H

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
  PERIPHERAL_LITE_SERVER_LED_C,
  PERIPHERAL_LITE_SERVER_SWITCH_C,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  PERIPHERAL_LITE_SERVER_CHAROPCODE_LAST
} PERIPHERAL_LITE_SERVER_CharOpcode_t;

typedef enum
{
  PERIPHERAL_LITE_SERVER_LED_C_READ_EVT,
  PERIPHERAL_LITE_SERVER_LED_C_WRITE_NO_RESP_EVT,
  PERIPHERAL_LITE_SERVER_SWITCH_C_NOTIFY_ENABLED_EVT,
  PERIPHERAL_LITE_SERVER_SWITCH_C_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  PERIPHERAL_LITE_SERVER_BOOT_REQUEST_EVT
} PERIPHERAL_LITE_SERVER_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} PERIPHERAL_LITE_SERVER_Data_t;

typedef struct
{
  PERIPHERAL_LITE_SERVER_OpcodeEvt_t       EvtOpcode;
  PERIPHERAL_LITE_SERVER_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} PERIPHERAL_LITE_SERVER_NotificationEvt_t;

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
void PERIPHERAL_LITE_SERVER_Init(void);
void PERIPHERAL_LITE_SERVER_Notification(PERIPHERAL_LITE_SERVER_NotificationEvt_t *p_Notification);
tBleStatus PERIPHERAL_LITE_SERVER_UpdateValue(PERIPHERAL_LITE_SERVER_CharOpcode_t CharOpcode, PERIPHERAL_LITE_SERVER_Data_t *pData);
tBleStatus PERIPHERAL_LITE_SERVER_NotifyValue(PERIPHERAL_LITE_SERVER_CharOpcode_t CharOpcode, PERIPHERAL_LITE_SERVER_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*PERIPHERAL_LITE_SERVER_H */
