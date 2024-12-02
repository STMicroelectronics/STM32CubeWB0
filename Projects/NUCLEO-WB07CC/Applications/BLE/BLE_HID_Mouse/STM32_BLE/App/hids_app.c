/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HIDS_app.c
  * @author  MCD Application Team
  * @brief   HIDS_app application definition.
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
#include "hids_app.h"
#include "hids.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Inputrep_NOTIFICATION_OFF,
  Inputrep_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  HIDS_APP_SENDINFORMATION_LAST
} HIDS_APP_SendInformation_t;

typedef struct
{
  HIDS_APP_SendInformation_t     Inputrep_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  VTIMER_HandleType           TimerUpdate_Id;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} HIDS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define HOST_TO_LE_32(buf, val)    ( ((buf)[0] =  (uint8_t) (val)     ) , \
                                   ((buf)[1] =  (uint8_t) ((val)>>8)  ) , \
                                   ((buf)[2] =  (uint8_t) ((val)>>16) ) , \
                                   ((buf)[3] =  (uint8_t) ((val)>>24) ) )

#define BUTTON_LEFT_PRESSED        1<<0
#define BUTTON_RIGHT_PRESSED       1<<1
#define BUTTON_STATUS_CHANGED_MASK 1<<2

#define NUM_REPORTS 1

#define NEW_MOUSE_VALUE_TIMEOUT 1000    //Max time to have a new mouse data value, time expressed in MTU

#define NUM_MOUSE_COORD         4
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static HIDS_APP_Context_t HIDS_APP_Context;

uint8_t a_HIDS_UpdateCharData[247];

/* USER CODE BEGIN PV */

uint8_t reportDesc[] = {
  0x05, 0x01,        //USAGE_PAGE(Generic Desktop)
  0x09, 0x02,        //USAGE(Mouse)
  0xA1, 0x01,        //COLLECTION(Application)
  0x85, REPORT_ID,   // REPORT ID
  0x09, 0x01,        //USAGE(Pointer)
  0xA1, 0x00,        //COLLECTION(Physical)
  0x05, 0x09,        //USAGE_PAGE(Button)
  0x19, 0x01,        //USAGE_MINIMUM(Button 1)
  0x29, 0x03,        //USAGE_MAXIMUM(Button 3)
  0x15, 0x00,        //LOGICAL_MINIMUM(0)
  0x25, 0x01,        //LOGICAL_MAXIMUM(1)
  0x75, 0x01,        //REPORT_SIZE(1)
  0x95, 0x03,        //REPORT_COUNT(3)
  0x81, 0x02,        //INPUT(Data, Variable, Absolute) ; Button states
  0x75, 0x05,        //REPORT_SIZE(5)
  0x95, 0x01,        //REPORT_COUNT(1)
  0x81, 0x01,        //INPUT(Constant, Variable, Absolute) ; Reserved bits
  0x05, 0x01,        //USAGE_PAGE(Generic Desktop) 
  0x09, 0x30,        //USAGE(X)
  0x09, 0x31,        //USAGE(Y)
  0x16, 0x01, 0xF8,  //LOGICAL_MINIMUM(-2047)
  0x26, 0xFF, 0x07,  //LOGICAL_MAXIMUM(2047)
  0x75, 0x0C,        //REPORT_SIZE(12 bits)
  0x95, 0x02,        //REPORT_COUNT(2)
  0x81, 0x06,        //INPUT(Data, Variable, Relative) ; X & Y
  0x09, 0x38,        //USAGE(Z)
  0x15, 0x81,        //LOGICAL_MINIMUM(-127)
  0x25, 0x7F,        //LOGICAL_MAXIMUM(127)
  0x75, 0x08,        //REPORT_SIZE(8)
  0x95, 0x01,        //REPORT_COUNT(1)
  0x81, 0x06,        //INPUT(Data, Variable, Relative) ; Z
  0xC0,              //END_COLLECTION
  0xC0               //END_COLLECTION
};

uint8_t rightButtonPressed, leftButtonPressed;

hidService_Type hid_param;
report_Type reportReferenceDesc[NUM_REPORTS];


HIDS_Coordinates_t mouse_coord[] = {
  {50,0},
  {0, 50},
  {-50, 0},
  {0, -50},  
};

uint8_t index_mouse_coord = 0;

extern devContext_Type devContext;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void HIDS_Inputrep_SendNotification(void);

