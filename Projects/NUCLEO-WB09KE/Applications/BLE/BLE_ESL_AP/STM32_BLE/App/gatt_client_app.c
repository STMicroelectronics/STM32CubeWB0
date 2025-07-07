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
#include "otp_client.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "esl_profile_ap.h"
#include "time_ref.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
#define ECP_TIMEOUT_MS         (30 * 1000)  // 30 seconds
/* USER CODE END PTD */

typedef enum
{
  NOTIFICATION_INFO_RECEIVED_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Opcode_t */
  ESL_NOTIFICATION_INFO_RECEIVED_EVT
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

  uint16_t ServiceChangedCharHdl;
  uint16_t ServiceChangedCharValueHdl;
  uint16_t ServiceChangedCharDescHdl;

  /* USER CODE BEGIN BleClientAppContext_t */
  /* Handles of ESL service */
  uint16_t ESLServiceHdl;
  uint16_t ESLServiceEndHdl;

  /* handles of the Tx characteristic - Write To Server */
  /* Handles of ESL Address characteristic */
  uint16_t ESLAddressCharHdl;
  uint16_t ESLAddressValueHdl;
  
  /* Handles of AP Sync Material characteristic */
  uint16_t APSyncKeyMaterialCharHdl;
  uint16_t APSyncKeyMaterialValueHdl;
  
  /* Handles of ESL Resp Key Material characteristic */
  uint16_t ESLRespKeyMaterialCharHdl;
  uint16_t ESLRespKeyMaterialValueHdl;
  
  /* Handles of ESL Current Absolute Time characteristic */
  uint16_t ESLCurrAbsTimeCharHdl;
  uint16_t ESLCurrAbsTimeValueHdl;
  
  /* handles of the Rx characteristic - Notification From Server */  
  /* Handles of ESL Control Point characteristic */
  uint16_t ESLControlPointCharHdl;
  uint16_t ESLControlPointValueHdl;
  uint16_t ESLControlPointCCCDHdl;
    
  /* Handles of ESL Display Information characteristic */
  uint16_t ESLDisplayInfoCharHdl;
  uint16_t ESLDisplayInfoValueHdl;
  
  /* Handles of ESL Image Information characteristic */
  uint16_t ESLImageInfoCharHdl;
  uint16_t ESLImageInfoValueHdl;

  /* Handles of ESL LED Information characteristic */
  uint16_t ESLSensorInfoCharHdl;
  uint16_t ESLSensorInfoValueHdl;
  
  /* Handles of ESL LED Information characteristic */
  uint16_t ESLLedInfoCharHdl;
  uint16_t ESLLedInfoValueHdl;
  
  OTSHandleContext_t OTSHandles;
  
  uint8_t gatt_error_code;
  
  /* ESL Control Point (ECP) timeout timerID*/
  VTIMER_HandleType ECP_timer_Id;    
  
  uint16_t att_mtu;
  uint16_t read_char_len;
  uint8_t read_char[512];
  uint16_t read_char_offset;
  bool b_ECP_failed; 
/* USER CODE END BleClientAppContext_t */

}BleClientAppContext_t;

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static BleClientAppContext_t a_ClientContext[CFG_MAX_NUM_CONNECTED_SERVERS];
static uint16_t gattCharValueHdl = 0;

/* USER CODE BEGIN PV */

tListNode esl_bonded;

uint16_t esl_address_conn;      

ESL_PROFILE_KeyMaterial_t ap_sync_key_material_config_value = {
  .Session_Key = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15},      //16 bytes
  .IV = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07},       //8 bytes
};

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */
extern bool bUpdatingTransition;
/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/

