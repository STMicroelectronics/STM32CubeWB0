/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RC_LR_Periph.h
  * @author  MCD Application Team
  * @brief   Header for RC_LR_Periph.c
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
#ifndef RC_LR_PERIPH_H
#define RC_LR_PERIPH_H

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
  RC_LR_PERIPH_LED_C,
  RC_LR_PERIPH_SWITCH_C,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  RC_LR_PERIPH_CHAROPCODE_LAST
} RC_LR_PERIPH_CharOpcode_t;

typedef enum
{
  RC_LR_PERIPH_LED_C_READ_EVT,
  RC_LR_PERIPH_LED_C_WRITE_NO_RESP_EVT,
  RC_LR_PERIPH_SWITCH_C_NOTIFY_ENABLED_EVT,
  RC_LR_PERIPH_SWITCH_C_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  RC_LR_PERIPH_BOOT_REQUEST_EVT
} RC_LR_PERIPH_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} RC_LR_PERIPH_Data_t;

typedef struct
{
  RC_LR_PERIPH_OpcodeEvt_t       EvtOpcode;
  RC_LR_PERIPH_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} RC_LR_PERIPH_NotificationEvt_t;

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
void RC_LR_PERIPH_Init(void);
void RC_LR_PERIPH_Notification(RC_LR_PERIPH_NotificationEvt_t *p_Notification);
tBleStatus RC_LR_PERIPH_UpdateValue(RC_LR_PERIPH_CharOpcode_t CharOpcode, RC_LR_PERIPH_Data_t *pData);
tBleStatus RC_LR_PERIPH_NotifyValue(RC_LR_PERIPH_CharOpcode_t CharOpcode, RC_LR_PERIPH_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*RC_LR_PERIPH_H */
