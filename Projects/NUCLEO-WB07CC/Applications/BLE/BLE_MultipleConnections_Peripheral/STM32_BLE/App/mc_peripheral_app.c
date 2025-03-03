/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    MC_Peripheral_app.c
  * @author  MCD Application Team
  * @brief   MC_Peripheral_app application definition.
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
#include "mc_peripheral_app.h"
#include "mc_peripheral.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
 typedef struct{
    uint8_t             Device_Led_Selection;
    uint8_t             Led1;
 }P2P_LedCharValue_t;

 typedef struct{
    uint8_t             Device_Button_Selection;
    uint8_t             ButtonStatus;
 }P2P_ButtonCharValue_t;
/* USER CODE END PTD */

typedef enum
{
  Switch_c_NOTIFICATION_OFF,
  Switch_c_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  MC_PERIPHERAL_APP_SENDINFORMATION_LAST
} MC_PERIPHERAL_APP_SendInformation_t;

typedef struct
{
  MC_PERIPHERAL_APP_SendInformation_t     Switch_c_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  P2P_LedCharValue_t              LedControl;
  P2P_ButtonCharValue_t           ButtonControl;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} MC_PERIPHERAL_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_NUM_BONDED_DEVICES 1

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static MC_PERIPHERAL_APP_Context_t MC_PERIPHERAL_APP_Context;

uint8_t a_MC_PERIPHERAL_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MC_PERIPHERAL_Switch_c_SendNotification(void);

/* USER CODE BEGIN PFP */
static void MC_PERIPHERAL_APP_LED_BUTTON_context_Init(void);

