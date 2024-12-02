/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SRVC_Privacy.c
  * @author  MCD Application Team
  * @brief   SRVC_Privacy definition.
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
#include "srvc_privacy.h"
#include "srvc_privacy_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  Srvc_privacySvcHdle;				/**< Srvc_privacy Service Handle */
  uint16_t  Char_PrivacyCharHdle;			/**< CHAR_PRIVACY Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}SRVC_PRIVACY_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define CHAR_PRIVACY_SIZE        2	/* CHRACTERISTIC PRIVACY Characteristic size */
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static SRVC_PRIVACY_Context_t SRVC_PRIVACY_Context;

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
 * UUIDs for SERVICE Privacy service
 */
#define SRVC_PRIVACY_UUID			0x66,0x9a,0x0c,0x20,0x00,0x08,0x68,0xb5,0xe8,0x11,0x5c,0xcc,0x20,0x3e,0xf0,0x57
#define CHAR_PRIVACY_UUID			0x66,0x9a,0x0c,0x20,0x00,0x08,0x68,0xb5,0xe8,0x11,0x5c,0xcc,0x21,0x3e,0xf0,0x57

BLE_GATT_SRV_CCCD_DECLARE(char_privacy, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t char_privacy_val_buffer[CHAR_PRIVACY_SIZE];

static ble_gatt_val_buffer_def_t char_privacy_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = CHAR_PRIVACY_SIZE,
  .buffer_len = sizeof(char_privacy_val_buffer),
  .buffer_p = char_privacy_val_buffer
};

/* SERVICE Privacy service CHAR_PRIVACY (notification) characteristics definition */
static const ble_gatt_chr_def_t srvc_privacy_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ | BLE_GATT_SRV_CHAR_PROP_WRITE | BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_AUTHEN_READ | BLE_GATT_SRV_PERM_ENCRY_READ | BLE_GATT_SRV_PERM_AUTHEN_WRITE | BLE_GATT_SRV_PERM_ENCRY_WRITE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(CHAR_PRIVACY_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(char_privacy),
            .descr_count = 1U,
        },
        .val_buffer_p = &char_privacy_val_buffer_def
    },
};

/* SERVICE Privacy service definition */
static const ble_gatt_srv_def_t srvc_privacy_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_128(SRVC_PRIVACY_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)srvc_privacy_chars,
       .chr_count = 1U,
   },
};

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t SRVC_PRIVACY_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  aci_gatt_srv_read_event_rp0    *p_read;
  SRVC_PRIVACY_NotificationEvt_t notification;
  /* USER CODE BEGIN Service1_EventHandler_1 */

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
      if(p_attribute_modified->Attr_Handle == (SRVC_PRIVACY_Context.Char_PrivacyCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
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

          /* USER CODE END Service1_Char_1_Disabled_BEGIN */
          notification.EvtOpcode = SRVC_PRIVACY_CHAR_PRIVACY_NOTIFY_DISABLED_EVT;
          SRVC_PRIVACY_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_1_Disabled_END */

          /* USER CODE END Service1_Char_1_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_BEGIN */

          /* USER CODE END Service1_Char_1_COMSVC_Notification_BEGIN */
          notification.EvtOpcode = SRVC_PRIVACY_CHAR_PRIVACY_NOTIFY_ENABLED_EVT;
          SRVC_PRIVACY_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_END */
          APP_DBG_MSG("-- GATT : CHAR PRIVACY NOTIFY ENABLED EVT\n");

          /* USER CODE END Service1_Char_1_COMSVC_Notification_END */
          break;

        default:
          /* USER CODE BEGIN Service1_Char_1_default */

          /* USER CODE END Service1_Char_1_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (SRVC_PRIVACY_Context.Char_PrivacyCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (SRVC_PRIVACY_Context.Char_PrivacyCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = SRVC_PRIVACY_CHAR_PRIVACY_WRITE_EVT;
        /* USER CODE BEGIN Service1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        APP_DBG_MSG("-- GATT : CHAR PRIVACY CONFIGURATION RECEIVED\n");
        notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length;
        notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
        /* USER CODE END Service1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        SRVC_PRIVACY_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (SRVC_PRIVACY_Context.Char_PrivacyCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */

      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
	  if(p_read->Attribute_Handle == (SRVC_PRIVACY_Context.Char_PrivacyCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
//#warning user shall call aci_gatt_srv_read_resp() function if allowed
		/*USER CODE END Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (SRVC_PRIVACY_Context.Char_PrivacyCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */

      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      if(p_write->Attribute_Handle == (SRVC_PRIVACY_Context.Char_PrivacyCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE */
//#warning user shall call aci_gatt_srv_write_resp() function if allowed
        /*USER CODE END Service1_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (SRVC_PRIVACY_Context.Char_PrivacyCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
}/* end SRVC_PRIVACY_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void SRVC_PRIVACY_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(SRVC_PRIVACY_Context);

  /* USER CODE BEGIN InitService1Svc_1 */

  /* USER CODE END InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(SRVC_PRIVACY_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&srvc_privacy_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: SRVC_Privacy, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: SRVC_Privacy \n");
  }

  SRVC_PRIVACY_Context.Srvc_privacySvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &srvc_privacy_service);
  SRVC_PRIVACY_Context.Char_PrivacyCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&srvc_privacy_chars[0]);

  /* USER CODE BEGIN InitService1Svc_2 */

  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering SRVC_Privacy handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus SRVC_PRIVACY_UpdateValue(SRVC_PRIVACY_CharOpcode_t CharOpcode, SRVC_PRIVACY_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {

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
tBleStatus SRVC_PRIVACY_NotifyValue(SRVC_PRIVACY_CharOpcode_t CharOpcode, SRVC_PRIVACY_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Notify_Char_1 */

  /* USER CODE END Service1_App_Notify_Char_1 */

  switch(CharOpcode)
  {
    case SRVC_PRIVACY_CHAR_PRIVACY:
      memcpy(char_privacy_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(char_privacy_val_buffer)));
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                SRVC_PRIVACY_Context.Char_PrivacyCharHdle + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify CHAR_PRIVACY command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify CHAR_PRIVACY command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1*/
      /* custom notification */
      if (ret == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("Send Notification -  pData->p_Payload[0] = 0x%02X\n", pData->p_Payload[0]);
        pData->p_Payload[0]++;
      }
      /* USER CODE END Service1_Char_Value_1*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Notify_Char_2 */

  /* USER CODE END Service1_App_Notify_Char_2 */

  return ret;
}
