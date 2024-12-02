/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BAS_app.c
  * @author  MCD Application Team
  * @brief   BAS_app application definition.
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
#include "bas_app.h"
#include "bas.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Bal_NOTIFICATION_OFF,
  Bal_NOTIFICATION_ON,
  /* USER CODE BEGIN Service3_APP_SendInformation_t */

  /* USER CODE END Service3_APP_SendInformation_t */
  BAS_APP_SENDINFORMATION_LAST
} BAS_APP_SendInformation_t;

typedef struct
{
  BAS_APP_SendInformation_t     Bal_Notification_Status;
  /* USER CODE BEGIN Service3_APP_Context_t */
  uint8_t                       BatLevelValue;
  /* USER CODE END Service3_APP_Context_t */
  uint16_t              ConnectionHandle;
} BAS_APP_Context_t;

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
static BAS_APP_Context_t BAS_APP_Context;

uint8_t a_BAS_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void BAS_Bal_SendNotification(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void BAS_Notification(BAS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_Notification_1 */

  /* USER CODE END Service3_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_Notification_Service3_EvtOpcode */

    /* USER CODE END Service3_Notification_Service3_EvtOpcode */

    case BAS_BAL_READ_EVT:
      /* USER CODE BEGIN Service3Char1_READ_EVT */

      /* USER CODE END Service3Char1_READ_EVT */
      break;

    case BAS_BAL_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service3Char1_NOTIFY_ENABLED_EVT */
      BAS_APP_Context.Bal_Notification_Status = Bal_NOTIFICATION_ON;
      APP_DBG_MSG("-- BATTERY LEVEL APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n\r");
      /* USER CODE END Service3Char1_NOTIFY_ENABLED_EVT */
      break;

    case BAS_BAL_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service3Char1_NOTIFY_DISABLED_EVT */
      BAS_APP_Context.Bal_Notification_Status = Bal_NOTIFICATION_OFF;
      APP_DBG_MSG("-- BATTERY LEVEL APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
      APP_DBG_MSG(" \n\r");      
      /* USER CODE END Service3Char1_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_Notification_default */

      /* USER CODE END Service3_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service3_Notification_2 */

  /* USER CODE END Service3_Notification_2 */
  return;
}

void BAS_APP_EvtRx(BAS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_APP_EvtRx_1 */

  /* USER CODE END Service3_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_APP_EvtRx_Service3_EvtOpcode */

    /* USER CODE END Service3_APP_EvtRx_Service3_EvtOpcode */
    case BAS_CONN_HANDLE_EVT :
      BAS_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;
      /* USER CODE BEGIN Service3_APP_CENTR_CONN_HANDLE_EVT */

      /* USER CODE END Service3_APP_CENTR_CONN_HANDLE_EVT */
      break;
    case BAS_DISCON_HANDLE_EVT :
      BAS_APP_Context.ConnectionHandle = 0xFFFF;
      /* USER CODE BEGIN Service3_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service3_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_APP_EvtRx_default */

      /* USER CODE END Service3_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service3_APP_EvtRx_2 */

  /* USER CODE END Service3_APP_EvtRx_2 */

  return;
}

void BAS_APP_Init(void)
{
  BAS_APP_Context.ConnectionHandle = 0xFFFF;
  BAS_Init();

  /* USER CODE BEGIN Service3_APP_Init */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_SEND_BAT_LEV_NOTIF_ID, UTIL_SEQ_RFU, BAS_Bal_SendNotification);

  BAS_APP_Context.Bal_Notification_Status  = Bal_NOTIFICATION_OFF;

  /* USER CODE END Service3_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void BAS_Bal_SendNotification(void) /* Property Notification */
{
  BAS_APP_SendInformation_t notification_on_off = Bal_NOTIFICATION_OFF;
  BAS_Data_t bas_notification_data;

  bas_notification_data.p_Payload = (uint8_t*)a_BAS_UpdateCharData;
  bas_notification_data.Length = 0;

  /* USER CODE BEGIN Service3Char1_NS_1*/

  /* USER CODE END Service3Char1_NS_1*/

  if (notification_on_off != Bal_NOTIFICATION_OFF && BAS_APP_Context.ConnectionHandle != 0xFFFF)
  {
    BAS_NotifyValue(BAS_BAL, &bas_notification_data, BAS_APP_Context.ConnectionHandle);
  }

  /* USER CODE BEGIN Service3Char1_NS_Last*/

  /* USER CODE END Service3Char1_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