static BLEEVT_EvtAckStatus_t ESL_APP_EventHandler(aci_blecore_event *p_evt);
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
static void context_init(uint8_t index);
static void ESL_AP_write_char(void);
static void ESL_AP_configuring_char(void);
static void ESL_AP_write_esl_address(void);
static void ESL_AP_ECP_Timeout(void *arg);
static void print_Info_Char(void);
static uint8_t ESL_APP_Read_Long_Char(uint16_t ValueHdl, uint16_t Offset);
static ESL_PROFILE_KeyMaterial_t ESL_APP_Return_KeyMaterial_Value(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void GATT_CLIENT_APP_Init(void)
{
  uint8_t index = 0;
  /* USER CODE BEGIN GATT_CLIENT_APP_Init_1 */

  /* USER CODE END GATT_CLIENT_APP_Init_1 */

  for(index = 0; index < CFG_MAX_NUM_CONNECTED_SERVERS; index++)
  {
    a_ClientContext[index].connStatus = APP_BLE_IDLE;
  }

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(ESL_APP_EventHandler);

  /* Register a task allowing to discover all services and characteristics and enable all notifications */
  UTIL_SEQ_RegTask(1U << CFG_TASK_DISCOVER_SERVICES_ID, UTIL_SEQ_RFU, client_discover_all);

  /* USER CODE BEGIN GATT_CLIENT_APP_Init_2 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_ESL_AP_WRITE_CHARS_ID, UTIL_SEQ_RFU, ESL_AP_write_char);
  
  context_init(0);
  
  TIMEREF_SetAbsoluteTime(0);
  
  OTP_CLIENT_Init();
  
  /* USER CODE END GATT_CLIENT_APP_Init_2 */
  return;
}

static void context_init(uint8_t index)
{                                   
  memset(&a_ClientContext[index], 0, sizeof(BleClientAppContext_t));
  a_ClientContext[index].state = GATT_CLIENT_APP_IDLE;
  a_ClientContext[index].connHdl = 0xFFFF;
  
  /* When the AP writes to the ECP, the AP shall start a timer with the value 
     set to the ESL Control Point Timeout period (30 seconds). If the timer 
     expires, then the ECP procedure shall be considered to have failed. */
  a_ClientContext[index].ECP_timer_Id.callback = ESL_AP_ECP_Timeout;
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
    {  
      uint8_t index = 0;
      
      context_init(index);
      
      a_ClientContext[index].state = GATT_CLIENT_APP_CONNECTED;
      a_ClientContext[index].connHdl = p_Notif->ConnHdl;      
      a_ClientContext[0].att_mtu = 23;      
      OTP_CLIENT_ConnectionComplete(&a_ClientContext[0].OTSHandles, a_ClientContext[0].connHdl);  
    }  
      /* USER CODE END PEER_CONN_HANDLE_EVT */
      break;

    case PEER_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */
    {
      uint8_t index = 0;

      while(index < CFG_MAX_NUM_CONNECTED_SERVERS)
      {
        if(a_ClientContext[index].connHdl == p_Notif->ConnHdl)
        {
          a_ClientContext[index].state = GATT_CLIENT_APP_IDLE;
          a_ClientContext[index].connHdl = 0xFFFF;
          HAL_RADIO_TIMER_StopVirtualTimer(&a_ClientContext[index].ECP_timer_Id);
          
          break;
        }
        index++;
      }
    }
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
#ifndef PTS_OTP
  if (!bUpdatingTransition)
  {  
    UTIL_SEQ_SetTask( 1u << CFG_TASK_ESL_AP_WRITE_CHARS_ID, CFG_SEQ_PRIO_0);
  }
#endif
  
  return;
}

uint8_t GATT_CLIENT_APP_Procedure_Gatt(uint8_t index, ProcGattId_t GattProcId)
{
  tBleStatus result = BLE_STATUS_SUCCESS;
  uint8_t status;

  if (index >= CFG_MAX_NUM_CONNECTED_SERVERS)
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

        APP_DBG_MSG("\nGATT services discovery\n");
        result = aci_gatt_clt_disc_all_primary_services(a_ClientContext[index].connHdl, BLE_GATT_UNENHANCED_ATT_L2CAP_CID);

        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
        }
        else
        {
          APP_DBG_MSG("aci_gatt_clt_disc_all_primary_services fail, status 0x%02X\n\n", result);
        }
      }

      break; /* PROC_GATT_DISC_ALL_PRIMARY_SERVICES */

      case PROC_GATT_DISC_ALL_CHARS:
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_CHARACS;

        APP_DBG_MSG("\nDiscover all Characteristics (handles [0x%04X - 0x%04X])\n",
                          0x0001,
                          0xFFFF);
        result = aci_gatt_clt_disc_all_char_of_service(
                           a_ClientContext[index].connHdl,
                           BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           0x0001,
                           0xFFFF);
        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
        }
        else
        {
          APP_DBG_MSG("aci_gatt_clt_disc_all_char_of_service fail, status 0x%02X\n\n", result);
        }
      }
      break; /* PROC_GATT_DISC_ALL_CHARS */

      case PROC_GATT_DISC_ALL_DESCS:
      {
        a_ClientContext[index].state = GATT_CLIENT_APP_DISCOVER_WRITE_DESC;

        APP_DBG_MSG("\nDiscover all Characteristics Descriptors [0x%04X - 0x%04X]\n",
                         0x0001,
                         0xFFFF);
        result = aci_gatt_clt_disc_all_char_desc(
                           a_ClientContext[index].connHdl,
			   BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           0x0001,
                           0xFFFF);
        if (result == BLE_STATUS_SUCCESS)
        {
          gatt_cmd_resp_wait();
        }
        else
        {
          APP_DBG_MSG("aci_gatt_clt_disc_all_char_desc fail, status 0x%02X\n\n", result);
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
        if(a_ClientContext[index].ESLControlPointCCCDHdl != 0x0000)
        {
          enable = 0x0001;
          APP_DBG_MSG("Enable notifications on ECP (handle 0x%04X)\n", a_ClientContext[index].ESLControlPointCCCDHdl);
          result = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                                      BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                      a_ClientContext[index].ESLControlPointCCCDHdl,
                                      2,
                                      (uint8_t *) &enable);
          gatt_cmd_resp_wait();
        }
        if(a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl != 0x0000)
        {
          enable = 0x0002;
          APP_DBG_MSG("Enable indications on OACP (handle 0x%04X)\n", a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl);
          result = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                                      BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                      a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl,
                                      2,
                                      (uint8_t *) &enable);
          gatt_cmd_resp_wait();
        }
        if(a_ClientContext[index].OTSHandles.ObjListCPCCCDHdl != 0x0000)
        {
          enable = 0x0002;
          APP_DBG_MSG("Enable indications on OLCP (handle 0x%04X)\n", a_ClientContext[index].OTSHandles.ObjListCPCCCDHdl);
          result = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                                      BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                      a_ClientContext[index].OTSHandles.ObjListCPCCCDHdl,
                                      2,
                                      (uint8_t *) &enable);
          gatt_cmd_resp_wait();
        }
        /* USER CODE END PROC_GATT_ENABLE_ALL_NOTIFICATIONS */

        if ((result == BLE_STATUS_SUCCESS) && (a_ClientContext[0].gatt_error_code == 0))
        {
          APP_DBG_MSG("Notifications enabled successfully\n");
        }
        else
        {
          APP_DBG_MSG("Enabling notifications failed, status 0x%02X, gatt error code=0x%02X\n", result, a_ClientContext[0].gatt_error_code);
        }
        a_ClientContext[0].gatt_error_code = 0;
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
static BLEEVT_EvtAckStatus_t ESL_APP_EventHandler(aci_blecore_event *p_evt)
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
  case ACI_GATT_CLT_INDICATION_VSEVT_CODE:
    {
      aci_gatt_clt_indication_event_rp0 *p_evt_rsp = (void*)p_evt->data;
      
      APP_DBG_MSG("ACI_GATT_CLT_INDICATION_VSEVT_CODE\n");
      if(p_evt_rsp->Attribute_Handle == a_ClientContext[0].ServiceChangedCharValueHdl)
      {
        APP_DBG_MSG("Service Changed Indication\n");
        
        UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);
      }      
      else if(p_evt_rsp->Attribute_Handle == a_ClientContext[0].OTSHandles.ObjListCPValueHdl)
      {
        return_value = BLEEVT_Ack;
        
        OTP_CLIENT_OLCPIndication(p_evt_rsp->Attribute_Value, p_evt_rsp->Attribute_Value_Length);
      }
      else if(p_evt_rsp->Attribute_Handle == a_ClientContext[0].OTSHandles.ObjActionCPValueHdl)
      {
        return_value = BLEEVT_Ack;
        
        OTP_CLIENT_OACPIndication(p_evt_rsp->Attribute_Value, p_evt_rsp->Attribute_Value_Length);
      }
      
      aci_gatt_clt_confirm_indication(p_evt_rsp->Connection_Handle, p_evt_rsp->CID);
    }
    break;    
    case ACI_GATT_CLT_PROC_COMPLETE_VSEVT_CODE:
    {
      aci_gatt_clt_proc_complete_event_rp0 *p_evt_rsp = (void*)p_evt->data;

      uint8_t index = 0;
      
      gatt_cmd_resp_release();
      a_ClientContext[index].gatt_error_code = p_evt_rsp->Error_Code;
      APP_DBG_MSG("GATT procedure complete: 0x%02x\n", p_evt_rsp->Error_Code);
    }
    break;/* ACI_GATT_PROC_COMPLETE_VSEVT_CODE */
    case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
    {
      aci_gatt_tx_pool_available_event_rp0 *tx_pool_available;
      tx_pool_available = (aci_gatt_tx_pool_available_event_rp0 *)p_evt->data;
      UNUSED(tx_pool_available);
      /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
      UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_TX_POOL_AVAILABLE_EVENT);
      /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
    }
    break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
    case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
    {
      aci_att_exchange_mtu_resp_event_rp0 * exchange_mtu_resp;
      exchange_mtu_resp = (aci_att_exchange_mtu_resp_event_rp0 *)p_evt->data;
      APP_DBG_MSG("  MTU exchanged size = %d\n",exchange_mtu_resp->MTU );
      a_ClientContext[0].att_mtu = exchange_mtu_resp->MTU;
      /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

      /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
    }
    break;
    /* USER CODE BEGIN BLECORE_EVT */
    case ACI_GATT_CLT_ERROR_RESP_VSEVT_CODE:
    {  
      aci_gatt_clt_error_resp_event_rp0 * clt_error_resp;
      clt_error_resp = (aci_gatt_clt_error_resp_event_rp0 *)p_evt->data;
      APP_DBG_MSG("  GATT Error Response code: 0x%02x \n", clt_error_resp->Error_Code);
      UNUSED(clt_error_resp);
    }
    break; 
    
    case ACI_ATT_CLT_READ_RESP_VSEVT_CODE:
    {
      aci_att_clt_read_resp_event_rp0 * clt_read_resp;
      clt_read_resp = (aci_att_clt_read_resp_event_rp0 *)p_evt->data;
      //APP_DBG_MSG("  GATT Read Response length: 0x%02x \n", clt_read_resp->Event_Data_Length);
      a_ClientContext[0].read_char_len = clt_read_resp->Event_Data_Length;
      memcpy(a_ClientContext[0].read_char, clt_read_resp->Attribute_Value, a_ClientContext[0].read_char_len);
      a_ClientContext[0].read_char_offset = a_ClientContext[0].read_char_len;
    }    
    break;
    
    case ACI_ATT_CLT_READ_BLOB_RESP_VSEVT_CODE:
    {
      aci_att_clt_read_blob_resp_event_rp0 * clt_read_resp;
      clt_read_resp = (aci_att_clt_read_blob_resp_event_rp0 *)p_evt->data;
      APP_DBG_MSG("  GATT Read Blob Response length: 0x%02x \n", clt_read_resp->Event_Data_Length);
      a_ClientContext[0].read_char_len += clt_read_resp->Event_Data_Length;
      memcpy(a_ClientContext[0].read_char + a_ClientContext[0].read_char_offset, clt_read_resp->Attribute_Value, a_ClientContext[0].read_char_len);
      a_ClientContext[0].read_char_offset = a_ClientContext[0].read_char_len;
    }
    break;
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
    case ESL_NOTIFICATION_INFO_RECEIVED_EVT:
      {
        //ECP notify: response of ESL to AT command sent writing the ECP      
        uint8_t index = 0;
        ECP_respCB(a_ClientContext[index].connHdl, p_Notif->DataTransfered.p_Payload); 
      }
      break;
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

