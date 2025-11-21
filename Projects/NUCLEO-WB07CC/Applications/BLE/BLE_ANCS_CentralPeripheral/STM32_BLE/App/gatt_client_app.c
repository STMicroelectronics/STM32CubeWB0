/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gatt_client_app.c
  * @author  MCD Application Team
  * @brief   GATT Client Application
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
#include "ble.h"
#include "gatt_client_app.h"
#include "stm32_seq.h"
#include "app_ble.h"
#include "ble_evt.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

typedef enum {
    EventIDNotificationAdded = 0,
    EventIDNotificationModified = 1,
    EventIDNotificationRemoved = 2,
    ReservedEventID = 3 // Reserved values from 3 to 255
} EventID;

typedef enum {
    EventFlagSilent = (1 << 0),
    EventFlagImportant = (1 << 1),
    EventFlagPreExisting = (1 << 2),
    EventFlagPositiveAction = (1 << 3),
    EventFlagNegativeAction = (1 << 4),
    ReservedEventFlags = (1 << 5) // Reserved values from (1 << 5) to (1 << 7)
} EventFlags;

typedef enum {
    CommandIDGetNotificationAttributes = 0,
    CommandIDGetAppAttributes = 1,
    CommandIDPerformNotificationAction = 2,
    ReservedCommandID = 3 // Reserved values from 3 to 255
} CommandID;

typedef enum {
    CategoryIDOther = 0,
    CategoryIDIncomingCall = 1,
    CategoryIDMissedCall = 2,
    CategoryIDVoicemail = 3,
    CategoryIDSocial = 4,
    CategoryIDSchedule = 5,
    CategoryIDEmail = 6,
    CategoryIDNews = 7,
    CategoryIDHealthAndFitness = 8,
    CategoryIDBusinessAndFinance = 9,
    CategoryIDLocation = 10,
    CategoryIDEntertainment = 11,
    ReservedCategoryID = 12 // Reserved values from 12 to 255
} CategoryID;

typedef enum {
    NotificationAttributeIDAppIdentifier = 0,
    NotificationAttributeIDTitle = 1, // Needs to be followed by a 2-bytes max length parameter
    NotificationAttributeIDSubtitle = 2, // Needs to be followed by a 2-bytes max length parameter
    NotificationAttributeIDMessage = 3, // Needs to be followed by a 2-bytes max length parameter
    NotificationAttributeIDMessageSize = 4,
    NotificationAttributeIDDate = 5,
    NotificationAttributeIDPositiveActionLabel = 6,
    NotificationAttributeIDNegativeActionLabel = 7,
    ReservedNotificationAttributeID = 8 // Reserved values from 8 to 255
} NotificationAttributeID;

typedef enum {
    ActionIDPositive = 0,
    ActionIDNegative = 1,
    ReservedActionID = 2 // Reserved values from 2 to 255
} ActionID;

typedef enum {
    AppAttributeIDDisplayName = 0,
    ReservedAppAttributeID = 1 // Reserved values from 1 to 255
} AppAttributeID;

typedef enum {
  UNUSED = 0,
  USED = 1
} UsedStatus;

typedef enum {
  EMPTY = 0,
  ADDED,
  GET_NOTIFICATION_ATTRIBUTES,
  TO_UPDATE
} RequestStatus;

/* USER CODE END PTD */

typedef enum
{
  NOTIFICATION_INFO_RECEIVED_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Opcode_t */
  ANCS_NOTIFICATION_INFO_RECEIVED_EVT,
  ANCS_GET_NOTIFICATION_ATTRIBUTES_RECEIVED_EVT,
  /* USER CODE END GATT_CLIENT_APP_Opcode_t */
}GATT_CLIENT_APP_Opcode_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t length;
}GATT_CLIENT_APP_Data_t;

typedef struct
{
  GATT_CLIENT_APP_Opcode_t Client_Evt_Opcode;
  GATT_CLIENT_APP_Data_t   DataTransfered;
}GATT_CLIENT_APP_Notification_evt_t;

typedef struct
{
  GATT_CLIENT_APP_State_t state;

  uint16_t connHdl;

  uint16_t ALLServiceStartHdl;
  uint16_t ALLServiceEndHdl;

  uint16_t GAPServiceStartHdl;
  uint16_t GAPServiceEndHdl;

  uint16_t GATTServiceStartHdl;
  uint16_t GATTServiceEndHdl;

  uint16_t ServiceChangedCharStartHdl;
  uint16_t ServiceChangedCharValueHdl;
  uint16_t ServiceChangedCharDescHdl;
  uint16_t ServiceChangedCharEndHdl;

  /* USER CODE BEGIN BleClientAppContext_t */
  
  APP_BLE_ConnStatus_t connStatus;

  /* Handles of ANCS service */
  uint16_t ANCSServiceHdl;
  uint16_t ANCSServiceEndHdl;

  /* Handles of the Notification Source characteristic */
  uint16_t ANCSNotificationSourceCharHdl;
  uint16_t ANCSNotificationSourceValueHdl;
  uint16_t ANCSNotificationSourceCCCDHdl;

  /* Handles of the Control Point characteristic */
  uint16_t ANCSControlPointCharHdl;
  uint16_t ANCSControlPointValueHdl;

  /* Handles of the Control Point Extended characteristic */
  uint16_t ANCSControlPointExtendedCharHdl;
  uint16_t ANCSControlPointExtendedValueHdl;

  /* Handles of the Data Source characteristic */
  uint16_t ANCSDataSourceCharHdl;
  uint16_t ANCSDataSourceValueHdl;
  uint16_t ANCSDataSourceCCCDHdl;

  uint8_t ANCSerror_proc;
  uint8_t ANCSservicePresent;

/* USER CODE END BleClientAppContext_t */

}BleClientAppContext_t;

typedef enum
{
  PROC_GATT_DISC_ALL_PRIMARY_SERVICES,
  PROC_GATT_DISC_ALL_CHARS,
  PROC_GATT_DISC_ALL_DESCS,
  PROC_GATT_ENABLE_ALL_NOTIFICATIONS,
  /* USER CODE BEGIN ProcGattId_t*/

  /* USER CODE END ProcGattId_t */
}ProcGattId_t;

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ANCS_UUID                0x79, 0x05, 0xF4, 0x31, 0xB5, 0xCE, 0x4E, 0x99, 0xA4, 0x0F, 0x4B, 0x1E, 0x12, 0x2D, 0x00, 0xD0
#define NOTIFICATION_CHAR_UUID   0x9F, 0xBF, 0x12, 0x0D, 0x63, 0x01, 0x42, 0xD9, 0x8C, 0x58, 0x25, 0xE6, 0x99, 0xA2, 0x1D, 0xBD
#define CONTROL_POINT_CHAR_UUID  0x69, 0xD1, 0xD8, 0xF3, 0x45, 0xE1, 0x49, 0xA8, 0x98, 0x21, 0x9B, 0xBD, 0xFD, 0xAA, 0xD9, 0xD9
#define DATA_SOURCE_CHAR_UUID    0x22, 0xEA, 0xC6, 0xE9, 0x24, 0xD6, 0x4B, 0xB5, 0xBE, 0x44, 0xB3, 0x6A, 0xCE, 0x7C, 0x7B, 0xFB

/* Implementation define */
#define MIN_DATA_BUFFER_SIZE 20
#define NOTIFY_SOURCE_DATA_LEN 8

#define ERROR_ANCS_NOT_PRESENT                   0x01
#define ERROR_ANCS_CONSUMER_ALREADY_SET          0x02
#define ERROR_MORE_THAN_ONE_ANCS_PRESENT         0x03
#define ERROR_WRONG_CONN_HANDLE                  0x04
#define ERROR_WRONG_CHARAC_PROP                  0x05
#define ERROR_ANCS_DISCOVERY_SERVICE             0x06
#define ERROR_ANCS_DISCOVERY_CHARACTERISTICS     0x07
#define ERROR_ANCS_DISCOVERY_EXTENDED_PROP       0x08
#define ERROR_ANCS_READ_PROC                     0x09
#define ERROR_ANCS_WRITE_PROC                    0x0A
#define ERROR_ANCS_DISCOVERY_CHARACTERISTIC_DESC 0x0B
#define ERROR_ANCS_BUSY                          0x0C
#define ERROR_INSUFFICIENT_MEMORY_SIZE           0x0D
#define ERROR_WRONG_NOTIFICATION_LENGTH          0x0E

#define COMMAND_DATA_SIZE                        (14U)
#define ANCS_APP_IDENTIFIER                     (100U)
#define ANCS_TITLE_MAX_LENGTH                    (50U)
#define ANCS_SUBTITLE_MAX_LENGTH                 (30U)
#define ANCS_MESSAGE_MAX_LENGTH                 (300U)
/* ANCS_MESSAGE_MAX_LENGTH is 300 characters. This value in hexadecimal is equal to 0x012C, the value to write in the notification command data */
#define ANCS_MESSAGE_MAX_LENGTH_LSB               0x2C
#define ANCS_MESSAGE_MAX_LENGTH_MSB               0x01
#define MAX_ANCS_NOTIFICATION                    (10U)

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))
/* USER CODE BEGIN PM */
#define UNPACK_4_BYTE_PARAMETER(ptr) \
    (uint32_t)((uint32_t)(ptr[0]) | \
    ((uint32_t)(ptr[1]) << 8) | \
    ((uint32_t)(ptr[2]) << 16) | \
    ((uint32_t)(ptr[3]) << 24))

typedef struct NotificationAttributeDataS
{
  uint8_t appIdentifier[ANCS_APP_IDENTIFIER];
  uint8_t idTitle[ANCS_TITLE_MAX_LENGTH];
  uint8_t idSubtitle[ANCS_SUBTITLE_MAX_LENGTH];
  uint8_t idMessage[ANCS_MESSAGE_MAX_LENGTH];
  uint8_t idMessageSize[2];
  uint8_t idDate[15];
  uint8_t actionLabel;
  uint8_t idNegativeActionLabel[10];
  uint8_t idPositiveActionLabel[10];
} NotificationAttributeData_type;

typedef struct ANCS_NotificationListS
{
  RequestStatus status;
  uint8_t used;
  EventID evID;
  EventFlags evFlag;
  CategoryID catID;
  uint8_t catCount;
  uint32_t notifUID;
  NotificationAttributeData_type notifData;
} ANCS_NotificationList_type;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static BleClientAppContext_t a_ClientContext[CFG_BLE_NUM_CLIENT_CONTEXTS];

/* USER CODE BEGIN PV */
uint8_t ancs_service_uuid[16] = {ANCS_UUID};
uint8_t notification_char_uuid[16] = {NOTIFICATION_CHAR_UUID};
uint8_t control_point_char_uuid[16] = {CONTROL_POINT_CHAR_UUID};
uint8_t data_source_char_uuid[16] = {DATA_SOURCE_CHAR_UUID};

