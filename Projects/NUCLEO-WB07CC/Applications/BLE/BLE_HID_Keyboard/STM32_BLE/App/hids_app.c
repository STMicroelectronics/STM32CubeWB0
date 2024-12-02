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

  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} HIDS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
typedef struct hidValueS {
  uint8_t key;
  uint8_t hid[2];
} hidValueType;

#define READ_DATA_SIZE      1024
#define READ_BUFFER_IS_EMPTY() (Read_ptr_in == Read_ptr_out)

#define NUM_0      0x30
#define NUM_9      0x39
#define CHAR_A     0x41
#define CHAR_Z     0x5A
#define CHAR_a     0x61
#define CHAR_z     0x7A
#define RETURN     0x0D
#define BACKSPACE  0x08
#define TAB        0x09
#define SPACE      0x20

#define KEY_TABLE_LEN 33

#define NUM_REPORTS 2

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

// Keyboard report descriptor
uint8_t reportDesc[] = {
        0x05, 0x01,                 // Usage Page (Generic Desktop)        
	0x09, 0x06,                 // Usage (Keyboard)        
	0xA1, 0x01,                 // Collection (Application)        
	0x05, 0x07,                 //     Usage Page (Key Codes)        
	0x19, 0xe0,                 //     Usage Minimum (224)        
	0x29, 0xe7,                 //     Usage Maximum (231)        
	0x15, 0x00,                 //     Logical Minimum (0)        
	0x25, 0x01,                 //     Logical Maximum (1)        
	0x75, 0x01,                 //     Report Size (1)        
	0x95, 0x08,                 //     Report Count (8)        
	0x81, 0x02,                 //     Input (Data, Variable, Absolute)            
	0x95, 0x01,                 //     Report Count (1)        
	0x75, 0x08,                 //     Report Size (8)        
	0x81, 0x01,                 //     Input (Constant) reserved byte(1)            
	0x95, 0x05,                 //     Report Count (5)        
	0x75, 0x01,                 //     Report Size (1)        
	0x05, 0x08,                 //     Usage Page (Page# for LEDs)        
	0x19, 0x01,                 //     Usage Minimum (1)        
	0x29, 0x05,                 //     Usage Maximum (5)        
	0x91, 0x02,                 //     Output (Data, Variable, Absolute), Led report        
	0x95, 0x01,                 //     Report Count (1)        
	0x75, 0x03,                 //     Report Size (3)        
	0x91, 0x01,                 //     Output (Data, Variable, Absolute), Led report padding
	0x95, 0x06,                 //     Report Count (6)        
	0x75, 0x08,                 //     Report Size (8)        
	0x15, 0x00,                 //     Logical Minimum (0)        
	0x25, 0x65,                 //     Logical Maximum (101)        
	0x05, 0x07,                 //     Usage Page (Key codes)        
	0x19, 0x00,                 //     Usage Minimum (0)        
	0x29, 0x65,                 //     Usage Maximum (101)        
	0x81, 0x00,                 //     Input (Data, Array) Key array(6 bytes)            
	0x09, 0x05,                 //     Usage (Vendor Defined)        
	0x15, 0x00,                 //     Logical Minimum (0)        
	0x26, 0xFF, 0x00,           //     Logical Maximum (255)        
	0x75, 0x08,                 //     Report Count (2)        
	0x95, 0x02,                 //     Report Size (8 bit)        
	0xB1, 0x02,                 //     Feature (Data, Variable, Absolute)        
	0xC0                        // End Collection (Application)    
};

