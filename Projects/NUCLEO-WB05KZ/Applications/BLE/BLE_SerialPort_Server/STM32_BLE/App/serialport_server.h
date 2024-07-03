/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    serialport_server.h
  * @author  MCD Application Team
  * @brief   Header for serialport_server.c
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
#ifndef SERIALPORT_SERVER_H
#define SERIALPORT_SERVER_H

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
  SERIALPORT_SERVER_TX,
  SERIALPORT_SERVER_RX,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  SERIALPORT_SERVER_CHAROPCODE_LAST
} SERIALPORT_SERVER_CharOpcode_t;

typedef enum
{
  SERIALPORT_SERVER_TX_NOTIFY_ENABLED_EVT,
  SERIALPORT_SERVER_TX_NOTIFY_DISABLED_EVT,
  SERIALPORT_SERVER_RX_WRITE_NO_RESP_EVT,
  SERIALPORT_SERVER_RX_WRITE_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  SERIALPORT_SERVER_BOOT_REQUEST_EVT
} SERIALPORT_SERVER_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} SERIALPORT_SERVER_Data_t;

typedef struct
{
  SERIALPORT_SERVER_OpcodeEvt_t       EvtOpcode;
  SERIALPORT_SERVER_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} SERIALPORT_SERVER_NotificationEvt_t;

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
void SERIALPORT_SERVER_Init(void);
void SERIALPORT_SERVER_Notification(SERIALPORT_SERVER_NotificationEvt_t *p_Notification);
tBleStatus SERIALPORT_SERVER_UpdateValue(SERIALPORT_SERVER_CharOpcode_t CharOpcode, SERIALPORT_SERVER_Data_t *pData);
tBleStatus SERIALPORT_SERVER_NotifyValue(SERIALPORT_SERVER_CharOpcode_t CharOpcode, SERIALPORT_SERVER_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */
void Data_Received(uint16_t length, uint8_t *data);
/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*SERIALPORT_SERVER_H */