ANCS_NotificationList_type ancs_notification_list[MAX_ANCS_NOTIFICATION];

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/

static BLEEVT_EvtAckStatus_t P2P_ROUTER_EventHandler(aci_blecore_event *p_evt);
static void gatt_parse_services(aci_att_clt_read_by_group_type_resp_event_rp0 *p_evt);
static void gatt_parse_services_by_UUID(aci_att_clt_find_by_type_value_resp_event_rp0 *p_evt);
static void gatt_parse_chars(aci_att_clt_read_by_type_resp_event_rp0 *p_evt);
static void gatt_parse_descs(aci_att_clt_find_info_resp_event_rp0 *p_evt);
static void gatt_parse_notification(aci_gatt_clt_notification_event_rp0 *p_evt);
static void gatt_Notification(GATT_CLIENT_APP_Notification_evt_t *p_Notif);
static void client_discover_all(void);
static void gatt_cmd_resp_release(void);
static void gatt_cmd_resp_wait(void);
static uint8_t gatt_procedure(uint8_t index, ProcGattId_t GattProcId);
/* USER CODE BEGIN PFP */

static void client_discover_ancs(void);
void GATT_Button1Action(void);
void GATT_Button2Action(void);
void GATT_Button3Action(void);
static void notification_ancs(void);
void aci_att_clt_find_by_type_value_resp_event(aci_att_clt_find_by_type_value_resp_event_rp0 *p_event);
void aci_gatt_clt_disc_read_char_by_uuid_resp_event(aci_gatt_clt_disc_read_char_by_uuid_resp_event_rp0 *p_event);
void aci_att_clt_find_info_resp_event(aci_att_clt_find_info_resp_event_rp0 *p_event);
void aci_att_clt_read_resp_event(aci_att_clt_read_resp_event_rp0 *p_event);
void aci_gatt_clt_indication_event(aci_gatt_clt_indication_event_rp0 *p_event);
void gatt_ShowANCSPacket(ANCS_NotificationList_type *notification);
void printANCSNotification(ANCS_NotificationList_type *notification);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void GATT_CLIENT_APP_Init(void)
{
  int ret;

  /* USER CODE BEGIN GATT_CLIENT_APP_Init_1 */
  uint8_t index = 0;
  
  for (index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    a_ClientContext[0].ANCSservicePresent = FALSE;

    /* Handles of ANCS service */
    a_ClientContext[index].ANCSServiceHdl = 0;
    a_ClientContext[index].ANCSServiceEndHdl = 0;
    /* Handles of the Notification Source characteristic */
    a_ClientContext[index].ANCSNotificationSourceCharHdl = 0;
    a_ClientContext[index].ANCSNotificationSourceValueHdl = 0;
    a_ClientContext[index].ANCSNotificationSourceCCCDHdl = 0;
    /* Handles of the Control Point characteristic */
    a_ClientContext[index].ANCSControlPointCharHdl = 0;
    a_ClientContext[index].ANCSControlPointValueHdl = 0;
    /* Handles of the Control Point Extended characteristic */
    a_ClientContext[index].ANCSControlPointExtendedCharHdl = 0;
    a_ClientContext[index].ANCSControlPointExtendedValueHdl = 0;
    /* Handles of the Data Source characteristic */
    a_ClientContext[index].ANCSDataSourceCharHdl = 0;
    a_ClientContext[index].ANCSDataSourceValueHdl = 0;
    a_ClientContext[index].ANCSDataSourceCCCDHdl = 0;

    a_ClientContext[index].ANCSerror_proc = 0;
    a_ClientContext[index].ANCSservicePresent = 0;
  }

  /* To reset the ANCS structure, it is only required to set the variable that stores all notification data as UNUSED */
  for (index = 0; index < MAX_ANCS_NOTIFICATION; index++)
  {
    ancs_notification_list[index].used = UNUSED;
  }

  /* USER CODE END GATT_CLIENT_APP_Init_1 */

  for(uint8_t index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    a_ClientContext[index].connHdl = 0xFFFF;
    a_ClientContext[index].state = GATT_CLIENT_APP_DISCONNECTED;
  }

  /**
   *  Register the event handler to the BLE controller
   */
  ret = BLEEVT_RegisterGattEvtHandler(P2P_ROUTER_EventHandler);
  if (ret != 0)
  {
    Error_Handler();
  }
  /* Register a task allowing to discover all services and characteristics and enable all notifications */
  UTIL_SEQ_RegTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, UTIL_SEQ_RFU, client_discover_all);

  /* USER CODE BEGIN GATT_CLIENT_APP_Init_2 */

  UTIL_SEQ_RegTask(1U << CFG_TASK_CLIENT_DISCOVER_ANCS, UTIL_SEQ_RFU, client_discover_ancs);

  UTIL_SEQ_RegTask(1U << CFG_TASK_GATT_CLIENT_BUTTON_1, UTIL_SEQ_RFU, GATT_Button1Action);
  UTIL_SEQ_RegTask(1U << CFG_TASK_GATT_CLIENT_BUTTON_2, UTIL_SEQ_RFU, GATT_Button2Action);
  UTIL_SEQ_RegTask(1U << CFG_TASK_GATT_CLIENT_BUTTON_3, UTIL_SEQ_RFU, GATT_Button3Action);
  UTIL_SEQ_RegTask(1U << CFG_TASK_NOTIFICATION_ANCS, UTIL_SEQ_RFU, notification_ancs);
  

  /* USER CODE END GATT_CLIENT_APP_Init_2 */
  return;
}

