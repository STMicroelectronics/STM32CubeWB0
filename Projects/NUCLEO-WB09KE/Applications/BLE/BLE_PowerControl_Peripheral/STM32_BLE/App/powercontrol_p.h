/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PowerControl_P.h
  * @author  MCD Application Team
  * @brief   Header for PowerControl_P.c
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
#ifndef POWERCONTROL_P_H
#define POWERCONTROL_P_H

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
  POWERCONTROL_P_LED_C,
  POWERCONTROL_P_SWITCH_C,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  POWERCONTROL_P_CHAROPCODE_LAST
} POWERCONTROL_P_CharOpcode_t;

typedef enum
{
  POWERCONTROL_P_LED_C_READ_EVT,
  POWERCONTROL_P_LED_C_WRITE_NO_RESP_EVT,
  POWERCONTROL_P_SWITCH_C_NOTIFY_ENABLED_EVT,
  POWERCONTROL_P_SWITCH_C_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  POWERCONTROL_P_BOOT_REQUEST_EVT
} POWERCONTROL_P_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} POWERCONTROL_P_Data_t;

typedef struct
{
  POWERCONTROL_P_OpcodeEvt_t       EvtOpcode;
  POWERCONTROL_P_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} POWERCONTROL_P_NotificationEvt_t;

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
void POWERCONTROL_P_Init(void);
void POWERCONTROL_P_Notification(POWERCONTROL_P_NotificationEvt_t *p_Notification);
tBleStatus POWERCONTROL_P_UpdateValue(POWERCONTROL_P_CharOpcode_t CharOpcode, POWERCONTROL_P_Data_t *pData);
tBleStatus POWERCONTROL_P_NotifyValue(POWERCONTROL_P_CharOpcode_t CharOpcode, POWERCONTROL_P_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*POWERCONTROL_P_H */
