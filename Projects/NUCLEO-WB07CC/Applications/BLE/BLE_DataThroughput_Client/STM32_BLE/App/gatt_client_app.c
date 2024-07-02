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
#include "stm32wb0x_hal_radio_timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

typedef struct
{
  /**
   * ID of the Write timeout
   */
  VTIMER_HandleType TimerDataThroughput_Id;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} DT_CLI_APP_Context_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */
  uint8_t pPayload_n_1;
  uint8_t pPayload_n;
  uint8_t Length32;
  /* USER CODE END Service1_Data_t */

} DT_Data_FromServer_t;
/* USER CODE END PTD */

typedef enum
{
  NOTIFICATION_INFO_RECEIVED_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Opcode_t */

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

  APP_BLE_ConnStatus_t connStatus;
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
  /* handles of DT service */
  uint16_t DTServiceHdl;
  uint16_t DTServiceEndHdl;

  /* handles of Tx characteristic - Notification from server */
  uint16_t DTTXCharHdle;
  uint16_t DTTXValueHdle;
  uint16_t DTTXDescHdl;

  /* handles of Rx characteristic - write to server */
  uint16_t DTRXCharHdle;
  uint16_t DTRXValueHdle;
  
  /* handles of DT DataThroughput characteristic - Notification from server, used to send result of write data received by server */
  uint16_t DTThroughputCharHdle;
  uint16_t DTThroughputValueHdle;
  uint16_t DTThroughputDescHdl;
  
  DTC_STM_Payload_t TxData;
  DTC_App_Transfer_Req_Status_t NotificationTransferReq;
  DTC_App_Transfer_Req_Status_t ButtonTransferReq;
  DTC_App_Flow_Status_t DtFlowStatus;
  uint8_t connectionstatus;
  uint16_t MTUSizeValue;
/* USER CODE END BleClientAppContext_t */

}BleClientAppContext_t;

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DT_TX_SERV_UUID                                               (0xFE80)
#define DT_TX_CHAR_UUID                                               (0xFE81)
#define DT_RX_CHAR_UUID                                               (0xFE82)
#define DT_THROUGHPUT_CHAR_UUID                                       (0xFE83)

#define TIMEUNIT  1
     
#define DATA_NOTIFICATION_MAX_PACKET_SIZE     244
/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static BleClientAppContext_t a_ClientContext[BLE_CFG_MAX_NBR_GATT_EVT_HANDLERS];
static uint16_t gattCharStartHdl = 0;
static uint16_t gattCharValueHdl = 0;

/* USER CODE BEGIN PV */
static uint8_t Notification_Data_Buffer[DATA_NOTIFICATION_MAX_PACKET_SIZE]; /* DATA_NOTIFICATION_MAX_PACKET_SIZE data + CRC */
static DT_CLI_APP_Context_t DT_CLI_APP_Context;
static uint16_t packet_lost;
static uint32_t DataThroughputValue;
static uint32_t packet_lost_local;
static DT_CLI_NotificationEvt_t NotificationData;
DTC_Context_t DTC_Context;
uint32_t dtc_n=0;
uint32_t DataTransfered = 0;
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/

