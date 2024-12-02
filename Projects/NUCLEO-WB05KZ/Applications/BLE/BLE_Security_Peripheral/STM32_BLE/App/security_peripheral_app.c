/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Security_Peripheral_app.c
  * @author  MCD Application Team
  * @brief   Security_Peripheral_app application definition.
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
#include "security_peripheral_app.h"
#include "security_peripheral.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
 typedef struct{
    uint8_t             Device_Led_Selection;
    uint8_t             Led1;
 }SECURITY_LedCharValue_t;

 typedef struct{
    uint8_t             Device_Button_Selection;
    uint8_t             ButtonStatus;
 }SECURITY_ButtonCharValue_t;
/* USER CODE END PTD */

typedef enum
{
  Switch_c_NOTIFICATION_OFF,
  Switch_c_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  SECURITY_PERIPHERAL_APP_SENDINFORMATION_LAST
} SECURITY_PERIPHERAL_APP_SendInformation_t;

typedef struct
{
  SECURITY_PERIPHERAL_APP_SendInformation_t     Switch_c_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  VTIMER_HandleType TimerSendNotification_Id;
  uint32_t TimerSendNotification_interval;
  SECURITY_LedCharValue_t              LedControl;
  SECURITY_ButtonCharValue_t           ButtonControl;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} SECURITY_PERIPHERAL_APP_Context_t;

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
static SECURITY_PERIPHERAL_APP_Context_t SECURITY_PERIPHERAL_APP_Context;

uint8_t a_SECURITY_PERIPHERAL_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SECURITY_PERIPHERAL_Switch_c_SendNotification(void);

