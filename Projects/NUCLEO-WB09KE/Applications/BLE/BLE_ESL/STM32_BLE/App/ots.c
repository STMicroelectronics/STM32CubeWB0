/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ots.c
  * @author  GPAM Application Team
  * @brief   Object Transfer Service definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "ots.h"
#include "ots_app.h"
#include "ble_evt.h"

#include "app_ble.h"

/* Private typedef -----------------------------------------------------------*/


typedef struct{
  uint16_t  OtsSvcHdle;				/**< OTS Service Handle */
  uint16_t  OtsFeatCharHdle;	    /**< OTS Feature Characteristic Handle */
  uint16_t  ObjNameCharHdle;	    /**< Object Name Characteristic Handle */
  uint16_t  ObjTypeCharHdle;	    /**< Object Type Characteristic Handle */
  uint16_t  ObjSizeCharHdle;	    /**< Object Size Characteristic Handle */
  uint16_t  ObjIdCharHdle;	        /**< Object ID Characteristic Handle */
  uint16_t  ObjPropCharHdle;	    /**< Object Properties Characteristic Handle */
  uint16_t  ObjActionCPCharHdle;	/**< Object Action Control Point Characteristic Handle */
  uint16_t  ObjListCPCharHdle;	    /**< Object List Control Point Characteristic Handle */
  
}OTS_Context_t;

/* Private defines -----------------------------------------------------------*/

/*
 * UUIDs for Object Transfer Service
 */
#define OBJECT_TRANSFER_SERVICE_UUID                                    (0x1825)
#define OTS_FEATURE_UUID                                                (0x2ABD)
#define OBJECT_NAME_UUID                                                (0x2ABE)
#define OBJECT_TYPE_UUID                                                (0x2ABF)
#define OBJECT_SIZE_UUID                                                (0x2AC0)
#define OBJECT_FIRST_CREATED_UUID                                       (0x2AC1)
#define OBJECT_LAST_MODIFIED_UUID                                       (0x2AC2)
#define OBJECT_ID_UUID                                                  (0x2AC3)
#define OBJECT_PROPERTIES_UUID                                          (0x2AC4)
#define OBJECT_ACTION_CONTROL_POINT_UUID                                (0x2AC5)
#define OBJECT_LIST_CONTROL_POINT_UUID                                  (0x2AC6)
#define OBJECT_LIST_FILTER_POINT_UUID                                   (0x2AC7)
#define OBJECT_CHANGED_UUID                                             (0x2AC8)

/* Op Codes for OACP procedures */
#define OACP_OPCODE_CREATE                                                  0x01
#define OACP_OPCODE_DELETE                                                  0x02
#define OACP_OPCODE_CALC_CHECKSUM                                           0x03
#define OACP_OPCODE_EXECUTE                                                 0x04
#define OACP_OPCODE_READ                                                    0x05
#define OACP_OPCODE_WRITE                                                   0x06
#define OACP_OPCODE_ABORT                                                   0x07
#define OACP_OPCODE_RESPONSE                                                0x60

/* OACP Features bitmask */
#define OACP_FEAT_CREATE                                              0x00000001
#define OACP_FEAT_DELETE                                              0x00000002
#define OACP_FEAT_CALC_CHECKSUM                                       0x00000004
#define OACP_FEAT_EXECUTE                                             0x00000008
#define OACP_FEAT_READ                                                0x00000010
#define OACP_FEAT_WRITE                                               0x00000020
#define OACP_FEAT_APPEND                                              0x00000040
#define OACP_FEAT_TRUNCATE                                            0x00000080
#define OACP_FEAT_PATCH                                               0x00000100
#define OACP_FEAT_ABORT                                               0x00000200

/* OLCP Features bitmask */
#define OLCP_FEAT_GOTO                                                0x00000001
#define OLCP_FEAT_ORDER                                               0x00000002
#define OLCP_FEAT_REQ_NUM_OBJ                                         0x00000004
#define OLCP_FEAT_CLEAR_MARK                                          0x00000008

#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET                             2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET                                  1

#define MAX_OACP_CHARAC_SIZE                                                  21
#define MAX_OLCP_CHARAC_SIZE                                                   7

/* OACP Write Op Code and patching Supported */
#define OACP_FEATURES_FIELD   (OACP_FEAT_WRITE|OACP_FEAT_TRUNCATE|OACP_FEAT_PATCH)
/* No optional OLCP features */
#define OLCP_FEATURES_FIELD                                           0x00000000

/* External variables --------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static OTS_Context_t OTS_Context;

/* Private function prototypes -----------------------------------------------*/

static uint8_t parseOACPFrame(aci_gatt_srv_write_event_rp0 *event);
static uint8_t parseOLCPFrame(aci_gatt_srv_write_event_rp0 *event);

