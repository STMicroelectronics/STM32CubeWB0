/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    esl.c
  * @author  MCD Application Team
  * @brief   esl definition.
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
#include "esl.h"
#include "esl_app.h"
#include "ble_evt.h"
#include "esl_device.h" 
#include "ots.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  ESL_SvcHdl;			                /**< ESL Service Handle */
  uint16_t  ESL_Address_CharHdl;			/**< ESL Address Characteristic Handle */
  uint16_t  AP_Sync_Key_Material_CharHdl;		/**< AP Sync Key Material Characteristic Handle */
  uint16_t  ESL_Resp_Key_Material_CharHdl;		/**< ESL Response Key Material Characteristic Handle */
  uint16_t  ESL_Curr_Abs_Time_CharHdl;			/**< ESL Current Absolute Time Characteristic Handle */
  uint16_t  ESL_Control_Point_CharHdl;			/**< ESL Control Point Characteristic Handle */
#if NUM_DISPLAYS
  uint16_t  ESL_Display_InformationHdl;                 /**< ESL Display Information Handle */
  uint16_t  ESL_Image_InformationHdl;                   /**< ESL Image Information Handle */
#endif
#if NUM_SENSORS
  uint16_t  ESL_Sensor_InformationHdl;                  /**< ESL Sensor Information Handle */
#endif
#if NUM_LEDS
  uint16_t  ESL_LED_InformationHdl;                     /**< ESL LED Information Handle */
#endif

/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}ESL_SERVICE_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET          2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET               1

#define ESL_ADDRESS_SIZE                                    2
#define AP_SYNC_KEY_MATERIAL_SIZE                           24
#define ESL_RESP_KEY_MATERIAL_SIZE                          24
#define ESL_CURR_ABS_TIME_SIZE                              4
#define ESL_CONTROL_POINT_SIZE                              17
#define ESL_SENSOR_INFO_SIZE                                5
#define ESL_LED_INFO_SIZE                                   1

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static ESL_SERVICE_Context_t ESL_SERVICE_Context;

/* USER CODE BEGIN PV */

#if NUM_DISPLAYS
/* The ESL Image Information characteristic contains a single field, called Max_Image_Index, 
   whose value is a 8-bit integer (uint8) in the range 0x00 to 0xFF. */
uint8_t ESL_Image_Info = NUM_IMAGES - 1;
#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

/*
 * UUIDs for ESL service
 */
#define ESL_SERVICE_UUID                0x1857
#define ESL_ADDRESS_UUID                0x2BF6
#define AP_SYNC_KEY_MATERIAL_UUID       0x2BF7
#define ESL_RESP_KEY_MATERIAL_UUID      0x2BF8
#define ESL_CURR_ABS_TIME_UUID          0x2BF9
#define ESL_CONTROL_POINT_UUID          0x2BFE
#define ESL_SENSOR_INFO_UUID            0x2BFC
#define ESL_LED_INFO_UUID               0x2BFD 

#define ESL_DISPLAY_INFO_UUID           0x2BFA
#define ESL_IMAGE_INFO_UUID             0x2BFB

BLE_GATT_SRV_CCCD_DECLARE(esl_control_point, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_PERM_ENCRY_WRITE,
                          BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

uint8_t esl_control_point_val_buffer[ESL_CONTROL_POINT_SIZE];

static ble_gatt_val_buffer_def_t esl_control_point_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG|BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = 0,
  .buffer_len = sizeof(esl_control_point_val_buffer),
  .buffer_p = esl_control_point_val_buffer
};

static const ble_gatt_chr_def_t esl_service_chars[] = {
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_WRITE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(ESL_ADDRESS_UUID),
    },
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_WRITE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(AP_SYNC_KEY_MATERIAL_UUID),
    },
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_WRITE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(ESL_RESP_KEY_MATERIAL_UUID),
    },
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_WRITE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(ESL_CURR_ABS_TIME_UUID),
    },
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP|BLE_GATT_SRV_CHAR_PROP_WRITE|BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_WRITE,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(ESL_CONTROL_POINT_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(esl_control_point),
            .descr_count = 1U,
        },
        .val_buffer_p = &esl_control_point_val_buffer_def
    },
#if NUM_DISPLAYS
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(ESL_DISPLAY_INFO_UUID),
    },    
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(ESL_IMAGE_INFO_UUID),
    },
#endif
#if NUM_SENSORS
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(ESL_SENSOR_INFO_UUID),
    },
#endif
#if NUM_LEDS
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(ESL_LED_INFO_UUID),
    }, 
