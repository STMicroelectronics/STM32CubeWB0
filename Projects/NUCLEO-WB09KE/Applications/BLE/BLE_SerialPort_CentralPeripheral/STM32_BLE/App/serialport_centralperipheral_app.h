/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SerialPort_CentralPeripheral_app.h
  * @author  MCD Application Team
  * @brief   Header for SerialPort_CentralPeripheral_app.c
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
#ifndef SERIALPORT_CENTRALPERIPHERAL_APP_H
#define SERIALPORT_CENTRALPERIPHERAL_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  SERIALPORT_CENTRALPERIPHERAL_CENTR_CONN_HANDLE_EVT,
  SERIALPORT_CENTRALPERIPHERAL_PERIPH_CONN_HANDLE_EVT,
  SERIALPORT_CENTRALPERIPHERAL_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  SERIALPORT_CENTRALPERIPHERAL_LAST_EVT,
} SERIALPORT_CENTRALPERIPHERAL_APP_OpcodeNotificationEvt_t;

typedef struct
{
  SERIALPORT_CENTRALPERIPHERAL_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN SERIALPORT_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t */

  /* USER CODE END SERIALPORT_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t */
} SERIALPORT_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t;
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
void SERIALPORT_CENTRALPERIPHERAL_APP_Init(void);
void SERIALPORT_CENTRALPERIPHERAL_APP_EvtRx(SERIALPORT_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*SERIALPORT_CENTRALPERIPHERAL_APP_H */