hidValueType lookupTable[KEY_TABLE_LEN] = {
  {0x21, {TRUE,  0x1E}},
  {0x22, {TRUE,  0x34}},
  {0x23, {TRUE,  0x20}},
  {0x24, {TRUE,  0x21}},
  {0x25, {TRUE,  0x22}},
  {0x26, {TRUE,  0x24}},
  {0x27, {FALSE, 0x34}},
  {0x28, {TRUE,  0x26}},
  {0x29, {TRUE,  0x27}},
  {0x2A, {TRUE,  0x25}},
  {0x2B, {TRUE,  0x2E}},
  {0x2C, {FALSE, 0x36}},
  {0x2D, {FALSE, 0x2D}},
  {0x2E, {FALSE, 0x37}},
  {0x2F, {FALSE, 0x38}},
  {0x3A, {TRUE,  0x33}},
  {0x3B, {FALSE, 0x33}},
  {0x3C, {TRUE,  0x36}},
  {0x3D, {FALSE, 0x2E}},
  {0x3E, {TRUE,  0x37}},
  {0x3F, {TRUE,  0x38}},
  {0x40, {TRUE,  0x1F}},
  {0x5B, {FALSE, 0x2F}},
  {0x5C, {FALSE, 0x31}},
  {0x5D, {FALSE, 0x30}},
  {0x5E, {TRUE,  0x23}},
  {0x5F, {TRUE,  0x2D}},
  {0x60, {FALSE, 0x35}},
  {0x7B, {TRUE,  0x2F}},
  {0x7C, {TRUE,  0x31}},
  {0x7D, {TRUE,  0x30}},
  {0x7E, {TRUE,  0x35}},
  {0x7F, {FALSE, 0x4C}},
};

hidService_Type hid_param;
report_Type reportReferenceDesc[NUM_REPORTS];

extern devContext_Type devContext;

static uint8_t  Read_Buffer[READ_DATA_SIZE]; 
static uint32_t Read_ptr_in = 0;
static uint32_t Read_ptr_out = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void HIDS_Inputrep_SendNotification(void);

/* USER CODE BEGIN PFP */
static void Read_Buffer_Push(uint8_t byte);
static uint8_t Read_Buffer_Pop(uint8_t *byte);

static void HIDS_APP_Terminal_Init(void);
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

    case HIDS_OUTPUTREP_READ_EVT:
      /* USER CODE BEGIN Service1Char5_READ_EVT */

      /* USER CODE END Service1Char5_READ_EVT */
      break;

    case HIDS_OUTPUTREP_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char5_WRITE_NO_RESP_EVT */

      /* USER CODE END Service1Char5_WRITE_NO_RESP_EVT */
      break;

    case HIDS_OUTPUTREP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char5_WRITE_EVT */

      /* USER CODE END Service1Char5_WRITE_EVT */
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
  
  /* Register task to Process Device Input Data */
  UTIL_SEQ_RegTask( 1<< CFG_TASK_HID_DEVICE_INPUT_DATA, UTIL_SEQ_RFU, HIDS_APP_DeviceInputData);

  // ************ MAKE ALL INITIALIZATION ************** //
  
  /* Set the Keyboard Report Map */
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

  HIDS_APP_Terminal_Init();
  
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