/* Functions Definition ------------------------------------------------------*/

/* Private functions ----------------------------------------------------------*/

BLE_GATT_SRV_CCCD_DECLARE(obj_action_control_point, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_PERM_ENCRY_WRITE, 0);
BLE_GATT_SRV_CCCD_DECLARE(obj_list_control_point, CFG_BLE_NUM_RADIO_TASKS, BLE_GATT_SRV_PERM_ENCRY_WRITE, 0);

/* Device Information service characteristics definition */
static const ble_gatt_chr_def_t ots_chars[] = {
  {
    .properties = BLE_GATT_SRV_CHAR_PROP_READ,
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
    .min_key_size = 16,
    .uuid = BLE_UUID_INIT_16(OTS_FEATURE_UUID),
    .val_buffer_p = NULL,
  },
  {
    .properties = BLE_GATT_SRV_CHAR_PROP_READ,
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
    .min_key_size = 16,
    .uuid = BLE_UUID_INIT_16(OBJECT_NAME_UUID),
    .val_buffer_p = NULL,
  },
  {
    .properties = BLE_GATT_SRV_CHAR_PROP_READ,
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
    .min_key_size = 16,
    .uuid = BLE_UUID_INIT_16(OBJECT_TYPE_UUID),
    .val_buffer_p = NULL,
  },
  {
    .properties = BLE_GATT_SRV_CHAR_PROP_READ,
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
    .min_key_size = 16,
    .uuid = BLE_UUID_INIT_16(OBJECT_SIZE_UUID),
    .val_buffer_p = NULL,
  },
  {
    .properties = BLE_GATT_SRV_CHAR_PROP_READ,
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
    .min_key_size = 16,
    .uuid = BLE_UUID_INIT_16(OBJECT_ID_UUID),
    .val_buffer_p = NULL,
  },
  {
    .properties = BLE_GATT_SRV_CHAR_PROP_READ,
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
    .min_key_size = 16,
    .uuid = BLE_UUID_INIT_16(OBJECT_PROPERTIES_UUID),
    .val_buffer_p = NULL,
  },
  {
    .properties = BLE_GATT_SRV_CHAR_PROP_WRITE|BLE_GATT_SRV_CHAR_PROP_INDICATE,
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ|BLE_GATT_SRV_PERM_ENCRY_WRITE,
    .min_key_size = 16,
    .uuid = BLE_UUID_INIT_16(OBJECT_ACTION_CONTROL_POINT_UUID),
    .val_buffer_p = NULL,
    .descrs = {
      .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(obj_action_control_point),
      .descr_count = 1U,
    },
  },
  {
    .properties = BLE_GATT_SRV_CHAR_PROP_WRITE|BLE_GATT_SRV_CHAR_PROP_INDICATE,
    .permissions = BLE_GATT_SRV_PERM_ENCRY_READ|BLE_GATT_SRV_PERM_ENCRY_WRITE,
    .min_key_size = 16,
    .uuid = BLE_UUID_INIT_16(OBJECT_LIST_CONTROL_POINT_UUID),
    .val_buffer_p = NULL,
    .descrs = {
      .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(obj_list_control_point),
      .descr_count = 1U,
    },
  },
};

/* Device Information service definition */
ble_gatt_srv_def_t ots_service_def = {
   .type = BLE_GATT_SRV_SECONDARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_16(OBJECT_TRANSFER_SERVICE_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)ots_chars,
       .chr_count = sizeof(ots_chars)/sizeof(ble_gatt_chr_def_t),
   },
};


