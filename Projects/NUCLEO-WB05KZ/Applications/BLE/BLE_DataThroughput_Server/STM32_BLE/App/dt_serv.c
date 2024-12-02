/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dt_serv.c
  * @author  MCD Application Team
  * @brief   dt_serv definition.
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
#include <app_common.h>
#include "ble.h"
#include "dt_serv.h"
#include "dt_serv_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */
#include "app_ble.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  Dt_servSvcHdle;				/**< Dt_serv Service Handle */
  uint16_t  Tx_CharCharHdle;			/**< TX_CHAR Characteristic Handle */
  uint16_t  Rx_CharCharHdle;			/**< RX_CHAR Characteristic Handle */
  uint16_t  Through_CharCharHdle;			/**< THROUGH_CHAR Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}DT_SERV_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define TX_CHAR_SIZE        255	/* TX CHAR Characteristic size */
#define RX_CHAR_SIZE        255	/* RX CHAR Characteristic size */
#define THROUGH_CHAR_SIZE        255	/* THROUGH CHAR Characteristic size */
/* USER CODE BEGIN PM */
static uint8_t g_notification_n_1;

uint16_t packet_lost = 0;
uint32_t DTS_N=0;
uint16_t MTUSizeValue;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static DT_SERV_Context_t DT_SERV_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

/*
 * UUIDs for DT SERV service
 */
#define DT_SERV_UUID			0x8f,0xe5,0xb3,0xd5,0x2e,0x7f,0x4a,0x98,0x2a,0x48,0x7a,0xcc,0x80,0xfe,0x00,0x00
#define TX_CHAR_UUID			0x19,0xed,0x82,0xae,0xed,0x21,0x4c,0x9d,0x41,0x45,0x22,0x8e,0x81,0xfe,0x00,0x00
#define RX_CHAR_UUID			0x19,0xed,0x82,0xae,0xed,0x21,0x4c,0x9d,0x41,0x45,0x22,0x8e,0x82,0xfe,0x00,0x00
#define THROUGH_CHAR_UUID			0x19,0xed,0x82,0xae,0xed,0x21,0x4c,0x9d,0x41,0x45,0x22,0x8e,0x83,0xfe,0x00,0x00

BLE_GATT_SRV_CCCD_DECLARE(tx_char, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);
BLE_GATT_SRV_CCCD_DECLARE(through_char, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t rx_char_val_buffer[RX_CHAR_SIZE];

static ble_gatt_val_buffer_def_t rx_char_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = RX_CHAR_SIZE,
  .buffer_len = sizeof(rx_char_val_buffer),
  .buffer_p = rx_char_val_buffer
};

/* DT SERV service TX_CHAR (notification) RX_CHAR (write without response), THROUGH_CHAR (notification) characteristics definition */
static const ble_gatt_chr_def_t dt_serv_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(TX_CHAR_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(tx_char),
            .descr_count = 1U,
        },
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ | BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(RX_CHAR_UUID),
        .val_buffer_p = &rx_char_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(THROUGH_CHAR_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(through_char),
            .descr_count = 1U,
        },
    },
};