#endif
};

ble_gatt_srv_def_t *included_services[] = {&ots_service_def};

/* ESL service definition */
static const ble_gatt_srv_def_t esl = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_16(ESL_SERVICE_UUID),
   .included_srv = {
     .incl_srv_count = 1,
     .included_srv_pp = included_services,
   },
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)esl_service_chars,
       .chr_count = sizeof(esl_service_chars)/sizeof(ble_gatt_chr_def_t),
   },
};

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t ESL_SERVICE_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_srv_write_event_rp0   *p_write;
  aci_gatt_srv_read_event_rp0    *p_read;  
  aci_att_srv_prepare_write_req_event_rp0 *p_prepare_write;
  /* USER CODE BEGIN Service1_EventHandler_1 */

  /* USER CODE END Service1_EventHandler_1 */
    
  switch(p_evt->ecode)
  {
    case ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
      p_attribute_modified = (aci_gatt_srv_attribute_modified_event_rp0*)p_evt->data;
      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
      p_attribute_modified = (aci_gatt_srv_attribute_modified_event_rp0*)p_evt->data;
      
      if(p_attribute_modified->Attr_Handle == (ESL_SERVICE_Context.ESL_Control_Point_CharHdl  + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        
        /* USER CODE BEGIN Service1_Char_5_ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        APP_DBG_MSG("Write on ECP\n"); 
        ESL_ControlPoint_received(p_attribute_modified->Attr_Data, p_attribute_modified->Attr_Data_Length);
        
        /* USER CODE END Service1_Char_5_ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
      }
      else if(p_attribute_modified->Attr_Handle == (ESL_SERVICE_Context.ESL_Control_Point_CharHdl  + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        /* USER CODE BEGIN Service1_Char_5 */
        
        /* USER CODE END Service1_Char_5 */
        switch(p_attribute_modified->Attr_Data[0])
        {
          /* USER CODE BEGIN Service1_Char_5_attribute_modified */
          
          /* USER CODE END Service1_Char_5_attribute_modified */

          /* Disabled Notification management */
        case (!BLE_GATT_SRV_CCCD_NOTIFICATION):
          /* USER CODE BEGIN Service1_Char_5_Disabled_BEGIN */
          APP_DBG_MSG("ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE ESL_SERVICE_CONTROL_POINT_NOTIFY_DISABLED_EVT\n");
          /* USER CODE END Service1_Char_5_Disabled_BEGIN */
          
          /* USER CODE BEGIN Service1_Char_5_Disabled_END */
          
          /* USER CODE END Service1_Char_5_Disabled_END */
          break;

          /* Enabled Notification management */
        case BLE_GATT_SRV_CCCD_NOTIFICATION:
          /* USER CODE BEGIN Service1_Char_5_COMSVC_Notification_BEGIN */
          APP_DBG_MSG("ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE ESL_SERVICE_CONTROL_POINT_NOTIFY_ENABLED_EVT\n");
          /* USER CODE END Service1_Char_5_COMSVC_Notification_BEGIN */
          
          /* USER CODE BEGIN Service1_Char_5_COMSVC_Notification_END */

          /* USER CODE END Service1_Char_5_COMSVC_Notification_END */          
          break;

        default:
          /* USER CODE BEGIN Service1_Char_5_default */

          /* USER CODE END Service1_Char_5_default */
          break;
        }
      } 

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
      
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */
      
      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
#if NUM_DISPLAYS
      if(p_read->Attribute_Handle == (ESL_SERVICE_Context.ESL_Display_InformationHdl + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
          return_value = BLEEVT_Ack;
          /*USER CODE BEGIN Service1_Char_6_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
          //APP_DBG_MSG("*** Read ESL_Display_InformationHdl \n");
          /*USER CODE END Service1_Char_6_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
          aci_gatt_srv_resp(p_read->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_read->Attribute_Handle,
                              BLE_ATT_ERR_NONE,
                              sizeof(ESL_Display_Info),
                              ESL_Display_Info);
          /*USER CODE BEGIN Service1_Char_6_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

          /*USER CODE END Service1_Char_6_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
      } /* if(p_read->Attribute_Handle == (ESL_SERVICE_Context.ESL_Display_InformationHdl + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
      if(p_read->Attribute_Handle == (ESL_SERVICE_Context.ESL_Image_InformationHdl + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
          return_value = BLEEVT_Ack;
          /*USER CODE BEGIN Service1_Char_7_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
          //APP_DBG_MSG("*** Read ESL_Image_InformationHdl \n");
          /*USER CODE END Service1_Char_7_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
          aci_gatt_srv_resp(p_read->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_read->Attribute_Handle,
                              BLE_ATT_ERR_NONE,
                              sizeof(ESL_Image_Info),
                              &ESL_Image_Info);          
          /*USER CODE BEGIN Service1_Char_7_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

          /*USER CODE END Service1_Char_7_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
      } /* if(p_read->Attribute_Handle == (ESL_SERVICE_Context.ESL_Image_InformationHdl + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
#endif
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */
#if NUM_SENSORS
      if(p_read->Attribute_Handle == (ESL_SERVICE_Context.ESL_Sensor_InformationHdl + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
          return_value = BLEEVT_Ack;
          /*USER CODE BEGIN Service1_Char_8_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
          //APP_DBG_MSG("*** Read ESL_Sensor_InformationHdl \n");
          /*USER CODE END Service1_Char_8_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
          aci_gatt_srv_resp(p_read->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_read->Attribute_Handle,
                              BLE_ATT_ERR_NONE,
                              sizeof(ESL_Sensor_info),
                              ESL_Sensor_info);
          /*USER CODE BEGIN Service1_Char_8_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

          /*USER CODE END Service1_Char_8_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
      } /* if(p_read->Attribute_Handle == (ESL_SERVICE_Context.ESL_Sensor_InformationHdl + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
#endif
#if NUM_LEDS
      if(p_read->Attribute_Handle == (ESL_SERVICE_Context.ESL_LED_InformationHdl + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
          return_value = BLEEVT_Ack;
          /*USER CODE BEGIN Service1_Char_9_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
          //APP_DBG_MSG("*** Read ESL_LED_InformationHdl \n");
          /*USER CODE END Service1_Char_9_ACI_GATT_SRV_READ_VSEVT_CODE_1 */
          aci_gatt_srv_resp(p_read->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_read->Attribute_Handle,
                              BLE_ATT_ERR_NONE,
                              sizeof(ESL_LED_info),
                              ESL_LED_info);          
          /*USER CODE BEGIN Service1_Char_9_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

          /*USER CODE END Service1_Char_9_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
      } /* if(p_read->Attribute_Handle == (ESL_SERVICE_Context.ESL_LED_InformationHdl + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
#endif
      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */      
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */
      uint8_t att_error = BLE_ATT_ERR_NONE;
      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      if(p_write->Attribute_Handle == (ESL_SERVICE_Context.ESL_Address_CharHdl  + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        
        /* USER CODE BEGIN Service1_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE */
        uint16_t ESL_address;
        
        ESL_address = LE_TO_HOST_16(&p_write->Data);
        APP_DBG_MSG("*** ESL_address: 0x%04X\n", ESL_address);
        att_error = ESL_APP_SetESLAddress(ESL_address);  

        if (p_write->Resp_Needed == 1U)
        {
            aci_gatt_srv_resp(p_write->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_write->Attribute_Handle,
                              att_error,
                              0,
                              NULL);
        }
        /* USER CODE END Service1_Char_1_ACI_GATT_SRV_WRITE_VSEVT_CODE */
      }
      else if(p_write->Attribute_Handle== (ESL_SERVICE_Context.AP_Sync_Key_Material_CharHdl  + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        
        /* USER CODE BEGIN Service1_Char_2_ACI_GATT_SRV_WRITE_VSEVT_CODE */
        ESL_APP_SetAPSyncKeyMaterial(p_write->Data);
            
        att_error = BLE_ATT_ERR_NONE;

        if (p_write->Resp_Needed == 1U)
        {
            aci_gatt_srv_resp(p_write->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_write->Attribute_Handle,
                              att_error,
                              0,
                              NULL);
        }        
        /* USER CODE END Service1_Char_2_ACI_GATT_SRV_WRITE_VSEVT_CODE */
      }
      else if(p_write->Attribute_Handle == (ESL_SERVICE_Context.ESL_Resp_Key_Material_CharHdl  + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        
        /* USER CODE BEGIN Service1_Char_3_ACI_GATT_SRV_WRITE_VSEVT_CODE */
        ESL_APP_SetESLResponseKeyMaterial(p_write->Data);

        att_error = BLE_ATT_ERR_NONE;

        if (p_write->Resp_Needed == 1U)
        {
            aci_gatt_srv_resp(p_write->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_write->Attribute_Handle,
                              att_error,
                              0,
                              NULL);
        }         
        /* USER CODE END Service1_Char_3_ACI_GATT_SRV_WRITE_VSEVT_CODE */
      }
      else if(p_write->Attribute_Handle == (ESL_SERVICE_Context.ESL_Curr_Abs_Time_CharHdl  + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        
        /* USER CODE BEGIN Service1_Char_4_ACI_GATT_SRV_WRITE_VSEVT_CODE */
        uint32_t currAbsTime;
        
        currAbsTime = LE_TO_HOST_32(&p_write->Data);
        ESL_APP_SetCurrentAbsoluteTime(currAbsTime);

        att_error = BLE_ATT_ERR_NONE;

        if (p_write->Resp_Needed == 1U)
        {
            aci_gatt_srv_resp(p_write->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_write->Attribute_Handle,
                              att_error,
                              0,
                              NULL);
        }        
        /* USER CODE END Service1_Char_4_ACI_GATT_SRV_WRITE_VSEVT_CODE */
      }
      
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_WRITE_END */
      
      /* USER CODE END EVT_BLUE_GATT_SRV_WRITE_END */  
      break;/* ACI_GATT_SRV_WRITE_VSEVT_CODE */
    }
    
    case ACI_ATT_SRV_PREPARE_WRITE_REQ_VSEVT_CODE:
    {      
      p_prepare_write = (aci_att_srv_prepare_write_req_event_rp0*)p_evt->data;
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */
      return_value = BLEEVT_Ack;
      
      uint8_t att_error = BLE_ATT_ERR_REQ_NOT_SUPP;
      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */
      
      aci_gatt_srv_resp(p_prepare_write->Connection_Handle,
                          BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                          p_prepare_write->Attribute_Handle,
                          att_error,
                          0,
                          NULL);
      
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_WRITE_END */
      
      /* USER CODE END EVT_BLUE_GATT_SRV_WRITE_END */  
      break;/* ACI_ATT_SRV_PREPARE_WRITE_REQ_VSEVT_CODE */
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
}/* end ESL_SERVICE_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void ESL_SERVICE_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  uint8_t charac_index = 0;
  UNUSED(ESL_SERVICE_Context);

  /* USER CODE BEGIN InitService1Svc_1 */

  /* USER CODE END InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  ret = BLEEVT_RegisterGattEvtHandler(ESL_SERVICE_EventHandler);
  APP_DBG_MSG("  BLEEVT_RegisterGattEvtHandler ret: 0x%x \n", ret);
  
  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&esl);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: ESL, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: ESL \n");
  }

  ESL_SERVICE_Context.ESL_SvcHdl = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &esl);
  ESL_SERVICE_Context.ESL_Address_CharHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
  ESL_SERVICE_Context.AP_Sync_Key_Material_CharHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
  ESL_SERVICE_Context.ESL_Resp_Key_Material_CharHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
  ESL_SERVICE_Context.ESL_Curr_Abs_Time_CharHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
  ESL_SERVICE_Context.ESL_Control_Point_CharHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
#if NUM_DISPLAYS
  ESL_SERVICE_Context.ESL_Display_InformationHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
  ESL_SERVICE_Context.ESL_Image_InformationHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
#endif
#if NUM_SENSORS
  ESL_SERVICE_Context.ESL_Sensor_InformationHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
#endif
#if NUM_LEDS
  ESL_SERVICE_Context.ESL_LED_InformationHdl = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&esl_service_chars[charac_index++]);
#endif
  /* USER CODE BEGIN InitService1Svc_2 */

  /* USER CODE END InitService1Svc_2 */

  return;
}

/**
 * @brief  Characteristic notification
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the data to be notified to the client
 * @param  ConnectionHandle: connection handle identifying the client to be notified.
 *
 */
tBleStatus ESL_SERVICE_NotifyValue(ESL_SERVICE_CharOpcode_t CharOpcode, ESL_SERVICE_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Notify_Char_1 */

  /* USER CODE END Service1_App_Notify_Char_1 */

  switch(CharOpcode)
  {

    case ESL_SERVICE_CONTROL_POINT:
      memcpy(esl_control_point_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(esl_control_point_val_buffer)));
      ret = aci_gatt_srv_notify(ConnectionHandle,
                                BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                                ESL_SERVICE_Context.ESL_Control_Point_CharHdl + 1,
                                GATT_NOTIFICATION,
                                pData->Length, /* charValueLen */
                                (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Fail   : aci_gatt_srv_notify CONTROL_POINT command, error code: 0x%02X\n", ret);
      }
      else
      {
        APP_DBG_MSG("  Success: aci_gatt_srv_notify CONTROL_POINT command\n");
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