static BLEEVT_EvtAckStatus_t P2P_CLIENT_EventHandler(aci_blecore_event *p_evt);
static void gatt_parse_services(aci_att_clt_read_by_group_type_resp_event_rp0 *p_evt);
static void gatt_parse_services_by_UUID(aci_att_clt_find_by_type_value_resp_event_rp0 *p_evt);
static void gatt_parse_chars(aci_att_clt_read_by_type_resp_event_rp0 *p_evt);
static void gatt_parse_descs(aci_att_clt_find_info_resp_event_rp0 *p_evt);
static void gatt_parse_notification(aci_gatt_clt_notification_event_rp0 *p_evt);
static void gatt_Notification(GATT_CLIENT_APP_Notification_evt_t *p_Notif);
static void client_discover_all(void);
static void gatt_cmd_resp_release(void);
static void gatt_cmd_resp_wait(void);
/* USER CODE BEGIN PFP */
static void SendDataWrite(void);
static void DataThroughputNotif_proc(void *arg);
static void DataT_Notification_Data( void );
static void BLE_SVC_GAP_Security_Req(void);
static void Resume_Write(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void GATT_CLIENT_APP_Init(void)
{
  uint8_t index =0;
  /* USER CODE BEGIN GATT_CLIENT_APP_Init_1 */
  uint8_t i;
  /* USER CODE END GATT_CLIENT_APP_Init_1 */

  for(index = 0; index < BLE_CFG_MAX_NBR_GATT_EVT_HANDLERS; index++)
  {
    a_ClientContext[index].connStatus = APP_BLE_IDLE;
  }

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(P2P_CLIENT_EventHandler);

  /* Register a task allowing to discover all services and characteristics and enable all notifications */
  UTIL_SEQ_RegTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, UTIL_SEQ_RFU, client_discover_all);

  /* USER CODE BEGIN GATT_CLIENT_APP_Init_2 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_WRITE_DATA_WO_RESP_ID, UTIL_SEQ_RFU, SendDataWrite);
  UTIL_SEQ_RegTask(1U << CFG_TASK_SECURITY_REQ_ID, UTIL_SEQ_RFU, BLE_SVC_GAP_Security_Req); 

  DT_CLI_APP_Context.TimerDataThroughput_Id.callback = DataThroughputNotif_proc;
  
  UTIL_SEQ_RegTask(1U << CFG_TASK_DATA_FROM_SERVER_ID, UTIL_SEQ_RFU, DataT_Notification_Data);

  /**
   * Initialize data buffer
   */
  for (i = 0 ; i < (DATA_NOTIFICATION_MAX_PACKET_SIZE - 1) ; i++)
  {
    Notification_Data_Buffer[i] = i;
  }

  a_ClientContext[0].state = GATT_CLIENT_APP_IDLE;
  a_ClientContext[0].connHdl = 0xFFFF;
  DTC_Context.connectionstatus = APP_BLE_IDLE;
  DTC_Context.NotificationTransferReq = DTC_APP_TRANSFER_REQ_OFF;
  DTC_Context.ButtonTransferReq = DTC_APP_TRANSFER_REQ_OFF;
  DTC_Context.DtFlowStatus = DTC_APP_FLOW_ON;

#if (CFG_DEBUG_APP_TRACE==1)
  APP_DBG_MSG("-- DT CLIENT INITIALIZED \n");
#else
  DT_INFO_MSG("-- DT CLIENT INITIALIZED \n");  
#endif 
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
      /* USER CODE BEGIN PEER_CONN_HANDLE_EVT */

      /* USER CODE END PEER_CONN_HANDLE_EVT */
      break;

    case PEER_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */

      /* USER CODE END PEER_DISCON_HANDLE_EVT */
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

uint8_t GATT_CLIENT_APP_Set_Conn_Handle(uint8_t index, uint16_t connHdl)
{
  uint8_t ret;

  if (index < BLE_CFG_MAX_NBR_GATT_EVT_HANDLERS)
  {
    a_ClientContext[index].connHdl = connHdl;
    ret = 0;
  }
  else
  {
    ret = 1;
  }

  return ret;
}

uint8_t GATT_CLIENT_APP_Get_State(uint8_t index)
{
  return a_ClientContext[index].state;
}

void GATT_CLIENT_APP_Discover_services(uint8_t index)
{
  GATT_CLIENT_APP_Procedure_Gatt(index, PROC_GATT_DISC_ALL_PRIMARY_SERVICES);
  GATT_CLIENT_APP_Procedure_Gatt(index, PROC_GATT_DISC_ALL_CHARS);
  GATT_CLIENT_APP_Procedure_Gatt(index, PROC_GATT_DISC_ALL_DESCS);
  GATT_CLIENT_APP_Procedure_Gatt(index, PROC_GATT_ENABLE_ALL_NOTIFICATIONS);

  return;
}

uint8_t GATT_CLIENT_APP_Procedure_Gatt(uint8_t index, ProcGattId_t GattProcId)
{
  tBleStatus result = BLE_STATUS_SUCCESS;
  uint8_t status;

  if (index >= BLE_CFG_MAX_NBR_GATT_EVT_HANDLERS)
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
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_SERVICES;

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
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_CHARACS;

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
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_WRITE_DESC;

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
        uint16_t enable = 0x0001; /* Buffer must be kept valid for aci_gatt_clt_write until a gatt procedure complete is received. */

        if (a_ClientContext[index].ServiceChangedCharDescHdl != 0x0000)
        {
          result = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                                      BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                      a_ClientContext[index].ServiceChangedCharDescHdl,
                                      2,
                                      (uint8_t *) &enable);
          gatt_cmd_resp_wait();
          APP_DBG_MSG(" ServiceChangedCharDescHdl =0x%04X\n",a_ClientContext[index].ServiceChangedCharDescHdl);
        }
        /* USER CODE BEGIN PROC_GATT_ENABLE_ALL_NOTIFICATIONS */
        if(a_ClientContext[index].DTTXDescHdl != 0x0000)
        {
          result |= aci_gatt_clt_write(a_ClientContext[index].connHdl,
                                       BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                       a_ClientContext[index].DTTXDescHdl,
                                       2,
                                       (uint8_t *) &enable);
          gatt_cmd_resp_wait();
          APP_DBG_MSG(" DTTXDescHdl =0x%04X\n",a_ClientContext[index].DTTXDescHdl);
        }
        if(a_ClientContext[index].DTThroughputDescHdl != 0x0000)
        {
          result |= aci_gatt_clt_write(a_ClientContext[index].connHdl,
                                       BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                       a_ClientContext[index].DTThroughputDescHdl,
                                       2,
                                       (uint8_t *) &enable);
          gatt_cmd_resp_wait();
          APP_DBG_MSG(" DTThroughputDescHdl =0x%04X\n",a_ClientContext[index].DTThroughputDescHdl);
        }
        UTIL_SEQ_SetTask(1U << CFG_TASK_CONN_UPDATE_ID, CFG_SEQ_PRIO_0);        
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
static BLEEVT_EvtAckStatus_t P2P_CLIENT_EventHandler(aci_blecore_event *p_evt)
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

      uint8_t index;
      for (index = 0 ; index < BLE_CFG_MAX_NBR_GATT_EVT_HANDLERS ; index++)
      {
        if (a_ClientContext[index].connHdl == p_evt_rsp->Connection_Handle)
        {
          break;
        }
      }

      if (a_ClientContext[index].connHdl == p_evt_rsp->Connection_Handle)
      {
        gatt_cmd_resp_release();
      }
    }
    break;/* ACI_GATT_PROC_COMPLETE_VSEVT_CODE */
    case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
    {
    aci_att_exchange_mtu_resp_event_rp0 *tx_pool_available;
    tx_pool_available = (aci_att_exchange_mtu_resp_event_rp0 *)p_evt->data;
    UNUSED(tx_pool_available);
    /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          Resume_Write();
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
          if (exchange_mtu_resp->MTU < DATA_NOTIFICATION_MAX_PACKET_SIZE)
          {
            a_ClientContext[0].MTUSizeValue = exchange_mtu_resp->MTU  - 3;
          }
          else
          {
            a_ClientContext[0].MTUSizeValue = DATA_NOTIFICATION_MAX_PACKET_SIZE;
          }
          APP_DBG_MSG("  MTU_size = %d\n", a_ClientContext[0].MTUSizeValue);
          tBleStatus status;
          status = hci_le_set_data_length_api(a_ClientContext[0].connHdl,251,2120);
          if (status != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("  Fail   : set data length command   : error code: 0x%x \n\r", status);
          }
          else
          {
            APP_DBG_MSG("  Success: set data length command  \n\r");
          }    
    /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
    }
    break;
    /* USER CODE BEGIN BLECORE_EVT */
    /* Manage ACI_GATT_INDICATION_VSEVT_CODE occurring on Android 12 */   
    case ACI_GATT_CLT_INDICATION_VSEVT_CODE:
    {
      tBleStatus status = BLE_STATUS_FAILED;
      aci_gatt_clt_indication_event_rp0 *pr = (void*)p_evt->data;
      status = aci_gatt_clt_confirm_indication(pr->Connection_Handle, BLE_GATT_UNENHANCED_ATT_L2CAP_CID);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_confirm_indication command, result: 0x%x \n", status);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_confirm_indication command\n");
      }   
    }
    break; /* end ACI_GATT_NOTIFICATION_VSEVT_CODE */
    /* USER CODE END BLECORE_EVT */
    default:
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

  for (index = 0 ; index < BLE_CFG_MAX_NBR_GATT_EVT_HANDLERS ; index++)
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
      else if (uuid == DT_TX_SERV_UUID)
      {
        a_ClientContext[index].DTServiceHdl = ServiceStartHdl;
        a_ClientContext[index].DTServiceEndHdl = ServiceEndHdl;

        APP_DBG_MSG(", GENERIC_ATTRIBUTE_SERVICE_UUID found\n");
      }
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

  for (index = 0 ; index < BLE_CFG_MAX_NBR_GATT_EVT_HANDLERS ; index++)
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
        else if (uuid == DT_TX_CHAR_UUID)
        {
          a_ClientContext[index].DTTXCharHdle = CharStartHdl;
          a_ClientContext[index].DTTXValueHdle = CharValueHdl;
          APP_DBG_MSG(", GATT TX_CHARACTERISTIC_UUID charac found\n");
        }
        else if (uuid == DT_RX_CHAR_UUID)
        {
          a_ClientContext[index].DTRXCharHdle = CharStartHdl;
          a_ClientContext[index].DTRXValueHdle = CharValueHdl;
          APP_DBG_MSG(", GATT RX_CHARACTERISTIC_UUID charac found\n");
        }
        else if (uuid == DT_THROUGHPUT_CHAR_UUID)
        {
          a_ClientContext[index].DTThroughputCharHdle = CharStartHdl;
          a_ClientContext[index].DTThroughputValueHdle = CharValueHdl;
          APP_DBG_MSG(", GATT THROUGHPUT_CHARACTERISTIC_UUID charac found\n");
        } 
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

  APP_DBG_MSG("ACI_ATT_FIND_INFO_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
              p_evt->Connection_Handle);

  for (index = 0 ; index < BLE_CFG_MAX_NBR_GATT_EVT_HANDLERS ; index++)
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
        UNUSED(gattCharStartHdl);
        if (a_ClientContext[index].DTTXValueHdle == gattCharValueHdl)
        {
          a_ClientContext[index].DTTXDescHdl = handle;
          APP_DBG_MSG(", DTTX Client descriptor found\n");
        }
        if (a_ClientContext[index].DTThroughputValueHdle == gattCharValueHdl)
        {
          a_ClientContext[index].DTThroughputDescHdl = handle;
          APP_DBG_MSG(", DTThroughput Client descriptor found\n");
        }
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
  uint8_t CRC_Result;
  uint8_t CRC_Received;

  if ((p_evt->Attribute_Handle == a_ClientContext[0].DTTXValueHdle) && 
      (p_evt->Attribute_Value_Length > (2)))
  {
    NotificationData.DataTransfered.Length = p_evt->Attribute_Value_Length;
    NotificationData.DataTransfered.pPayload = (p_evt->Attribute_Value);
    NotificationData.DataTransfered.pPayload_n = *((uint8_t*) &(p_evt->Attribute_Value[0]));
    
    if (NotificationData.DataTransfered.pPayload_n >= (NotificationData.DataTransfered.pPayload_n_1 + 2))
    {
      packet_lost +=
        ((NotificationData.DataTransfered.pPayload_n - NotificationData.DataTransfered.pPayload_n_1) - 1);
    }
    /* CRC computation */
    CRC_Result = APP_BLE_ComputeCRC8((uint8_t*) (p_evt->Attribute_Value), (p_evt->Attribute_Value_Length) - 1);
    /* get low weight byte */
    CRC_Received = (uint8_t) (p_evt->Attribute_Value[a_ClientContext[0].MTUSizeValue - 1]);
    
    if (CRC_Received != CRC_Result)
    {
      dtc_n += 1;
      APP_DBG_MSG("** data error **  N= %d \r\n",(int32_t)dtc_n);
    }
    NotificationData.DataTransfered.pPayload_n_1 = NotificationData.DataTransfered.pPayload_n;
    if (DataTransfered == 0)
    {
      HAL_RADIO_TIMER_StartVirtualTimer(&(DT_CLI_APP_Context.TimerDataThroughput_Id), 1000);
    }
    DataTransfered += NotificationData.DataTransfered.Length;
  }