void GATT_CLIENT_APP_Notification(GATT_CLIENT_APP_ConnHandle_Notif_evt_t *p_Notif)
{
  /* USER CODE BEGIN GATT_CLIENT_APP_Notification_1 */

  /* USER CODE END GATT_CLIENT_APP_Notification_1 */
  switch(p_Notif->ConnOpcode)
  {
    /* USER CODE BEGIN ConnOpcode */

    /* USER CODE END ConnOpcode */

    case PEER_CONN_HANDLE_EVT :
      {
        uint8_t index;

        for(index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
        {
          if(a_ClientContext[index].state == GATT_CLIENT_APP_DISCONNECTED)
          {
            a_ClientContext[index].connHdl = p_Notif->ConnHdl;
            a_ClientContext[index].state = GATT_CLIENT_APP_CONNECTED;
            /* USER CODE BEGIN PEER_CONN_HANDLE_EVT_1 */

            /* USER CODE END PEER_CONN_HANDLE_EVT_1 */

            break;
          }
        }
        if(index == CFG_BLE_NUM_CLIENT_CONTEXTS)
        {
          APP_DBG_MSG("Error: reached maximum number of connected servers!\n");
          aci_gap_terminate(p_Notif->ConnHdl, BLE_ERROR_TERMINATED_REMOTE_USER);
        }

        /* USER CODE BEGIN PEER_CONN_HANDLE_EVT */
        
        /* USER CODE END PEER_CONN_HANDLE_EVT */
      }
      break;

    case PEER_DISCON_HANDLE_EVT :
      {
        for(uint8_t index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
        {
          if(a_ClientContext[index].connHdl == p_Notif->ConnHdl)
          {
            /* Set all handles to 0. */
            memset(&a_ClientContext[index], 0, sizeof(BleClientAppContext_t));
            a_ClientContext[index].connHdl = 0xFFFF;
            a_ClientContext[index].state = GATT_CLIENT_APP_DISCONNECTED;

            /* USER CODE BEGIN PEER_DISCON_HANDLE_EVT_1 */

            /* USER CODE END PEER_DISCON_HANDLE_EVT_1 */

            break;
          }
        }
        /* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */
        
        /* USER CODE END PEER_DISCON_HANDLE_EVT */
      }
      break;

    default:
      /* USER CODE BEGIN ConnOpcode_Default */

      /* USER CODE END ConnOpcode_Default */
      break;
  }
  /* USER CODE BEGIN GATT_CLIENT_APP_Notification_2 */

  /* USER CODE END GATT_CLIENT_APP_Notification_2 */
  return;
}

void GATT_CLIENT_APP_Discover_services(uint16_t connection_handle)
{
  for(uint8_t index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    if(a_ClientContext[index].connHdl == connection_handle)
    {
      a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_SERVICES;

      /* USER CODE BEGIN GATT_CLIENT_APP_Discover_services */

      /* USER CODE END GATT_CLIENT_APP_Discover_services */

      UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);

      break;
    }
  }

  return;
}

uint8_t gatt_procedure(uint8_t index, ProcGattId_t GattProcId)
{
  tBleStatus result = BLE_STATUS_SUCCESS;
  uint8_t status;

  if (index >= CFG_BLE_NUM_CLIENT_CONTEXTS)
  {
    status = 1;
  }
  else
  {
    status = 0;
    switch (GattProcId)
    {
      case PROC_GATT_DISC_ALL_PRIMARY_SERVICES:
      {
        APP_DBG_MSG("GATT services discovery\n");
        result = aci_gatt_clt_disc_all_primary_services(a_ClientContext[index].connHdl, BLE_GATT_UNENHANCED_ATT_L2CAP_CID);

        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
          APP_DBG_MSG("PROC_GATT_CTL_DISC_ALL_PRIMARY_SERVICES services discovered Successfully\n\n");
        }
        else
        {
          APP_DBG_MSG("PROC_GATT_CTL_DISC_ALL_PRIMARY_SERVICES aci_gatt_clt_disc_all_primary_services cmd NOK status =0x%02X\n\n", result);
        }
      }

      break; /* PROC_GATT_DISC_ALL_PRIMARY_SERVICES */

      case PROC_GATT_DISC_ALL_CHARS:
      {
        APP_DBG_MSG("DISCOVER_ALL_CHARS ConnHdl=0x%04X ALLServiceHandle[0x%04X - 0x%04X]\n",
                          a_ClientContext[index].connHdl,
                          a_ClientContext[index].ALLServiceStartHdl,
                          a_ClientContext[index].ALLServiceEndHdl);
        result = aci_gatt_clt_disc_all_char_of_service(
                           a_ClientContext[index].connHdl,
                           BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           a_ClientContext[index].ALLServiceStartHdl,
                           a_ClientContext[index].ALLServiceEndHdl);
        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
          APP_DBG_MSG("All characteristics discovered Successfully\n\n");
        }
        else
        {
          APP_DBG_MSG("All characteristics discovery Failed, status =0x%02X\n\n", result);
        }
      }
      break; /* PROC_GATT_DISC_ALL_CHARS */

      case PROC_GATT_DISC_ALL_DESCS:
      {
        APP_DBG_MSG("DISCOVER_ALL_CHAR_DESCS [0x%04X - 0x%04X]\n",
                         a_ClientContext[index].ALLServiceStartHdl,
                         a_ClientContext[index].ALLServiceEndHdl);
        result = aci_gatt_clt_disc_all_char_desc(
                           a_ClientContext[index].connHdl,
						   BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           a_ClientContext[index].ALLServiceStartHdl,
                           a_ClientContext[index].ALLServiceEndHdl);
        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
          APP_DBG_MSG("All characteristic descriptors discovered Successfully\n\n");
        }
        else
        {
          APP_DBG_MSG("All characteristic descriptors discovery Failed, status =0x%02X\n\n", result);
        }
      }
      break; /* PROC_GATT_DISC_ALL_DESCS */
      case PROC_GATT_ENABLE_ALL_NOTIFICATIONS:
      {
        uint16_t enable; /* Buffer must be kept valid for aci_gatt_clt_write until a gatt procedure complete is received. */

        if (a_ClientContext[index].ServiceChangedCharDescHdl != 0x0000)
        {
          enable = 0x0002;
          result = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                                      BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                      a_ClientContext[index].ServiceChangedCharDescHdl,
                                      2,
                                      (uint8_t *) &enable);
          gatt_cmd_resp_wait();
          APP_DBG_MSG(" ServiceChangedCharDescHdl =0x%04X\n",a_ClientContext[index].ServiceChangedCharDescHdl);
        }
        /* USER CODE BEGIN PROC_GATT_ENABLE_ALL_NOTIFICATIONS */

        /* USER CODE END PROC_GATT_ENABLE_ALL_NOTIFICATIONS */

        if (result == BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("All notifications enabled Successfully\n\n");
        }
        else
        {
          APP_DBG_MSG("All notifications enabled Failed, status =0x%02X\n\n", result);
        }
      }
      break; /* PROC_GATT_ENABLE_ALL_NOTIFICATIONS */
    default:
      break;
    }
  }

  return status;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Event handler
 * @param  Event: Address of the buffer holding the Event
 * @retval Ack: Return whether the Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t P2P_ROUTER_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  GATT_CLIENT_APP_Notification_evt_t Notification;
  UNUSED(Notification);

  return_value = BLEEVT_NoAck;

  switch (p_evt->ecode)
  {
    case ACI_ATT_CLT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE:
    {
      aci_att_clt_read_by_group_type_resp_event_rp0 *p_evt_rsp = (void*)p_evt->data;
      gatt_parse_services((aci_att_clt_read_by_group_type_resp_event_rp0 *)p_evt_rsp);
    }
    break; /* ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE */
  case ACI_ATT_CLT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE:
    {
      aci_att_clt_find_by_type_value_resp_event_rp0 *p_evt_rsp = (void*) p_evt->data;
      gatt_parse_services_by_UUID((aci_att_clt_find_by_type_value_resp_event_rp0 *)p_evt_rsp);
    }
    break; /* ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE */
    case ACI_ATT_CLT_READ_BY_TYPE_RESP_VSEVT_CODE:
    {
      aci_att_clt_read_by_type_resp_event_rp0 *p_evt_rsp = (void*)p_evt->data;
      gatt_parse_chars((aci_att_clt_read_by_type_resp_event_rp0 *)p_evt_rsp);
    }
    break; /* ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE */
    case ACI_ATT_CLT_FIND_INFO_RESP_VSEVT_CODE:
    {
      aci_att_clt_find_info_resp_event_rp0 *p_evt_rsp = (void*)p_evt->data;
      gatt_parse_descs((aci_att_clt_find_info_resp_event_rp0 *)p_evt_rsp);
    }
    break; /* ACI_ATT_FIND_INFO_RESP_VSEVT_CODE */
    case ACI_GATT_CLT_NOTIFICATION_VSEVT_CODE:
    {
      aci_gatt_clt_notification_event_rp0 *p_evt_rsp = (void*)p_evt->data;
      gatt_parse_notification((aci_gatt_clt_notification_event_rp0 *)p_evt_rsp);
    }
    break;/* ACI_GATT_NOTIFICATION_VSEVT_CODE */
    case ACI_GATT_CLT_PROC_COMPLETE_VSEVT_CODE:
    {
      aci_gatt_clt_proc_complete_event_rp0 *p_evt_rsp = (void*)p_evt->data;
      UNUSED(p_evt_rsp);
      gatt_cmd_resp_release();
    }
    break;/* ACI_GATT_PROC_COMPLETE_VSEVT_CODE */
    case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
    {
    aci_att_exchange_mtu_resp_event_rp0 *tx_pool_available;
    tx_pool_available = (aci_att_exchange_mtu_resp_event_rp0 *)p_evt->data;
    UNUSED(tx_pool_available);
    /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */

    /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
    }
    break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
    case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
    {
    aci_att_exchange_mtu_resp_event_rp0 * exchange_mtu_resp;
    exchange_mtu_resp = (aci_att_exchange_mtu_resp_event_rp0 *)p_evt->data;
    APP_DBG_MSG("  MTU exchanged size = %d\n",exchange_mtu_resp->MTU );
    UNUSED(exchange_mtu_resp);
    /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

    /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
    }
    break;
    /* USER CODE BEGIN BLECORE_EVT */

    /*
     * Handle the event for discovering characteristics by UUID response.
     * This case handles the event code ACI_GATT_CLT_DISC_READ_CHAR_BY_UUID_RESP_VSEVT_CODE.
     * It processes the response of a GATT client characteristic discovery by UUID.
     * The function aci_gatt_clt_disc_read_char_by_uuid_resp_event() is called to handle the event.
     * The attribute handle of the discovered characteristic is logged for debugging purposes.
     */
  case ACI_GATT_CLT_DISC_READ_CHAR_BY_UUID_RESP_VSEVT_CODE:
  {
    aci_gatt_clt_disc_read_char_by_uuid_resp_event_rp0 *p_disc_read_char_by_uuid_resp;
    p_disc_read_char_by_uuid_resp = (aci_gatt_clt_disc_read_char_by_uuid_resp_event_rp0 *)p_evt->data;
    aci_gatt_clt_disc_read_char_by_uuid_resp_event(p_disc_read_char_by_uuid_resp);
    break;
  }

    /*
     * Handle the ACI_ATT_CLT_READ_RESP_VSEVT_CODE event.
     * This case handles the ACI_ATT_CLT_READ_RESP_VSEVT_CODE event by
     * processing the response data and calling the appropriate function to
     * handle the read response event.
     * The function aci_att_clt_read_resp_event is called with the
     * parsed response data.
     */
  case ACI_ATT_CLT_READ_RESP_VSEVT_CODE:
  {
    aci_att_clt_read_resp_event_rp0 *p_read_resp;
    p_read_resp = (aci_att_clt_read_resp_event_rp0 *)p_evt->data;
    aci_att_clt_read_resp_event(p_read_resp);
    break;
  }

  case ACI_GATT_CLT_INDICATION_VSEVT_CODE:
  {
    aci_gatt_clt_indication_event_rp0 *p_indication_event;
    p_indication_event = (aci_gatt_clt_indication_event_rp0 *)p_evt->data;
    aci_gatt_clt_indication_event(p_indication_event);
    break;
  }

    /* USER CODE END BLECORE_EVT */
    default:
      APP_DBG_MSG("EVENT OPCODE: 0x%04X\n", p_evt->ecode);
      break;
  }/* end switch (p_evt->ecode) */

  return(return_value);
}