//  APP_DBG_MSG("ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
//                p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_MAX_NUM_CONNECTED_SERVERS ; index++)
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
      APP_DBG_MSG("UUID=0x%04X, handle [0x%04X - 0x%04X]", uuid, ServiceStartHdl,ServiceEndHdl);

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
      else if (uuid == ESL_SERVICE_UUID)
      {
        a_ClientContext[index].ESLServiceHdl = ServiceStartHdl;
        a_ClientContext[index].ESLServiceEndHdl = ServiceEndHdl;

        APP_DBG_MSG(", ESL_SERVICE_UUID found\n");
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

//  APP_DBG_MSG("ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
//                p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_MAX_NUM_CONNECTED_SERVERS ; index++)
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

    //APP_DBG_MSG("  ConnHdl=0x%04X, number of value pair = %d\n", a_ClientContext[index].connHdl, numHdlValuePair);
    /* Loop on number of attribute value tuples */
    for (i = 0; i < numHdlValuePair; i++)
    {
      CharStartHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset - 5]);
      CharProperties = p_evt->Handle_Value_Pair_Data[uuid_offset - 3];
      CharValueHdl = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset - 2]);
      uuid = UNPACK_2_BYTE_PARAMETER(&p_evt->Handle_Value_Pair_Data[uuid_offset + uuid_short_offset]);

      if ( (uuid != 0x0) && (CharProperties != 0x0) && (CharStartHdl != 0x0) && (CharValueHdl != 0) )
      {
        APP_DBG_MSG("UUID=0x%04X, Properties=0x%04X, CharHandle [0x%04X - 0x%04X]", uuid, CharProperties, CharStartHdl, CharValueHdl);

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
          a_ClientContext[index].ServiceChangedCharHdl = CharStartHdl;
          a_ClientContext[index].ServiceChangedCharValueHdl = CharValueHdl;
          APP_DBG_MSG(", GATT SERVICE_CHANGED_CHARACTERISTIC_UUID charac found\n");
        }