uint8_t hidSendReport(uint8_t id, uint8_t type, uint8_t reportLen, uint8_t *reportData)
{
  uint8_t endReportList, ret, i;
  HIDS_Data_t ReportBuffer;

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

static uint8_t hid_keyboard_map(uint8_t charac, uint8_t *upperCase)
{
  uint8_t hidValue, i;

  hidValue = 0;
  *upperCase = FALSE;

  if ((charac >= NUM_0) && (charac <= NUM_9)) {
    hidValue = charac - 0x30;
    if (hidValue == 0)
      hidValue = 0x27;
    else
      hidValue += 0x1D;
  }
  
  if ((charac >= CHAR_A)  && (charac <= CHAR_Z)) {
    hidValue = charac - 0x41 + 0x04;
    *upperCase = TRUE;
  }

  if ((charac >= CHAR_a)  && (charac <= CHAR_z)) {
    hidValue = charac - 0x61 + 0x04;
  } else {
    for (i=0; i<KEY_TABLE_LEN; i++) {
      if (lookupTable[i].key == charac) {
        *upperCase = lookupTable[i].hid[0];
        hidValue = lookupTable[i].hid[1];
        break;
      }
    }
  }

  switch(charac) {
  case RETURN:
    hidValue = 0x28;
    break;
  case BACKSPACE:
    hidValue = 0x02A;
    break;
  case SPACE:
    hidValue = 0x2C;
    break;
  case TAB:
    hidValue = 0x2B;
    break;
  }

  return hidValue;
}

void processInputData(uint8_t* data_buffer, uint8_t Nb_bytes)
{
  uint8_t ret, i, upperCase, nmbTimes, keys[8]={0,0,0,0,0,0,0,0};
  
  if (devContext.deviceReadyToNotify)
  {
    BSP_LED_On(LED_RED);
    
    for (i=0; i<Nb_bytes; i++) {
      APP_DBG_MSG("%c", data_buffer[i]);
      
      keys[2] = hid_keyboard_map(data_buffer[i], &upperCase);
      if (upperCase)
	keys[0] = 0x02;
      else
	keys[0] = 0x00;
      ret = hidSendReport(0, INPUT_REPORT, sizeof(keys), keys);
      if (ret != BLE_STATUS_SUCCESS)
        APP_DBG_MSG("Error while sending the report 0x%02x\n", ret);
      keys[0] = 0;
      keys[2] = 0;
      nmbTimes = 0;   
      do {
	ret = hidSendReport(0, INPUT_REPORT, sizeof(keys), keys);
        nmbTimes++;
      } while ((ret == BLE_STATUS_INSUFFICIENT_RESOURCES) && (nmbTimes < 20));
    }
  } 
  else 
  {
    BSP_LED_Off(LED_RED);
  }
}

void setDefaultHidParams(void)
{
  hid_param.bootSupport = FALSE;
  hid_param.reportSupport = TRUE;
  hid_param.num_reports = NUM_REPORTS;
  hid_param.reportReferenceDesc = reportReferenceDesc;
  hid_param.reportReferenceDesc[0].ID = REPORT_ID;
  hid_param.reportReferenceDesc[0].type = INPUT_REPORT;
  hid_param.reportReferenceDesc[1].ID = REPORT_ID;
  hid_param.reportReferenceDesc[1].type = OUTPUT_REPORT;
  hid_param.isBootDevKeyboard = FALSE;
  hid_param.isBootDevMouse = FALSE;
  hid_param.externalReportEnabled = 0;
  hid_param.includedServiceEnabled = FALSE;
  hid_param.informationCharac[0] = 0x01;
  hid_param.informationCharac[1] = 0x01;
  hid_param.informationCharac[2] = 0;
  hid_param.informationCharac[3] = 0x01;
  
  hid_param.reportDescLen = sizeof(reportDesc);
  hid_param.reportDesc = reportDesc;
}

void HIDS_APP_DeviceInputData(void)
{
  uint8_t char_read;
  
  while (Read_Buffer_Pop(&char_read)) {
    processInputData(&char_read, 1);
  }
}

static uint8_t Read_Buffer_Pop(uint8_t *byte)
{
  if(READ_BUFFER_IS_EMPTY())
    return 0;
  
  *byte = Read_Buffer[Read_ptr_out];
  Read_ptr_out++;
  
  if(Read_ptr_out == READ_DATA_SIZE)
    Read_ptr_out = 0;  
  
  return 1;
}

/**
  * @brief  This function initialize terminal
  * @param  None
  * @retval None
  */
static void HIDS_APP_Terminal_Init(void)
{ 

  return;
}


void UartRxCpltCallback(uint8_t * pdata, uint16_t size)
{
  for(uint16_t i = 0; i < size; i++){
      Read_Buffer_Push(pdata[i]);
  }
  
  UTIL_SEQ_SetTask(1 << CFG_TASK_HID_DEVICE_INPUT_DATA, CFG_SEQ_PRIO_0);
}

static void Read_Buffer_Push(uint8_t byte)
{
  Read_Buffer[Read_ptr_in] = byte;
  
  Read_ptr_in++;
  
  /* To avoid buffer overflow */
  if(Read_ptr_in == READ_DATA_SIZE)
  {
    Read_ptr_in = 0;
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

/* USER CODE END FD_LOCAL_FUNCTIONS*/