__USED static void gatt_Notification(GATT_CLIENT_APP_Notification_evt_t *p_Notif)
{
  /* USER CODE BEGIN gatt_Notification_1*/

  /* USER CODE END gatt_Notification_1 */
  switch (p_Notif->Client_Evt_Opcode)
  {
    /* USER CODE BEGIN Client_Evt_Opcode */
  case ANCS_NOTIFICATION_INFO_RECEIVED_EVT:
    /**
     * @brief This structure represents the ANCS (Apple Notification Center Service) notification payload.
     * 
     * The `p_Notif->DataTransfered.p_Payload` contains the ANCS notification data. The payload is structured as follows:
     * 
     * Byte 0: Event ID - Indicates the type of event (e.g., notification added, modified, or removed).
     * Byte 1: Event Flags - Provides additional information about the event (e.g., silent, important).
     * Byte 2: Category ID - Specifies the category of the notification (e.g., incoming call, missed call).
     * Byte 3: Category Count - Number of active notifications in the given category.
     * Bytes 4-7: Notification UID - A unique identifier for the notification.
     * Bytes 8-n: Notification Attributes - Contains various attributes of the notification such as title, message, etc.
     * 
     * The exact length and content of the notification attributes depend on the specific notification and its attributes.
     */

    /* Event ID switch case */
    switch (p_Notif->DataTransfered.p_Payload[0])
    {
      case EventIDNotificationAdded:
        {
          uint8_t found=0;
          /* Handle notification added event */
          for(uint8_t index=0; index<MAX_ANCS_NOTIFICATION; index++)
          {
            if(ancs_notification_list[index].used == UNUSED)
            {
              found=1;
              ancs_notification_list[index].used = USED;
              ancs_notification_list[index].status = ADDED;
              ancs_notification_list[index].evID = (EventID) p_Notif->DataTransfered.p_Payload[0];
              ancs_notification_list[index].evFlag = (EventFlags) p_Notif->DataTransfered.p_Payload[1];
              ancs_notification_list[index].catID = (CategoryID) p_Notif->DataTransfered.p_Payload[2];
              ancs_notification_list[index].catCount = p_Notif->DataTransfered.p_Payload[3];
              uint8_t *ptr = 0x0;
              ptr = &(p_Notif->DataTransfered.p_Payload[4]);
              ancs_notification_list[index].notifUID = UNPACK_4_BYTE_PARAMETER(ptr);
              APP_DBG_MSG("Notification ADDED.\n");

              APP_DBG_MSG("Notification : EventID             0x%02X\n", ancs_notification_list[index].evID);
              APP_DBG_MSG("               EventFlags          0x%02X\n", ancs_notification_list[index].evFlag);
              APP_DBG_MSG("               CategoryID          0x%02X\n", ancs_notification_list[index].catID);
              APP_DBG_MSG("               CategoryCount       0x%02X\n", ancs_notification_list[index].catCount);
              APP_DBG_MSG("               UID           0x%08X\n", ancs_notification_list[index].notifUID);

              UTIL_SEQ_SetTask(1U << CFG_TASK_NOTIFICATION_ANCS, CFG_SEQ_PRIO_0);

              break;
            }
          }
          if(found==0)
          {
            APP_DBG_MSG("Notification NOT ADDED to the notification stored on the device. Handle notification cannot be added. All notifications data structure is used. The limit is %d notifications.\n", MAX_ANCS_NOTIFICATION);
          }
        }
        break;
      case EventIDNotificationModified:
        /* Handle notification modified event */
        {
          uint8_t *ptr = 0x0;
          ptr = &(p_Notif->DataTransfered.p_Payload[4]);
          uint32_t notification_uid = UNPACK_4_BYTE_PARAMETER(ptr);
          for(uint8_t index=0; index<MAX_ANCS_NOTIFICATION; index++)
          {
            if(ancs_notification_list[index].notifUID == notification_uid)
            {
              APP_DBG_MSG("Notification MODIFIED\n");
              APP_DBG_MSG("notification_uid = 0x%08X\n", notification_uid);
              ancs_notification_list[index].status = TO_UPDATE;
              UTIL_SEQ_SetTask(1U << CFG_TASK_NOTIFICATION_ANCS, CFG_SEQ_PRIO_0);
            }
          }
        }
        break;
      case EventIDNotificationRemoved:
        /* Handle notification removed event */
        {
          uint8_t *ptr = 0x0;
          ptr = &(p_Notif->DataTransfered.p_Payload[4]);
          uint32_t notification_uid = UNPACK_4_BYTE_PARAMETER(ptr);
          for(uint8_t index=0; index<MAX_ANCS_NOTIFICATION; index++)
          {
            if(ancs_notification_list[index].notifUID == notification_uid)
            {
              APP_DBG_MSG("Notification REMOVED\n");
              APP_DBG_MSG("notification_uid = 0x%08X\n", notification_uid);
              ancs_notification_list[index].used = UNUSED;
            }
          }
        }
        break;
      default:
        APP_DBG_MSG("Event ID switch case : Handle (0x%02X) reserved event IDs or unknown event\n", p_Notif->DataTransfered.p_Payload[0]);
        break;
    }
    break; /* ANCS_NOTIFICATION_INFO_RECEIVED_EVT */
    
    case ANCS_GET_NOTIFICATION_ATTRIBUTES_RECEIVED_EVT:
    {
      uint32_t notification_uid = (uint32_t)p_Notif->DataTransfered.p_Payload[1] | ((uint32_t)p_Notif->DataTransfered.p_Payload[2] << 8) | ((uint32_t)p_Notif->DataTransfered.p_Payload[3] << 16) | ((uint32_t)p_Notif->DataTransfered.p_Payload[4] << 24);
      APP_DBG_MSG("notification_uid = 0x%08X\n", notification_uid);
      for(uint8_t index=0; index<MAX_ANCS_NOTIFICATION; index++)
      {
        if(ancs_notification_list[index].notifUID == notification_uid)
        {
          uint16_t dim = 0;
          uint16_t i = 0;
          APP_DBG_MSG("ancs_notification_list[%d].notifUID = 0x%08X\n", index, ancs_notification_list[index].notifUID);
          /* NotificationAttributeIDAppIdentifier  */
          if(p_Notif->DataTransfered.p_Payload[5] == NotificationAttributeIDAppIdentifier)
          {
            dim = (uint16_t)p_Notif->DataTransfered.p_Payload[6] | (uint16_t)((p_Notif->DataTransfered.p_Payload[7]) << 8);
            i = 8;
            for(uint16_t j=0 ; j<dim; j++)
            {
              ancs_notification_list[index].notifData.appIdentifier[j] = p_Notif->DataTransfered.p_Payload[i++];
            }
            ancs_notification_list[index].notifData.appIdentifier[dim] = '\0';
          }
          
          /* NotificationAttributeIDTitle */
          if(p_Notif->DataTransfered.p_Payload[i+0] == NotificationAttributeIDTitle)
          {
            dim = (uint16_t)p_Notif->DataTransfered.p_Payload[i+1] | (uint16_t)((p_Notif->DataTransfered.p_Payload[i+2]) << 8);
            i = i + 3;
            for(uint16_t j=0 ; j<dim; j++)
            {
              ancs_notification_list[index].notifData.idTitle[j] = p_Notif->DataTransfered.p_Payload[i++];
            }
            ancs_notification_list[index].notifData.idTitle[dim] = '\0';
          }
          
          /* NotificationAttributeIDSubtitle */
          if(p_Notif->DataTransfered.p_Payload[i+0] == NotificationAttributeIDSubtitle)
          {
            dim = (uint16_t)p_Notif->DataTransfered.p_Payload[i+1] | (uint16_t)((p_Notif->DataTransfered.p_Payload[i+2]) << 8);
            i = i + 3;
            for(uint16_t j=0 ; j<dim; j++)
            {
              ancs_notification_list[index].notifData.idSubtitle[j] = p_Notif->DataTransfered.p_Payload[i++];
            }
            ancs_notification_list[index].notifData.idSubtitle[dim] = '\0';
          }

          /* NotificationAttributeIDMessage */
          if(p_Notif->DataTransfered.p_Payload[i+0] == NotificationAttributeIDMessage)
          {
            dim = (uint16_t)p_Notif->DataTransfered.p_Payload[i+1] | (uint16_t)((p_Notif->DataTransfered.p_Payload[i+2]) << 8);
            i = i + 3;
            for(uint16_t j=0 ; j<dim; j++)
            {
              ancs_notification_list[index].notifData.idMessage[j] = p_Notif->DataTransfered.p_Payload[i++];
            }
            ancs_notification_list[index].notifData.idMessage[dim] = '\0';
          }

          /* NotificationAttributeIDMessageSize */
          if(p_Notif->DataTransfered.p_Payload[i+0] == NotificationAttributeIDMessageSize)
          {
            dim = (uint16_t)p_Notif->DataTransfered.p_Payload[i+1] | (uint16_t)((p_Notif->DataTransfered.p_Payload[i+2]) << 8);
            i = i + 3;
            for(uint16_t j=0 ; j<dim; j++)
            {
              ancs_notification_list[index].notifData.idMessageSize[j] = p_Notif->DataTransfered.p_Payload[i++];
            }
            ancs_notification_list[index].notifData.idMessageSize[dim] = '\0';
          }

          /* NotificationAttributeIDDate */
          if(p_Notif->DataTransfered.p_Payload[i+0] == NotificationAttributeIDDate)
          {
            dim = (uint16_t)p_Notif->DataTransfered.p_Payload[i+1] | (uint16_t)((p_Notif->DataTransfered.p_Payload[i+2]) << 8);
            i = i + 3;
            for(uint16_t j=0 ; j<dim; j++)
            {
              ancs_notification_list[index].notifData.idDate[j] = p_Notif->DataTransfered.p_Payload[i++];
            }
            ancs_notification_list[index].notifData.idDate[dim] = '\0';
          }

          /* NotificationAttributeIDPositiveActionLabel */
          if(p_Notif->DataTransfered.p_Payload[i+0] == NotificationAttributeIDPositiveActionLabel)
          {
            dim = (uint16_t)p_Notif->DataTransfered.p_Payload[i+1] | (uint16_t)((p_Notif->DataTransfered.p_Payload[i+2]) << 8);
            i = i + 3;
            for(uint16_t j=0 ; j<dim; j++)
            {
              ancs_notification_list[index].notifData.idPositiveActionLabel[j] = p_Notif->DataTransfered.p_Payload[i++];
            }
            ancs_notification_list[index].notifData.idPositiveActionLabel[dim] = '\0';
          }

          /* NotificationAttributeIDNegativeActionLabel */
          if(p_Notif->DataTransfered.p_Payload[i+0] == NotificationAttributeIDNegativeActionLabel)
          {
            dim = (uint16_t)p_Notif->DataTransfered.p_Payload[i+1] | (uint16_t)((p_Notif->DataTransfered.p_Payload[i+2]) << 8);
            i = i + 3;
            for(uint16_t j=0 ; j<dim; j++)
            {
              ancs_notification_list[index].notifData.idNegativeActionLabel[j] = p_Notif->DataTransfered.p_Payload[i++];
            }
            ancs_notification_list[index].notifData.idNegativeActionLabel[dim] = '\0';
          }

          APP_DBG_MSG("Notification Attributes:\n");
          APP_DBG_MSG("  AppIdentifier: %s\n", ancs_notification_list[index].notifData.appIdentifier); 
          printANCSNotification(&ancs_notification_list[index]);
        }
      }
    }
    break; /* ANCS_GET_NOTIFICATION_ATTRIBUTES_RECEIVED_EVT */   
    /* USER CODE END Client_Evt_Opcode */

    case NOTIFICATION_INFO_RECEIVED_EVT:
      /* USER CODE BEGIN NOTIFICATION_INFO_RECEIVED_EVT */

      /* USER CODE END NOTIFICATION_INFO_RECEIVED_EVT */
      break;

    default:
      /* USER CODE BEGIN Client_Evt_Opcode_Default */

      /* USER CODE END Client_Evt_Opcode_Default */
      break;
  }
  /* USER CODE BEGIN gatt_Notification_2*/

  /* USER CODE END gatt_Notification_2 */
  return;
}

/**
* function of GATT service parse
*/
static void gatt_parse_services(aci_att_clt_read_by_group_type_resp_event_rp0 *p_evt)
{
  uint16_t uuid, ServiceStartHdl, ServiceEndHdl;
  uint8_t uuid_offset, uuid_size, uuid_short_offset;
  uint8_t i, idx, numServ, index;

  APP_DBG_MSG("ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
                p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  /* check connection handle related to response before processing */
  if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
  {
    /* Number of attribute value tuples */
    numServ = (p_evt->Data_Length) / p_evt->Attribute_Data_Length;

    /* event data in Attribute_Data_List contains:
    * 2 bytes for start handle
    * 2 bytes for end handle
    * 2 or 16 bytes data for UUID
    */
    if (p_evt->Attribute_Data_Length == 20) /* we are interested in the UUID is 128 bit.*/
    {
      idx = 16;                /*UUID index of 2 bytes read part in Attribute_Data_List */
      uuid_offset = 4;         /*UUID offset in bytes in Attribute_Data_List */
      uuid_size = 16;          /*UUID size in bytes */
      uuid_short_offset = 12;  /*UUID offset of 2 bytes read part in UUID field*/
    }
    if (p_evt->Attribute_Data_Length == 6) /* we are interested in the UUID is 16 bit.*/
    {
      idx = 4;
      uuid_offset = 4;
      uuid_size = 2;
      uuid_short_offset = 0;
    }
    UNUSED(idx);
    UNUSED(uuid_size);

    /* Loop on number of attribute value tuples */
    for (i = 0; i < numServ; i++)
    {
      ServiceStartHdl =  UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset - 4]);
      ServiceEndHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Attribute_Data_List[uuid_offset + uuid_short_offset]);
      APP_DBG_MSG("  %d/%d short UUID=0x%04X, handle [0x%04X - 0x%04X]",
                   i + 1, numServ, uuid, ServiceStartHdl,ServiceEndHdl);

      /* complete context fields */
      if ( (a_ClientContext[index].ALLServiceStartHdl == 0x0000) || (ServiceStartHdl < a_ClientContext[index].ALLServiceStartHdl) )
      {
        a_ClientContext[index].ALLServiceStartHdl = ServiceStartHdl;
      }
      if ( (a_ClientContext[index].ALLServiceEndHdl == 0x0000) || (ServiceEndHdl > a_ClientContext[index].ALLServiceEndHdl) )
      {
        a_ClientContext[index].ALLServiceEndHdl = ServiceEndHdl;
      }

      if (uuid == GAP_SERVICE_UUID)
      {
        a_ClientContext[index].GAPServiceStartHdl = ServiceStartHdl;
        a_ClientContext[index].GAPServiceEndHdl = ServiceEndHdl;

        APP_DBG_MSG(", GAP_SERVICE_UUID found\n");
      }
      else if (uuid == GATT_SERVICE_UUID)
      {
        a_ClientContext[index].GATTServiceStartHdl = ServiceStartHdl;
        a_ClientContext[index].GATTServiceEndHdl = ServiceEndHdl;

        APP_DBG_MSG(", GENERIC_ATTRIBUTE_SERVICE_UUID found\n");
      }
/* USER CODE BEGIN gatt_parse_services_1 */

/* USER CODE END gatt_parse_services_1 */
      else
      {
        APP_DBG_MSG("\n");
      }

      uuid_offset += p_evt->Attribute_Data_Length;
    }
  }
  else
  {
    APP_DBG_MSG("ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE, failed no free index in connection table !\n");
  }

  return;
}

