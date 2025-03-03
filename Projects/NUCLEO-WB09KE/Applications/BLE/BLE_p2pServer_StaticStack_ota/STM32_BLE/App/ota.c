/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ota.c
  * @author  MCD Application Team
  * @brief   Firmware Update OTA definition.
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
#include "ota.h"
#include "ota_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */
#include "ota_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef enum
{
  OTA_No_Pending    = 2,
  OTA_Pending       = 3,
  OTA_Ready_Pending = 4
} OTA_Confirmation_Status_t;
/* USER CODE END PTD */

typedef struct{
  uint16_t  OtaSvcHdle;				/**< Ota Service Handle */
  uint16_t  Base_AdrCharHdle;			/**< BASE_ADR Characteristic Handle */
  uint16_t  ConfCharHdle;			/**< CONF Characteristic Handle */
  uint16_t  Raw_DataCharHdle;			/**< RAW_DATA Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}OTA_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define BASE_ADR_SIZE        6	/* Base Adr Characteristic size */
#define CONF_SIZE        1	/* Conf Characteristic size */
#define RAW_DATA_SIZE        244	/* Raw Data Characteristic size */
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static OTA_Context_t OTA_Context;

/* USER CODE BEGIN PV */
static OTA_Confirmation_Status_t OTA_Conf_Status;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

/*
 * UUIDs for OTA service
 */
#define OTA_UUID			0x8f,0xe5,0xb3,0xd5,0x2e,0x7f,0x4a,0x98,0x2a,0x48,0x7a,0xcc,0x20,0xfe,0x00,0x00
#define BASE_ADR_UUID			0x19,0xed,0x82,0xae,0xed,0x21,0x4c,0x9d,0x41,0x45,0x22,0x8e,0x22,0xfe,0x00,0x00
#define CONF_UUID			0x19,0xed,0x82,0xae,0xed,0x21,0x4c,0x9d,0x41,0x45,0x22,0x8e,0x23,0xfe,0x00,0x00
#define RAW_DATA_UUID			0x19,0xed,0x82,0xae,0xed,0x21,0x4c,0x9d,0x41,0x45,0x22,0x8e,0x24,0xfe,0x00,0x00

BLE_GATT_SRV_CCCD_DECLARE(conf, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t base_adr_val_buffer[BASE_ADR_SIZE];

static ble_gatt_val_buffer_def_t base_adr_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = BASE_ADR_SIZE,
  .buffer_len = sizeof(base_adr_val_buffer),
  .buffer_p = base_adr_val_buffer
};

uint8_t conf_val_buffer[CONF_SIZE];

static ble_gatt_val_buffer_def_t conf_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = CONF_SIZE,
  .buffer_len = sizeof(conf_val_buffer),
  .buffer_p = conf_val_buffer
};

uint8_t raw_data_val_buffer[RAW_DATA_SIZE];

static ble_gatt_val_buffer_def_t raw_data_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = RAW_DATA_SIZE,
  .buffer_len = sizeof(raw_data_val_buffer),
  .buffer_p = raw_data_val_buffer
};

/* OTA service BASE_ADR (write without response), RAW_DATA (write without response), characteristics definition */
static const ble_gatt_chr_def_t ota_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(BASE_ADR_UUID),
        .val_buffer_p = &base_adr_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_INDICATE,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(CONF_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(conf),
            .descr_count = 1U,
        },
        .val_buffer_p = &conf_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(RAW_DATA_UUID),
        .val_buffer_p = &raw_data_val_buffer_def
    },
};

