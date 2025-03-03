/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    MC_CentralPeripheral.h
  * @author  MCD Application Team
  * @brief   Header for MC_CentralPeripheral.c
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
#ifndef MC_CENTRALPERIPHERAL_H
#define MC_CENTRALPERIPHERAL_H

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
  MC_CENTRALPERIPHERAL_LED_C,
  MC_CENTRALPERIPHERAL_SWITCH_C,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  MC_CENTRALPERIPHERAL_CHAROPCODE_LAST
} MC_CENTRALPERIPHERAL_CharOpcode_t;

typedef enum
{
  MC_CENTRALPERIPHERAL_LED_C_READ_EVT,
  MC_CENTRALPERIPHERAL_LED_C_WRITE_NO_RESP_EVT,
  MC_CENTRALPERIPHERAL_SWITCH_C_NOTIFY_ENABLED_EVT,
  MC_CENTRALPERIPHERAL_SWITCH_C_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  MC_CENTRALPERIPHERAL_BOOT_REQUEST_EVT
} MC_CENTRALPERIPHERAL_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} MC_CENTRALPERIPHERAL_Data_t;

typedef struct
{
  MC_CENTRALPERIPHERAL_OpcodeEvt_t       EvtOpcode;
  MC_CENTRALPERIPHERAL_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} MC_CENTRALPERIPHERAL_NotificationEvt_t;

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
void MC_CENTRALPERIPHERAL_Init(void);
void MC_CENTRALPERIPHERAL_Notification(MC_CENTRALPERIPHERAL_NotificationEvt_t *p_Notification);
tBleStatus MC_CENTRALPERIPHERAL_UpdateValue(MC_CENTRALPERIPHERAL_CharOpcode_t CharOpcode, MC_CENTRALPERIPHERAL_Data_t *pData);
tBleStatus MC_CENTRALPERIPHERAL_NotifyValue(MC_CENTRALPERIPHERAL_CharOpcode_t CharOpcode, MC_CENTRALPERIPHERAL_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*MC_CENTRALPERIPHERAL_H */