/* USER CODE END gatt_parse_notification_1 */

  return;
}

static void client_discover_all(void)
{
  GATT_CLIENT_APP_Discover_services(0);
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

void DTC_Button1TriggerReceived( void )
{
  if (DTC_Context.connectionstatus != APP_BLE_CONNECTED_CLIENT)
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_START_SCAN_ID, CFG_SEQ_PRIO_0);
  }
  else
  {
    if(DTC_Context.ButtonTransferReq != DTC_APP_TRANSFER_REQ_OFF)
    {
      BSP_LED_Off(LED_BLUE);
      DTC_Context.ButtonTransferReq = DTC_APP_TRANSFER_REQ_OFF;
    }
    else
    {
      BSP_LED_On(LED_BLUE);
      DTC_Context.ButtonTransferReq = DTC_APP_TRANSFER_REQ_ON;
      UTIL_SEQ_SetTask(1U << CFG_TASK_WRITE_DATA_WO_RESP_ID, CFG_SEQ_PRIO_1);
    }
  }
  BLEStack_Process_Schedule();
  
  return;
}

void DTC_Button2TriggerReceived( void )
{
  tBleStatus status;
  if (DTC_Context.connectionstatus != APP_BLE_CONNECTED_CLIENT)
  {
    status = aci_gap_clear_security_db();
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : Clear security DB 0x%x\n", status);
    }
    else
    {
      APP_DBG_MSG("  Success: Clear security DB \n");
    }
  }
  else
  {
    UTIL_SEQ_SetTask(1 << CFG_TASK_SECURITY_REQ_ID, CFG_SEQ_PRIO_0);
  }
  
  return;
}