/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t OTS_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_read_event_rp0    *p_read;
  aci_gatt_srv_write_event_rp0   *p_write;

  switch(p_evt->ecode)
  {
    case ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE:
    {
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
	  if(p_read->Attribute_Handle == (OTS_Context.OtsFeatCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
        uint32_t ots_feature[2] = {OACP_FEATURES_FIELD, OLCP_FEATURES_FIELD};
        
        aci_gatt_srv_resp(p_read->Connection_Handle,
                          BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                          p_read->Attribute_Handle,
                          BLE_ATT_ERR_NONE,
                          sizeof(ots_feature),
                          (uint8_t *)ots_feature);
	  }
      else if(p_read->Attribute_Handle == (OTS_Context.ObjNameCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
        char * name_p;
        
        OTS_GetCurrentObjName(&name_p);
        
        aci_gatt_srv_resp(p_read->Connection_Handle,
                          BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                          p_read->Attribute_Handle,
                          BLE_ATT_ERR_NONE,
                          strlen(name_p),
                          (uint8_t *)name_p);
	  }
      else if(p_read->Attribute_Handle == (OTS_Context.ObjTypeCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
        uint8_t uuid_type;
        uint8_t *uuid_p;
        
        OTS_GetCurrentObjType(&uuid_type, &uuid_p);
        
        aci_gatt_srv_resp(p_read->Connection_Handle,
                          BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                          p_read->Attribute_Handle,
                          BLE_ATT_ERR_NONE,
                          (uuid_type == 0)? 2 : 16,
                          uuid_p);
	  }
      else if(p_read->Attribute_Handle == (OTS_Context.ObjSizeCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
        uint32_t obj_size[2]; /* current size and allocated size */
        
        OTS_GetCurrentObjSize(&obj_size[0], &obj_size[1]);
        
        aci_gatt_srv_resp(p_read->Connection_Handle,
                          BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                          p_read->Attribute_Handle,
                          BLE_ATT_ERR_NONE,
                          sizeof(obj_size),
                          (uint8_t *)obj_size);
	  }
      else if(p_read->Attribute_Handle == (OTS_Context.ObjIdCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
        uint8_t *id_p;
        
        OTS_GetCurrentObjID(&id_p);
        
        aci_gatt_srv_resp(p_read->Connection_Handle,
                          BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                          p_read->Attribute_Handle,
                          BLE_ATT_ERR_NONE,
                          OBJECT_ID_SIZE,
                          id_p);
	  }
      else if(p_read->Attribute_Handle == (OTS_Context.ObjPropCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
        uint32_t obj_prop;
        
        OTS_GetCurrentObjProp(&obj_prop);
        
        aci_gatt_srv_resp(p_read->Connection_Handle,
                          BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                          p_read->Attribute_Handle,
                          BLE_ATT_ERR_NONE,
                          sizeof(obj_prop),
                          (uint8_t *)&obj_prop);
	  }
      
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      p_write = (aci_gatt_srv_write_event_rp0*)p_evt->data;
      
      uint8_t att_error = BLE_ATT_ERR_NONE;
      
      if(p_write->Attribute_Handle == (OTS_Context.ObjActionCPCharHdle  + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        
        if(p_write->Data_Length > MAX_OACP_CHARAC_SIZE)
        {
          att_error = BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN;          
        }
        else
        {
          tBleStatus ret;
          uint16_t val_len;
          uint8_t  *val_p = NULL;
          uint16_t cccd_val;
          
          ret = aci_gatt_srv_read_multiple_instance_handle_value(p_write->Connection_Handle,
                                                                 OTS_Context.ObjActionCPCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET,
                                                                 &val_len, &val_p);
          if(ret != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("Read CCC error\n");
            
            return return_value;
          }          
          cccd_val = LE_TO_HOST_16(val_p);          
          if((cccd_val & BLE_GATT_SRV_CCCD_INDICATION) == 0)
          {
            att_error = BLE_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
          }
        }
        
        if(att_error == 0)
        {
          att_error = parseOACPFrame(p_write);
        }

        if (p_write->Resp_Needed == 1U)
        {          
            aci_gatt_srv_resp(p_write->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_write->Attribute_Handle,
                              att_error,
                              0,
                              NULL);
        }
      }
      else if(p_write->Attribute_Handle == (OTS_Context.ObjListCPCharHdle  + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
      {
        return_value = BLEEVT_Ack;
        
        if(p_write->Data_Length > MAX_OLCP_CHARAC_SIZE)
        {
          att_error = BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN;          
        }
        else
        {
          tBleStatus ret;
          uint16_t val_len;
          uint8_t  *val_p = NULL;
          uint16_t cccd_val;
          
          ret = aci_gatt_srv_read_multiple_instance_handle_value(p_write->Connection_Handle,
                                                                 OTS_Context.ObjListCPCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET,
                                                                 &val_len, &val_p);
          if(ret != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("Read CCC error\n");
            
            return return_value;
          }          
          cccd_val = LE_TO_HOST_16(val_p);          
          if((cccd_val & BLE_GATT_SRV_CCCD_INDICATION) == 0)
          {
            att_error = BLE_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
          }
        }
        
        if(att_error == 0)
        {
          att_error = parseOLCPFrame(p_write);
        }

        if (p_write->Resp_Needed == 1U)
        {
            aci_gatt_srv_resp(p_write->Connection_Handle,
                              BLE_GATT_UNENHANCED_ATT_L2CAP_CID,
                              p_write->Attribute_Handle,
                              att_error,
                              0,
                              NULL);
        }
      }
      
      break;/* ACI_GATT_SRV_WRITE_VSEVT_CODE */
    }
    case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
    {
      aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
      p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_evt->data;
      UNUSED(p_tx_pool_available_event);

      break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
    }
    case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
    {
      aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
      p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_evt->data;
      UNUSED(p_exchange_mtu);

      break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
    }

  default:
    
    break;
  }
  
  return(return_value);
}/* end DIS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void OTS_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;

  /**
   *  Register the event handler to the BLE controller
   */
  if(BLEEVT_RegisterGattEvtHandler(OTS_EventHandler) != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering OTS handlers\n");
  }

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&ots_service_def);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: OTS, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: OTS \n");
  }

  OTS_Context.OtsSvcHdle            = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &ots_service_def);
  OTS_Context.OtsFeatCharHdle       = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ots_chars[0]);
  OTS_Context.ObjNameCharHdle       = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ots_chars[1]);
  OTS_Context.ObjTypeCharHdle       = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ots_chars[2]);
  OTS_Context.ObjSizeCharHdle       = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ots_chars[3]);
  OTS_Context.ObjIdCharHdle         = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ots_chars[4]);
  OTS_Context.ObjPropCharHdle       = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ots_chars[5]);
  OTS_Context.ObjActionCPCharHdle   = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ots_chars[6]);
  OTS_Context.ObjListCPCharHdle     = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&ots_chars[7]);
  
  /*
  APP_DBG_MSG("OTS Service handles:\n");
  APP_DBG_MSG("Service: 0x%04X\n", OTS_Context.OtsSvcHdle);
  APP_DBG_MSG("OTS Features: 0x%04X\n", OTS_Context.OtsFeatCharHdle);
  APP_DBG_MSG("Object Name: 0x%04X\n", OTS_Context.ObjNameCharHdle);
  APP_DBG_MSG("Object Type: 0x%04X\n", OTS_Context.ObjTypeCharHdle);
  APP_DBG_MSG("Object Size: 0x%04X\n", OTS_Context.ObjSizeCharHdle);
  APP_DBG_MSG("Object Id: 0x%04X\n", OTS_Context.ObjIdCharHdle);
  APP_DBG_MSG("Object Properties: 0x%04X\n", OTS_Context.ObjPropCharHdle);
  APP_DBG_MSG("Object Action CP: 0x%04X\n", OTS_Context.ObjActionCPCharHdle);
  APP_DBG_MSG("Object List CP: 0x%04X\n", OTS_Context.ObjListCPCharHdle);
  */

  return;
}