/* USER CODE BEGIN gatt_parse_chars_1 */
#ifndef PTS_OTP
        else if (uuid == ESL_CONTROL_POINT_UUID)
        {
          a_ClientContext[index].ESLControlPointCharHdl = CharStartHdl;
          a_ClientContext[index].ESLControlPointValueHdl = CharValueHdl;
          APP_DBG_MSG(", ESL_CONTROL_POINT_UUID charac found\n");
        }
        else if (uuid == ESL_ADDRESS_UUID)
        {
          a_ClientContext[index].ESLAddressCharHdl = CharStartHdl;
          a_ClientContext[index].ESLAddressValueHdl = CharValueHdl;
          APP_DBG_MSG(", ESL_ADDRESS_UUID charac found\n");
        }
        else if (uuid == AP_SYNC_KEY_MATERIAL_UUID)
        {
          a_ClientContext[index].APSyncKeyMaterialCharHdl = CharStartHdl;
          a_ClientContext[index].APSyncKeyMaterialValueHdl = CharValueHdl;
          APP_DBG_MSG(", AP_SYNC_KEY_MATERIAL_UUID charac found\n");
        }       
        else if (uuid == ESL_RESP_KEY_MATERIAL_UUID)
        {
          a_ClientContext[index].ESLRespKeyMaterialCharHdl = CharStartHdl;
          a_ClientContext[index].ESLRespKeyMaterialValueHdl = CharValueHdl;
          APP_DBG_MSG(", ESL_RESP_KEY_MATERIAL_UUID charac found\n");
        }
        else if (uuid == ESL_CURR_ABS_TIME_UUID)
        {
          a_ClientContext[index].ESLCurrAbsTimeCharHdl = CharStartHdl;
          a_ClientContext[index].ESLCurrAbsTimeValueHdl = CharValueHdl;
          APP_DBG_MSG(", ESL_CURR_ABS_TIME_UUID charac found\n");
        }
        else if (uuid == ESL_DISPLAY_INFO_UUID)
        {
          a_ClientContext[index].ESLDisplayInfoCharHdl = CharStartHdl;
          a_ClientContext[index].ESLDisplayInfoValueHdl = CharValueHdl;
          APP_DBG_MSG(", ESL_DISPLAY_INFO_UUID charac found\n");
        }
        else if (uuid == ESL_IMAGE_INFO_UUID)
        {
          a_ClientContext[index].ESLImageInfoCharHdl = CharStartHdl;
          a_ClientContext[index].ESLImageInfoValueHdl = CharValueHdl;
          APP_DBG_MSG(", ESL_IMAGE_INFO_UUID charac found\n");
        }
        else if (uuid == ESL_SENSOR_INFO_UUID)
        {
          a_ClientContext[index].ESLSensorInfoCharHdl = CharStartHdl;
          a_ClientContext[index].ESLSensorInfoValueHdl = CharValueHdl;
          APP_DBG_MSG(", ESL_SENSOR_INFO_UUID charac found\n");
        }
        else if (uuid == ESL_LED_INFO_UUID)
        {
          a_ClientContext[index].ESLLedInfoCharHdl = CharStartHdl;
          a_ClientContext[index].ESLLedInfoValueHdl = CharValueHdl;
          APP_DBG_MSG(", ESL_LED_INFO_UUID charac found\n");
        }
#endif
        else if (uuid == OTS_FEATURE_UUID)
        {
          a_ClientContext[index].OTSHandles.OTSFeatureValueHdl = CharValueHdl;
          APP_DBG_MSG(", OTS_FEATURE_UUID charac found\n");
        }
        else if (uuid == OBJECT_NAME_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjNameValueHdl = CharValueHdl;
          APP_DBG_MSG(", OBJECT_NAME_UUID charac found\n");
        }
        else if (uuid == OBJECT_TYPE_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjTypeValueHdl = CharValueHdl;
          APP_DBG_MSG(", OBJECT_TYPE_UUID charac found\n");
        }
        else if (uuid == OBJECT_SIZE_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjSizeValueHdl = CharValueHdl;
          APP_DBG_MSG(", OBJECT_SIZE_UUID charac found\n");
        }
        else if (uuid == OBJECT_LAST_MODIFIED_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjLastModifiedValueHdl = CharValueHdl;
          a_ClientContext[index].OTSHandles.ObjLastModifiedProp = CharProperties;
          APP_DBG_MSG(", OBJECT_LAST_MODIFIED_UUID charac found\n");
        }
        else if (uuid == OBJECT_ID_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjIdValueHdl = CharValueHdl;
          APP_DBG_MSG(", OBJECT_ID_UUID charac found\n");
        }
        else if (uuid == OBJECT_PROPERTIES_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjPropValueHdl = CharValueHdl;
          APP_DBG_MSG(", OBJECT_PROPERTIES_UUID charac found\n");
        }
        else if (uuid == OBJECT_ACTION_CONTROL_POINT_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjActionCPValueHdl = CharValueHdl;
          APP_DBG_MSG(", OBJECT_ACTION_CONTROL_POINT_UUID charac found\n");
        }
        else if (uuid == OBJECT_LIST_CONTROL_POINT_UUID)
        {
          a_ClientContext[index].OTSHandles.ObjListCPValueHdl = CharValueHdl;
          APP_DBG_MSG(", OBJECT_LIST_CONTROL_POINT_UUID charac found\n");
        }
        else if (uuid == OBJECT_LIST_FILTER_POINT_UUID)
        {
          for(uint8_t i = 0; i < 3; i++)
          {
            if(a_ClientContext[index].OTSHandles.ObjListFilterValueHdl[i] == 0)
            {
              a_ClientContext[index].OTSHandles.ObjListFilterValueHdl[i] = CharValueHdl;
              break;
            }
          }
          APP_DBG_MSG(", OBJECT_LIST_FILTER_POINT_UUID charac found\n");
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

//  APP_DBG_MSG("ACI_ATT_FIND_INFO_RESP_VSEVT_CODE - ConnHdl=0x%04X\n",
//              p_evt->Connection_Handle);

  for (index = 0 ; index < CFG_MAX_NUM_CONNECTED_SERVERS ; index++)
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
        //APP_DBG_MSG("PRIMARY_SERVICE_UUID=0x%04X handle=0x%04X\n", uuid, handle);
      }
      else if (uuid == CHARACTERISTIC_UUID)
      {
        /* reset UUID & handle */
        gattCharValueHdl = 0;
        //APP_DBG_MSG("CHARACTERISTIC_UUID=0x%04X CharStartHandle=0x%04X\n", uuid, handle);
      }
      else if ( (uuid == CHAR_EXTENDED_PROPERTIES_DESCRIPTOR_UUID)
             || (uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID) )
      {

        APP_DBG_MSG("Descriptor UUID=0x%04X, handle=0x%04X",
                      uuid, handle);
        if (a_ClientContext[index].ServiceChangedCharValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].ServiceChangedCharDescHdl = handle;
          APP_DBG_MSG(", Service Changed CCCD found\n");
        }
