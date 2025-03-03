/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PowerControl_P_app.h
  * @author  MCD Application Team
  * @brief   Header for PowerControl_P_app.c
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
#ifndef POWERCONTROL_P_APP_H
#define POWERCONTROL_P_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  POWERCONTROL_P_CONN_HANDLE_EVT,
  POWERCONTROL_P_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  POWERCONTROL_P_LAST_EVT,
} POWERCONTROL_P_APP_OpcodeNotificationEvt_t;

typedef struct
{
  POWERCONTROL_P_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN POWERCONTROL_P_APP_ConnHandleNotEvt_t */

  /* USER CODE END POWERCONTROL_P_APP_ConnHandleNotEvt_t */
} POWERCONTROL_P_APP_ConnHandleNotEvt_t;
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
void POWERCONTROL_P_APP_Init(void);
void POWERCONTROL_P_APP_EvtRx(POWERCONTROL_P_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*POWERCONTROL_P_APP_H */
