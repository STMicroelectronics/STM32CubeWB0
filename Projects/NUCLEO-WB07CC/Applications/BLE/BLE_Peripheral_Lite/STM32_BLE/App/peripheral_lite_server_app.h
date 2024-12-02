/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Peripheral_Lite_Server_app.h
  * @author  MCD Application Team
  * @brief   Header for Peripheral_Lite_Server_app.c
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
#ifndef PERIPHERAL_LITE_SERVER_APP_H
#define PERIPHERAL_LITE_SERVER_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  PERIPHERAL_LITE_SERVER_CONN_HANDLE_EVT,
  PERIPHERAL_LITE_SERVER_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  PERIPHERAL_LITE_SERVER_LAST_EVT,
} PERIPHERAL_LITE_SERVER_APP_OpcodeNotificationEvt_t;

typedef struct
{
  PERIPHERAL_LITE_SERVER_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN PERIPHERAL_LITE_SERVER_APP_ConnHandleNotEvt_t */

  /* USER CODE END PERIPHERAL_LITE_SERVER_APP_ConnHandleNotEvt_t */
} PERIPHERAL_LITE_SERVER_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define CFG_DEV_ID_PERIPHERAL_LITE_SERVER1                  (0x83)
#define CFG_BOARD_ID_WB0                        (0x8D)
#define CFG_FW_DATA3_FUOTA                      (0x01)

#define ADV_TIMEOUT_MS                 (500)
#define CONN_TIMEOUT_MS                (100)
#define NOTIFICATION_TIMEOUT_MS        (1 * 1000)

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void PERIPHERAL_LITE_SERVER_APP_Init(void);
void PERIPHERAL_LITE_SERVER_APP_EvtRx(PERIPHERAL_LITE_SERVER_APP_ConnHandleNotEvt_t *p_Notification);
void PERIPHERAL_LITE_SERVER_APP_Process(void);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*PERIPHERAL_LITE_SERVER_APP_H */