/* OTA service definition */
static const ble_gatt_srv_def_t ota_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_128(OTA_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)ota_chars,
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
static BLEEVT_EvtAckStatus_t OTA_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  OTA_NotificationEvt_t notification;
  /* USER CODE BEGIN Service2_EventHandler_1 */

  /* USER CODE END Service2_EventHandler_1 */

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
      if(p_attribute_modified->Attr_Handle == (OTA_Context.ConfCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service2_Char_2 */
        
        /* USER CODE END Service2_Char_2 */

        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service2_Char_2_attribute_modified */
          
          /* USER CODE END Service2_Char_2_attribute_modified */

          /* Disabled Indication management */
        case (!BLE_GATT_SRV_CCCD_INDICATION):
          /* USER CODE BEGIN Service2_Char_2_Disabled_BEGIN */
            
          /* USER CODE END Service2_Char_2_Disabled_BEGIN */
          notification.EvtOpcode = OTA_CONF_INDICATE_DISABLED_EVT;
          OTA_Notification(&notification);
          /* USER CODE BEGIN Service2_Char_2_Disabled_END */
            
          /* USER CODE END Service2_Char_2_Disabled_END */
          break;

          /* Enabled Indication management */
        case BLE_GATT_SRV_CCCD_INDICATION:
          /* USER CODE BEGIN Service2_Char_2_COMSVC_Indication_BEGIN */

          /* USER CODE END Service2_Char_2_COMSVC_Indication_BEGIN */
          notification.EvtOpcode = OTA_CONF_INDICATE_ENABLED_EVT;
          OTA_Notification(&notification);
          /* USER CODE BEGIN Service2_Char_2_COMSVC_Indication_END */

          /* USER CODE END Service2_Char_2_COMSVC_Indication_END */
          break;

          default:
          /* USER CODE BEGIN Service2_Char_2_default */
          
          /* USER CODE END Service2_Char_2_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (OTA_Context.CONFHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (OTA_Context.Base_AdrCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = OTA_BASE_ADR_WRITE_NO_RESP_EVT;
        /* USER CODE BEGIN Service2_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        /**
        * Base Address
        */
        notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length;
        notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
        /* USER CODE END Service2_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        OTA_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (OTA_Context.Base_AdrCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
      else if(p_attribute_modified->Attr_Handle == (OTA_Context.Raw_DataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = OTA_RAW_DATA_WRITE_NO_RESP_EVT;
        /* USER CODE BEGIN Service2_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        /**
        * Raw Data
        */
        notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length;
        notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
        /* USER CODE END Service2_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        OTA_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (OTA_Context.Raw_DataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      
      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */

      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */

      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      if(p_write->Attribute_Handle == (OTA_Context.Base_AdrCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service2_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE */

        /*USER CODE END Service2_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (OTA_Context.Base_AdrCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      else if(p_write->Attribute_Handle == (OTA_Context.Raw_DataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service2_Char_3_ACI_GATT_SRV_WRITE_VSEVT_CODE */

        /*USER CODE END Service2_Char_3_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (OTA_Context.Raw_DataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
      
      /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
      break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
    }
    case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
    {
      aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
      p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_evt->data;
      UNUSED(p_exchange_mtu);

      /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
      
      /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
      break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
    }
    /* USER CODE BEGIN BLECORE_EVT */
  case ACI_GATT_SRV_CONFIRMATION_VSEVT_CODE:
    {
      aci_gatt_srv_confirmation_event_rp0 *p_srv_confirmation_event;
      p_srv_confirmation_event = (aci_gatt_srv_confirmation_event_rp0 *) p_evt->data;
      UNUSED(p_srv_confirmation_event);
   
      OTA_Conf_Status = (OTA_Confirmation_Status_t)OTA_APP_GetConfStatus();
      
      if(OTA_Conf_Status == OTA_Pending)
      {
        /**
        * Confirmation Event
        */
        OTA_Conf_Status = OTA_No_Pending;
        
        notification.EvtOpcode = OTA_CONF_EVT;
        OTA_Notification( &notification );
      }
      else if( OTA_Conf_Status == OTA_Ready_Pending)
      {
        /**
        * Confirmation Event
        */
        OTA_Conf_Status = OTA_No_Pending;
        
        notification.EvtOpcode = OTA_READY_EVT;
        OTA_Notification( &notification );
      }
    }
    break; /* end ACI_GATT_CLT_INDICATION_VSEVT_CODE */   
    
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
    
//    /* Manage ACI_GATT_INDICATION_VSEVT_CODE occurring on Android 12 */   
//  case ACI_GATT_CLT_INDICATION_VSEVT_CODE:
//    {
//      tBleStatus status = BLE_STATUS_FAILED;
//      aci_gatt_clt_indication_event_rp0 *pr = (void*)p_evt->data;
//      status = aci_gatt_clt_confirm_indication(pr->Connection_Handle, BLE_GATT_UNENHANCED_ATT_L2CAP_CID);
//      UNUSED(status);
//      OTA_Conf_Status = (OTA_Confirmation_Status_t)OTA_APP_GetConfStatus();
//      if(OTA_Conf_Status == OTA_Pending)
//      {
//        /**
//        * Confirmation Event
//        */
//        OTA_Conf_Status = OTA_No_Pending;
//        
//        return_value = BLEEVT_Ack;
//        
//        notification.EvtOpcode = OTA_CONF_EVT;
//        OTA_Notification( &notification );
//      }
//      else if( OTA_Conf_Status == OTA_Ready_Pending)
//      {
//        /**
//        * Confirmation Event
//        */
//        OTA_Conf_Status = OTA_No_Pending;
//        
//        return_value = BLEEVT_Ack;
//        
//        notification.EvtOpcode = OTA_READY_EVT;
//        OTA_Notification( &notification );
//      }
//    }
//    break; /* end ACI_GATT_CLT_INDICATION_VSEVT_CODE */
    
    
    /* USER CODE END BLECORE_EVT */
  default:
    /* USER CODE BEGIN EVT_DEFAULT */
    
    /* USER CODE END EVT_DEFAULT */
    break;
  }

  /* USER CODE BEGIN Service2_EventHandler_2 */

  /* USER CODE END Service2_EventHandler_2 */

  return(return_value);
}/* end OTA_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void OTA_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(OTA_Context);

  /* USER CODE BEGIN InitService2Svc_1 */

  /* USER CODE END InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(OTA_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&ota_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: OTA, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: OTA \n");
  }

  OTA_Context.OtaSvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &ota_service);
  OTA_Context.Base_AdrCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ota_chars[0]);
  OTA_Context.ConfCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ota_chars[1]);
  OTA_Context.Raw_DataCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ota_chars[2]);

  /* USER CODE BEGIN InitService1Svc_2 */

  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering OTA handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus OTA_UpdateValue(OTA_CharOpcode_t CharOpcode, OTA_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case OTA_BASE_ADR:
      memcpy(base_adr_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(base_adr_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_1*/

      /* USER CODE END Service2_Char_Value_1*/
      break;

    case OTA_CONF:
      memcpy(conf_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(conf_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_2*/

      /* USER CODE END Service2_Char_Value_2*/
      break;

    case OTA_RAW_DATA:
      memcpy(raw_data_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(raw_data_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_3*/

      /* USER CODE END Service2_Char_Value_3*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Update_Char_2 */

  /* USER CODE END Service2_App_Update_Char_2 */

  return ret;
}

/**
 * @brief  Characteristic notification
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the data to be notified to the client
 * @param  ConnectionHandle: connection handle identifying the client to be notified.
 *
 */
tBleStatus OTA_NotifyValue(OTA_CharOpcode_t CharOpcode, OTA_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Notify_Char_1 */

  /* USER CODE END Service2_App_Notify_Char_1 */

  switch(CharOpcode)
  {

    case OTA_CONF:
      memcpy(conf_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(conf_val_buffer)));
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                OTA_Context.ConfCharHdle + 1,
                                GATT_INDICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify CONF command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify CONF command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_2*/

      /* USER CODE END Service2_Char_Value_2*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Notify_Char_2 */

  /* USER CODE END Service2_App_Notify_Char_2 */

  return ret;
}
