/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HIDS_app.h
  * @author  MCD Application Team
  * @brief   Header for HIDS_app.c
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
#ifndef HIDS_APP_H
#define HIDS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  HIDS_CONN_HANDLE_EVT,
  HIDS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  HIDS_LAST_EVT,
} HIDS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  HIDS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN HIDS_APP_ConnHandleNotEvt_t */

  /* USER CODE END HIDS_APP_ConnHandleNotEvt_t */
} HIDS_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */
#define REPORT_ID                       0x00    /* Keyboard */
#define INPUT_REPORT                    0x01
#define OUTPUT_REPORT                   0x02
#define FEATURE_REPORT                  0x03
#define BOOT_KEYBOARD_INPUT_REPORT      0x04
#define BOOT_KEYBOARD_OUTPUT_REPORT     0x05
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
void HIDS_APP_Init(void);
void HIDS_APP_EvtRx(HIDS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EF */
void setDefaultHidParams(void);
uint8_t hidSendReport(uint8_t id, uint8_t type, uint8_t reportLen, uint8_t *reportData);
void processInputData(uint8_t* data_buffer, uint8_t Nb_bytes);
/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*HIDS_APP_H */