/* USER CODE BEGIN gatt_parse_descs_1 */
        else if (a_ClientContext[index].ESLControlPointValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].ESLControlPointCCCDHdl = handle;
          APP_DBG_MSG(", ESL Control Point CCCD found: handle=0x%04X\n", handle);
        }
        else if (a_ClientContext[index].OTSHandles.ObjActionCPValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].OTSHandles.ObjActionCPCCCDHdl = handle;
          APP_DBG_MSG(", Oject Action Control Point CCCD found: handle=0x%04X\n", handle);
        }
        else if (a_ClientContext[index].OTSHandles.ObjListCPValueHdl == gattCharValueHdl)
        {
          a_ClientContext[index].OTSHandles.ObjListCPCCCDHdl = handle;
          APP_DBG_MSG(", Oject List Control Point CCCD found: handle=0x%04X\n", handle);
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

        //APP_DBG_MSG("  UUID=0x%04X, handle=0x%04X\n", uuid, handle);
        
/* USER CODE BEGIN gatt_parse_descs_2 */
        
/* USER CODE END gatt_parse_descs_2 */
        
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
//  APP_DBG_MSG("ACI_GATT_NOTIFICATION_VSEVT_CODE - ConnHdl=0x%04X, Attribute_Handle=0x%04X\n",
//              p_evt->Connection_Handle,
//              p_evt->Attribute_Handle);
/* USER CODE BEGIN gatt_parse_notification_1 */
  GATT_CLIENT_APP_Notification_evt_t Notification;
  uint8_t index;

  for (index = 0 ; index < CFG_MAX_NUM_CONNECTED_SERVERS ; index++)
  {
    if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
    {
      break;
    }
  }

  if (a_ClientContext[index].connHdl == p_evt->Connection_Handle)
  {
    if (p_evt->Attribute_Handle == a_ClientContext[index].ESLControlPointValueHdl)
    {
      /* The AP shall stop the timer when a notification of the ESL Control Point 
         characteristic is received in response to the command.*/
      HAL_RADIO_TIMER_StopVirtualTimer(&a_ClientContext[index].ECP_timer_Id);

      APP_DBG_MSG("  Incoming Nofification from ECP\n");
      Notification.Client_Evt_Opcode = ESL_NOTIFICATION_INFO_RECEIVED_EVT;
      Notification.DataTransfered.length = p_evt->Attribute_Value_Length;
      Notification.DataTransfered.p_Payload = &p_evt->Attribute_Value[0];

      gatt_Notification(&Notification);
    }
  }
  else
  {
    APP_DBG_MSG("ACI_GATT_NOTIFICATION_VSEVT_CODE, failed handle not found in connection table !\n");
  }
/* USER CODE END gatt_parse_notification_1 */

  return;
}

static void client_discover_all(void)
{ 
  tBleStatus status;

  /* Exchange configuration must be done only once. If alraeady done, it will return error.  */
  status = aci_gatt_clt_exchange_config(a_ClientContext[0].connHdl);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gatt_clt_exchange_config failure: reason=0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> aci_gatt_clt_exchange_config : Success\n");
    gatt_cmd_resp_wait();
  } 
  
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

void ESL_AP_List_Init(void)
{  
  LST_init_head(&esl_bonded);
}

/* insert the ESL on list if it is not already present, else update Conn_Handle */
bool ESL_AP_Insert_ESL_In_List(uint16_t Conn_Handle, uint8_t Peer_Address[6], uint8_t Peer_Address_Type, uint16_t esl_address)
{
  esl_bonded_t * esl_node;
  
  esl_node = Search_by_Peer_address_In_List(Peer_Address, Peer_Address_Type);
  if(esl_node == NULL)  
  {  
    esl_node = (esl_bonded_t *)malloc(sizeof(esl_bonded_t));

    for (uint8_t i = 0; i < 6; i++)
      esl_node->esl_info.Peer_Address[i] = Peer_Address[i];
    esl_node->esl_info.Peer_Address_Type = Peer_Address_Type;
    
    esl_node->esl_info.state = ESL_STATE_CONFIGURING;
    esl_node->esl_info.esl_address = esl_address;
    esl_node->esl_info.esl_resp_key_material = ESL_APP_Return_KeyMaterial_Value();    
    
    if(esl_node != NULL) 
    {  
      LST_insert_tail(&esl_bonded, &esl_node->esl_queue);
    }  
  }
  else
  {     
    APP_DBG_MSG("The ESL device is already present in bonded device list!\n");
  }  
  //To save the address of the connected ESL to be used to identify esl_node when writing characters
  esl_address_conn = esl_node->esl_info.esl_address;
  
  esl_node->esl_info.Conn_Handle = Conn_Handle;
  
  display_all_ESL_bonded(&esl_bonded);
  return true;
}

void ESL_AP_Remove_ESL_from_List(esl_bonded_t * esl_node)
{
  APP_DBG_MSG("!!! ESL_AP_Remove_ESL_from_List\n");
  aci_gap_remove_bonded_device(esl_node->esl_info.Peer_Address_Type, esl_node->esl_info.Peer_Address);
  LST_remove_node(&esl_node->esl_queue);
  free(esl_node);
}

void display_all_ESL_bonded(tListNode *list_head_p)
{
  esl_bonded_t *current_node_p;
  
  current_node_p = (esl_bonded_t *)list_head_p->next;
  APP_DBG_MSG("List of ESL bonded device: \n");

  while (&current_node_p->esl_queue != list_head_p)
  {
      APP_DBG_MSG("Peer Address: ");
      for (int i = 5; i >=0; i--) {
          APP_DBG_MSG("%02X", current_node_p->esl_info.Peer_Address[i]);
          if (i > 0) APP_DBG_MSG(":");
      }
      APP_DBG_MSG(", State: %d, ESL Address: %04X\n", current_node_p->esl_info.state, current_node_p->esl_info.esl_address);
      LST_get_next_node(&current_node_p->esl_queue, (tListNode **)&current_node_p);
  }
}

/* Search by Peer address and return an ESL node on the list */
esl_bonded_t* Search_by_Peer_address_In_List(uint8_t peerAddress[6], uint8_t Peer_Address_Type)
{  
  tListNode *list_head_p;
  esl_bonded_t *current_node_p;

  list_head_p = &esl_bonded;
  current_node_p = (esl_bonded_t *)list_head_p->next;
  
  while (&current_node_p->esl_queue != list_head_p)
  {
    if ((memcmp(current_node_p->esl_info.Peer_Address, peerAddress, sizeof(current_node_p->esl_info.Peer_Address)) == 0) &&
       (Peer_Address_Type == current_node_p->esl_info.Peer_Address_Type))
    {
        // return the esl_node related to peerAddress
        return current_node_p;
    }
    LST_get_next_node(&current_node_p->esl_queue, (tListNode **)&current_node_p);
  }
//  APP_DBG_MSG("Node with Peer address ");
//  for (int i = 5; i >= 0; i--) {
//      APP_DBG_MSG("%02X", peerAddress[i]);
//      if (i > 0) APP_DBG_MSG(":");
//  }
//  APP_DBG_MSG(" not present in ESL bonded list.\n");
  return NULL;
}

/* Search by ESL address and return an ESL node on the list */
esl_bonded_t* Search_by_ESL_address_In_List(uint16_t esl_address)
{  
  tListNode *list_head_p;
  esl_bonded_t *current_node_p;

  list_head_p = &esl_bonded;
  current_node_p = (esl_bonded_t *)list_head_p->next;

  while (&current_node_p->esl_queue != list_head_p)
  {
    if (current_node_p->esl_info.esl_address == esl_address)
    {
        // return the esl_node related to esl_address
        return current_node_p;
    }
    LST_get_next_node(&current_node_p->esl_queue, (tListNode **)&current_node_p);
  }
  APP_DBG_MSG("Node with ESL address %04X not found.\n", esl_address);
  return NULL;
}

