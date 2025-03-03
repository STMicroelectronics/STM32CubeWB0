/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    MC_Peripheral.h
  * @author  MCD Application Team
  * @brief   Header for MC_Peripheral.c
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
#ifndef MC_PERIPHERAL_H
#define MC_PERIPHERAL_H

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
  MC_PERIPHERAL_LED_C,
  MC_PERIPHERAL_SWITCH_C,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  MC_PERIPHERAL_CHAROPCODE_LAST
} MC_PERIPHERAL_CharOpcode_t;

typedef enum
{
  MC_PERIPHERAL_LED_C_READ_EVT,
  MC_PERIPHERAL_LED_C_WRITE_NO_RESP_EVT,
  MC_PERIPHERAL_SWITCH_C_NOTIFY_ENABLED_EVT,
  MC_PERIPHERAL_SWITCH_C_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  MC_PERIPHERAL_BOOT_REQUEST_EVT
} MC_PERIPHERAL_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} MC_PERIPHERAL_Data_t;

typedef struct
{
  MC_PERIPHERAL_OpcodeEvt_t       EvtOpcode;
  MC_PERIPHERAL_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} MC_PERIPHERAL_NotificationEvt_t;

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
void MC_PERIPHERAL_Init(void);
void MC_PERIPHERAL_Notification(MC_PERIPHERAL_NotificationEvt_t *p_Notification);
tBleStatus MC_PERIPHERAL_UpdateValue(MC_PERIPHERAL_CharOpcode_t CharOpcode, MC_PERIPHERAL_Data_t *pData);
tBleStatus MC_PERIPHERAL_NotifyValue(MC_PERIPHERAL_CharOpcode_t CharOpcode, MC_PERIPHERAL_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*MC_PERIPHERAL_H */
