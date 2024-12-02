/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SCPS_app.c
  * @author  MCD Application Team
  * @brief   SCPS_app application definition.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "app_ble.h"
#include "ble.h"
#include "scps_app.h"
#include "scps.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Scr_NOTIFICATION_OFF,
  Scr_NOTIFICATION_ON,
  /* USER CODE BEGIN Service4_APP_SendInformation_t */

  /* USER CODE END Service4_APP_SendInformation_t */
  SCPS_APP_SENDINFORMATION_LAST
} SCPS_APP_SendInformation_t;

typedef struct
{
  SCPS_APP_SendInformation_t     Scr_Notification_Status;
  /* USER CODE BEGIN Service4_APP_Context_t */

  /* USER CODE END Service4_APP_Context_t */
  uint16_t              ConnectionHandle;
} SCPS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static SCPS_APP_Context_t SCPS_APP_Context;

uint8_t a_SCPS_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SCPS_Scr_SendNotification(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void SCPS_Notification(SCPS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service4_Notification_1 */

  /* USER CODE END Service4_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service4_Notification_Service4_EvtOpcode */

    /* USER CODE END Service4_Notification_Service4_EvtOpcode */

    case SCPS_SIW_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service4Char1_WRITE_NO_RESP_EVT */

      /* USER CODE END Service4Char1_WRITE_NO_RESP_EVT */
      break;

    case SCPS_SCR_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service4Char2_NOTIFY_ENABLED_EVT */

      /* USER CODE END Service4Char2_NOTIFY_ENABLED_EVT */
      break;

    case SCPS_SCR_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service4Char2_NOTIFY_DISABLED_EVT */

      /* USER CODE END Service4Char2_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service4_Notification_default */

      /* USER CODE END Service4_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service4_Notification_2 */

  /* USER CODE END Service4_Notification_2 */
  return;
}

void SCPS_APP_EvtRx(SCPS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service4_APP_EvtRx_1 */

  /* USER CODE END Service4_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service4_APP_EvtRx_Service4_EvtOpcode */

    /* USER CODE END Service4_APP_EvtRx_Service4_EvtOpcode */
    case SCPS_CONN_HANDLE_EVT :
      SCPS_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;
      /* USER CODE BEGIN Service4_APP_CENTR_CONN_HANDLE_EVT */

      /* USER CODE END Service4_APP_CENTR_CONN_HANDLE_EVT */
      break;
    case SCPS_DISCON_HANDLE_EVT :
      SCPS_APP_Context.ConnectionHandle = 0xFFFF;
      /* USER CODE BEGIN Service4_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service4_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service4_APP_EvtRx_default */

      /* USER CODE END Service4_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service4_APP_EvtRx_2 */

  /* USER CODE END Service4_APP_EvtRx_2 */

  return;
}

void SCPS_APP_Init(void)
{
  SCPS_APP_Context.ConnectionHandle = 0xFFFF;
  SCPS_Init();

  /* USER CODE BEGIN Service4_APP_Init */

  /* USER CODE END Service4_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void SCPS_Scr_SendNotification(void) /* Property Notification */
{
  SCPS_APP_SendInformation_t notification_on_off = Scr_NOTIFICATION_OFF;
  SCPS_Data_t scps_notification_data;

  scps_notification_data.p_Payload = (uint8_t*)a_SCPS_UpdateCharData;
  scps_notification_data.Length = 0;

  /* USER CODE BEGIN Service4Char2_NS_1*/

  /* USER CODE END Service4Char2_NS_1*/

  if (notification_on_off != Scr_NOTIFICATION_OFF && SCPS_APP_Context.ConnectionHandle != 0xFFFF)
  {
    SCPS_NotifyValue(SCPS_SCR, &scps_notification_data, SCPS_APP_Context.ConnectionHandle);
  }

  /* USER CODE BEGIN Service4Char2_NS_Last*/

  /* USER CODE END Service4Char2_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