/* Search by Connection Handle and return an ESL node on the list */
esl_bonded_t* Search_by_Conn_Handle_In_List(uint16_t conn_handle)
{  
  tListNode *list_head_p;
  esl_bonded_t *current_node_p;

  list_head_p = &esl_bonded;
  current_node_p = (esl_bonded_t *)list_head_p->next;

  while (&current_node_p->esl_queue != list_head_p)
  {
    if (current_node_p->esl_info.Conn_Handle == conn_handle)
    {
        // return the esl_node related to conn_handle
        return current_node_p;
    }
    LST_get_next_node(&current_node_p->esl_queue, (tListNode **)&current_node_p);
  }
  APP_DBG_MSG("Node with Connection Handle %04X not found.\n", conn_handle);
  return NULL;
}

uint16_t ESL_AP_return_ESL_address(uint8_t group_id, uint8_t esl_id)
{
  uint16_t esl_addr;
  
  esl_addr = (group_id << 8) | (esl_id & 0x00FF);
  return(esl_addr);
}

/* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
esl_bonded_t* ESL_AP_return_ESL_bonded(uint8_t group_id, uint8_t esl_id)
{
  uint16_t esl_address;  
  esl_bonded_t* esl_device;   
 
  // return ESL_addres by group_id and esl_id
  esl_address = ESL_AP_return_ESL_address(group_id, esl_id);
  
  // return the esl_node using Search_by_ESL_address_In_List     
  esl_device = Search_by_ESL_address_In_List(esl_address);

  return esl_device;
}

/* Update ESL queue with info written on characteristics during configuration */
void Update_Info_to_ESL_queue(esl_bonded_t* esl_node, ESL_Profile_Context_t new_info, ESL_PROFILE_KeyMaterial_t new_ap_sync_key_material) 
{
  esl_node->esl_info.state = new_info.state;
  esl_node->esl_info.esl_address = new_info.esl_address;
  esl_node->esl_info.esl_resp_key_material = new_info.esl_resp_key_material;

  APP_DBG_MSG("ESL Node Updated.\n");
}

static void ESL_AP_write_char(void)
{ 
  switch (get_AP_Status())
  {
    case ESL_AP_CONFIGURING_ESL:
      {
        ESL_APP_Read_All_Info_Chars();
        ESL_AP_configuring_char();
      }
      break;
    case ESL_AP_UPDATING_ESL_ADDRESS:
      {
        ESL_AP_write_esl_address();
      }
      break;  
  }    
    
}

/* If the AP establish a bond with an ESL and ESL is on Configuring state,
   the AP can configure ESL by writing some ESL Service characteristics  
   (called on ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE) */
static void ESL_AP_configuring_char(void)
{
  uint8_t index = 0;
  esl_bonded_t *esl_node;
  uint8_t key[24];
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;  
  
  //APP_DBG_MSG("ESL_AP_write_char!\n");
  esl_node = Search_by_ESL_address_In_List(esl_address_conn);
  if (esl_node != NULL)
  {
  
    /* ESL Address characteristic */
    APP_DBG_MSG("Writing ESL Address (0x%04X)\n", esl_node->esl_info.esl_address);
    ret = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                             BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                             a_ClientContext[index].ESLAddressValueHdl,
                             2,
                             (uint8_t *)&esl_node->esl_info.esl_address); 
    
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG(" Failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                  a_ClientContext[index].connHdl,
                  a_ClientContext[index].ESLAddressValueHdl);
    }
    else
    {
      /* wait until a gatt procedure complete is received */
      gatt_cmd_resp_wait();   
    }
    
    //AP Sync Material characteristic
    APP_DBG_MSG("Writing AP Sync Material\n");
    memcpy(&key[0], ap_sync_key_material_config_value.Session_Key, 16);
    memcpy(&key[16], ap_sync_key_material_config_value.IV, 8);
    
    ret = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                             BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                             a_ClientContext[index].APSyncKeyMaterialValueHdl,
                             24,
                             key); 
    
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG(" Failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                  a_ClientContext[index].connHdl,
                  a_ClientContext[index].APSyncKeyMaterialValueHdl);
    }
    else
    {
      /* wait until a gatt procedure complete is received */
      gatt_cmd_resp_wait();     
    }
    
    //ESL Resp Key Material characteristic
    APP_DBG_MSG("Writing Response Key Material\n");      
    memcpy(&key[0], esl_node->esl_info.esl_resp_key_material.Session_Key, 16);
    memcpy(&key[16], esl_node->esl_info.esl_resp_key_material.IV, 8);
    
    ret = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                             BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                             a_ClientContext[index].ESLRespKeyMaterialValueHdl,
                             24,
                             key); //ESL Resp Key Material
    
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG(" Failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                  a_ClientContext[index].connHdl,
                  a_ClientContext[index].ESLRespKeyMaterialValueHdl);
    }
    else
    {
      /* wait until a gatt procedure complete is received */
      gatt_cmd_resp_wait();
    }
    
    /* ESL Current Absolute Time characteristic */  
    uint32_t absoluteTime = TIMEREF_GetCurrentAbsTime();
    APP_DBG_MSG("Writing Absolute time (%d)\n", absoluteTime);  
    ret = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                             BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                             a_ClientContext[index].ESLCurrAbsTimeValueHdl,
                             4,
                             (uint8_t *)&absoluteTime);
    
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG(" Failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                  a_ClientContext[index].connHdl,
                  a_ClientContext[index].ESLCurrAbsTimeValueHdl);
    }
    else
    {
      /* wait until a gatt procedure complete is received */
      gatt_cmd_resp_wait();     
    }
 
    Update_Info_to_ESL_queue(esl_node, esl_node->esl_info, ap_sync_key_material_config_value);

    esl_node->esl_info.state = ESL_STATE_CONFIGURING;  
      
    /* After configuration of the ESL has been completed, the AP shall send the Update Complete opcode (0x04)
       using the ESL Control Point characteristic and shall commence the Periodic Advertising Synchronization
       Transfer (PAST) procedure */
    
    APP_DBG_MSG("Sending Update Complete\n");
    ret = ESL_AP_send_Update_Complete_cmd(esl_node->esl_info.esl_address);  
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("Update Complete command failed: 0x%02X \n", ret);
    }
    else
    {
      APP_DBG_MSG("Update Complete command success! (Configuring state) \n");
    }
    
    // Start PAST procedure (call "periodic_sync_info_transfer")
    UTIL_SEQ_SetTask( 1U << CFG_TASK_START_INFO_TRANSFER, CFG_SEQ_PRIO_0);
    
    esl_node->esl_info.state = ESL_STATE_SYNCHRONIZED;
  }  
  return;
}

