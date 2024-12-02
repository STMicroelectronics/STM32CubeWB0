/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hrs.c
  * @author  MCD Application Team
  * @brief   hrs definition.
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
#include "hrs.h"
#include "hrs_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  HrsSvcHdle;				/**< Hrs Service Handle */
  uint16_t  HrmeCharHdle;			/**< HRME Characteristic Handle */
  uint16_t  BslCharHdle;			/**< BSL Characteristic Handle */
  uint16_t  HrcpCharHdle;			/**< HRCP Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}HRS_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */
#define HRS_CNTL_POINT_RESET_ENERGY_EXPENDED      (0x01)
#define HRS_CNTL_POINT_VALUE_IS_SUPPORTED         (0x00)
#define HRS_CNTL_POINT_VALUE_NOT_SUPPORTED        (0x80)
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define HRME_SIZE        23	/* Heart Rate Measurement Characteristic size */
#define BSL_SIZE        1	/* Body Sensor Location Characteristic size */
#define HRCP_SIZE        1	/* Heart Rate Control Point Characteristic size */
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static HRS_Context_t HRS_Context;

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
 * UUIDs for HRS service
 */
#define HRS_UUID			(0x180D)
#define HEART_RATE_MEASUREMENT_UUID			(0x2A37)
#define BODY_SENSOR_LOCATION_UUID			(0x2A38)
#define HEART_RATE_CONTROL_POINT_UUID			(0x2A39)

BLE_GATT_SRV_CCCD_DECLARE(hrme, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t bsl_val_buffer[BSL_SIZE];

static ble_gatt_val_buffer_def_t bsl_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = BSL_SIZE,
  .buffer_len = sizeof(bsl_val_buffer),
  .buffer_p = bsl_val_buffer
};

/* HRS service HRME (notification) characteristics definition */
static const ble_gatt_chr_def_t hrs_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(HEART_RATE_MEASUREMENT_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(hrme),
            .descr_count = 1U,
        },
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(BODY_SENSOR_LOCATION_UUID),
        .val_buffer_p = &bsl_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(HEART_RATE_CONTROL_POINT_UUID),
    },
};

/* HRS service definition */
static const ble_gatt_srv_def_t hrs_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_16(HRS_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)hrs_chars,
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
static BLEEVT_EvtAckStatus_t HRS_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  aci_gatt_srv_read_event_rp0    *p_read;
  HRS_NotificationEvt_t notification;
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
      if(p_attribute_modified->Attr_Handle == (HRS_Context.HrmeCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
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
          APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HRS_NOTIFICATION_DISABLED\n");
          /* USER CODE END Service1_Char_1_Disabled_BEGIN */
          notification.EvtOpcode = HRS_HRME_NOTIFY_DISABLED_EVT;
          HRS_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_1_Disabled_END */

          /* USER CODE END Service1_Char_1_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_BEGIN */
          APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HRS_NOTIFICATION_ENABLED\n");
          /* USER CODE END Service1_Char_1_COMSVC_Notification_BEGIN */
          notification.EvtOpcode = HRS_HRME_NOTIFY_ENABLED_EVT;
          HRS_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_END */

          /* USER CODE END Service1_Char_1_COMSVC_Notification_END */
          break;

        default:
          /* USER CODE BEGIN Service1_Char_1_default */

          /* USER CODE END Service1_Char_1_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (HRS_Context.HrmeCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      
      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */
      
      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
	  if(p_read->Attribute_Handle == (HRS_Context.BslCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service1_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service1_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service1_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service1_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (HRS_Context.BslCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */

      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      if(p_write->Attribute_Handle == (HRS_Context.HrcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service1_Char_3_ACI_GATT_SRV_WRITE_VSEVT_CODE */
        
        if (p_write->Data[0] == HRS_CNTL_POINT_RESET_ENERGY_EXPENDED)
        {
          /* received a correct value for HRM control point char */
          aci_gatt_srv_resp(p_write->Connection_Handle,
                            BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                            p_write->Attribute_Handle,
                            (uint8_t)HRS_CNTL_POINT_VALUE_IS_SUPPORTED, /* err_code */
                            p_write->Data_Length,
                            (uint8_t *)&p_write->Data[0]);
          /**
           * Notify the application to Reset The Energy Expended Value
           */
          notification.EvtOpcode = HRS_RESET_ENERGY_EXPENDED_EVT;
          HRS_Notification(&notification);
        }
        else
        {
          /* received value of HRM control point char is incorrect */
          aci_gatt_srv_resp(p_write->Connection_Handle,
                            BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                            p_write->Attribute_Handle,
                            (uint8_t)HRS_CNTL_POINT_VALUE_NOT_SUPPORTED, /* err_code */
                            p_write->Data_Length,
                            (uint8_t *)&p_write->Data[0]);
        }
        
        /*USER CODE END Service1_Char_3_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (HRS_Context.HrcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
}/* end HRS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void HRS_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(HRS_Context);

  /* USER CODE BEGIN InitService1Svc_1 */

  /* USER CODE END InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(HRS_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&hrs_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: HRS, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: HRS \n");
  }

  HRS_Context.HrsSvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &hrs_service);
  HRS_Context.HrmeCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hrs_chars[0]);
  HRS_Context.BslCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hrs_chars[1]);
  HRS_Context.HrcpCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hrs_chars[2]);

  /* USER CODE BEGIN InitService1Svc_2 */

  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering HRS handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus HRS_UpdateValue(HRS_CharOpcode_t CharOpcode, HRS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case HRS_BSL:
      memcpy(bsl_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(bsl_val_buffer)));
      /* USER CODE BEGIN Service1_Char_Value_2*/

      /* USER CODE END Service1_Char_Value_2*/
      break;

    case HRS_HRCP:
      /* USER CODE BEGIN Service1_Char_Value_3*/

      /* USER CODE END Service1_Char_Value_3*/
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
tBleStatus HRS_NotifyValue(HRS_CharOpcode_t CharOpcode, HRS_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Notify_Char_1 */

  /* USER CODE END Service1_App_Notify_Char_1 */

  switch(CharOpcode)
  {
    case HRS_HRME:
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                HRS_Context.HrmeCharHdle + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify HRME command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify HRME command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1*/

      /* USER CODE END Service1_Char_Value_1*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Notify_Char_2 */

  /* USER CODE END Service1_App_Notify_Char_2 */

  return ret;
}
