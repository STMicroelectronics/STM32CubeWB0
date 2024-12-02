/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SCPS.c
  * @author  MCD Application Team
  * @brief   SCPS definition.
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
#include "scps.h"
#include "scps_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */
#include "app_ble.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  ScpsSvcHdle;				/**< Scps Service Handle */
  uint16_t  SiwCharHdle;			/**< SIW Characteristic Handle */
  uint16_t  ScrCharHdle;			/**< SCR Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}SCPS_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define SIW_SIZE        4	/* Scan Interval Window Characteristic size */
#define SCR_SIZE        1	/* Scan Refresh Characteristic size */
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static SCPS_Context_t SCPS_Context;

/* USER CODE BEGIN PV */
extern devContext_Type devContext;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

/*
 * UUIDs for Scan Parameters service
 */
#define SCAN_PARAMETERS_UUID			(0x1813)
#define SCAN_INTERVAL_WINDOW_UUID			(0x2A4F)
#define SCAN_REFRESH_UUID			(0x2A31)

BLE_GATT_SRV_CCCD_DECLARE(scr, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t siw_val_buffer[SIW_SIZE];

static ble_gatt_val_buffer_def_t siw_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = SIW_SIZE,
  .buffer_len = sizeof(siw_val_buffer),
  .buffer_p = siw_val_buffer
};

/* Scan Parameters service SIW (write without response), SCR (notification) characteristics definition */
static const ble_gatt_chr_def_t scps_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(SCAN_INTERVAL_WINDOW_UUID),
        .val_buffer_p = &siw_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(SCAN_REFRESH_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(scr),
            .descr_count = 1U,
        },
    },
};

/* Scan Parameters service definition */
static const ble_gatt_srv_def_t scps_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_16(SCAN_PARAMETERS_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)scps_chars,
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
static BLEEVT_EvtAckStatus_t SCPS_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  SCPS_NotificationEvt_t notification;
  /* USER CODE BEGIN Service4_EventHandler_1 */

  /* USER CODE END Service4_EventHandler_1 */

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
      if(p_attribute_modified->Attr_Handle == (SCPS_Context.ScrCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service4_Char_2 */

        /* USER CODE END Service4_Char_2 */
        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service4_Char_2_attribute_modified */

          /* USER CODE END Service4_Char_2_attribute_modified */

          /* Disabled Notification management */
        case (!BLE_GATT_SRV_CCCD_NOTIFICATION):
          /* USER CODE BEGIN Service4_Char_2_Disabled_BEGIN */

          /* USER CODE END Service4_Char_2_Disabled_BEGIN */
          notification.EvtOpcode = SCPS_SCR_NOTIFY_DISABLED_EVT;
          SCPS_Notification(&notification);
          /* USER CODE BEGIN Service4_Char_2_Disabled_END */

          /* USER CODE END Service4_Char_2_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service4_Char_2_COMSVC_Notification_BEGIN */

          /* USER CODE END Service4_Char_2_COMSVC_Notification_BEGIN */
          notification.EvtOpcode = SCPS_SCR_NOTIFY_ENABLED_EVT;
          SCPS_Notification(&notification);
          /* USER CODE BEGIN Service4_Char_2_COMSVC_Notification_END */

          /* USER CODE END Service4_Char_2_COMSVC_Notification_END */
          break;

        default:
          /* USER CODE BEGIN Service4_Char_2_default */

          /* USER CODE END Service4_Char_2_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (SCPS_Context.ScrCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (SCPS_Context.SiwCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = SCPS_SIW_WRITE_NO_RESP_EVT;
        /* USER CODE BEGIN Service4_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

        /* USER CODE END Service4_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        SCPS_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (SCPS_Context.SiwCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
      if(p_write->Attribute_Handle == (SCPS_Context.SiwCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service4_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE */

        /*USER CODE END Service4_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (SCPS_Context.SiwCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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

    /* USER CODE END BLECORE_EVT */
  default:
    /* USER CODE BEGIN EVT_DEFAULT */

    /* USER CODE END EVT_DEFAULT */
    break;
  }

  /* USER CODE BEGIN Service4_EventHandler_2 */

  /* USER CODE END Service4_EventHandler_2 */

  return(return_value);
}/* end SCPS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void SCPS_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(SCPS_Context);

  /* USER CODE BEGIN InitService4Svc_1 */

  /* USER CODE END InitService4Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(SCPS_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&scps_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: SCPS, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: SCPS \n");
  }

  SCPS_Context.ScpsSvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &scps_service);
  SCPS_Context.SiwCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&scps_chars[0]);
  SCPS_Context.ScrCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&scps_chars[1]);

  /* USER CODE BEGIN InitService1Svc_2 */
  devContext.scanParameterServHandle = SCPS_Context.ScpsSvcHdle;
  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering SCPS handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus SCPS_UpdateValue(SCPS_CharOpcode_t CharOpcode, SCPS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service4_App_Update_Char_1 */

  /* USER CODE END Service4_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case SCPS_SIW:
      memcpy(siw_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(siw_val_buffer)));
      /* USER CODE BEGIN Service4_Char_Value_1*/

      /* USER CODE END Service4_Char_Value_1*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service4_App_Update_Char_2 */

  /* USER CODE END Service4_App_Update_Char_2 */

  return ret;
}

/**
 * @brief  Characteristic notification
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the data to be notified to the client
 * @param  ConnectionHandle: connection handle identifying the client to be notified.
 *
 */
tBleStatus SCPS_NotifyValue(SCPS_CharOpcode_t CharOpcode, SCPS_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service4_App_Notify_Char_1 */

  /* USER CODE END Service4_App_Notify_Char_1 */

  switch(CharOpcode)
  {

    case SCPS_SCR:
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                SCPS_Context.ScrCharHdle + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify SCR command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify SCR command\n");
      }
      /* USER CODE BEGIN Service4_Char_Value_2*/

      /* USER CODE END Service4_Char_Value_2*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service4_App_Notify_Char_2 */

  /* USER CODE END Service4_App_Notify_Char_2 */

  return ret;
}
