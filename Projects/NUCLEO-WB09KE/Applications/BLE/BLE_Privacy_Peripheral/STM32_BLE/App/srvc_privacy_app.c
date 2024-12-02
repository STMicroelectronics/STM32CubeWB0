/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SRVC_Privacy_app.c
  * @author  MCD Application Team
  * @brief   SRVC_Privacy_app application definition.
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
#include "srvc_privacy_app.h"
#include "srvc_privacy.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Char_privacy_NOTIFICATION_OFF,
  Char_privacy_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  SRVC_PRIVACY_APP_SENDINFORMATION_LAST
} SRVC_PRIVACY_APP_SendInformation_t;

typedef struct
{
  SRVC_PRIVACY_APP_SendInformation_t     Char_privacy_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  VTIMER_HandleType TimerSendNotification_Id;
  uint32_t TimerSendNotification_interval;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} SRVC_PRIVACY_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CHAR_DATA_SIZE 1
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static SRVC_PRIVACY_APP_Context_t SRVC_PRIVACY_APP_Context;

uint8_t a_SRVC_PRIVACY_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SRVC_PRIVACY_Char_privacy_SendNotification(void);

/* USER CODE BEGIN PFP */
static void Timer_Callback_SendNotification(void *args);
__USED void Peripheral_Security_Request(void);
__USED void Terminate_Connection_After_Bonding(void);
__USED void Do_Get_Bonded_Devices(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void SRVC_PRIVACY_Notification(SRVC_PRIVACY_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case SRVC_PRIVACY_CHAR_PRIVACY_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case SRVC_PRIVACY_CHAR_PRIVACY_WRITE_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_EVT */

      /* USER CODE END Service1Char1_WRITE_EVT */
      break;

    case SRVC_PRIVACY_CHAR_PRIVACY_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_ENABLED_EVT */
      SRVC_PRIVACY_APP_Context.Char_privacy_Notification_Status = Char_privacy_NOTIFICATION_ON;
      APP_DBG_MSG(" \n");
      APP_DBG_MSG("-- PRIVACY APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n");
      /* Start the timer to send the notification */
      HAL_RADIO_TIMER_StartVirtualTimer(&(SRVC_PRIVACY_APP_Context.TimerSendNotification_Id), SRVC_PRIVACY_APP_Context.TimerSendNotification_interval); 
        
      /* USER CODE END Service1Char1_NOTIFY_ENABLED_EVT */
      break;

    case SRVC_PRIVACY_CHAR_PRIVACY_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_DISABLED_EVT */
      SRVC_PRIVACY_APP_Context.Char_privacy_Notification_Status = Char_privacy_NOTIFICATION_OFF;
      APP_DBG_MSG(" \n");
      APP_DBG_MSG("-- PRIVACY APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
      APP_DBG_MSG(" \n");
      /* USER CODE END Service1Char1_NOTIFY_DISABLED_EVT */
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

void SRVC_PRIVACY_APP_EvtRx(SRVC_PRIVACY_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case SRVC_PRIVACY_CONN_HANDLE_EVT :
      SRVC_PRIVACY_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;
      /* USER CODE BEGIN Service1_APP_CENTR_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CENTR_CONN_HANDLE_EVT */
      break;
    case SRVC_PRIVACY_DISCON_HANDLE_EVT :
      SRVC_PRIVACY_APP_Context.ConnectionHandle = 0xFFFF;
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */

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

void SRVC_PRIVACY_APP_Init(void)
{
  SRVC_PRIVACY_APP_Context.ConnectionHandle = 0xFFFF;
  SRVC_PRIVACY_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  
  /* ms */
  SRVC_PRIVACY_APP_Context.TimerSendNotification_interval = 500; 
  SRVC_PRIVACY_APP_Context.TimerSendNotification_Id.callback = Timer_Callback_SendNotification;
  
  UTIL_SEQ_RegTask(1U << CFG_TASK_SEND_NOTIF_ID, UTIL_SEQ_RFU, SRVC_PRIVACY_Char_privacy_SendNotification);
  
  /* Register tasks in the utility sequencer */
  UTIL_SEQ_RegTask(1U << CFG_TASK_PERIPHERAL_SECURITY_REQUEST, UTIL_SEQ_RFU, Peripheral_Security_Request);
  UTIL_SEQ_RegTask(1U << CFG_TASK_GET_BONDED_DEVICES, UTIL_SEQ_RFU, Do_Get_Bonded_Devices);
  
  UTIL_SEQ_RegTask(1U << CFG_TASK_TERMINATE_CONNECTION_AFTER_BONDING, UTIL_SEQ_RFU, Terminate_Connection_After_Bonding);
  UTIL_SEQ_RegTask(1U << CFG_TASK_CONFIGURE_FILTER_LIST_AND_NEW_ADV, UTIL_SEQ_RFU, Configure_Filterlist);
  

  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void SRVC_PRIVACY_Char_privacy_SendNotification(void) /* Property Notification */
{
  SRVC_PRIVACY_APP_SendInformation_t notification_on_off = Char_privacy_NOTIFICATION_OFF;
  SRVC_PRIVACY_Data_t srvc_privacy_notification_data;

  srvc_privacy_notification_data.p_Payload = (uint8_t*)a_SRVC_PRIVACY_UpdateCharData;
  srvc_privacy_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_NS_1*/
  
  /* Update notification data length */
  srvc_privacy_notification_data.Length = 1; 
  
  if(SRVC_PRIVACY_APP_Context.Char_privacy_Notification_Status == Char_privacy_NOTIFICATION_ON)
  { 
    /* APP_DBG_MSG("-- PRIVACY APPLICATION SERVER : NOTIFICATION ENABLED\n"); */
    notification_on_off = Char_privacy_NOTIFICATION_ON;
    /* It can be done here, or as in this example, in a dedicated function that checks if there are some bonded devices. */
    /* HAL_RADIO_TIMER_StartVirtualTimer(&(SRVC_PRIVACY_APP_Context.TimerSendNotification_Id), SRVC_PRIVACY_APP_Context.TimerSendNotification_interval); */
  } 
  else
  {
    APP_DBG_MSG("-- PRIVACY APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
    notification_on_off = Char_privacy_NOTIFICATION_OFF;
    HAL_RADIO_TIMER_StopVirtualTimer(&SRVC_PRIVACY_APP_Context.TimerSendNotification_Id);
  }
  
  /* USER CODE END Service1Char1_NS_1*/

  if (notification_on_off != Char_privacy_NOTIFICATION_OFF && SRVC_PRIVACY_APP_Context.ConnectionHandle != 0xFFFF)
  {
    SRVC_PRIVACY_NotifyValue(SRVC_PRIVACY_CHAR_PRIVACY, &srvc_privacy_notification_data, SRVC_PRIVACY_APP_Context.ConnectionHandle);
  }

  /* USER CODE BEGIN Service1Char1_NS_Last*/

  /* USER CODE END Service1Char1_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/**
* @brief Handle the timer creation to send in defined cycle the notification.
*/
static void Timer_Callback_SendNotification(void *args)
{ 
  /* Send the notification */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_SEND_NOTIF_ID, CFG_SEQ_PRIO_0); 
  HAL_RADIO_TIMER_StartVirtualTimer(&(SRVC_PRIVACY_APP_Context.TimerSendNotification_Id), SRVC_PRIVACY_APP_Context.TimerSendNotification_interval);
}

/**
* @brief Handle the slave security request task.
*/
__USED void Peripheral_Security_Request(void)
{
  /* Specific implementation for CFG_TASK_PERIPHERAL_SECURITY_REQUEST */
  uint8_t status = 0;
  status = aci_gap_set_security(SRVC_PRIVACY_APP_Context.ConnectionHandle,
                                GAP_SECURITY_LEVEL_3,
                                0x01);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : Pairing request: 0x%x\n", status);
  }
  else
  {
    APP_DBG_MSG("  Success: Pairing request\n");
    BLEStack_Process_Schedule();
  }
  return;
}

/**
* @brief This function checks how many devices have been bonded with the peripheral. 
*        If there are bonded devices, the task to start the notification procedure is set.
*/
__USED void Do_Get_Bonded_Devices(void)
{ 
  /* Specific implementation for CFG_TASK_GET_BONDED_DEVICES */
  uint8_t status = 0;
  uint8_t num_of_addresses = 0;

  Bonded_Device_Entry_t bonded_device_entry_53[MAX_NUM_BONDED_DEVICES] = {0}; 
  
  status = aci_gap_get_bonded_devices(0, MAX_NUM_BONDED_DEVICES, &num_of_addresses, bonded_device_entry_53);
  if (status != BLE_STATUS_SUCCESS) 
  {
    APP_DBG_MSG("  Fail   : aci_gap_get_bonded_devices() failed:0x%02x\r\n", status);
  }
  else
  {
////////    APP_DBG_MSG("  Success: aci_gap_get_bonded_devices; N: %d, update_char_value = %d\n", num_of_addresses, update_char_value);
    APP_DBG_MSG("  Success: aci_gap_get_bonded_devices; N: %d\n", num_of_addresses);
    if (num_of_addresses>=1)
    {
////////      if (update_char_value) 
////////      {
        /* Start the timer to send the notification */
        HAL_RADIO_TIMER_StartVirtualTimer(&(SRVC_PRIVACY_APP_Context.TimerSendNotification_Id), SRVC_PRIVACY_APP_Context.TimerSendNotification_interval);  
////////      }
    }
  }
  return;
}
      

/* USER CODE END FD_LOCAL_FUNCTIONS*/
