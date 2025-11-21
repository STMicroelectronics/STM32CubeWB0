/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    p2p_server.c
  * @author  MCD Application Team
  * @brief   p2p_server definition.
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
#include "p2p_server.h"
#include "p2p_server_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  P2p_serverSvcHdle;				/**< P2p_server Service Handle */
  uint16_t  Led_CCharHdle;			/**< LED_C Characteristic Handle */
  uint16_t  Switch_CCharHdle;			/**< SWITCH_C Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}P2P_SERVER_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define LED_C_SIZE        2	/* LED C Characteristic size */
#define SWITCH_C_SIZE        2	/* SWITCH C Characteristic size */
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static P2P_SERVER_Context_t P2P_SERVER_Context;

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
 * UUIDs for p2p Server service
 */
#define P2P_SERVER_UUID			0x8f,0xe5,0xb3,0xd5,0x2e,0x7f,0x4a,0x98,0x2a,0x48,0x7a,0xcc,0x40,0xfe,0x00,0x00
#define LED_C_UUID			0x19,0xed,0x82,0xae,0xed,0x21,0x4c,0x9d,0x41,0x45,0x22,0x8e,0x41,0xfe,0x00,0x00
#define SWITCH_C_UUID			0x19,0xed,0x82,0xae,0xed,0x21,0x4c,0x9d,0x41,0x45,0x22,0x8e,0x42,0xfe,0x00,0x00

BLE_GATT_SRV_CCCD_DECLARE(switch_c, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t led_c_val_buffer[LED_C_SIZE];

static ble_gatt_val_buffer_def_t led_c_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = LED_C_SIZE,
  .buffer_len = sizeof(led_c_val_buffer),
  .buffer_p = led_c_val_buffer
};

uint8_t switch_c_val_buffer[SWITCH_C_SIZE];

static ble_gatt_val_buffer_def_t switch_c_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = SWITCH_C_SIZE,
  .buffer_len = sizeof(switch_c_val_buffer),
  .buffer_p = switch_c_val_buffer
};

/* p2p Server service LED_C (write without response), SWITCH_C (notification) characteristics definition */
static const ble_gatt_chr_def_t p2p_server_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ | BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(LED_C_UUID),
        .val_buffer_p = &led_c_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_128(SWITCH_C_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(switch_c),
            .descr_count = 1U,
        },
        .val_buffer_p = &switch_c_val_buffer_def
    },
};

/* p2p Server service definition */
static const ble_gatt_srv_def_t p2p_server_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_128(P2P_SERVER_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)p2p_server_chars,
       .chr_count = 2U,
   },
};

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t P2P_SERVER_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  aci_gatt_srv_read_event_rp0    *p_read;
  P2P_SERVER_NotificationEvt_t notification;
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
      if(p_attribute_modified->Attr_Handle == (P2P_SERVER_Context.Switch_CCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service1_Char_2 */

        /* USER CODE END Service1_Char_2 */
        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service1_Char_2_attribute_modified */

          /* USER CODE END Service1_Char_2_attribute_modified */

          /* Disabled Notification management */
        case (!BLE_GATT_SRV_CCCD_NOTIFICATION):
          /* USER CODE BEGIN Service1_Char_2_Disabled_BEGIN */

          /* USER CODE END Service1_Char_2_Disabled_BEGIN */
          notification.EvtOpcode = P2P_SERVER_SWITCH_C_NOTIFY_DISABLED_EVT;
          P2P_SERVER_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_2_Disabled_END */

          /* USER CODE END Service1_Char_2_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service1_Char_2_COMSVC_Notification_BEGIN */

          /* USER CODE END Service1_Char_2_COMSVC_Notification_BEGIN */
          notification.EvtOpcode = P2P_SERVER_SWITCH_C_NOTIFY_ENABLED_EVT;
          P2P_SERVER_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_2_COMSVC_Notification_END */

          /* USER CODE END Service1_Char_2_COMSVC_Notification_END */
          break;

        default:
          /* USER CODE BEGIN Service1_Char_2_default */

          /* USER CODE END Service1_Char_2_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (P2P_SERVER_Context.Switch_CCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (P2P_SERVER_Context.Led_CCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = P2P_SERVER_LED_C_WRITE_NO_RESP_EVT;
        /* USER CODE BEGIN Service1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        APP_DBG_MSG("-- GATT : LED CONFIGURATION RECEIVED\n");
        notification.DataTransfered.Length = p_attribute_modified->Attr_Data_Length;
        notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
        /* USER CODE END Service1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        P2P_SERVER_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (P2P_SERVER_Context.Led_CCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */

      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
	  if(p_read->Attribute_Handle == (P2P_SERVER_Context.Led_CCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (P2P_SERVER_Context.Led_CCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */

      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      if(p_write->Attribute_Handle == (P2P_SERVER_Context.Led_CCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE */

        /*USER CODE END Service1_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (P2P_SERVER_Context.Led_CCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
}/* end P2P_SERVER_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void P2P_SERVER_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(P2P_SERVER_Context);

  /* USER CODE BEGIN InitService1Svc_1 */

  /* USER CODE END InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(P2P_SERVER_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&p2p_server_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: p2p_Server, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: p2p_Server \n");
  }

  P2P_SERVER_Context.P2p_serverSvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &p2p_server_service);
  P2P_SERVER_Context.Led_CCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&p2p_server_chars[0]);
  P2P_SERVER_Context.Switch_CCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&p2p_server_chars[1]);

  /* USER CODE BEGIN InitService1Svc_2 */

  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering p2p_Server handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus P2P_SERVER_UpdateValue(P2P_SERVER_CharOpcode_t CharOpcode, P2P_SERVER_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case P2P_SERVER_LED_C:
      memcpy(led_c_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(led_c_val_buffer)));
      /* USER CODE BEGIN Service1_Char_Value_1*/

      /* USER CODE END Service1_Char_Value_1*/
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
tBleStatus P2P_SERVER_NotifyValue(P2P_SERVER_CharOpcode_t CharOpcode, P2P_SERVER_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Notify_Char_1 */

  /* USER CODE END Service1_App_Notify_Char_1 */

  switch(CharOpcode)
  {

    case P2P_SERVER_SWITCH_C:
      memcpy(switch_c_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(switch_c_val_buffer)));
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                P2P_SERVER_Context.Switch_CCharHdle + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify SWITCH_C command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify SWITCH_C command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_2*/

      /* USER CODE END Service1_Char_Value_2*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Notify_Char_2 */

  /* USER CODE END Service1_App_Notify_Char_2 */

  return ret;
}
