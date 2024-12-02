/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hts.c
  * @author  MCD Application Team
  * @brief   hts definition.
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
#include "hts.h"
#include "hts_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  HtsSvcHdle;				/**< Hts Service Handle */
  uint16_t  TemmCharHdle;			/**< TEMM Characteristic Handle */
  uint16_t  TetCharHdle;			/**< TET Characteristic Handle */
  uint16_t  IntCharHdle;			/**< INT Characteristic Handle */
  uint16_t  MeiCharHdle;			/**< MEI Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}HTS_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define TEMM_SIZE        13	/* Temperature Measurement Characteristic size */
#define TET_SIZE        1	/* Temperature Type Characteristic size */
#define INT_SIZE        13	/* Intermediate Temperature Characteristic size */
#define MEI_SIZE        2	/* Measurement Interval Characteristic size */
/* USER CODE BEGIN PM */
#define INTERVAL_OUT_OF_RANGE                           (0x80)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static HTS_Context_t HTS_Context;

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
 * UUIDs for Health Thermometer service
 */
#define HEALTH_THERMOMETER_UUID			(0x1809)
#define TEMPERATURE_MEASUREMENT_UUID			(0x2A1C)
#define TEMPERATURE_TYPE_UUID			(0x2A1D)
#define INTERMEDIATE_TEMPERATURE_UUID			(0x2A1E)
#define MEASUREMENT_INTERVAL_UUID			(0x2A21)

BLE_GATT_SRV_CCCD_DECLARE(temm, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);
BLE_GATT_SRV_CCCD_DECLARE(int, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);
BLE_GATT_SRV_CCCD_DECLARE(mei, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t tet_val_buffer[TET_SIZE];

static ble_gatt_val_buffer_def_t tet_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = TET_SIZE,
  .buffer_len = sizeof(tet_val_buffer),
  .buffer_p = tet_val_buffer
};

uint8_t mei_val_buffer[MEI_SIZE];

static ble_gatt_val_buffer_def_t mei_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = MEI_SIZE,
  .buffer_len = sizeof(mei_val_buffer),
  .buffer_p = mei_val_buffer
};

/* Health Thermometer service INT (notification) characteristics definition */
static const ble_gatt_chr_def_t hts_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_INDICATE,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(TEMPERATURE_MEASUREMENT_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(temm),
            .descr_count = 1U,
        },
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(TEMPERATURE_TYPE_UUID),
        .val_buffer_p = &tet_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(INTERMEDIATE_TEMPERATURE_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(int),
            .descr_count = 1U,
        },
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ | BLE_GATT_SRV_CHAR_PROP_WRITE | BLE_GATT_SRV_CHAR_PROP_INDICATE,
        .permissions = BLE_GATT_SRV_PERM_AUTHEN_WRITE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(MEASUREMENT_INTERVAL_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(mei),
            .descr_count = 1U,
        },
        .val_buffer_p = &mei_val_buffer_def
    },
};