static uint8_t parseOACPFrame(aci_gatt_srv_write_event_rp0 *event)
{
  uint8_t op_code;
  uint8_t resp[3];
  uint8_t resp_len;
  uint8_t ret = OACP_RESULT_SUCCESS;
  
  op_code = event->Data[0];
  
  resp[0] = OACP_OPCODE_RESPONSE;         /* Op Code */
  resp[1] = op_code;                      /* Request Op Code */
  resp_len = 2;
  
  /* Only write procedure supported. */
  
  switch(op_code)
  {
  case OACP_OPCODE_WRITE: 
    {
      uint32_t offset, length;
      uint8_t mode;
      
      if(event->Data_Length != 10)
      {
        return BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN;
      }
      
      offset = LE_TO_HOST_32(&event->Data[1]);
      length = LE_TO_HOST_32(&event->Data[5]);
      mode = event->Data[9];
      
      ret = OTS_OACPWrite(offset, length, mode);
      
      resp[resp_len] = ret;
      resp_len += 1;
    }    
    break;
    
  default:
    
    resp[resp_len] = OACP_RESULT_NOT_SUPPORTED;    /* Result code */
    resp_len += 1;
    
    break;
  }
  
  aci_gatt_srv_notify(event->Connection_Handle, event->CID, event->Attribute_Handle, GATT_INDICATION, resp_len, resp);
  
  return BLE_ATT_ERR_NONE;
}

static uint8_t parseOLCPFrame(aci_gatt_srv_write_event_rp0 *event)
{
  uint8_t op_code;
  uint8_t resp[3];
  uint8_t resp_len;
  uint8_t ret = OLCP_RESULT_SUCCESS;
  
  if(event->Data_Length != 1)
  {
    /* Other commands are not supported. */
    return BLE_ATT_ERR_INVAL_ATTR_VALUE_LEN;
  }
  
  op_code = event->Data[0];
  
  resp[0] = OLCP_OPCODE_RESPONSE;         /* Op Code */
  resp[1] = op_code;                      /* Request Op Code */
  resp_len = 2;
  
  ret = OTS_OLCP(op_code);
  
  resp[resp_len] = ret;
  resp_len += 1;
  
  aci_gatt_srv_notify(event->Connection_Handle, event->CID, event->Attribute_Handle, GATT_INDICATION, resp_len, resp);
  
  return BLE_ATT_ERR_NONE;
}