void DTC_Button3TriggerReceived( void )
{
  
  return;
}

static void SendDataWrite( void )
{
  tBleStatus status = BLE_STATUS_INVALID_PARAMS;
  uint8_t crc_result;

  if( (DTC_Context.ButtonTransferReq != DTC_APP_TRANSFER_REQ_OFF) && 
      (DTC_Context.DtFlowStatus != DTC_APP_FLOW_OFF) )
  {   
    /*Data Packet to send to remote*/
    Notification_Data_Buffer[0] += 1;
    /* compute CRC */
    crc_result = APP_BLE_ComputeCRC8((uint8_t*) Notification_Data_Buffer, (a_ClientContext[0].MTUSizeValue - 1));
    Notification_Data_Buffer[a_ClientContext[0].MTUSizeValue - 1] = crc_result;

    DTC_Context.TxData.pPayload = Notification_Data_Buffer;
    DTC_Context.TxData.Length =  a_ClientContext[0].MTUSizeValue;

    status = aci_gatt_clt_write_without_resp(a_ClientContext[0].connHdl,                                          
                                             BLE_GATT_UNENHANCED_ATT_L2CAP_CID,   
                                             a_ClientContext[0].DTRXValueHdle,
                                             a_ClientContext[0].MTUSizeValue,
                                             (uint8_t*)(DTC_Context.TxData.pPayload));
    
    if (status == BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      DTC_Context.DtFlowStatus = DTC_APP_FLOW_OFF;
      (Notification_Data_Buffer[0])-=1;
    }
    else
    {
      BLEStack_Process_Schedule();
      /* Reschedule task to send data. Priority must be lower than or equal to BLEStack_Process()
         to avoid taking all the CPU time. */
      UTIL_SEQ_SetTask(1U << CFG_TASK_WRITE_DATA_WO_RESP_ID, CFG_SEQ_PRIO_1);
    }
  }

  return;
}

