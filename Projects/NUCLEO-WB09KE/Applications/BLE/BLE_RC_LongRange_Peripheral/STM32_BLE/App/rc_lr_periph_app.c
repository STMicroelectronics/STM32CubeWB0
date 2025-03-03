/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RC_LR_Periph_app.c
  * @author  MCD Application Team
  * @brief   RC_LR_Periph_app application definition.
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
#include "rc_lr_periph_app.h"
#include "rc_lr_periph.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
 typedef struct{
    uint8_t             Device_Led_Selection;
    uint8_t             Led1;
 }LedCharValue_t;

 typedef struct{
    uint8_t             Device_Button_Selection;
    uint8_t             ButtonStatus;
 }ButtonCharValue_t;
/* USER CODE END PTD */

typedef enum
{
  Switch_c_NOTIFICATION_OFF,
  Switch_c_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  RC_LR_PERIPH_APP_SENDINFORMATION_LAST
} RC_LR_PERIPH_APP_SendInformation_t;

typedef struct
{
  RC_LR_PERIPH_APP_SendInformation_t     Switch_c_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  LedCharValue_t              LedControl;
  ButtonCharValue_t           ButtonControl;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} RC_LR_PERIPH_APP_Context_t;

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
static RC_LR_PERIPH_APP_Context_t RC_LR_PERIPH_APP_Context;

uint8_t a_RC_LR_PERIPH_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void RC_LR_PERIPH_Switch_c_SendNotification(void);

/* USER CODE BEGIN PFP */
void RC_LR_PERIPH_APP_LED_BUTTON_context_Init(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void RC_LR_PERIPH_Notification(RC_LR_PERIPH_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case RC_LR_PERIPH_LED_C_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case RC_LR_PERIPH_LED_C_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */
      /* The Central writes to a characteristic to switch LED1 on and off on the peripheral. */
      /* The state of LED1 or LED3 changes every 300ms, which means that the Central sends the write command every 300ms. */
      /* LED1 on the Central indicates that the write is successful, while LED3 indicates that it has failed.*/

      BSP_LED_Toggle(LD1);
      // APP_DBG_MSG("-- APPLICATION SERVER : LED1 toggling.\n");
      APP_DBG_MSG("* "); 

      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case RC_LR_PERIPH_SWITCH_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
      RC_LR_PERIPH_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_ON;
      APP_DBG_MSG("-- APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n");
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case RC_LR_PERIPH_SWITCH_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
      RC_LR_PERIPH_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_OFF;
      APP_DBG_MSG("-- APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
      APP_DBG_MSG(" \n");
      /* USER CODE END Service1Char2_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_Notification_default */

      /* USER CODE END Service1_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service1_Notification_2 */

  /* USER CODE END Service1_Notification_2 */
  return;
}

void RC_LR_PERIPH_APP_EvtRx(RC_LR_PERIPH_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case RC_LR_PERIPH_CONN_HANDLE_EVT :
      RC_LR_PERIPH_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;
      /* USER CODE BEGIN Service1_APP_CENTR_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CENTR_CONN_HANDLE_EVT */
      break;
    case RC_LR_PERIPH_DISCON_HANDLE_EVT :
      RC_LR_PERIPH_APP_Context.ConnectionHandle = 0xFFFF;
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      RC_LR_PERIPH_APP_LED_BUTTON_context_Init();
      /* USER CODE END Service1_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_APP_EvtRx_default */

      /* USER CODE END Service1_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service1_APP_EvtRx_2 */

  /* USER CODE END Service1_APP_EvtRx_2 */

  return;
}

void RC_LR_PERIPH_APP_Init(void)
{
  RC_LR_PERIPH_APP_Context.ConnectionHandle = 0xFFFF;
  RC_LR_PERIPH_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  UTIL_SEQ_RegTask(1U << CFG_TASK_SEND_NOTIF_ID, UTIL_SEQ_RFU, RC_LR_PERIPH_Switch_c_SendNotification);

  /**
   * Initialize LedButton Service
   */
  RC_LR_PERIPH_APP_Context.Switch_c_Notification_Status= Switch_c_NOTIFICATION_OFF;
  RC_LR_PERIPH_APP_LED_BUTTON_context_Init();
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void RC_LR_PERIPH_APP_LED_BUTTON_context_Init(void)
{
  RC_LR_PERIPH_APP_Context.LedControl.Device_Led_Selection=0x01; /* Device1 */
  RC_LR_PERIPH_APP_Context.LedControl.Led1=0x00; /* led OFF */
  RC_LR_PERIPH_APP_Context.ButtonControl.Device_Button_Selection=0x01;/* Device1 */
  RC_LR_PERIPH_APP_Context.ButtonControl.ButtonStatus=0x00;

  return;
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void RC_LR_PERIPH_Switch_c_SendNotification(void) /* Property Notification */
{
  RC_LR_PERIPH_APP_SendInformation_t notification_on_off = Switch_c_NOTIFICATION_OFF;
  RC_LR_PERIPH_Data_t rc_lr_periph_notification_data;

  rc_lr_periph_notification_data.p_Payload = (uint8_t*)a_RC_LR_PERIPH_UpdateCharData;
  rc_lr_periph_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1*/

  if(RC_LR_PERIPH_APP_Context.ButtonControl.ButtonStatus == 0x00)
  {
    RC_LR_PERIPH_APP_Context.ButtonControl.ButtonStatus = 0x01;
  } 
  else
  {
    RC_LR_PERIPH_APP_Context.ButtonControl.ButtonStatus = 0x00;
  }
  a_RC_LR_PERIPH_UpdateCharData[0] = 0x01; /* Device Led selection */
  a_RC_LR_PERIPH_UpdateCharData[1] = RC_LR_PERIPH_APP_Context.ButtonControl.ButtonStatus;
  /* Update notification data length */
  rc_lr_periph_notification_data.Length = (rc_lr_periph_notification_data.Length) + 2; 
  
  if(RC_LR_PERIPH_APP_Context.Switch_c_Notification_Status == Switch_c_NOTIFICATION_ON)
  { 
    APP_DBG_MSG("-- APPLICATION SERVER : INFORM CLIENT BUTTON 1 PUSHED\n");
    notification_on_off = Switch_c_NOTIFICATION_ON;
  } 
  else
  {
    APP_DBG_MSG("-- APPLICATION SERVER : CAN'T INFORM CLIENT - NOTIFICATION DISABLED\n"); 
  }
  /* USER CODE END Service1Char2_NS_1*/

  if (notification_on_off != Switch_c_NOTIFICATION_OFF && RC_LR_PERIPH_APP_Context.ConnectionHandle != 0xFFFF)
  {
    RC_LR_PERIPH_NotifyValue(RC_LR_PERIPH_SWITCH_C, &rc_lr_periph_notification_data, RC_LR_PERIPH_APP_Context.ConnectionHandle);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last*/

  /* USER CODE END Service1Char2_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