uint8_t ESL_AP_send_Update_Complete_cmd(uint16_t esl_address)
{
  uint8_t cmd[2];
  uint8_t esl_id = (esl_address & 0x00FF);
  
  cmd[0] = ESL_CMD_UPDATE_COMPLETE;
  cmd[1] = esl_id;
  return ESL_AP_write_ECP(cmd, 2, NULL, false); //No response 
}

/* If bResponse is true the command wait for an ESL response, else the 
   command has no response, so the timer ECP_TIMEOUT_MS must not start */
uint8_t ESL_AP_write_ECP(uint8_t* cmd, uint8_t len_cmd, uint16_t* conn_handle, bool bResponse)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  uint8_t index = 0;
  
  /* If an ECP procedure times out, then the AP shall not start a new ECP 
   procedure until a new link is established with the ESL.*/
  if(!a_ClientContext[index].b_ECP_failed)
  {
    if (conn_handle!= NULL)
      *conn_handle = a_ClientContext[index].connHdl;
    
    APP_DBG_MSG("Writing ECP\n");
    
    ret = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                           BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           a_ClientContext[index].ESLControlPointValueHdl,
                           len_cmd,
                           cmd);
    
    if (ret != BLE_STATUS_SUCCESS)
    {  
      APP_DBG_MSG(" Failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                  a_ClientContext[index].connHdl,
                  a_ClientContext[index].ESLControlPointValueHdl);
      return ret;
    }
    /* wait until a gatt procedure complete is received */
    gatt_cmd_resp_wait();
    
    if(a_ClientContext[index].gatt_error_code != 0)
    {
      APP_DBG_MSG(" Procedure failed with gatt_error_code = 0x%02X\n",
                  a_ClientContext[index].gatt_error_code);
      return a_ClientContext[index].gatt_error_code;
    }
   
    /* When the AP writes to the ECP, the AP shall start a timer with the value  
       set to the ESL Control Point Timeout period.*/
    if (bResponse)
      HAL_RADIO_TIMER_StartVirtualTimer(&a_ClientContext[index].ECP_timer_Id, ECP_TIMEOUT_MS);
  }  
  return ret;
}

static void ESL_AP_write_esl_address(void)
{
  uint8_t index = 0;
  uint16_t new_esl_address;
  esl_bonded_t *esl_node;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  new_esl_address = ESL_AP_New_esl_address();
  
  /* ESL Address characteristic */
  ret = aci_gatt_clt_write(a_ClientContext[index].connHdl,
                           BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           a_ClientContext[index].ESLAddressValueHdl,
                           2,
                           (uint8_t *)&new_esl_address); 
  
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("New ESL_address aci_gatt_clt_write failed, connHdl=0x%04X, ValueHdl=0x%04X\n",  
                a_ClientContext[index].connHdl,
                a_ClientContext[index].ESLAddressValueHdl);
  }
  else
  {
    /* wait until a gatt procedure complete is received */
    gatt_cmd_resp_wait();    
    APP_DBG_MSG("New ESL_address 0x%04X aci_gatt_clt_write success, connHdl=0x%04X, ValueHdl=0x%04X\n",
                new_esl_address,              
                a_ClientContext[index].connHdl,
                a_ClientContext[index].ESLAddressValueHdl);
  }
  
  esl_node = ESL_AP_return_ESL_to_Update();
  if (esl_node != NULL)
  { 
    esl_node->esl_info.esl_address = new_esl_address;
    esl_node->esl_info.state = ESL_STATE_UPDATING;
  }
  
  ret = ESL_AP_send_Update_Complete_cmd(new_esl_address);  
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Update Complete command failed: 0x%02X \n", ret);
  }
  else
  {
    APP_DBG_MSG("Update Complete command success! (Reconf command) \n");
  }
  // Start PAST procedure (call "periodic_sync_info_transfer")
  UTIL_SEQ_SetTask( 1U << CFG_TASK_START_INFO_TRANSFER, CFG_SEQ_PRIO_0);
  esl_node->esl_info.state = ESL_STATE_SYNCHRONIZED;
}

static void ESL_AP_ECP_Timeout(void *arg)
{  
  APP_DBG_MSG("ECP procedure failed\n");
  /* If an ECP procedure times out, then the AP shall not start a new ECP 
     procedure until a new link is established with the ESL.*/
  set_ECP_Failed(true);
}

void set_ECP_Failed(bool bValue)
{
  uint8_t index = 0;
  
  a_ClientContext[index].b_ECP_failed = bValue;
}

void ESL_APP_DisconnectionComplete(uint16_t conn_handle)
{
  esl_bonded_t * esl_node;
  
  esl_node = Search_by_Conn_Handle_In_List(conn_handle);
  if(esl_node != NULL)  
  {
    if(esl_node->esl_info.state == ESL_STATE_CONFIGURING)
    {
      /* If link loss occurs before configuration of the ESL has been completed, 
         the AP should not commence the PAST procedure and should attempt to 
         reconnect to the ESL to continue the configuration. */
         
      /* When link loss occurs before configuration of the ESL has been completed,  
         the ESL reverts to the Unassociated state */
      esl_node->esl_info.state = ESL_STATE_UNASSOCIATED;
      APP_DBG_MSG("Unassociated State transition for link loss\n ");
    }
    else if(esl_node->esl_info.state == ESL_STATE_UPDATING)
    {
      /* If the connection is lost owing to link loss occurring in the Updating state, 
         then the ESL shall transition to the Unsynchronized state */
      esl_node->esl_info.state = ESL_STATE_UNSYNCHRONIZED;
      APP_DBG_MSG("Usynchronized State transition for link loss\n ");      
    }
  }
  
  OTP_CLIENT_DisconnectionComplete();
}

void ESL_APP_ReconnectionStateTransition(uint8_t Peer_Address[6], uint8_t Peer_Address_Type)
{
  esl_bonded_t * esl_node;
  
  esl_node = Search_by_Peer_address_In_List(Peer_Address, Peer_Address_Type);
  if(esl_node != NULL)  
  {
    if(esl_node->esl_info.state == ESL_STATE_UNSYNCHRONIZED)
    {
      esl_node->esl_info.state = ESL_STATE_UPDATING;
      APP_DBG_MSG("Updating State transition from Unsynchronized state\n ");
    }
  }    
}