/* USER CODE BEGIN PFP */
static void SECURITY_PERIPHERAL_APP_LED_BUTTON_context_Init(void);
static void Timer_Callback_SendNotification(void *args);
__USED void SotpTimerNotification(void);
__USED void Peripheral_Security_Request(void);
__USED void Start_timer_send_notification(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void SECURITY_PERIPHERAL_Notification(SECURITY_PERIPHERAL_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case SECURITY_PERIPHERAL_LED_C_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case SECURITY_PERIPHERAL_LED_C_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */
      if(p_Notification->DataTransfered.p_Payload[1] == 0x01)
      {
        BSP_LED_On(LED_BLUE);
        APP_DBG_MSG("-- SECURITY APPLICATION SERVER : LED1 ON\n"); 
        SECURITY_PERIPHERAL_APP_Context.LedControl.Led1 = 0x01; /* LED1 ON */
      }
      if(p_Notification->DataTransfered.p_Payload[1] == 0x00)
      {
        BSP_LED_Off(LED_BLUE);
        APP_DBG_MSG("-- SECURITY APPLICATION SERVER : LED1 OFF\n"); 
        SECURITY_PERIPHERAL_APP_Context.LedControl.Led1 = 0x00; /* LED1 OFF */
      }
      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case SECURITY_PERIPHERAL_SWITCH_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
      SECURITY_PERIPHERAL_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_ON;
      APP_DBG_MSG("-- SECURITY APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n");
      /* Start the timer to send the notification */
      HAL_RADIO_TIMER_StartVirtualTimer(&(SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_Id), SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_interval); 
        
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case SECURITY_PERIPHERAL_SWITCH_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
      SECURITY_PERIPHERAL_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_OFF;
      APP_DBG_MSG("-- SECURITY APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
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

void SECURITY_PERIPHERAL_APP_EvtRx(SECURITY_PERIPHERAL_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case SECURITY_PERIPHERAL_CONN_HANDLE_EVT :
      SECURITY_PERIPHERAL_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;
      /* USER CODE BEGIN Service1_APP_CENTR_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CENTR_CONN_HANDLE_EVT */
      break;
    case SECURITY_PERIPHERAL_DISCON_HANDLE_EVT :
      SECURITY_PERIPHERAL_APP_Context.ConnectionHandle = 0xFFFF;
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      SECURITY_PERIPHERAL_APP_LED_BUTTON_context_Init();
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

void SECURITY_PERIPHERAL_APP_Init(void)
{
  SECURITY_PERIPHERAL_APP_Context.ConnectionHandle = 0xFFFF;
  SECURITY_PERIPHERAL_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  
  /* ms */
  SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_interval = 1000; 
  SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_Id.callback = Timer_Callback_SendNotification;
  
  UTIL_SEQ_RegTask(1U << CFG_TASK_SEND_NOTIF_ID, UTIL_SEQ_RFU, SECURITY_PERIPHERAL_Switch_c_SendNotification);

  UTIL_SEQ_RegTask(1U << CFG_TASK_PERIPHERAL_SECURITY_REQUEST, UTIL_SEQ_RFU, Peripheral_Security_Request);
  UTIL_SEQ_RegTask(1U << CFG_TASK_TIMER_STOP_SEND_NOTIF_ID, UTIL_SEQ_RFU, SotpTimerNotification);
  UTIL_SEQ_RegTask(1U << CFG_TASK_SEND_NOTIF_ID, UTIL_SEQ_RFU, SECURITY_PERIPHERAL_Switch_c_SendNotification);
  UTIL_SEQ_RegTask(1U << CFG_TASK_TIMER_SEND_NOTIF_ID, UTIL_SEQ_RFU, Start_timer_send_notification);


  /**
   * Initialize LedButton Service
   */
  SECURITY_PERIPHERAL_APP_Context.Switch_c_Notification_Status= Switch_c_NOTIFICATION_OFF;
  SECURITY_PERIPHERAL_APP_LED_BUTTON_context_Init();
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void SECURITY_PERIPHERAL_APP_LED_BUTTON_context_Init(void)
{  
  BSP_LED_Off(LED_BLUE);
  SECURITY_PERIPHERAL_APP_Context.LedControl.Device_Led_Selection=0x01; /* Device1 */
  SECURITY_PERIPHERAL_APP_Context.LedControl.Led1=0x00; /* led OFF */

  return;
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void SECURITY_PERIPHERAL_Switch_c_SendNotification(void) /* Property Notification */
{
  SECURITY_PERIPHERAL_APP_SendInformation_t notification_on_off = Switch_c_NOTIFICATION_OFF;
  SECURITY_PERIPHERAL_Data_t security_peripheral_notification_data;

  security_peripheral_notification_data.p_Payload = (uint8_t*)a_SECURITY_PERIPHERAL_UpdateCharData;
  security_peripheral_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1*/

  if(SECURITY_PERIPHERAL_APP_Context.LedControl.Led1 == 0x00)
  {
    SECURITY_PERIPHERAL_APP_Context.LedControl.Led1 = 0x01;
  } 
  else
  {
    SECURITY_PERIPHERAL_APP_Context.LedControl.Led1 = 0x00;
  }
  a_SECURITY_PERIPHERAL_UpdateCharData[0] = 0x01; /* Device Led selection */
  a_SECURITY_PERIPHERAL_UpdateCharData[1] = SECURITY_PERIPHERAL_APP_Context.LedControl.Led1;
  /* Update notification data length */
  security_peripheral_notification_data.Length = (security_peripheral_notification_data.Length) + 2; 
  
  if(SECURITY_PERIPHERAL_APP_Context.Switch_c_Notification_Status == Switch_c_NOTIFICATION_ON)
  { 
    APP_DBG_MSG("-- SECURITY APPLICATION PERIPHERAL : SWITCH CENTRAL LED ");
    if(SECURITY_PERIPHERAL_APP_Context.LedControl.Led1 == 0x00)
    {
      APP_DBG_MSG("OFF\n");;
    } 
    else
    {
      APP_DBG_MSG("ON\n");
    }
    notification_on_off = Switch_c_NOTIFICATION_ON;
  } 
  else
  {
    APP_DBG_MSG("-- SECURITY APPLICATION PERIPHERAL : CAN'T INFORM CLIENT - NOTIFICATION DISABLED\n"); 
    notification_on_off = Switch_c_NOTIFICATION_OFF;
    HAL_RADIO_TIMER_StopVirtualTimer(&SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_Id);
  }
  /* USER CODE END Service1Char2_NS_1*/

  if (notification_on_off != Switch_c_NOTIFICATION_OFF && SECURITY_PERIPHERAL_APP_Context.ConnectionHandle != 0xFFFF)
  {
    SECURITY_PERIPHERAL_NotifyValue(SECURITY_PERIPHERAL_SWITCH_C, &security_peripheral_notification_data, SECURITY_PERIPHERAL_APP_Context.ConnectionHandle);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last*/

  /* USER CODE END Service1Char2_NS_Last*/

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
  HAL_RADIO_TIMER_StartVirtualTimer(&(SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_Id), SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_interval);
}

/**
* @brief Handle the stop timer notification.
*/
__USED void SotpTimerNotification(void)
{ 
  /* Specific implementation for CFG_TASK_TIMER_STOP_SEND_NOTIF_ID */
  
  /* Stop the timer to send the notifications */
  HAL_RADIO_TIMER_StopVirtualTimer(&(SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_Id));
}

/**
* @brief Handle the slave security request task.
*/
__USED void Peripheral_Security_Request(void)
{
  /* Specific implementation for CFG_TASK_PERIPHERAL_SECURITY_REQUEST */
  uint8_t status = 0;
  status = aci_gap_set_security(SECURITY_PERIPHERAL_APP_Context.ConnectionHandle,
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
* @brief Start the timer to send the notification.
*/
__USED void Start_timer_send_notification(void)
{ 
  /* Start the timer to send the notification */
  HAL_RADIO_TIMER_StartVirtualTimer(&(SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_Id), SECURITY_PERIPHERAL_APP_Context.TimerSendNotification_interval);
  return;
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
