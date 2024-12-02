/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HIDS.c
  * @author  MCD Application Team
  * @brief   HIDS definition.
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
#include "hids.h"
#include "hids_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */
#include "app_ble.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  HidsSvcHdle;				/**< Hids Service Handle */
  uint16_t  HiiCharHdle;			/**< HII Characteristic Handle */
  uint16_t  HcpCharHdle;			/**< HCP Characteristic Handle */
  uint16_t  RemCharHdle;			/**< REM Characteristic Handle */
  uint16_t  InputrepCharHdle;			/**< INPUTREP Characteristic Handle */
  uint16_t  OutputrepCharHdle;			/**< OUTPUTREP Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/
  uint16_t  InputRepRefDescHdle;          /**< INPUTREP REF Descriptor Handle */
  uint16_t  OutputRepRefDescHdle;         /**< OUTPUTREP REF Descriptor Handle */
/* USER CODE END Context */
}HIDS_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
extern hidService_Type hid_param;  
/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define HII_SIZE        4	/* HID Information Characteristic size */
#define HCP_SIZE        1	/* HID Control Point Characteristic size */
#define REM_SIZE        80	/* Report Map Characteristic size */
#define INPUTREP_SIZE        80	/* InputReport Characteristic size */
#define OUTPUTREP_SIZE        80	/* OutputReport Characteristic size */
/* USER CODE BEGIN PM */
#define REPREFDESCR_SIZE        2       /* Report Reference Descriptor size */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static HIDS_Context_t HIDS_Context;

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
 * UUIDs for HumanInterfaceDevice service
 */
#define HUMANINTERFACEDEVICE_UUID			(0x1812)
#define HID_INFORMATION_UUID			(0x2A4A)
#define HID_CONTROL_POINT_UUID			(0x2A4C)
#define REPORT_MAP_UUID			(0x2A4B)
#define INPUTREPORT_UUID			(0x2A4D)
#define OUTPUTREPORT_UUID			(0x2A4D)

BLE_GATT_SRV_CCCD_DECLARE(inputrep, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* Place holder for Characteristic Descriptors */  
static uint8_t inreportRef_val_buffer[REPREFDESCR_SIZE];

/* Input Report characteristic descriptor format definition */
static ble_gatt_val_buffer_def_t inreportRef_val_buffer_def[] =
{
  {
    .op_flags = 0,
    .val_len = REPREFDESCR_SIZE,
    .buffer_len = sizeof(inreportRef_val_buffer),
    .buffer_p = inreportRef_val_buffer,            
  },
};

/* Input Report characteristic descriptors definition */
ble_gatt_descr_def_t inputrep_descrs[] =
{
  {
    BLE_GATT_SRV_CCCD_DEF_STR_FIELDS(inputrep, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_CCCD_PERM_DEFAULT),
  },
  {
    .uuid = BLE_UUID_INIT_16(REPORT_REFERENCE_DESCRIPTOR_UUID),
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ | BLE_GATT_SRV_PERM_AUTHEN_READ,
    .properties = BLE_GATT_SRV_DESCR_PROP_READ,
    .min_key_size = BLE_GATT_SRV_MAX_ENCRY_KEY_SIZE,
    .val_buffer_p = &inreportRef_val_buffer_def[0],
  }
};

static uint8_t outreportRef_val_buffer[REPREFDESCR_SIZE];

/* Output Report characteristic descriptor format definition */
static ble_gatt_val_buffer_def_t outreportRef_val_buffer_def[] =
{
  {
    .op_flags = 0,
    .val_len = REPREFDESCR_SIZE,
    .buffer_len = sizeof(outreportRef_val_buffer),
    .buffer_p = outreportRef_val_buffer,
  },
};

/* Output Report characteristic descriptors definition */
ble_gatt_descr_def_t outputrep_descrs[] =
{
  {
    .uuid = BLE_UUID_INIT_16(REPORT_REFERENCE_DESCRIPTOR_UUID),
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ | BLE_GATT_SRV_PERM_AUTHEN_READ,
    .properties = BLE_GATT_SRV_DESCR_PROP_READ,
    .min_key_size = BLE_GATT_SRV_MAX_ENCRY_KEY_SIZE,
    .val_buffer_p = &outreportRef_val_buffer_def[0],
  }
};
/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t hii_val_buffer[HII_SIZE];

static ble_gatt_val_buffer_def_t hii_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = HII_SIZE,
  .buffer_len = sizeof(hii_val_buffer),
  .buffer_p = hii_val_buffer
};