/* DT SERV service definition */
static const ble_gatt_srv_def_t dt_serv_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_128(DT_SERV_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)dt_serv_chars,
       .chr_count = 3U,
   },
};

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t DT_SERV_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  aci_gatt_srv_read_event_rp0    *p_read;
  DT_SERV_NotificationEvt_t notification;
  /* USER CODE BEGIN Service1_EventHandler_1 */
  notification.DataTransfered.pPayload_n_1 = g_notification_n_1;
  /* USER CODE END Service1_EventHandler_1 */

  switch(p_evt->ecode)
  {
    case ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
      
      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
      p_attribute_modified = (aci_gatt_srv_attribute_modified_event_rp0*)p_evt->data;
      notification.ConnectionHandle         = p_attribute_modified->Connection_Handle;
      notification.AttributeHandle          = p_attribute_modified->Attr_Handle;
      notification.DataTransfered.Length    = p_attribute_modified->Attr_Data_Length;
      notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
      if(p_attribute_modified->Attr_Handle == (DT_SERV_Context.Tx_CharCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service1_Char_1 */
        
        /* USER CODE END Service1_Char_1 */
        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service1_Char_1_attribute_modified */
          
          /* USER CODE END Service1_Char_1_attribute_modified */

          /* Disabled Notification management */
        case (!BLE_GATT_SRV_CCCD_NOTIFICATION):
          /* USER CODE BEGIN Service1_Char_1_Disabled_BEGIN */
          APP_DBG_MSG("==>> Notification disabled\n");
          /* USER CODE END Service1_Char_1_Disabled_BEGIN */
          notification.EvtOpcode = DT_SERV_TX_CHAR_NOTIFY_DISABLED_EVT;
          DT_SERV_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_1_Disabled_END */
          
          /* USER CODE END Service1_Char_1_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_BEGIN */
          APP_DBG_MSG("==>> Tx_char_handle Notification enabled\n");
          /* USER CODE END Service1_Char_1_COMSVC_Notification_BEGIN */
          notification.EvtOpcode = DT_SERV_TX_CHAR_NOTIFY_ENABLED_EVT;
          DT_SERV_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_END */
          
          /* USER CODE END Service1_Char_1_COMSVC_Notification_END */
          break;

        default:
          /* USER CODE BEGIN Service1_Char_1_default */
          
          /* USER CODE END Service1_Char_1_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (DT_SERV_Context.Tx_CharCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (DT_SERV_Context.Through_CharCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service1_Char_3 */
        APP_DBG_MSG("==>> Through_char_handle attribute modified\n");
        /* USER CODE END Service1_Char_3 */
        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service1_Char_3_attribute_modified */

          /* USER CODE END Service1_Char_3_attribute_modified */

          /* Disabled Notification management */
        case (!BLE_GATT_SRV_CCCD_NOTIFICATION):
          /* USER CODE BEGIN Service1_Char_3_Disabled_BEGIN */

          /* USER CODE END Service1_Char_3_Disabled_BEGIN */
          notification.EvtOpcode = DT_SERV_THROUGH_CHAR_NOTIFY_DISABLED_EVT;
          DT_SERV_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_3_Disabled_END */

          /* USER CODE END Service1_Char_3_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service1_Char_3_COMSVC_Notification_BEGIN */
            APP_DBG_MSG("==>> Through_char_handle attribute enabled\n");
          /* USER CODE END Service1_Char_3_COMSVC_Notification_BEGIN */
          notification.EvtOpcode = DT_SERV_THROUGH_CHAR_NOTIFY_ENABLED_EVT;
          DT_SERV_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_3_COMSVC_Notification_END */

          /* USER CODE END Service1_Char_3_COMSVC_Notification_END */
          break;

        default:
          /* USER CODE BEGIN Service1_Char_3_default */

          /* USER CODE END Service1_Char_3_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (DT_SERV_Context.Through_CharCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (DT_SERV_Context.Rx_CharCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = DT_SERV_RX_CHAR_WRITE_NO_RESP_EVT;
        /* USER CODE BEGIN Service1_Char_2_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        uint8_t CRC_Result;
        uint8_t CRC_Received;
        notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length; 
        notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
        notification.DataTransfered.pPayload_n = *((uint8_t*) &(p_attribute_modified->Attr_Data[0]));
        if ((notification.DataTransfered.pPayload_n - notification.DataTransfered.pPayload_n_1) != 0)
        {
          if (notification.DataTransfered.pPayload_n >= (notification.DataTransfered.pPayload_n_1 + 2))
          {
            packet_lost +=
              ((notification.DataTransfered.pPayload_n - notification.DataTransfered.pPayload_n_1) - 1);
            APP_DBG_MSG("** Missing packets [ %3d - %3d ]  %3d \r\n",(uint32_t)notification.DataTransfered.pPayload_n_1 + 1, (uint32_t)notification.DataTransfered.pPayload_n - 1, (uint32_t)notification.DataTransfered.pPayload_n - (uint32_t)notification.DataTransfered.pPayload_n_1 - 1);
            
          }
          /* CRC computation */
          CRC_Result = APP_BLE_ComputeCRC8((uint8_t*) (p_attribute_modified->Attr_Data), (p_attribute_modified->Attr_Data_Length) - 1);
          /* get low weight byte */
          CRC_Received = (uint8_t) (p_attribute_modified->Attr_Data[notification.DataTransfered.Length - 1]);
          
          if (CRC_Received != CRC_Result)
          {
            DTS_N+=1;
            APP_DBG_MSG("** data error **  N= %d \r\n",(int)DTS_N);
          }
        }
        notification.DataTransfered.pPayload_n_1 = notification.DataTransfered.pPayload_n;
        g_notification_n_1 = notification.DataTransfered.pPayload_n;
        /* USER CODE END Service1_Char_2_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        DT_SERV_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (DT_SERV_Context.Rx_CharCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      
      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */

      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
	  if(p_read->Attribute_Handle == (DT_SERV_Context.Rx_CharCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service1_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service1_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service1_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service1_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (DT_SERV_Context.Rx_CharCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */

      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      if(p_write->Attribute_Handle == (DT_SERV_Context.Rx_CharCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service1_Char_2_ACI_GATT_SRV_WRITE_VSEVT_CODE */

        /*USER CODE END Service1_Char_2_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (DT_SERV_Context.Rx_CharCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_WRITE_END */

      /* USER CODE END EVT_BLUE_GATT_SRV_WRITE_END */
      break;/* ACI_GATT_SRV_WRITE_VSEVT_CODE */
    }
    case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
    {
      aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
      p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_evt->data;
      UNUSED(p_tx_pool_available_event);

      /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
      Resume_Notification();
      /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
      break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
    }
    case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
    {
      aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
      p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_evt->data;
      UNUSED(p_exchange_mtu);

      /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
      if (p_exchange_mtu->MTU < DATA_NOTIFICATION_MAX_PACKET_SIZE)
      {
        MTUSizeValue = p_exchange_mtu->MTU - 3;
      }
      else
      {
        MTUSizeValue = DATA_NOTIFICATION_MAX_PACKET_SIZE;
      }
      APP_DBG_MSG(">>== Connection Handle = %d \n", p_exchange_mtu->Connection_Handle );
      APP_DBG_MSG(">>== MTU_size = %d \n",MTUSizeValue );
      APP_DBG_MSG("\r\n\r");       
      /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
      break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
    }
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
    /* USER CODE BEGIN EVT_DEFAULT */
    
    /* USER CODE END EVT_DEFAULT */
    break;
  }

  /* USER CODE BEGIN Service1_EventHandler_2 */

  /* USER CODE END Service1_EventHandler_2 */

  return(return_value);
}/* end DT_SERV_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void DT_SERV_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(DT_SERV_Context);

  /* USER CODE BEGIN InitService1Svc_1 */

  /* USER CODE END InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(DT_SERV_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&dt_serv_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: DT_SERV, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: DT_SERV \n");
  }

  DT_SERV_Context.Dt_servSvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &dt_serv_service);
  DT_SERV_Context.Tx_CharCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&dt_serv_chars[0]);
  DT_SERV_Context.Rx_CharCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&dt_serv_chars[1]);
  DT_SERV_Context.Through_CharCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&dt_serv_chars[2]);

  /* USER CODE BEGIN InitService1Svc_2 */

  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering DT_SERV handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus DT_SERV_UpdateValue(DT_SERV_CharOpcode_t CharOpcode, DT_SERV_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case DT_SERV_RX_CHAR:
      memcpy(rx_char_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(rx_char_val_buffer)));
      /* USER CODE BEGIN Service1_Char_Value_2*/

      /* USER CODE END Service1_Char_Value_2*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Update_Char_2 */

  /* USER CODE END Service1_App_Update_Char_2 */

  return ret;
}

/**
 * @brief  Characteristic notification
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the data to be notified to the client
 * @param  ConnectionHandle: connection handle identifying the client to be notified.
 *
 */
tBleStatus DT_SERV_NotifyValue(DT_SERV_CharOpcode_t CharOpcode, DT_SERV_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Notify_Char_1 */

  /* USER CODE END Service1_App_Notify_Char_1 */

  switch(CharOpcode)
  {
    case DT_SERV_TX_CHAR:
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                DT_SERV_Context.Tx_CharCharHdle + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify TX_CHAR command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify TX_CHAR command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1*/

      /* USER CODE END Service1_Char_Value_1*/
      break;

    case DT_SERV_THROUGH_CHAR:
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                DT_SERV_Context.Through_CharCharHdle + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify THROUGH_CHAR command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify THROUGH_CHAR command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_3*/

      /* USER CODE END Service1_Char_Value_3*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Notify_Char_2 */

  /* USER CODE END Service1_App_Notify_Char_2 */

  return ret;
}