static void Resume_Write(void)
{
  DTC_Context.DtFlowStatus = DTC_APP_FLOW_ON;
  UTIL_SEQ_SetTask(1U << CFG_TASK_WRITE_DATA_WO_RESP_ID, CFG_SEQ_PRIO_1);
}

static void DataThroughputNotif_proc(void *arg)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_DATA_FROM_SERVER_ID, CFG_SEQ_PRIO_0);
}

static void DataT_Notification_Data( void )
{
  DataThroughputValue = (uint32_t)(DataTransfered / TIMEUNIT);
  packet_lost_local = (uint32_t)(packet_lost / TIMEUNIT);
  DataTransfered = 0;
  packet_lost = 0;
  
#if (CFG_DEBUG_APP_TRACE==1)
  APP_DBG_MSG("  DataThroughput = %d bytes/s lost = %d \n",DataThroughputValue, packet_lost_local);
#else
  DT_INFO_MSG("  DataThroughput = %d bytes/s lost = %d \n",DataThroughputValue, packet_lost_local);
#endif 
  
  return;
}

static void BLE_SVC_GAP_Security_Req( void )
{
  tBleStatus status;
  
  status = aci_gap_set_security(a_ClientContext[0].connHdl, GAP_SECURITY_LEVEL_3 , 0x01);
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

/* USER CODE END LF */
