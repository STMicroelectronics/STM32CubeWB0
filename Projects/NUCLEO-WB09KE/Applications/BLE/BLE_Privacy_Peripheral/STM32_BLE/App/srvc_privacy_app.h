/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SRVC_Privacy_app.h
  * @author  MCD Application Team
  * @brief   Header for SRVC_Privacy_app.c
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
#ifndef SRVC_PRIVACY_APP_H
#define SRVC_PRIVACY_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  SRVC_PRIVACY_CONN_HANDLE_EVT,
  SRVC_PRIVACY_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  SRVC_PRIVACY_LAST_EVT,
} SRVC_PRIVACY_APP_OpcodeNotificationEvt_t;

typedef struct
{
  SRVC_PRIVACY_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN SRVC_PRIVACY_APP_ConnHandleNotEvt_t */

  /* USER CODE END SRVC_PRIVACY_APP_ConnHandleNotEvt_t */
} SRVC_PRIVACY_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define MAX_NUM_BONDED_DEVICES 10
/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void SRVC_PRIVACY_APP_Init(void);
void SRVC_PRIVACY_APP_EvtRx(SRVC_PRIVACY_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*SRVC_PRIVACY_APP_H */
