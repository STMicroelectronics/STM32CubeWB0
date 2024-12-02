/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Security_Peripheral.h
  * @author  MCD Application Team
  * @brief   Header for Security_Peripheral.c
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
#ifndef SECURITY_PERIPHERAL_H
#define SECURITY_PERIPHERAL_H

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
  SECURITY_PERIPHERAL_LED_C,
  SECURITY_PERIPHERAL_SWITCH_C,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  SECURITY_PERIPHERAL_CHAROPCODE_LAST
} SECURITY_PERIPHERAL_CharOpcode_t;

typedef enum
{
  SECURITY_PERIPHERAL_LED_C_READ_EVT,
  SECURITY_PERIPHERAL_LED_C_WRITE_NO_RESP_EVT,
  SECURITY_PERIPHERAL_SWITCH_C_NOTIFY_ENABLED_EVT,
  SECURITY_PERIPHERAL_SWITCH_C_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  SECURITY_PERIPHERAL_BOOT_REQUEST_EVT
} SECURITY_PERIPHERAL_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} SECURITY_PERIPHERAL_Data_t;

typedef struct
{
  SECURITY_PERIPHERAL_OpcodeEvt_t       EvtOpcode;
  SECURITY_PERIPHERAL_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} SECURITY_PERIPHERAL_NotificationEvt_t;

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
void SECURITY_PERIPHERAL_Init(void);
void SECURITY_PERIPHERAL_Notification(SECURITY_PERIPHERAL_NotificationEvt_t *p_Notification);
tBleStatus SECURITY_PERIPHERAL_UpdateValue(SECURITY_PERIPHERAL_CharOpcode_t CharOpcode, SECURITY_PERIPHERAL_Data_t *pData);
tBleStatus SECURITY_PERIPHERAL_NotifyValue(SECURITY_PERIPHERAL_CharOpcode_t CharOpcode, SECURITY_PERIPHERAL_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*SECURITY_PERIPHERAL_H */