/* Health Thermometer service definition */
static const ble_gatt_srv_def_t hts_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_16(HEALTH_THERMOMETER_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)hts_chars,
       .chr_count = 4U,
   },
};

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t HTS_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  aci_gatt_srv_read_event_rp0    *p_read;
  HTS_NotificationEvt_t notification;
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
      if(p_attribute_modified->Attr_Handle == (HTS_Context.TemmCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service2_Char_1 */

        /* USER CODE END Service2_Char_1 */

        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service2_Char_1_attribute_modified */

          /* USER CODE END Service2_Char_1_attribute_modified */

          /* Disabled Indication management */
        case (!BLE_GATT_SRV_CCCD_INDICATION):
          /* USER CODE BEGIN Service2_Char_1_Disabled_BEGIN */
          APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_TEMM_INDICATE_DISABLED_EVT\n");
          /* USER CODE END Service2_Char_1_Disabled_BEGIN */
          notification.EvtOpcode = HTS_TEMM_INDICATE_DISABLED_EVT;
          HTS_Notification(&notification);
          /* USER CODE BEGIN Service2_Char_1_Disabled_END */

          /* USER CODE END Service2_Char_1_Disabled_END */
          break;

          /* Enabled Indication management */
        case BLE_GATT_SRV_CCCD_INDICATION:
          /* USER CODE BEGIN Service2_Char_1_COMSVC_Indication_BEGIN */
          APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_TEMM_INDICATE_ENABLED_EVT\n");
          /* USER CODE END Service2_Char_1_COMSVC_Indication_BEGIN */
          notification.EvtOpcode = HTS_TEMM_INDICATE_ENABLED_EVT;
          HTS_Notification(&notification);
          /* USER CODE BEGIN Service2_Char_1_COMSVC_Indication_END */

          /* USER CODE END Service2_Char_1_COMSVC_Indication_END */
          break;

          default:
          /* USER CODE BEGIN Service2_Char_1_default */

          /* USER CODE END Service2_Char_1_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (HTS_Context.TEMMHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (HTS_Context.IntCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service2_Char_3 */
        
        /* USER CODE END Service2_Char_3 */
        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service2_Char_3_attribute_modified */
          
          /* USER CODE END Service2_Char_3_attribute_modified */

          /* Disabled Notification management */
        case (!BLE_GATT_SRV_CCCD_NOTIFICATION):
          /* USER CODE BEGIN Service2_Char_3_Disabled_BEGIN */
            APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_INT_NOTIFY_DISABLED_EVT\n");
          /* USER CODE END Service2_Char_3_Disabled_BEGIN */
          notification.EvtOpcode = HTS_INT_NOTIFY_DISABLED_EVT;
          HTS_Notification(&notification);
          /* USER CODE BEGIN Service2_Char_3_Disabled_END */

          /* USER CODE END Service2_Char_3_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service2_Char_3_COMSVC_Notification_BEGIN */
            APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_INT_NOTIFY_ENABLED_EVT\n");
          /* USER CODE END Service2_Char_3_COMSVC_Notification_BEGIN */
          notification.EvtOpcode = HTS_INT_NOTIFY_ENABLED_EVT;
          HTS_Notification(&notification);
          /* USER CODE BEGIN Service2_Char_3_COMSVC_Notification_END */
            
          /* USER CODE END Service2_Char_3_COMSVC_Notification_END */
          break;

        default:
          /* USER CODE BEGIN Service2_Char_3_default */
          
          /* USER CODE END Service2_Char_3_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (HTS_Context.IntCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service2_Char_4 */
        
        /* USER CODE END Service2_Char_4 */

        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service2_Char_4_attribute_modified */
          
          /* USER CODE END Service2_Char_4_attribute_modified */

          /* Disabled Indication management */
        case (!BLE_GATT_SRV_CCCD_INDICATION):
          /* USER CODE BEGIN Service2_Char_4_Disabled_BEGIN */
            APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_MEI_INDICATE_DISABLED_EVT\n");
          /* USER CODE END Service2_Char_4_Disabled_BEGIN */
          notification.EvtOpcode = HTS_MEI_INDICATE_DISABLED_EVT;
          HTS_Notification(&notification);
          /* USER CODE BEGIN Service2_Char_4_Disabled_END */

          /* USER CODE END Service2_Char_4_Disabled_END */
          break;

          /* Enabled Indication management */
        case BLE_GATT_SRV_CCCD_INDICATION:
          /* USER CODE BEGIN Service2_Char_4_COMSVC_Indication_BEGIN */

          /* USER CODE END Service2_Char_4_COMSVC_Indication_BEGIN */
          notification.EvtOpcode = HTS_MEI_INDICATE_ENABLED_EVT;
          HTS_Notification(&notification);
          /* USER CODE BEGIN Service2_Char_4_COMSVC_Indication_END */
            APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HTS_MEI_INDICATE_ENABLED_EVT\n");
          /* USER CODE END Service2_Char_4_COMSVC_Indication_END */
          break;

          default:
          /* USER CODE BEGIN Service2_Char_4_default */
          
          /* USER CODE END Service2_Char_4_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (HTS_Context.MEIHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      
      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */
      
      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
	  if(p_read->Attribute_Handle == (HTS_Context.TetCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service2_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service2_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service2_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service2_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (HTS_Context.TetCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
	  else if(p_read->Attribute_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service2_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
                aci_gatt_srv_resp(p_read->Connection_Handle,
                        BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                        p_read->Attribute_Handle,
                        0x00, /* err_code */
                        MEI_SIZE,
                        (uint8_t *)&mei_val_buffer[0]);
		/*USER CODE END Service2_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service2_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service2_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */

      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      if(p_write->Attribute_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service2_Char_4_ACI_GATT_SRV_WRITE_VSEVT_CODE */
        uint32_t validrangevalue =p_write->Data[0] + (p_write->Data[1] << 8);
        
        if ((validrangevalue == 0) ||
            ((validrangevalue >= BLE_CFG_HTS_TEMPERATURE_INTERVAL_MIN_VALUE) &&
             (validrangevalue <= BLE_CFG_HTS_TEMPERATURE_INTERVAL_MAX_VALUE)))
        {          
          /* received a correct value */
          aci_gatt_srv_resp(p_write->Connection_Handle,
                            BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                            p_write->Attribute_Handle,
                            0x00, /* err_code */
                            p_write->Data_Length,
                            (uint8_t *)&p_write->Data[0]);
          /**
           * Notify to the application a new interval has been received
           */
          notification.RangeInterval = (uint16_t)validrangevalue;
          notification.EvtOpcode = HTS_MEI_READ_EVT;
          HTS_Notification(&notification);
        }
        else
        {
          /* received value of HRM control point char is incorrect */
          aci_gatt_srv_resp(p_write->Connection_Handle,
                            BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                            p_write->Attribute_Handle,
                            INTERVAL_OUT_OF_RANGE, /* err_code */
                            p_write->Data_Length,
                            (uint8_t *)&p_write->Data[0]);
        }
        
        /*USER CODE END Service2_Char_4_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (HTS_Context.MeiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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

  /* USER CODE BEGIN Service2_EventHandler_2 */

  /* USER CODE END Service2_EventHandler_2 */

  return(return_value);
}/* end HTS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void HTS_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(HTS_Context);

  /* USER CODE BEGIN InitService2Svc_1 */

  /* USER CODE END InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(HTS_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&hts_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: HTS, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: HTS \n");
  }

  HTS_Context.HtsSvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &hts_service);
  HTS_Context.TemmCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hts_chars[0]);
  HTS_Context.TetCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hts_chars[1]);
  HTS_Context.IntCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hts_chars[2]);
  HTS_Context.MeiCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hts_chars[3]);

  /* USER CODE BEGIN InitService1Svc_2 */

  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering HTS handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus HTS_UpdateValue(HTS_CharOpcode_t CharOpcode, HTS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case HTS_TEMM:
      /* USER CODE BEGIN Service2_Char_Value_1*/

      /* USER CODE END Service2_Char_Value_1*/
      break;

    case HTS_TET:
      memcpy(tet_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(tet_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_2*/

      /* USER CODE END Service2_Char_Value_2*/
      break;

    case HTS_MEI:
      memcpy(mei_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(mei_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_4*/

      /* USER CODE END Service2_Char_Value_4*/
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
tBleStatus HTS_NotifyValue(HTS_CharOpcode_t CharOpcode, HTS_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Notify_Char_1 */

  /* USER CODE END Service2_App_Notify_Char_1 */

  switch(CharOpcode)
  {
    case HTS_TEMM:
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                HTS_Context.TemmCharHdle + 1,
                                GATT_INDICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify TEMM command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify TEMM command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_1*/

      /* USER CODE END Service2_Char_Value_1*/
      break;

    case HTS_INT:
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                HTS_Context.IntCharHdle + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify INT command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify INT command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_3*/

      /* USER CODE END Service2_Char_Value_3*/
      break;

    case HTS_MEI:
      memcpy(mei_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(mei_val_buffer)));
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                HTS_Context.MeiCharHdle + 1,
                                GATT_INDICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify MEI command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify MEI command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_4*/

      /* USER CODE END Service2_Char_Value_4*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Notify_Char_2 */

  /* USER CODE END Service2_App_Notify_Char_2 */

  return ret;
}