uint8_t hcp_val_buffer[HCP_SIZE];

static ble_gatt_val_buffer_def_t hcp_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = HCP_SIZE,
  .buffer_len = sizeof(hcp_val_buffer),
  .buffer_p = hcp_val_buffer
};

uint8_t rem_val_buffer[REM_SIZE];

static ble_gatt_val_buffer_def_t rem_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = REM_SIZE,
  .buffer_len = sizeof(rem_val_buffer),
  .buffer_p = rem_val_buffer
};

uint8_t inputrep_val_buffer[INPUTREP_SIZE];

static ble_gatt_val_buffer_def_t inputrep_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = INPUTREP_SIZE,
  .buffer_len = sizeof(inputrep_val_buffer),
  .buffer_p = inputrep_val_buffer
};

uint8_t outputrep_val_buffer[OUTPUTREP_SIZE];

static ble_gatt_val_buffer_def_t outputrep_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = OUTPUTREP_SIZE,
  .buffer_len = sizeof(outputrep_val_buffer),
  .buffer_p = outputrep_val_buffer
};

/* HumanInterfaceDevice service HCP (write without response), INPUTREP (notification) OUTPUTREP (write without response), characteristics definition */
static const ble_gatt_chr_def_t hids_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(HID_INFORMATION_UUID),
/* USER CODE BEGIN Service1_char1_DESCRIPTORS_DEFINITION */
/* Place holder for Characteristic Descriptors */

/* USER CODE END Service1_char1_DESCRIPTORS_DEFINITION */
        .val_buffer_p = &hii_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(HID_CONTROL_POINT_UUID),
        .val_buffer_p = &hcp_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(REPORT_MAP_UUID),
        .val_buffer_p = &rem_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ | BLE_GATT_SRV_CHAR_PROP_WRITE | BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(INPUTREPORT_UUID),
/* USER CODE BEGIN Service1_char4_DESCRIPTORS_DEFINITION */
/* Place holder for Characteristic Descriptors */
        .descrs = {
            .descrs_p =  inputrep_descrs,
            .descr_count = 2U,
        }, 
/* USER CODE END Service1_char4_DESCRIPTORS_DEFINITION */
        .val_buffer_p = &inputrep_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ | BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP | BLE_GATT_SRV_CHAR_PROP_WRITE,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(OUTPUTREPORT_UUID),
/* USER CODE BEGIN Service1_char5_DESCRIPTORS_DEFINITION */
/* Place holder for Characteristic Descriptors */
        .descrs = {
            .descrs_p =  outputrep_descrs,
            .descr_count = 1U,
        }, 
/* USER CODE END Service1_char5_DESCRIPTORS_DEFINITION */
        .val_buffer_p = &outputrep_val_buffer_def
    },
};