/**
* function of GATT service parse by UUID
*/
static void gatt_parse_services_by_UUID(aci_att_clt_find_by_type_value_resp_event_rp0 *p_evt)
{
  uint8_t i;

  APP_DBG_MSG("ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE - ConnHdl=0x%04X, Num_of_Handle_Pair=%d\n",
                p_evt->Connection_Handle,
                p_evt->Num_of_Handle_Pair);

  for(i = 0 ; i < p_evt->Num_of_Handle_Pair ; i++)
  {
    APP_DBG_MSG("ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE - PaitId=%d Found_Attribute_Handle=0x%04X, Group_End_Handle=0x%04X\n",
                  i,
                  p_evt->Attribute_Group_Handle_Pair[i].Found_Attribute_Handle,
                  p_evt->Attribute_Group_Handle_Pair[i].Group_End_Handle);
  }

/* USER CODE BEGIN gatt_parse_services_by_UUID_1 */
  aci_att_clt_find_by_type_value_resp_event(p_evt);
/* USER CODE END gatt_parse_services_by_UUID_1 */

  return;
}

/**
* function of GATT characteristics parse
*/
static void gatt_parse_chars(aci_att_clt_read_by_type_resp_event_rp0 *p_evt)
{
  uint16_t uuid, CharStartHdl, CharValueHdl;
  uint8_t uuid_offset, uuid_size, uuid_short_offset;
  uint8_t i, idx, numHdlValuePair, index;
  uint8_t CharProperties;

  APP_DBG_MSG("ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
                p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
  {
    /* event data in Attribute_Data_List contains:
    * 2 bytes for start handle
    * 1 byte char properties
    * 2 bytes handle
    * 2 or 16 bytes data for UUID
    */

    /* Number of attribute value tuples */
    numHdlValuePair = p_evt->Data_Length / p_evt->Handle_Value_Pair_Length;

    if (p_evt->Handle_Value_Pair_Length == 21) /* we are interested in  128 bit UUIDs */
    {
      idx = 17;                /* UUID index of 2 bytes read part in Attribute_Data_List */
      uuid_offset = 5;         /* UUID offset in bytes in Attribute_Data_List */
      uuid_size = 16;          /* UUID size in bytes */
      uuid_short_offset = 12;  /* UUID offset of 2 bytes read part in UUID field */
    }
    if (p_evt->Handle_Value_Pair_Length == 7) /* we are interested in  16 bit UUIDs */
    {
      idx = 5;
      uuid_offset = 5;
      uuid_size = 2;
      uuid_short_offset = 0;
    }
    UNUSED(idx);
    UNUSED(uuid_size);

    p_evt->Data_Length -= 1;

    APP_DBG_MSG("  ConnHdl=0x%04X, number of value pair = %d\n", a_ClientContext[index].connHdl, numHdlValuePair);
    /* Loop on number of attribute value tuples */
    for (i = 0; i < numHdlValuePair; i++)
    {
      CharStartHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset - 5]);
      CharProperties = p_evt->Handle_Value_Pair_Data[uuid_offset - 3];
      CharValueHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset + uuid_short_offset]);

      if ( (uuid != 0x0) && (CharProperties != 0x0) && (CharStartHdl != 0x0) && (CharValueHdl != 0) )
      {
        APP_DBG_MSG("    %d/%d short UUID=0x%04X, Properties=0x%04X, CharHandle [0x%04X - 0x%04X]",
                     i + 1, numHdlValuePair, uuid, CharProperties, CharStartHdl, CharValueHdl);

        if (uuid == DEVICE_NAME_UUID)
        {
          APP_DBG_MSG(", GAP DEVICE_NAME charac found\n");
        }
        else if (uuid == APPEARANCE_UUID)
        {
          APP_DBG_MSG(", GAP APPEARANCE charac found\n");
        }
        else if (uuid == SERVICE_CHANGED_UUID)
        {
          a_ClientContext[index].ServiceChangedCharStartHdl = CharStartHdl;
          a_ClientContext[index].ServiceChangedCharValueHdl = CharValueHdl;
          APP_DBG_MSG(", GATT SERVICE_CHANGED_CHARACTERISTIC_UUID charac found\n");
        }
/* USER CODE BEGIN gatt_parse_chars_1 */

/* USER CODE END gatt_parse_chars_1 */
        else
        {
          APP_DBG_MSG("\n");
        }

      }
      uuid_offset += p_evt->Handle_Value_Pair_Length;
    }
  }
  else
  {
    APP_DBG_MSG("ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
  }

  return;
}
/**
* function of GATT descriptor parse
*/
static void gatt_parse_descs(aci_att_clt_find_info_resp_event_rp0 *p_evt)
{
  uint16_t uuid, handle;
  uint8_t uuid_offset, uuid_size, uuid_short_offset;
  uint8_t i, numDesc, handle_uuid_pair_size, index;
  static uint16_t gattCharStartHdl = 0;
  static uint16_t gattCharValueHdl = 0;

  APP_DBG_MSG("ACI_ATT_FIND_INFO_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
              p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_BLE_NUM_CLIENT_CONTEXTS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
  {
    /* event data in Attribute_Data_List contains:
    * 2 bytes handle
    * 2 or 16 bytes data for UUID
    */
    if (p_evt->Format == UUID_TYPE_16)
    {
      uuid_size = 2;
      uuid_offset = 2;
      uuid_short_offset = 0;
      handle_uuid_pair_size = 4;
    }
    if (p_evt->Format == UUID_TYPE_128)
    {
      uuid_size = 16;
      uuid_offset = 2;
      uuid_short_offset = 12;
      handle_uuid_pair_size = 18;
    }
    UNUSED(uuid_size);

    /* Number of handle uuid pairs */
    numDesc = (p_evt->Event_Data_Length) / handle_uuid_pair_size;

    for (i = 0; i < numDesc; i++)
    {
      handle = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_UUID_Pair[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_UUID_Pair[uuid_offset + uuid_short_offset]);

      if (uuid == PRIMARY_SERVICE_UUID)
      {
        APP_DBG_MSG("PRIMARY_SERVICE_UUID=0x%04X handle=0x%04X\n", uuid, handle);
      }
      else if (uuid == CHARACTERISTIC_UUID)
      {
        /* reset UUID & handle */
        gattCharStartHdl = 0;
        gattCharValueHdl = 0;

        gattCharStartHdl = handle;
        APP_DBG_MSG("reset - UUID & handle - CHARACTERISTIC_UUID=0x%04X CharStartHandle=0x%04X\n", uuid, handle);
      }
      else if ( (uuid == CHAR_EXTENDED_PROPERTIES_DESCRIPTOR_UUID)
             || (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID) )
      {

        APP_DBG_MSG("Descriptor UUID=0x%04X, handle=0x%04X-0x%04X-0x%04X",
                      uuid,
                      gattCharStartHdl,
                      gattCharValueHdl,
                      handle);
        if (a_ClientContext[index].ServiceChangedCharValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].ServiceChangedCharDescHdl = handle;
          APP_DBG_MSG(", Service Changed found\n");
        }
/* USER CODE BEGIN gatt_parse_descs_1 */

        else if (1)
          aci_att_clt_find_info_resp_event(p_evt);

/* USER CODE END gatt_parse_descs_1 */
        else
        {
          APP_DBG_MSG("\n");
        }
      }
      else
      {
        gattCharValueHdl = handle;

        APP_DBG_MSG("  UUID=0x%04X, handle=0x%04X", uuid, handle);

        if (uuid == DEVICE_NAME_UUID)
        {
          APP_DBG_MSG(", found GAP DEVICE_NAME_UUID\n");
        }
        else if (uuid == APPEARANCE_UUID)
        {
          APP_DBG_MSG(", found GAP APPEARANCE_UUID\n");
        }
        else if (uuid == SERVICE_CHANGED_UUID)
        {
          APP_DBG_MSG(", found GATT SERVICE_CHANGED_CHARACTERISTIC_UUID\n");
        }
/* USER CODE BEGIN gatt_parse_descs_2 */

/* USER CODE END gatt_parse_descs_2 */
        else
        {
          APP_DBG_MSG("\n");
        }
      }
    uuid_offset += handle_uuid_pair_size;
    }
  }
  else
  {
    APP_DBG_MSG("ACI_ATT_FIND_INFO_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
  }

  return;
}

static void gatt_parse_notification(aci_gatt_clt_notification_event_rp0 *p_evt)
{
  APP_DBG_MSG("ACI_GATT_NOTIFICATION_VSEVT_CODE - ConnHdl=0x%04X, Attribute_Handle=0x%04X\n",
              p_evt->Connection_Handle,
              p_evt->Attribute_Handle);
/* USER CODE BEGIN gatt_parse_notification_1 */

  GATT_CLIENT_APP_Notification_evt_t notification;
  uint8_t index;

  for (index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      if (p_evt->Attribute_Handle == (a_ClientContext[index].ANCSNotificationSourceCharHdl + 1))
      {
        notification.Client_Evt_Opcode = ANCS_NOTIFICATION_INFO_RECEIVED_EVT;
        notification.DataTransfered.length = p_evt->Attribute_Value_Length;
        notification.DataTransfered.p_Payload = &p_evt->Attribute_Value[0];
        gatt_Notification(&notification);
      }
      if (p_evt->Attribute_Handle == (a_ClientContext[index].ANCSDataSourceCharHdl + 1))
      {
        notification.Client_Evt_Opcode = ANCS_GET_NOTIFICATION_ATTRIBUTES_RECEIVED_EVT;
        notification.DataTransfered.length = p_evt->Attribute_Value_Length;
        notification.DataTransfered.p_Payload = &p_evt->Attribute_Value[0];
        gatt_Notification(&notification);
      }
    }
  }

/* USER CODE END gatt_parse_notification_1 */

  return;
}