/* USER CODE BEGIN PFP */
static void HIDS_APP_Update_Cb(void *args);
static void HIDS_APP_DeviceInputData(void);

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void HIDS_Notification(HIDS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case HIDS_HII_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case HIDS_HCP_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char2_WRITE_NO_RESP_EVT */
      if(((p_Notification->DataTransfered.p_Payload[0]) & 0x01) == 0x00)
      {
        APP_DBG_MSG("Suspend\r\n");
      }
      else
      {
        APP_DBG_MSG("Exit suspend\r\n");
      }
      /* USER CODE END Service1Char2_WRITE_NO_RESP_EVT */
      break;

    case HIDS_REM_READ_EVT:
      /* USER CODE BEGIN Service1Char3_READ_EVT */

      /* USER CODE END Service1Char3_READ_EVT */
      break;

    case HIDS_INPUTREP_READ_EVT:
      /* USER CODE BEGIN Service1Char4_READ_EVT */

      /* USER CODE END Service1Char4_READ_EVT */
      break;

    case HIDS_INPUTREP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char4_WRITE_EVT */

      /* USER CODE END Service1Char4_WRITE_EVT */
      break;

    case HIDS_INPUTREP_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char4_NOTIFY_ENABLED_EVT */
      APP_DBG_MSG("HIDS_INPUTREP_NOTIFY_ENABLED_EVT\n");
      HIDS_APP_Context.Inputrep_Notification_Status = Inputrep_NOTIFICATION_ON;
      /* USER CODE END Service1Char4_NOTIFY_ENABLED_EVT */
      break;

    case HIDS_INPUTREP_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char4_NOTIFY_DISABLED_EVT */
      APP_DBG_MSG("HIDS_INPUTREP_NOTIFY_DISABLED_EVT\n");
      HIDS_APP_Context.Inputrep_Notification_Status = Inputrep_NOTIFICATION_OFF;
      /* USER CODE END Service1Char4_NOTIFY_DISABLED_EVT */
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

void HIDS_APP_EvtRx(HIDS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case HIDS_CONN_HANDLE_EVT :
      HIDS_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;
      /* USER CODE BEGIN Service1_APP_CENTR_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CENTR_CONN_HANDLE_EVT */
      break;
    case HIDS_DISCON_HANDLE_EVT :
      HIDS_APP_Context.ConnectionHandle = 0xFFFF;
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      HAL_RADIO_TIMER_StopVirtualTimer(&(HIDS_APP_Context.TimerUpdate_Id));
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

void HIDS_APP_Init(void)
{
  HIDS_APP_Context.ConnectionHandle = 0xFFFF;
  HIDS_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
    
  UNUSED(HIDS_APP_Context);
  
  HIDS_Data_t msg_conf;
  
  /* Timer for state sampling */
  HIDS_APP_Context.TimerUpdate_Id.callback = HIDS_APP_Update_Cb;
   
  /* Register task to Process Device Input Data */
  UTIL_SEQ_RegTask( 1<< CFG_TASK_HID_DEVICE_INPUT_DATA, UTIL_SEQ_RFU, HIDS_APP_DeviceInputData);
  
  // ************ MAKE ALL INITIALIZATION ************** //
  
  /* Set the Mouse Report Map */
  memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
  memcpy((void*)a_HIDS_UpdateCharData, (void *)&reportDesc, sizeof(reportDesc));
  msg_conf.p_Payload = a_HIDS_UpdateCharData;
  msg_conf.Length = sizeof(reportDesc);
  result = HIDS_UpdateValue(HIDS_REM, &msg_conf);
  if( result != BLE_STATUS_SUCCESS )
  {
    APP_DBG_MSG("Sending of Report Map Failed error 0x%X\n", result);
  }
  
  /* Set the HID Information */
  memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
  memcpy((void*)a_HIDS_UpdateCharData, (void *)&hid_param.informationCharac, sizeof(hid_param.informationCharac));
  msg_conf.p_Payload = a_HIDS_UpdateCharData;
  msg_conf.Length = sizeof(hid_param.informationCharac);
  result = HIDS_UpdateValue(HIDS_HII, &msg_conf);
  if( result != BLE_STATUS_SUCCESS )
  {
    APP_DBG_MSG("Sending of HID Information Failed error 0x%X\n", result);
  }
  
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void startNotifyTimer(void)
{
  HAL_RADIO_TIMER_StartVirtualTimer(&(HIDS_APP_Context.TimerUpdate_Id), NEW_MOUSE_VALUE_TIMEOUT);
}

uint8_t hidSendReport(uint8_t id, uint8_t type, uint8_t reportLen, uint8_t *reportData)
{
  uint8_t endReportList, ret, i;
  HIDS_Data_t ReportBuffer;
  
  APP_DBG_MSG("Report Data: ");
  for(i = 0; i < reportLen - 1; i++)
  {
    APP_DBG_MSG("%0x - ", reportData[i]);
  }
  APP_DBG_MSG("%0x\n", reportData[reportLen - 1]);

  ReportBuffer.p_Payload = reportData;
  ReportBuffer.Length = reportLen;
  
  endReportList = FALSE;
  i = 0;
  while(!endReportList)
  {    
    if((id == devContext.hidReportID[i]) &&
       (devContext.hidReportType[i] == INPUT_REPORT))
    {
      ret = HIDS_NotifyValue(HIDS_INPUTREP, &ReportBuffer, HIDS_APP_Context.ConnectionHandle);  
      return ret;
    }
    i++;    
    if(i == devContext.num_reports)
    {
      endReportList = TRUE;
    }
  }
  return BLE_STATUS_INVALID_PARAMS;
}


void setDefaultHidParams(void)
{
  hid_param.bootSupport = FALSE;
  hid_param.reportSupport = TRUE;
  hid_param.num_reports = NUM_REPORTS;
  hid_param.reportReferenceDesc = reportReferenceDesc;
  hid_param.reportReferenceDesc[0].ID = REPORT_ID;
  hid_param.reportReferenceDesc[0].type = INPUT_REPORT;
  hid_param.isBootDevKeyboard = FALSE;
  hid_param.isBootDevMouse = FALSE;
  hid_param.externalReportEnabled = 0;
  hid_param.includedServiceEnabled = FALSE;
  hid_param.informationCharac[0] = 0x01;
  hid_param.informationCharac[1] = 0x01;
  hid_param.informationCharac[2] = 0;
  hid_param.informationCharac[3] = 0x03;
  
  hid_param.reportDescLen = sizeof(reportDesc);
  hid_param.reportDesc = reportDesc;
}


static void getButtonStatus(uint8_t *buttonMask)
{
  static uint8_t lastButtonStatus=0;

  *buttonMask = 0;

  if ((BSP_PB_GetState(B1) == SET) || leftButtonPressed) {
    *buttonMask |= BUTTON_LEFT_PRESSED;
    leftButtonPressed = FALSE;
  }

  if ((BSP_PB_GetState(B2) == SET) || rightButtonPressed) {
    *buttonMask |= BUTTON_RIGHT_PRESSED;
    rightButtonPressed = FALSE;
  } 

  if (lastButtonStatus != *buttonMask) {
    lastButtonStatus = *buttonMask;
    *buttonMask |= BUTTON_STATUS_CHANGED_MASK;
  }
}

static void newMouseValue(int16_t *x, int16_t *y)
{
  *x = 0;
  *y = 0;
  
  *x = mouse_coord[index_mouse_coord].x;
  *y = mouse_coord[index_mouse_coord].y;
  index_mouse_coord++;
  if (index_mouse_coord == NUM_MOUSE_COORD)
    index_mouse_coord = 0;  
}

void HIDS_APP_DeviceInputData(void)
{
  int16_t x, y;
  uint8_t ret, buttonMask, buff[5];

  if (devContext.deviceReadyToNotify){  
    buttonMask = 0;
    x = 0;
    y = 0;
    /* Read the button Status */
    getButtonStatus(&buttonMask);
    
    if (!(buttonMask & BUTTON_STATUS_CHANGED_MASK)) {
      newMouseValue(&x, &y);   
    }
    
    if ((x != 0) || (y != 0) || (buttonMask != 0)) {
      buttonMask &= ~(uint8_t)(BUTTON_STATUS_CHANGED_MASK); // Flag to be cleared before to send the packet
      buff[0] = buttonMask;
      HOST_TO_LE_32(&buff[1], ((uint32_t)(x&0x0FFF)|(uint32_t)(y&0x0FFF)<<12));
      ret = hidSendReport(REPORT_ID, INPUT_REPORT, sizeof(buff), buff);

      if (ret != BLE_STATUS_SUCCESS && ret != BLE_STATUS_INSUFFICIENT_RESOURCES)
        APP_DBG_MSG("Error while sending the hid report. (error = 0x%02x)\n", ret) ;
    }
    else {
      BSP_LED_Off(LED_RED);
    }
  }
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void HIDS_Inputrep_SendNotification(void) /* Property Notification */
{
  HIDS_APP_SendInformation_t notification_on_off = Inputrep_NOTIFICATION_OFF;
  HIDS_Data_t hids_notification_data;

  hids_notification_data.p_Payload = (uint8_t*)a_HIDS_UpdateCharData;
  hids_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char4_NS_1*/

  /* USER CODE END Service1Char4_NS_1*/

  if (notification_on_off != Inputrep_NOTIFICATION_OFF && HIDS_APP_Context.ConnectionHandle != 0xFFFF)
  {
    HIDS_NotifyValue(HIDS_INPUTREP, &hids_notification_data, HIDS_APP_Context.ConnectionHandle);
  }

  /* USER CODE BEGIN Service1Char4_NS_Last*/

  /* USER CODE END Service1Char4_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
/**
 * @brief  Timer callback of the Jostick state sampling  
 * @param  void *args arguments
 * @retval None
 */
static void HIDS_APP_Update_Cb(void *args)
{
  HAL_RADIO_TIMER_StopVirtualTimer(&(HIDS_APP_Context.TimerUpdate_Id));
  UTIL_SEQ_SetTask(1<<CFG_TASK_HID_DEVICE_INPUT_DATA, CFG_SEQ_PRIO_0);
  startNotifyTimer();
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