/* HumanInterfaceDevice service definition */
static const ble_gatt_srv_def_t hids_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_16(HUMANINTERFACEDEVICE_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)hids_chars,
       .chr_count = 5U,
   },
};

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t HIDS_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  aci_gatt_srv_read_event_rp0    *p_read;
  HIDS_NotificationEvt_t notification;
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
      if(p_attribute_modified->Attr_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service1_Char_4 */

        /* USER CODE END Service1_Char_4 */
        switch(p_attribute_modified->Attr_Data[0])
		{
          /* USER CODE BEGIN Service1_Char_4_attribute_modified */

          /* USER CODE END Service1_Char_4_attribute_modified */

          /* Disabled Notification management */
        case (!BLE_GATT_SRV_CCCD_NOTIFICATION):
          /* USER CODE BEGIN Service1_Char_4_Disabled_BEGIN */
          APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HIDS_INPUTREP_NOTIFY_DISABLED_EVT\n");
          /* USER CODE END Service1_Char_4_Disabled_BEGIN */
          notification.EvtOpcode = HIDS_INPUTREP_NOTIFY_DISABLED_EVT;
          HIDS_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_4_Disabled_END */

          /* USER CODE END Service1_Char_4_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service1_Char_4_COMSVC_Notification_BEGIN */
          APP_DBG_MSG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE HIDS_INPUTREP_NOTIFY_ENABLED_EVT\n");
          /* USER CODE END Service1_Char_4_COMSVC_Notification_BEGIN */
          notification.EvtOpcode = HIDS_INPUTREP_NOTIFY_ENABLED_EVT;
          HIDS_Notification(&notification);
          /* USER CODE BEGIN Service1_Char_4_COMSVC_Notification_END */

          /* USER CODE END Service1_Char_4_COMSVC_Notification_END */
          break;

        default:
          /* USER CODE BEGIN Service1_Char_4_default */

          /* USER CODE END Service1_Char_4_default */
          break;
        }
      }  /* if(p_attribute_modified->Attr_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

      else if(p_attribute_modified->Attr_Handle == (HIDS_Context.HcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = HIDS_HCP_WRITE_NO_RESP_EVT;
        /* USER CODE BEGIN Service1_Char_2_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

        /* USER CODE END Service1_Char_2_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        HIDS_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (HIDS_Context.HcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
      else if(p_attribute_modified->Attr_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = HIDS_INPUTREP_WRITE_EVT;
        /* USER CODE BEGIN Service1_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

        /* USER CODE END Service1_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        HIDS_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
      else if(p_attribute_modified->Attr_Handle == (HIDS_Context.OutputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;

        notification.EvtOpcode = HIDS_OUTPUTREP_WRITE_NO_RESP_EVT;
        /* USER CODE BEGIN Service1_Char_5_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

        /* USER CODE END Service1_Char_5_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        HIDS_Notification(&notification);
      } /* if(p_attribute_modified->Attr_Handle == (HIDS_Context.OutputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */

      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
	  if(p_read->Attribute_Handle == (HIDS_Context.HiiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service1_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (HIDS_Context.HiiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
	  else if(p_read->Attribute_Handle == (HIDS_Context.RemCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service1_Char_3_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service1_Char_3_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service1_Char_3_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service1_Char_3_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (HIDS_Context.RemCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
	  else if(p_read->Attribute_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service1_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service1_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service1_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service1_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
	  else if(p_read->Attribute_Handle == (HIDS_Context.OutputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service1_Char_5_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service1_Char_5_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service1_Char_5_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service1_Char_5_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (HIDS_Context.OutputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */

      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      if(p_write->Attribute_Handle == (HIDS_Context.HcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service1_Char_2_ACI_GATT_SRV_WRITE_VSEVT_CODE */

        /*USER CODE END Service1_Char_2_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (HIDS_Context.HcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      else if(p_write->Attribute_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service1_Char_4_ACI_GATT_SRV_WRITE_VSEVT_CODE */
        aci_gatt_srv_resp(p_write->Connection_Handle,
                          BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                          p_write->Attribute_Handle,
                          0x00, /* write_status = 0 (no error))*/
                          p_write->Data_Length,
                          (uint8_t *)&p_write->Data[0]);
        /*USER CODE END Service1_Char_4_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (HIDS_Context.InputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      else if(p_write->Attribute_Handle == (HIDS_Context.OutputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /*USER CODE BEGIN Service1_Char_5_ACI_GATT_SRV_WRITE_VSEVT_CODE */

        /*USER CODE END Service1_Char_5_ACI_GATT_SRV_WRITE_VSEVT_CODE*/
      } /*if(p_write->Attribute_Handle == (HIDS_Context.OutputrepCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
}/* end HIDS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void HIDS_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(HIDS_Context);

  /* USER CODE BEGIN InitService1Svc_1 */
  
  /* USER CODE END InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(HIDS_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&hids_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: HIDS, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: HIDS \n");
  }

  HIDS_Context.HidsSvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &hids_service);
  HIDS_Context.HiiCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hids_chars[0]);
  HIDS_Context.HcpCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hids_chars[1]);
  HIDS_Context.RemCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hids_chars[2]);
  HIDS_Context.InputrepCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hids_chars[3]);
  HIDS_Context.OutputrepCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&hids_chars[4]);

  /* USER CODE BEGIN InitService1Svc_2 */
  HIDS_Context.InputRepRefDescHdle = aci_gatt_srv_get_descriptor_handle((ble_gatt_descr_def_t *)&inputrep_descrs[1]);
  HIDS_Context.OutputRepRefDescHdle = aci_gatt_srv_get_descriptor_handle((ble_gatt_descr_def_t *)&outputrep_descrs[0]);
  
  devContext.hidServHandle = HIDS_Context.HidsSvcHdle;
  devContext.deviceReadyToNotify = FALSE;
  
  if(hid_param.reportSupport)
  {
    uint8_t i, reportRef[2];
    
    devContext.num_reports = hid_param.num_reports;
    /* Allocate memory to hold report information: handle, ID and type.  */ 
    devContext.hidReportID = malloc(sizeof(uint8_t) * devContext.num_reports);    
    devContext.hidReportType = malloc(sizeof(uint8_t) * devContext.num_reports);
    
    for(i = 0; i < devContext.num_reports; i++)
    {
      devContext.hidReportID[i] = hid_param.reportReferenceDesc[i].ID;
      reportRef[0] = hid_param.reportReferenceDesc[i].ID;

      if(hid_param.reportReferenceDesc[i].type == INPUT_REPORT)
      {
        reportRef[1] = INPUT_REPORT;
        devContext.hidReportType[i] = INPUT_REPORT;
        memcpy(inputrep_descrs[1].val_buffer_p->buffer_p, reportRef, 2);
      }
      else if(hid_param.reportReferenceDesc[i].type == OUTPUT_REPORT)
      {
        reportRef[1] = OUTPUT_REPORT;
        devContext.hidReportType[i] = OUTPUT_REPORT;
        memcpy(outputrep_descrs[0].val_buffer_p->buffer_p, reportRef, 2);
      }
      else
      {
        APP_DBG_MSG("BLE_STATUS_INVALID_PARAMS\n");
      }
    } 
  }
  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering HIDS handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus HIDS_UpdateValue(HIDS_CharOpcode_t CharOpcode, HIDS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case HIDS_HII:
      memcpy(hii_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(hii_val_buffer)));
      /* USER CODE BEGIN Service1_Char_Value_1*/

      /* USER CODE END Service1_Char_Value_1*/
      break;

    case HIDS_HCP:
      memcpy(hcp_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(hcp_val_buffer)));
      /* USER CODE BEGIN Service1_Char_Value_2*/

      /* USER CODE END Service1_Char_Value_2*/
      break;

    case HIDS_REM:
      memcpy(rem_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(rem_val_buffer)));
      /* USER CODE BEGIN Service1_Char_Value_3*/

      /* USER CODE END Service1_Char_Value_3*/
      break;

    case HIDS_OUTPUTREP:
      memcpy(outputrep_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(outputrep_val_buffer)));
      /* USER CODE BEGIN Service1_Char_Value_5*/

      /* USER CODE END Service1_Char_Value_5*/
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
tBleStatus HIDS_NotifyValue(HIDS_CharOpcode_t CharOpcode, HIDS_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Notify_Char_1 */

  /* USER CODE END Service1_App_Notify_Char_1 */

  switch(CharOpcode)
  {

    case HIDS_INPUTREP:
      memcpy(inputrep_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(inputrep_val_buffer)));
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                HIDS_Context.InputrepCharHdle + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify INPUTREP command, error code: 0x%2X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify INPUTREP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_4*/

      /* USER CODE END Service1_Char_Value_4*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Notify_Char_2 */

  /* USER CODE END Service1_App_Notify_Char_2 */

  return ret;
}