static void client_discover_all(void)
{
  uint8_t index;

  for(index = 0; index < CFG_BLE_NUM_CLIENT_CONTEXTS; index++)
  {
    if(a_ClientContext[index].state == GATT_CLIENT_APP_DISCOVER_SERVICES)
    {
      gatt_procedure(index, PROC_GATT_DISC_ALL_PRIMARY_SERVICES);
      gatt_procedure(index, PROC_GATT_DISC_ALL_CHARS);
      gatt_procedure(index, PROC_GATT_DISC_ALL_DESCS);
      gatt_procedure(index, PROC_GATT_ENABLE_ALL_NOTIFICATIONS);

      a_ClientContext[index].state = GATT_CLIENT_APP_CONNECTED;

      /* Check if in the meantime another server has been connected. */
      UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);

      break;
    }
  }

  return;
}

static void gatt_cmd_resp_release(void)
{
  UTIL_SEQ_SetEvt(1U << CFG_IDLEEVT_PROC_GATT_COMPLETE);
  return;
}

static void gatt_cmd_resp_wait(void)
{
  UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_COMPLETE);
  return;
}

/* USER CODE BEGIN LF */

/* CFG_TASK_DISCOVER_ANCS_SERVICE_ID */

static void client_discover_ancs(void)
{
  UUID_t uuid = {0};
  tBleStatus result = BLE_STATUS_SUCCESS;

  /*  0x0001 Buffer must be kept valid for aci_gatt_clt_write until a gatt procedure complete is received. */
  static uint16_t enable_notif = 0x0001;

  switch (a_ClientContext[0].connStatus)
  {
    case APP_BLE_IDLE:
      if(a_ClientContext[0].state == GATT_CLIENT_APP_CONNECTED)
      {
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_DISC_SERVICE;
      }
      break; /* APP_BLE_IDLE */
    case APP_BLE_ANCS_STATE_DISC_SERVICE:
      /* Initialize the UUID structure with the ANCS service UUID */
      for (int i = 0; i < 16; i++)
        uuid.UUID_128[i] = ancs_service_uuid[15 - i];

      /* Start the discovery of the primary service by UUID */
      result = aci_gatt_clt_disc_primary_service_by_uuid(a_ClientContext[0].connHdl, BLE_GATT_UNENHANCED_ATT_L2CAP_CID, 0x02, &uuid);

      if (result == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("Success aci_gatt_clt_disc_primary_service_by_uuid\n");
        /* Wait for the response of the service discovery */
        APP_DBG_MSG("aci_gatt_clt_disc_primary_service_by_uuid Wait for the response\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_SERVICE;
        gatt_cmd_resp_wait();
        APP_DBG_MSG("aci_gatt_clt_disc_primary_service_by_uuid Response received\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_DISC_NOTIFICATION;
      }
      else
      {
        APP_DBG_MSG("aci_gatt_clt_disc_primary_service_by_uuid Failed, status = 0x%02X\n", result);
      }
      break; /* ANCS_STATE_DISC_SERVICE */
    case APP_BLE_ANCS_STATE_DISC_NOTIFICATION:
      if (!a_ClientContext[0].ANCSservicePresent)
      {
        APP_DBG_MSG("ANCSservicePresent = False\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
        return;
      }
      /* Discovery Notification Source */
      for (int i = 0; i < 16; i++)
        uuid.UUID_128[i] = notification_char_uuid[15 - i];

      result = aci_gatt_clt_disc_char_by_uuid(a_ClientContext[0].connHdl,
                                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                              a_ClientContext[0].ANCSServiceHdl,
                                              a_ClientContext[0].ANCSServiceEndHdl,
                                              0x02,
                                              &uuid);
      if (result == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("Success aci_gatt_clt_disc_char_by_uuid\n\n");

        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Wait for the response\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_NOTIFICATION;
        gatt_cmd_resp_wait();
        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Response received\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_DISC_CONTROL_POINT;
      }
      else
      {
        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Failed ANCS_STATE_WAITING_DISC_NOTIFICATION, status = 0x%02X\n", result);
      }
      break; /* ANCS_STATE_DISC_NOTIFICATION */
    case APP_BLE_ANCS_STATE_DISC_CONTROL_POINT:
      if (!a_ClientContext[0].ANCSservicePresent)
      {
        APP_DBG_MSG("ANCSservicePresent = False\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
        return;
      }
      /* Discovery Control Point */
      for (int i = 0; i < 16; i++)
        uuid.UUID_128[i] = control_point_char_uuid[15 - i];
      result = aci_gatt_clt_disc_char_by_uuid(a_ClientContext[0].connHdl,
                                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                              a_ClientContext[0].ANCSServiceHdl,
                                              a_ClientContext[0].ANCSServiceEndHdl,
                                              0x02,
                                              &uuid);
      if (result == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("Success aci_gatt_clt_disc_char_by_uuid\n");
        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Wait for the response\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_CONTROL_POINT;
        gatt_cmd_resp_wait();
        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Response received\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_DISC_DATA_SOURCE;
      }
      else
      {
        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Failed, status =0x%02X\n", result);
      }
      break; /* ANCS_STATE_DISC_CONTROL_POINT */

    case APP_BLE_ANCS_STATE_DISC_DATA_SOURCE:
      /* Discovery Data Source */
      if (!a_ClientContext[0].ANCSservicePresent)
      {
        APP_DBG_MSG("ANCSservicePresent = False\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
        return;
      }
      for (int i = 0; i < 16; i++)
        uuid.UUID_128[i] = data_source_char_uuid[15 - i];

      result = aci_gatt_clt_disc_char_by_uuid(a_ClientContext[0].connHdl,
                                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                              a_ClientContext[0].ANCSServiceHdl,
                                              a_ClientContext[0].ANCSServiceEndHdl,
                                              0x02,
                                              &uuid);
      if (result == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("Success aci_gatt_clt_disc_char_by_uuid\n\n");
        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Wait for the response\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_DATA_SOURCE;
        gatt_cmd_resp_wait();
        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Response received\n");
        /* 0x80 additional characteristic properties are defined in the Characteristic Extended Properties Descriptor*/
        if (a_ClientContext[0].ANCSControlPointValueHdl & 0x80)
        {
          a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_DISC_CTRLPOINT_EXTENDED_PROP;
        }
        else
        {
          a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE;
        }
      }
      else
      {
        APP_DBG_MSG("aci_gatt_clt_disc_char_by_uuid Failed, status =0x%02X\n", result);
      }
      break; /* ANCS_STATE_DISC_DATA_SOURCE */
    case APP_BLE_ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE:
      /* 0x10 means that a notification is present */
      if ((a_ClientContext[0].ANCSNotificationSourceCharHdl != 0) && (a_ClientContext[0].ANCSNotificationSourceValueHdl & 0x10))
      {
        result = aci_gatt_clt_write(a_ClientContext[0].connHdl,
                                    BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                    a_ClientContext[0].ANCSNotificationSourceCharHdl + 2,
                                    0x02,
                                    (uint8_t *)&enable_notif);
        if (result == BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("Success aci_gatt_clt_write\n");
          APP_DBG_MSG("aci_gatt_clt_write Wait for the response\n");
          a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_SUBSCRIBE_NOTIFICATION_SOURCE;
          gatt_cmd_resp_wait();
          APP_DBG_MSG("aci_gatt_clt_write Response received\n");
          a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_SUBSCRIBE_DATA_SOURCE;
        }
        else
        {
          APP_DBG_MSG("aci_gatt_clt_write Failed, status = 0x%02X\n", result);
        }
      }
      break; /* ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE */
    case APP_BLE_ANCS_STATE_SUBSCRIBE_DATA_SOURCE:
      /* 0x10 means that a notification is present */
      if ((a_ClientContext[0].ANCSDataSourceCharHdl != 0) && (a_ClientContext[0].ANCSDataSourceValueHdl & 0x10))
      {
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_SUBSCRIBE_DATA_SOURCE;
        result = aci_gatt_clt_write(a_ClientContext[0].connHdl,
                                    BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                    a_ClientContext[0].ANCSDataSourceCharHdl + 2,
                                    2,
                                    (uint8_t *)&enable_notif);
        if (result == BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("Success aci_gatt_clt_write\n\n");

          APP_DBG_MSG("aci_gatt_clt_write Wait for the response\n");
          a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_SUBSCRIBE_DATA_SOURCE;
          gatt_cmd_resp_wait();
          APP_DBG_MSG("aci_gatt_clt_write Response received\n");
          a_ClientContext[0].connStatus = APP_BLE_ANCS_COMPLETE_DISCOVER;
        }
        else
        {
          APP_DBG_MSG("aci_gatt_clt_write Failed, status =0x%02X\n", result);
        }
      }
      break; /* ANCS_STATE_SUBSCRIBE_DATA_SOURCE */
    case APP_BLE_ANCS_STATE_DISC_CTRLPOINT_EXTENDED_PROP:
      {
      uint16_t min_char_handle, max_char_handle;
      min_char_handle = MIN(a_ClientContext[0].ANCSNotificationSourceCharHdl, a_ClientContext[0].ANCSDataSourceCharHdl);
      max_char_handle = MAX(a_ClientContext[0].ANCSNotificationSourceCharHdl, a_ClientContext[0].ANCSDataSourceCharHdl);
      if (a_ClientContext[0].ANCSControlPointExtendedCharHdl > max_char_handle)
      {
        result = aci_gatt_clt_disc_all_char_desc(a_ClientContext[0].connHdl,
                                                 BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                                 a_ClientContext[0].ANCSControlPointExtendedCharHdl + 1,
                                                 a_ClientContext[0].ANCSServiceEndHdl);
      }
      else
      {
        if (a_ClientContext[0].ANCSControlPointExtendedCharHdl < min_char_handle)
        {
          result = aci_gatt_clt_disc_all_char_desc(a_ClientContext[0].connHdl,
                                                   BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                                   a_ClientContext[0].ANCSControlPointExtendedCharHdl + 1,
                                                   min_char_handle - 1);
        }
        else
        {
          result = aci_gatt_clt_disc_all_char_desc(a_ClientContext[0].connHdl,
                                                   BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                                   a_ClientContext[0].ANCSControlPointExtendedCharHdl + 1,
                                                   max_char_handle - 1);
        }
      }
      if (result != BLE_STATUS_SUCCESS)
      {
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
        a_ClientContext[0].ANCSerror_proc = ERROR_ANCS_DISCOVERY_CHARACTERISTIC_DESC;
        APP_DBG_MSG("Error in aci_gatt_clt_disc_all_char_desc() = 0x%02X (DISC CTRL EXTENDED PROP STATE)\n", result);
      }
      else
      {
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_CTRLPOINT_EXTENDED_PROP;
        APP_DBG_MSG("Success aci_gatt_clt_disc_all_char_desc \n");
        APP_DBG_MSG("aci_gatt_clt_disc_all_char_desc Wait for the response\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_CTRLPOINT_EXTENDED_PROP;
        gatt_cmd_resp_wait();
        APP_DBG_MSG("aci_gatt_clt_disc_all_char_desc Response received\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_READ_CTRLPOINT_EXTENDED_PROP_VALUE;
      }
      }
      break; /* ANCS_STATE_DISC_CTRLPOINT_EXTENDED_PROP */
    case APP_BLE_ANCS_STATE_READ_CTRLPOINT_EXTENDED_PROP_VALUE:
      result = aci_gatt_clt_read(a_ClientContext[0].connHdl,
                                 BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                 a_ClientContext[0].ANCSControlPointExtendedCharHdl);
      if (result == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("Success aci_gatt_clt_read\n\n");

        APP_DBG_MSG("aci_gatt_clt_read Wait for the response\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_READ_CTRLPOINT_EXTENDED_PROP_VALUE;
        gatt_cmd_resp_wait();
        APP_DBG_MSG("aci_gatt_clt_read Response received\n");
        a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE;
      }
      else
      {
        APP_DBG_MSG("aci_gatt_clt_read Failed, status =0x%02X\n", result);
      }
      break; /* ANCS_STATE_READ_CTRLPOINT_EXTENDED_PROP_VALUE */
	  default:
		  break;
  }

  /* ANCS_COMPLETE_DISCOVER is used to end the procedure of discovering ANCS services and characteristics */
  if (a_ClientContext[0].connStatus != APP_BLE_ANCS_COMPLETE_DISCOVER)
  {
    if(a_ClientContext[0].state == GATT_CLIENT_APP_CONNECTED)
    {
      UTIL_SEQ_SetTask(1U << CFG_TASK_CLIENT_DISCOVER_ANCS, CFG_SEQ_PRIO_0);
    }
  }
  return;
}


static void notification_ancs(void)
{
  for(uint8_t index=0; index<MAX_ANCS_NOTIFICATION; index++)
  {
    if(ancs_notification_list[index].used == USED)
    {
      switch (ancs_notification_list[index].status)
      {
        case TO_UPDATE:
          /* TO_UPDATE same action of the ADDED case to update thenotification data */
        case ADDED:
          ancs_notification_list[index].status = GET_NOTIFICATION_ATTRIBUTES;
          
          APP_DBG_MSG("Notification GET_NOTIFICATION_ATTRIBUTES.\n");

          APP_DBG_MSG("Notification : EventID             0x%02X\n", ancs_notification_list[index].evID);
          APP_DBG_MSG("               EventFlags          0x%02X\n", ancs_notification_list[index].evFlag);
          APP_DBG_MSG("               CategoryID          0x%02X\n", ancs_notification_list[index].catID);
          APP_DBG_MSG("               CategoryCount       0x%02X\n", ancs_notification_list[index].catCount);
          APP_DBG_MSG("               UID           0x%08X\n", ancs_notification_list[index].notifUID);

          /* This application will show all data related to the notification just received. */
          gatt_ShowANCSPacket(&ancs_notification_list[index]);
          break; /* ADDED */
        
        default:
          break;
      }

    }
  }
  return;
}

void gatt_ShowANCSPacket(ANCS_NotificationList_type *notification)
{
  uint8_t notificationAttributeCommand[1 + 4 + COMMAND_DATA_SIZE] = {0};
  
  /* CommandID: Should be set to 0 (CommandIDGetNotificationAttributes). */
  notificationAttributeCommand[0] = CommandIDGetNotificationAttributes;
  /* Notification ID - from uint32_t to uint8_t*/
  notificationAttributeCommand[1] = notification->notifUID & 0x000000FF;
  notificationAttributeCommand[2] = (notification->notifUID & 0x0000FF00) >> 8;
  notificationAttributeCommand[3] = (notification->notifUID & 0x00FF0000) >> 16;
  notificationAttributeCommand[4] = (notification->notifUID & 0xFF000000) >> 24;
  /* App Identifier */
  notificationAttributeCommand[5+0] = NotificationAttributeIDAppIdentifier;
  /* Title */
  notificationAttributeCommand[5+1] = NotificationAttributeIDTitle;
  /* Title Length */
  notificationAttributeCommand[5+2] = ANCS_TITLE_MAX_LENGTH;
  notificationAttributeCommand[5+3] = 0;
  /* Subtitle */
  notificationAttributeCommand[5+4] = NotificationAttributeIDSubtitle;
  /* Subtitle Length */
  notificationAttributeCommand[5+5] = ANCS_SUBTITLE_MAX_LENGTH;
  notificationAttributeCommand[5+6] = 0;
  /* Message */
  notificationAttributeCommand[5+7] = NotificationAttributeIDMessage;
  /* Message Length */
  notificationAttributeCommand[5+8] = ANCS_MESSAGE_MAX_LENGTH_LSB;
  notificationAttributeCommand[5+9] = ANCS_MESSAGE_MAX_LENGTH_MSB;
  /* Message Size */
  notificationAttributeCommand[5+10] = NotificationAttributeIDMessageSize;
  /* Date */
  notificationAttributeCommand[5+11] = NotificationAttributeIDDate;
  /* Positive Action Label */
  notificationAttributeCommand[5+12] = NotificationAttributeIDPositiveActionLabel;
  /* Negative Action Label */
  notificationAttributeCommand[5+13] = NotificationAttributeIDNegativeActionLabel;

  /* Write specific commands to the Control Point characteristic to retrieve information about the notification */
  tBleStatus result = aci_gatt_clt_write(a_ClientContext[0].connHdl,
                                         BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                         a_ClientContext[0].ANCSControlPointCharHdl+1, 
                                         (uint16_t)(1 + 4 + COMMAND_DATA_SIZE), 
                                         notificationAttributeCommand);
  if (result != BLE_STATUS_SUCCESS)
  {
      APP_DBG_MSG("Failed to send GetNotificationAttributesCommand, status = 0x%02X\n", result);
  }
  else
  {
      APP_DBG_MSG("GetNotificationAttributesCommand sent successfully\n");
      APP_DBG_MSG("aci_gatt_clt_write Waiting for response...\n");
      a_ClientContext[0].state = GATT_CLIENT_APP_GET_NOTIFICATION_ATTRIBUTES_WAITING;
      gatt_cmd_resp_wait();
      APP_DBG_MSG("aci_gatt_clt_write Response received\n");
  }
  
}


/**
 * @brief  This event is generated when a Find By Type Value Response is received from the server.
 *         The response contains information about the handles that match the requested attribute type and value.
 * @param  p_event Pointer to the event structure containing the response data.
 * @retval None
 */
void aci_att_clt_find_by_type_value_resp_event(aci_att_clt_find_by_type_value_resp_event_rp0 *p_event)
{
  if (a_ClientContext[0].connStatus == APP_BLE_ANCS_STATE_WAITING_DISC_SERVICE)
  {
    a_ClientContext[0].ANCSservicePresent = TRUE;
    if ((p_event->Connection_Handle != a_ClientContext[0].connHdl) || (p_event->Num_of_Handle_Pair != 1))
    {
      a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
      a_ClientContext[0].ANCSerror_proc = ERROR_MORE_THAN_ONE_ANCS_PRESENT;
      return;
    }
    a_ClientContext[0].ANCSServiceHdl = p_event->Attribute_Group_Handle_Pair[0].Found_Attribute_Handle;
    a_ClientContext[0].ANCSServiceEndHdl = p_event->Attribute_Group_Handle_Pair[0].Group_End_Handle;
    a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_SERVICE_SUCCESS;
  }
}

/**
 * @brief This event can be generated during a "Discover Characteristics By UUID"
 * procedure or a "Read using Characteristic UUID" procedure.
 *
 * During a "Discover Characteristics By UUID" procedure, Attribute_Value is a
 * characteristic declaration as defined in Bluetooth Core spec (vol.3, Part G, ch. 3.3.1),
 * i.e. it is composed by: Characteristic Properties (1 octet), Characteristic Value Handle
 * (2 octets) and Characteristic UUID (2 or 16 octets). During a "Read using Characteristic UUID"
 * procedure, Attribute_Value is the value of the characteristic.
 *
 * @param p_event Pointer to the event structure containing the response data.
 *
 * @retval None
 */
void aci_gatt_clt_disc_read_char_by_uuid_resp_event(aci_gatt_clt_disc_read_char_by_uuid_resp_event_rp0 *p_event)
{
  if (p_event->Connection_Handle != a_ClientContext[0].connHdl)
  {
    a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
    a_ClientContext[0].ANCSerror_proc = ERROR_WRONG_CONN_HANDLE;
    return;
  }

  APP_DBG_MSG(">>== aci_gatt_clt_disc_read_char_by_uuid_resp_event\n");

  switch (a_ClientContext[0].connStatus)
  {
  case APP_BLE_ANCS_STATE_WAITING_DISC_NOTIFICATION:
    /* Discovery Notification Source */
    /* 0x10 means that a notification is present */
    if ((p_event->Attribute_Value[0] & 0x10) == 0)
    {
      a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
      a_ClientContext[0].ANCSerror_proc = ERROR_WRONG_CHARAC_PROP;
      return;
    }
    a_ClientContext[0].ANCSNotificationSourceCharHdl = p_event->Attribute_Handle;
    a_ClientContext[0].ANCSNotificationSourceValueHdl = p_event->Attribute_Value[0];
    a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_NOTIFICATION_SUCCESS;
    break; /* ANCS_STATE_WAITING_DISC_NOTIFICATION*/
  case APP_BLE_ANCS_STATE_WAITING_DISC_CONTROL_POINT:
    /* Discovery Control Point */
    if ((p_event->Attribute_Value[0] & 0x08) == 0)
    {
      a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
      a_ClientContext[0].ANCSerror_proc = ERROR_WRONG_CHARAC_PROP;
      return;
    }
    a_ClientContext[0].ANCSControlPointCharHdl = p_event->Attribute_Handle;
    a_ClientContext[0].ANCSControlPointValueHdl = p_event->Attribute_Value[0];
    a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_CONTROL_POINT_SUCCESS;
    break; /* ANCS_STATE_WAITING_DISC_CONTROL_POINT*/
  case APP_BLE_ANCS_STATE_WAITING_DISC_DATA_SOURCE:
    /* Discovery Data Source */
    /* 0x10 means that a notification is present */
    if ((p_event->Attribute_Value[0] & 0x10) == 0)
    {
      a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_NOT_CONFIGURED;
      a_ClientContext[0].ANCSerror_proc = ERROR_WRONG_CHARAC_PROP;
      return;
    }
    a_ClientContext[0].ANCSDataSourceCharHdl = p_event->Attribute_Handle;
    a_ClientContext[0].ANCSDataSourceValueHdl = p_event->Attribute_Value[0];
    a_ClientContext[0].connStatus = APP_BLE_ANCS_STATE_WAITING_DISC_DATA_SOURCE_SUCCESS;
    break; /* ANCS_STATE_WAITING_DISC_DATA_SOURCE*/
	default:
		break;
  }

}

/**
 * @brief This event is generated in response to a Find Information Request during a
 * discovery procedure for all the characteristic descriptors.
 *
 * @param p_event Pointer to the event structure containing the response data.
 *
 * @retval None
 */
void aci_att_clt_find_info_resp_event(aci_att_clt_find_info_resp_event_rp0 *p_event)
{
  uint8_t i;
  uint16_t app_handle, app_uuid;

  if ((p_event->Format != 1) || (a_ClientContext[0].connHdl != p_event->Connection_Handle))
    return;

  for (i = 0; i < p_event->Event_Data_Length; i += 4)
  {
    app_handle = p_event->Handle_UUID_Pair[i] | (uint16_t)(p_event->Handle_UUID_Pair[i + 1] << 8);
    app_uuid = p_event->Handle_UUID_Pair[i + 2] | (uint16_t)(p_event->Handle_UUID_Pair[i + 3] << 8);

    if (app_uuid == CHAR_EXTENDED_PROP_DESC_UUID)
    {
      a_ClientContext[0].ANCSControlPointExtendedCharHdl = app_handle;
      /* APP_DBG_MSG(" ANCSControlPointExtendedCharHdl: 0x%04X\n", a_ClientContext[0].ANCSControlPointExtendedCharHdl); */
      break;
    }
  }
}

/**
 * @brief This event is generated in response to a Read Request.
 *
 * @param p_event Pointer to the structure containing the read response event data.
 *
 * This function handles the read response event by invoking the ANCS_ReadResp_Callback
 * with the connection handle, event data length, and attribute value from the event.
 *
 * @param p_event->Connection_Handle Connection handle related to the response.
 * @param p_event->Event_Data_Length Length of the attribute value data.
 * @param p_event->Attribute_Value The value of the attribute.
 */
void aci_att_clt_read_resp_event(aci_att_clt_read_resp_event_rp0 *p_event)
{
  if (a_ClientContext[0].connStatus == APP_BLE_ANCS_STATE_WAITING_READ_CTRLPOINT_EXTENDED_PROP_VALUE)
  {
    a_ClientContext[0].ANCSControlPointExtendedValueHdl = p_event->Attribute_Value[0] | (uint16_t)(p_event->Attribute_Value[1] << 8);
  }
}


/**
 * @brief Event handler for GATT client indication event.
 *        This function is called when an indication is received from the server.
 *        It confirms the indication by calling aci_gatt_clt_confirm_indication.
 * @param p_event Pointer to the event structure containing the connection handle.
 */
void aci_gatt_clt_indication_event(aci_gatt_clt_indication_event_rp0 *p_event)
{
  APP_DBG_MSG(">>== aci_gatt_clt_indication_event\n");
  aci_gatt_clt_confirm_indication(p_event->Connection_Handle, p_event->CID);
}

void printANCSNotification(ANCS_NotificationList_type *notification) 
{
    APP_DBG_MSG("Notification:\n");
    APP_DBG_MSG("  Used: %d\n", notification->used);
    APP_DBG_MSG("  Event ID: %d\n", notification->evID);
    APP_DBG_MSG("  Event Flags: %d\n", notification->evFlag);
    APP_DBG_MSG("  Category ID: %d\n", notification->catID);
    APP_DBG_MSG("  Category Count: %d\n", notification->catCount);
    APP_DBG_MSG("  Notification UID: 0x%08X\n", notification->notifUID);

    APP_DBG_MSG("  Notification Data:\n");
    APP_DBG_MSG("    App Identifier: ");
    for (int i = 0; i < 0x13; i++) {
        APP_DBG_MSG("%c", notification->notifData.appIdentifier[i]);
    }
    APP_DBG_MSG("\n");

    APP_DBG_MSG("    Title: ");
    for (int i = 0; i < ANCS_TITLE_MAX_LENGTH; i++) {
        APP_DBG_MSG("%c", notification->notifData.idTitle[i]);
    }
    APP_DBG_MSG("\n");

    APP_DBG_MSG("    Subtitle: ");
    for (int i = 0; i < ANCS_SUBTITLE_MAX_LENGTH; i++) {
        APP_DBG_MSG("%c", notification->notifData.idSubtitle[i]);
    }
    APP_DBG_MSG("\n");

    APP_DBG_MSG("    Message: ");
    for (int i = 0; i < ANCS_MESSAGE_MAX_LENGTH; i++) {
        APP_DBG_MSG("%c", notification->notifData.idMessage[i]);
    }
    APP_DBG_MSG("\n");

    APP_DBG_MSG("    Date: ");
    for (int i = 0; i < 15; i++) {
        APP_DBG_MSG("%c", notification->notifData.idDate[i]);
    }
    APP_DBG_MSG("\n");


    APP_DBG_MSG("    Positive Action Label: ");
    for (int i = 0; i < 0x13; i++) {
        APP_DBG_MSG("%c", notification->notifData.idPositiveActionLabel[i]);
    }
    APP_DBG_MSG("\n");

    APP_DBG_MSG("    Negative Action Label: ");
    for (int i = 0; i < 10; i++) {
        APP_DBG_MSG("%c", notification->notifData.idNegativeActionLabel[i]);
    }
    APP_DBG_MSG("\n");
    
}

uint8_t index_current_notification=0;
uint32_t current_notification_uid = 0x0;

void GATT_Button1Action(void)
{
  uint32_t i=0;
  
  while(1)
  {
    
    if(index_current_notification==MAX_ANCS_NOTIFICATION)
    {
      index_current_notification = 0;
    }

    if(ancs_notification_list[index_current_notification].notifUID == current_notification_uid)
    {
      index_current_notification++;
    }
    
    if(ancs_notification_list[index_current_notification].used == USED)
    {
      current_notification_uid = ancs_notification_list[index_current_notification].notifUID;
      printANCSNotification(&ancs_notification_list[index_current_notification]);
      APP_DBG_MSG("Press push button 1 to show next notification.\n");
      APP_DBG_MSG("Press push button 2 to perform the Positive Action [%s] on the 0x%08X UID.\n", ancs_notification_list[index_current_notification].notifData.idPositiveActionLabel ,current_notification_uid);
      APP_DBG_MSG("Press push button 3 to perform the Negative Action [%s] on the 0x%08X UID.\n", ancs_notification_list[index_current_notification].notifData.idNegativeActionLabel, current_notification_uid);
      break;
    }
    
    i++;
    index_current_notification++;
    
    if(i==MAX_ANCS_NOTIFICATION)
    {
      index_current_notification = 0;
      current_notification_uid = 0;
      break;
    }
    
  }
  return;
}

void GATT_Button2Action(void)
{
  if(ancs_notification_list[index_current_notification].used == USED)
  {
    if(ancs_notification_list[index_current_notification].notifUID == current_notification_uid)
    {
      APP_DBG_MSG("Positive Action [%s] on the 0x%08X UID.\n", ancs_notification_list[index_current_notification].notifData.idPositiveActionLabel , current_notification_uid);
      uint8_t notificationAction[1 + 4 + 1] = {0};
      
      notificationAction[0] = CommandIDPerformNotificationAction;
      notificationAction[1] =  current_notification_uid & 0x000000FF;
      notificationAction[2] = (current_notification_uid & 0x0000FF00) >> 8;
      notificationAction[3] = (current_notification_uid & 0x00FF0000) >> 16;
      notificationAction[4] = (current_notification_uid & 0xFF000000) >> 24;
      notificationAction[5] = ActionIDPositive;

      /* Write specific commands to the Control Point characteristic to retrieve information about the notification */
      tBleStatus result = aci_gatt_clt_write(a_ClientContext[0].connHdl,
                                             BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                             a_ClientContext[0].ANCSControlPointCharHdl+1, 
                                             (uint16_t)(1 + 4 + 1), 
                                             notificationAction);
      if (result != BLE_STATUS_SUCCESS)
      {
          APP_DBG_MSG("Failed to send PerformNotificationAction, status = 0x%02X\n", result);
      }
      else
      {
          APP_DBG_MSG("PerformNotificationAction sent successfully\n");
          APP_DBG_MSG("aci_gatt_clt_write Waiting for response...\n");
          a_ClientContext[0].state = GATT_CLIENT_APP_GET_NOTIFICATION_ATTRIBUTES_WAITING;
          gatt_cmd_resp_wait();
          APP_DBG_MSG("aci_gatt_clt_write Response received\n");
      }
    }
  }
  return;
}

void GATT_Button3Action(void)
{
  if(ancs_notification_list[index_current_notification].used == USED)
  {
    if(ancs_notification_list[index_current_notification].notifUID == current_notification_uid)
    {
      APP_DBG_MSG("Negative Action [%s] on the 0x%08X UID.\n", ancs_notification_list[index_current_notification].notifData.idNegativeActionLabel , current_notification_uid);
      uint8_t notificationAction[1 + 4 + 1] = {0};
      
      notificationAction[0] = CommandIDPerformNotificationAction;
      notificationAction[1] =  current_notification_uid & 0x000000FF;
      notificationAction[2] = (current_notification_uid & 0x0000FF00) >> 8;
      notificationAction[3] = (current_notification_uid & 0x00FF0000) >> 16;
      notificationAction[4] = (current_notification_uid & 0xFF000000) >> 24;
      notificationAction[5] = ActionIDNegative;

      /* Write specific commands to the Control Point characteristic to retrieve information about the notification */
      tBleStatus result = aci_gatt_clt_write(a_ClientContext[0].connHdl,
                                             BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                             a_ClientContext[0].ANCSControlPointCharHdl+1, 
                                             (uint16_t)(1 + 4 + 1), 
                                             notificationAction);
      if (result != BLE_STATUS_SUCCESS)
      {
          APP_DBG_MSG("Failed to send PerformNotificationAction, status = 0x%02X\n", result);
      }
      else
      {
          APP_DBG_MSG("PerformNotificationAction sent successfully\n");
          APP_DBG_MSG("aci_gatt_clt_write Waiting for response...\n");
          a_ClientContext[0].state = GATT_CLIENT_APP_GET_NOTIFICATION_ATTRIBUTES_WAITING;
          gatt_cmd_resp_wait();
          APP_DBG_MSG("aci_gatt_clt_write Response received\n");
      }
    }
  }

  return;
}


/* USER CODE END LF */