__USED void Peripheral_Security_Request(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void MC_PERIPHERAL_Notification(MC_PERIPHERAL_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case MC_PERIPHERAL_LED_C_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case MC_PERIPHERAL_LED_C_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */
      if(p_Notification->DataTransfered.p_Payload[1] == 0x01)
      {
        BSP_LED_On(LED_BLUE);
        APP_DBG_MSG("-- APPLICATION SERVER : LED1 ON\n"); 
        MC_PERIPHERAL_APP_Context.LedControl.Led1 = 0x01; /* LED1 ON */
      }
      if(p_Notification->DataTransfered.p_Payload[1] == 0x00)
      {
        BSP_LED_Off(LED_BLUE);
        APP_DBG_MSG("-- APPLICATION SERVER : LED1 OFF\n"); 
        MC_PERIPHERAL_APP_Context.LedControl.Led1 = 0x00; /* LED1 OFF */
      }
      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case MC_PERIPHERAL_SWITCH_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
      MC_PERIPHERAL_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_ON;
      APP_DBG_MSG("-- APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n");
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case MC_PERIPHERAL_SWITCH_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
      MC_PERIPHERAL_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_OFF;
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

void MC_PERIPHERAL_APP_EvtRx(MC_PERIPHERAL_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case MC_PERIPHERAL_CONN_HANDLE_EVT :
      MC_PERIPHERAL_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;
      /* USER CODE BEGIN Service1_APP_CENTR_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CENTR_CONN_HANDLE_EVT */
      break;
    case MC_PERIPHERAL_DISCON_HANDLE_EVT :
      MC_PERIPHERAL_APP_Context.ConnectionHandle = 0xFFFF;
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      MC_PERIPHERAL_APP_LED_BUTTON_context_Init();
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

void MC_PERIPHERAL_APP_Init(void)
{
  MC_PERIPHERAL_APP_Context.ConnectionHandle = 0xFFFF;
  MC_PERIPHERAL_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  UTIL_SEQ_RegTask(1U << CFG_TASK_PERIPHERAL_SECURITY_REQUEST, UTIL_SEQ_RFU, Peripheral_Security_Request);
  UTIL_SEQ_RegTask(1U << CFG_TASK_SEND_NOTIF_ID, UTIL_SEQ_RFU, MC_PERIPHERAL_Switch_c_SendNotification);

  /**
   * Initialize LedButton Service
   */
  MC_PERIPHERAL_APP_Context.Switch_c_Notification_Status= Switch_c_NOTIFICATION_OFF;
  MC_PERIPHERAL_APP_LED_BUTTON_context_Init();
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void MC_PERIPHERAL_APP_LED_BUTTON_context_Init(void)
{  
  BSP_LED_Off(LED_BLUE);
  MC_PERIPHERAL_APP_Context.LedControl.Device_Led_Selection=0x01; /* Device1 */
  MC_PERIPHERAL_APP_Context.LedControl.Led1=0x00; /* led OFF */
  MC_PERIPHERAL_APP_Context.ButtonControl.Device_Button_Selection=0x01;/* Device1 */
  MC_PERIPHERAL_APP_Context.ButtonControl.ButtonStatus=0x00;

  return;
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void MC_PERIPHERAL_Switch_c_SendNotification(void) /* Property Notification */
{
  MC_PERIPHERAL_APP_SendInformation_t notification_on_off = Switch_c_NOTIFICATION_OFF;
  MC_PERIPHERAL_Data_t mc_peripheral_notification_data;

  mc_peripheral_notification_data.p_Payload = (uint8_t*)a_MC_PERIPHERAL_UpdateCharData;
  mc_peripheral_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1*/

  if(MC_PERIPHERAL_APP_Context.ButtonControl.ButtonStatus == 0x00)
  {
    MC_PERIPHERAL_APP_Context.ButtonControl.ButtonStatus = 0x01;
  } 
  else
  {
    MC_PERIPHERAL_APP_Context.ButtonControl.ButtonStatus = 0x00;
  }
  a_MC_PERIPHERAL_UpdateCharData[0] = 0x01; /* Device Led selection */
  a_MC_PERIPHERAL_UpdateCharData[1] = MC_PERIPHERAL_APP_Context.ButtonControl.ButtonStatus;
  /* Update notification data length */
  mc_peripheral_notification_data.Length = (mc_peripheral_notification_data.Length) + 2; 
  
  if(MC_PERIPHERAL_APP_Context.Switch_c_Notification_Status == Switch_c_NOTIFICATION_ON)
  { 
    APP_DBG_MSG("-- APPLICATION SERVER : INFORM CLIENT BUTTON 1 PUSHED\n");
    notification_on_off = Switch_c_NOTIFICATION_ON;
  } 
  else
  {
    APP_DBG_MSG("-- APPLICATION SERVER : CAN'T INFORM CLIENT - NOTIFICATION DISABLED\n"); 
  }
  /* USER CODE END Service1Char2_NS_1*/

  if (notification_on_off != Switch_c_NOTIFICATION_OFF && MC_PERIPHERAL_APP_Context.ConnectionHandle != 0xFFFF)
  {
    MC_PERIPHERAL_NotifyValue(MC_PERIPHERAL_SWITCH_C, &mc_peripheral_notification_data, MC_PERIPHERAL_APP_Context.ConnectionHandle);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last*/

  /* USER CODE END Service1Char2_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
/**
* @brief Handle the security request task.
*/
__USED void Peripheral_Security_Request(void)
{
  /* Specific implementation for CFG_TASK_PERIPHERAL_SECURITY_REQUEST */
  uint8_t status = 0;

  /**  
   * @note BLE Security Levels and Dependencies:
   *       - GAP_SECURITY_LEVEL_1: No security (no authentication or encryption).
   *          - No MITM protection required.
   *          - No dependency on IO_CAPABILITY.
   *       - GAP_SECURITY_LEVEL_2: Unauthenticated pairing with encryption.
   *          - No MITM protection required.
   *          - No dependency on IO_CAPABILITY.
   *       - GAP_SECURITY_LEVEL_3: Authenticated pairing with encryption.
   *          - MITM protection required.
   *          - Depends on IO_CAPABILITY for pairing method (e.g., Passkey Entry, Numeric Comparison).
   *       - GAP_SECURITY_LEVEL_4: Authenticated LE Secure Connections pairing with encryption using a 128-bit strength encryption key.
   *          - MITM protection required.
   *          - Depends on IO_CAPABILITY for pairing method (e.g., Numeric Comparison, Passkey Entry).
   */  
  
 
    status = aci_gap_set_security(MC_PERIPHERAL_APP_Context.ConnectionHandle,
                            GAP_SECURITY_LEVEL_2,
                            0x01);
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : [ConnectionHandle: 0x%04X] Pairing request 0x%X\n", MC_PERIPHERAL_APP_Context.ConnectionHandle, status);
    }
    else
    {
      APP_DBG_MSG("  Success: [ConnectionHandle: 0x%04X] Pairing request \n", MC_PERIPHERAL_APP_Context.ConnectionHandle);
      BLEStack_Process_Schedule();
    }

    
  return;
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