static void print_Info_Char(void)
{
  for (int i = 0; i < a_ClientContext[0].read_char_len; i++) 
  {
    APP_DBG_MSG("%02X", a_ClientContext[0].read_char[i]);
    if (i < a_ClientContext[0].read_char_len-1) 
      APP_DBG_MSG(":");
  }    
  APP_DBG_MSG("\n");
}

uint8_t ESL_APP_Read_All_Info_Chars(void)
{
  tBleStatus ret = 0;
  
//  APP_DBG_MSG("ESL_AP_Read_Info_Chars!\n");
  
  APP_DBG_MSG("\nRead Display Information\n");  
  ESL_APP_Read_Display_Info_Chars(); 
  
  APP_DBG_MSG("\nRead Image Information\n");  
  ESL_APP_Read_Image_Info_Chars();
  
  APP_DBG_MSG("\nRead Sensor Information\n");  
  ESL_APP_Read_Sensor_Info_Chars();
  
  APP_DBG_MSG("\nRead LED Information\n");  
  ESL_APP_Read_Led_Info_Chars();
  
  return ret;
}
  

uint8_t ESL_APP_Read_Display_Info_Chars(void)
{
  uint8_t index = 0;

  return ESL_APP_Read_Info_Char(a_ClientContext[index].ESLDisplayInfoValueHdl); 
}

uint8_t ESL_APP_Read_Image_Info_Chars(void)
{
  uint8_t index = 0;

  return ESL_APP_Read_Info_Char(a_ClientContext[index].ESLImageInfoValueHdl); 
}

uint8_t ESL_APP_Read_Sensor_Info_Chars(void)
{
  uint8_t index = 0;

  return ESL_APP_Read_Info_Char(a_ClientContext[index].ESLSensorInfoValueHdl); 
}

uint8_t ESL_APP_Read_Led_Info_Chars(void)
{
  uint8_t index = 0;

  return ESL_APP_Read_Info_Char(a_ClientContext[index].ESLLedInfoValueHdl); 
}

uint8_t ESL_APP_Clear_Security_DB(void)
{
  APP_DBG_MSG("Clear Security DB!\n");
  return aci_gap_clear_security_db(); 
}

uint8_t ESL_APP_Read_Info_Char(uint16_t ValueHdl)
{
  uint8_t index = 0;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  //APP_DBG_MSG("ESL_AP_Read_Info_Chars!\n");
  
  a_ClientContext[index].read_char_offset = 0;

  /* Display Information characteristic */
  ret = aci_gatt_clt_read(a_ClientContext[index].connHdl,
                           BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           ValueHdl); 

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gatt_clt_read failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                a_ClientContext[index].connHdl,
                ValueHdl);
    return ret;
  }
  APP_DBG_MSG("aci_gatt_clt_read, connHdl=0x%04X, ValueHdl=0x%04X\n",
            a_ClientContext[index].connHdl,
            ValueHdl);
  /* wait until a gatt procedure complete is received */
  gatt_cmd_resp_wait();
  
  if (a_ClientContext[0].read_char_len >= (a_ClientContext[0].att_mtu - 1))
  {
    /* Display Information characteristic Read long */
    ret = ESL_APP_Read_Long_Char(ValueHdl, a_ClientContext[0].read_char_len);
  }  
  print_Info_Char();
  
  return ret;
}

static uint8_t ESL_APP_Read_Long_Char(uint16_t ValueHdl, uint16_t Offset)
{
  uint8_t index = 0;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  APP_DBG_MSG("Read Long\n");

  ret = aci_gatt_clt_read_long(a_ClientContext[index].connHdl,
                               BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                               ValueHdl,
                               Offset); 

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gatt_clt_read_long failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                a_ClientContext[index].connHdl,
                ValueHdl);
    return ret;
  }
  /* wait until a gatt procedure complete is received */
  gatt_cmd_resp_wait();  
  APP_DBG_MSG("aci_gatt_clt_read_long, connHdl=0x%04X, ValueHdl=0x%04X\n",
              a_ClientContext[index].connHdl,
              ValueHdl);
  
  return a_ClientContext[0].gatt_error_code;  
}

static ESL_PROFILE_KeyMaterial_t ESL_APP_Return_KeyMaterial_Value(void) 
{
    ESL_PROFILE_KeyMaterial_t keyMaterial;

    hci_le_rand(&keyMaterial.Session_Key[0]);
    hci_le_rand(&keyMaterial.Session_Key[8]);

    hci_le_rand(&keyMaterial.IV[0]);

    return keyMaterial;
}

tBleStatus GATT_CLIENT_Read_Char(uint16_t ValueHdl, uint8_t **data_p, uint16_t *data_length_p)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  a_ClientContext[0].read_char_offset = 0;

  ret = aci_gatt_clt_read(a_ClientContext[0].connHdl,
                           BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           ValueHdl); 

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gatt_clt_read failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                a_ClientContext[0].connHdl,
                ValueHdl);
    return ret;
  }
  APP_DBG_MSG("aci_gatt_clt_read, connHdl=0x%04X, ValueHdl=0x%04X\n",
            a_ClientContext[0].connHdl,
            ValueHdl);
  /* wait until a gatt procedure complete is received */
  gatt_cmd_resp_wait();
  
  if (a_ClientContext[0].read_char_len >= (a_ClientContext[0].att_mtu - 1))
  {
    ret = ESL_APP_Read_Long_Char(ValueHdl, a_ClientContext[0].read_char_len);
  }
  
  *data_p = a_ClientContext[0].read_char;
  *data_length_p = a_ClientContext[0].read_char_len;
  
  //TODO: check if ESL_APP_Read_Long_Char returned "Attribute Not Long".
  
  return ret;
}

tBleStatus GATT_CLIENT_Write_Char(uint16_t ValueHdl, uint8_t *data, uint16_t data_length)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  a_ClientContext[0].gatt_error_code = 0;

  ret = aci_gatt_clt_write(a_ClientContext[0].connHdl,
                           BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                           ValueHdl,
                           data_length,
                           data); 

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gatt_clt_write failed, connHdl=0x%04X, ValueHdl=0x%04X\n",
                a_ClientContext[0].connHdl,
                ValueHdl);
    return ret;
  }
  APP_DBG_MSG("aci_gatt_clt_write, connHdl=0x%04X, ValueHdl=0x%04X\n",
            a_ClientContext[0].connHdl,
            ValueHdl);
  /* wait until a gatt procedure complete is received */
  gatt_cmd_resp_wait();
  
  return a_ClientContext[0].gatt_error_code;
}

/* USER CODE END LF */

