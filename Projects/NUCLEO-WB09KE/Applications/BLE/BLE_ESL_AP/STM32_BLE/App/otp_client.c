/**
  ******************************************************************************
  * @file    otp_client.c
  * @author  GPM WBL Application Team
  * @brief   Implementation of Object Client role of Object Transfer Profile.
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

#include "stm32wb0x.h"
#include "app_common.h"
#include "otp_client.h"
#include "gatt_client_app.h"
#include "stm32_seq.h"
#include "app_conf.h"

#define OTP_DEBUG                                                              0
    
/* SPSM for OTS */
#define SPSM_OTS                                                          0x0025

/* Object Properties Definition  */
#define OBJ_PROP_DELETE                                               0x00000001
#define OBJ_PROP_EXEC                                                 0x00000002
#define OBJ_PROP_READ                                                 0x00000004
#define OBJ_PROP_WRITE                                                0x00000008
#define OBJ_PROP_APPEND                                               0x00000010
#define OBJ_PROP_TRUNC                                                0x00000020
#define OBJ_PROP_PATCH                                                0x00000040
#define OBJ_PROP_MARK                                                 0x00000080

static void OTP_CLIENT_Process(void);

typedef enum
{
  OTP_CLIENT_STATE_DISCONNECTED = 0,
  OTP_CLIENT_STATE_IDLE,
  OTP_CLIENT_STATE_DISC_FEATURES,
  
}OTPClientState_t;

typedef enum
{
  OTP_EVENT_NONE = 0,
  OTP_EVENT_OLCP_RESPONSE,
  OTP_EVENT_OACP_RESPONSE,
  OTP_EVENT_DISCONNECTION,
  OTP_EVENT_TIMEOUT,
  OTP_EVENT_L2CAP_OPEN,
  OTP_EVENT_L2CAP_CREDITS,
  OTP_EVENT_L2CAP_ERROR,
}OTPEvent_t;

typedef struct
{
  OTPClientState_t state;
  uint16_t connection_handle;
  uint16_t cid;
  uint16_t peer_mtu;
  OTSHandleContext_t *OTSHandleContext_p;  
  uint32_t OACPFeautures;
  uint32_t OLCPFeautures;
  uint8_t CP_response_length;
  uint8_t CP_response[7];
  bool features_discovered;  /* TODO: use bitmask to save space. */
  bool truncate;
  OTPEvent_t event;
  VTIMER_HandleType timer;
  char *search_name;
}OTPClientContext_t;

OTPClientContext_t OTPClientContext;

void OTP_CLIENT_Init(void)
{
  UTIL_SEQ_RegTask(1U << CFG_TASK_OTP_CLIENT_PROC, UTIL_SEQ_RFU, OTP_CLIENT_Process);
}

void OTP_CLIENT_ConnectionComplete(OTSHandleContext_t *OTSHandleContext_p, uint16_t connection_handle)
{
  if(OTPClientContext.OTSHandleContext_p == NULL)
  {   
    OTPClientContext.OTSHandleContext_p = OTSHandleContext_p;
    
    OTPClientContext.connection_handle = connection_handle;  
    OTPClientContext.state = OTP_CLIENT_STATE_IDLE;
    OTPClientContext.cid = 0;
    OTPClientContext.OACPFeautures = 0;
    OTPClientContext.OLCPFeautures = 0;
    OTPClientContext.features_discovered = false;
    UTIL_SEQ_ClrEvt(1 << CFG_IDLEEVT_OTP_EVENT);
  }
}

void OTP_CLIENT_DisconnectionComplete(void)
{
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_OTP_EVENT);
  OTPClientContext.event = OTP_EVENT_DISCONNECTION;
  
  OTPClientContext.state = OTP_CLIENT_STATE_DISCONNECTED;
  OTPClientContext.OTSHandleContext_p = NULL;
  OTPClientContext.cid = 0;
  OTPClientContext.OACPFeautures = 0;
  OTPClientContext.OLCPFeautures = 0;
  OTPClientContext.features_discovered = false;
  
  //memset(&OTPClientContext, 0, sizeof(OTPClientContext));
}

void OTP_CLIENT_L2CAPDisconnectionComplete(void)
{
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_OTP_EVENT);
  OTPClientContext.event = OTP_EVENT_DISCONNECTION;
  
  OTPClientContext.cid = 0;
}

//TODO: do it automatically before first OTP operation? Need to have the handles passed in any case.
//TODO: add return parameter
uint8_t OTP_CLIENT_DiscoverFeatures(void)
{
  uint8_t *data_p;
  uint16_t data_length;
  uint8_t ret;
  
  if(OTPClientContext.state != OTP_CLIENT_STATE_IDLE)
  {
    return 1;
  }
  
  OTPClientContext.state = OTP_CLIENT_STATE_DISC_FEATURES;
  
  ret = GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->OTSFeatureValueHdl, &data_p, &data_length);
  
  if(ret == 0 && data_length == 8)
  {
    OTPClientContext.OACPFeautures = LE_TO_HOST_32(data_p);
    OTPClientContext.OLCPFeautures = LE_TO_HOST_32(data_p + 4);
    
    APP_DBG_MSG("OACP Features 0x%08X\n", OTPClientContext.OACPFeautures);
    APP_DBG_MSG("OLCP Features 0x%08X\n", OTPClientContext.OLCPFeautures);
  }
  
  OTPClientContext.state = OTP_CLIENT_STATE_IDLE;
  
  OTPClientContext.features_discovered = true;
  
  return 0;
}

static void proc_timeout(void *arg)
{
  APP_DBG_MSG("OLCP timeout\n");
  OTPClientContext.event = OTP_EVENT_TIMEOUT;
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_OTP_EVENT);
}

static int waitOLCPResponse(void)
{
  int ret = 0;
  bool end_wait = false;
  
  OTPClientContext.timer.callback = proc_timeout;
  HAL_RADIO_TIMER_StartVirtualTimer(&OTPClientContext.timer, OTP_PROCEDURE_TIMEOUT * 1000);
  
  //APP_DBG_MSG("Waiting response\n");
  
  while(1)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_OTP_EVENT);
    
    switch(OTPClientContext.event)
    {
    case OTP_EVENT_OLCP_RESPONSE:
      { 
#if OTP_DEBUG
        APP_DBG_MSG("Response: ");
        for(int i = 0; i < sizeof(OTPClientContext.CP_response_length); i++)
        {
          APP_DBG_MSG("%02X ", OTPClientContext.CP_response[i]);
        }
        APP_DBG_MSG("\n");
#endif
        
        if(OTPClientContext.CP_response[2] == OLCP_RESULT_SUCCESS)
        {
          //APP_DBG_MSG("OLCP Success\n");
          ret = 0;
        }
        else if(OTPClientContext.CP_response[2] == OLCP_RESULT_NO_OBJ ||
                OTPClientContext.CP_response[2] == OLCP_RESULT_OUT_OF_BOUNDS)
        {
          APP_DBG_MSG("OLCP List end\n");
          ret = 1;
        }
        else
        {
          APP_DBG_MSG("OLCP error\n");
          ret = -1;
        }
        end_wait = true;
      }
      break;
    case OTP_EVENT_DISCONNECTION:
    case OTP_EVENT_TIMEOUT:
      {
        APP_DBG_MSG("OLCP procedure interrupted\n");
        ret = -2;
        end_wait = true;
      }
      break;      
    default:
      break;
    }
    
    if(end_wait)
    {
      break;      
    }    
  }
  
  HAL_RADIO_TIMER_StopVirtualTimer(&OTPClientContext.timer);
  
  return ret;
}

static int waitOACPResponse(void)
{
  int ret = 0;
  bool end_wait = false;
  
  OTPClientContext.timer.callback = proc_timeout;
  HAL_RADIO_TIMER_StartVirtualTimer(&OTPClientContext.timer, OTP_PROCEDURE_TIMEOUT * 1000);
  
  //APP_DBG_MSG("Waiting response\n");
  
  while(1)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_OTP_EVENT);
    
    switch(OTPClientContext.event)
    {
    case OTP_EVENT_OACP_RESPONSE:
      {
#if OTP_DEBUG
        APP_DBG_MSG("Response: ");
        for(int i = 0; i < sizeof(OTPClientContext.CP_response_length); i++)
        {
          APP_DBG_MSG("%02X ", OTPClientContext.CP_response[i]);
        }
        APP_DBG_MSG("\n");
#endif        
        
        if(OTPClientContext.CP_response[2] == OACP_RESULT_SUCCESS)
        {
          //APP_DBG_MSG("OACP Success\n");
          ret = 0;
        }
        else
        {
          APP_DBG_MSG("OACP error\n");
          ret = -1;
        }
        end_wait = true;
      }
      break;
    case OTP_EVENT_DISCONNECTION:
    case OTP_EVENT_TIMEOUT:
      {
        APP_DBG_MSG("OACP procedure interrupted\n");
        ret = -2;
        end_wait = true;
      }
      break;      
    default:
      break;
    }
    
    if(end_wait)
    {
      break;      
    }    
  }
  
  HAL_RADIO_TIMER_StopVirtualTimer(&OTPClientContext.timer);
  
  return ret;
}

static int waitL2CAPEvent(OTPEvent_t event)
{  
  APP_DBG_MSG("Waiting L2CAP event %d\n", event);
  
  while(1)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_OTP_EVENT);
    
    if(OTPClientContext.event == event)
    {
      APP_DBG_MSG("L2CAP Event received\n");
      
      return 0;
    }
    else if(OTPClientContext.event == OTP_EVENT_DISCONNECTION ||
            OTPClientContext.event == OTP_EVENT_L2CAP_ERROR)
    {
      APP_DBG_MSG("L2CAP procedure failure\n");
      
      return -2;
    }
  }
}

int OTP_CLIENT_DiscoverAllObjects(objectFoundCB_t object_found_cb)
{
  uint8_t data;
  uint8_t *name_p = NULL;
  uint16_t name_length = 0;
  tBleStatus ble_ret;
  int olcp_ret;
  
  if(OTPClientContext.state != OTP_CLIENT_STATE_IDLE)
  {
    return -1;
  }
  
  UTIL_SEQ_ClrEvt(1 << CFG_IDLEEVT_OTP_EVENT);
  
  if(OTPClientContext.features_discovered == false)
  {
    OTP_CLIENT_DiscoverFeatures();
  }
  
  for(uint8_t i = 0; i < 3; i++)
  {
    if(OTPClientContext.OTSHandleContext_p->ObjListFilterValueHdl[i] != 0)
    {
      uint8_t *data_p;
      uint16_t data_length;
      /* Disable Filter. */
      data = 0;
      ble_ret = GATT_CLIENT_Write_Char(OTPClientContext.OTSHandleContext_p->ObjListFilterValueHdl[i],
                             &data, 1);
      if(ble_ret != BLE_STATUS_SUCCESS)
      {
        return -1;
      }
      ble_ret = GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjListFilterValueHdl[i], &data_p, &data_length);
      if(ble_ret != BLE_STATUS_SUCCESS)
      {
        return -1;
      }
#if OTP_DEBUG
      APP_DBG_MSG("Object List Filter: ");
      for(int i = 0; i < data_length; i++)
      {
        APP_DBG_MSG("%02X ", data_p[i]);
      }
      APP_DBG_MSG("\n");
#endif
    }    
  }
      
  data = OLCP_OPCODE_FIRST;
  
  //APP_DBG_MSG("OTP first\n");
  
  do {
    
    //APP_DBG_MSG("Writing OLCP\n");
    
    ble_ret = GATT_CLIENT_Write_Char(OTPClientContext.OTSHandleContext_p->ObjListCPValueHdl, &data, 1);
    if(ble_ret != BLE_STATUS_SUCCESS)
    {
      return -2;
    }
    
    olcp_ret = waitOLCPResponse();
    if(olcp_ret == 1)
    {
      /* End of list */
      break;      
    }
    else if(olcp_ret < 0) /* Unexpected error */
    {
      return -2;
    }
    
    //APP_DBG_MSG("Reading name\n");
    
    ble_ret = GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjNameValueHdl, &name_p, &name_length);
    if(ble_ret != BLE_STATUS_SUCCESS)
    {
      return -1;
    }
    
    if(object_found_cb((char *)name_p, name_length) == 1)
    {
      return 1;
    }
    
    data = OLCP_OPCODE_NEXT;
    
    //APP_DBG_MSG("OTP next\n");
  
  }while(1);
  
  return 0;  
}

static int objectFoundCB(const char * name, uint16_t name_length)
{
  if(strlen(OTPClientContext.search_name) == name_length &&
     memcmp(OTPClientContext.search_name, name, name_length) == 0)
  {
    /* Object found */
    APP_DBG_MSG("Object name found!\n");
    return 1;
  }
  
  return 0;
}

int OTP_CLIENT_SearchForObject(char *name)
{
  int ret;
  
  OTPClientContext.search_name = name;
  
  ret = OTP_CLIENT_DiscoverAllObjects(objectFoundCB);
  
  if(ret == 1)
  {
    /* Object found */
    return 0;
  }
  else if(ret == 0)
  {
    /* Object not found */
    return 1;
  }
  else
  {
    /* Error while searching */
    return ret;
  }  
}

int OTP_CLIENT_ReadMetadata(OTPObjectMeatadata_t *metadata)
{
  uint8_t *data_p = NULL;
  uint16_t data_length = 0;
  
  memset(metadata, 0, sizeof(OTPObjectMeatadata_t));
  
  if(GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjNameValueHdl, &data_p, &data_length) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }  
  
  memcpy(metadata->name, data_p, MIN(data_length, sizeof(metadata->name) - 1));
  
  if(GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjTypeValueHdl, &data_p, &data_length) != BLE_STATUS_SUCCESS)
  
  if(data_length != 2 && data_length != 16)
  {
    return -1;
  }
  
  metadata->type_length = data_length;
  memcpy(metadata->type, data_p, data_length);
  
  if(GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjSizeValueHdl, &data_p, &data_length) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }  
  
  if(data_length != 8)
  {
    return -1;
  }
  
  metadata->curr_size = LE_TO_HOST_32(data_p);
  metadata->alloc_size = LE_TO_HOST_32(data_p + 4);
  
  if(OTPClientContext.OTSHandleContext_p->ObjIdValueHdl != 0x0000)
  {
    if(GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjIdValueHdl, &data_p, &data_length) != BLE_STATUS_SUCCESS)
    {
      return -2;
    }
    
    if(data_length != OBJECT_ID_SIZE)
    {
      return -1;
    }
    
    memcpy(metadata->id, data_p, OBJECT_ID_SIZE);
  }
  
  if(GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjPropValueHdl, &data_p, &data_length) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }
  
  if(data_length != sizeof(metadata->properties))
  {
    return -1;
  }
  
  metadata->properties = LE_TO_HOST_32(data_p);
  
  return 0;  
}

int OTP_CLIENT_WriteStart(bool truncate)
{
  int ret;
  
  if(OTPClientContext.cid == 0)
  {
    ret = aci_l2cap_cos_connection_req(OTPClientContext.connection_handle, SPSM_OTS, 23, CFG_BLE_COC_MPS_MAX, L2CAP_CHANNEL_TYPE_LE_CFC, 1);
    if(ret != BLE_STATUS_SUCCESS)
    {
      return -4;
    }
    ret = waitL2CAPEvent(OTP_EVENT_L2CAP_OPEN);
    if(ret < 0)
    {
      return -4;
    }
  }
  
  OTPClientContext.truncate = truncate;
  
  return 0;
}

int OTP_CLIENT_WriteObj(const uint8_t *obj_data, uint16_t obj_data_length)
{
  int ret;
  uint8_t *att_data_p = NULL;
  uint16_t att_data_length = 0;
  uint8_t oacp_packet[10];
  uint16_t obj_data_index = 0;
  uint32_t obj_prop;
  uint32_t curr_size;
  uint32_t alloc_size;
  
  if(OTPClientContext.cid == 0)
  {
    return -4;
  }
  
  /* Read and check object properties */  
  if(GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjPropValueHdl, &att_data_p, &att_data_length) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }  
  if(att_data_length != sizeof(obj_prop))
  {
    return -1;
  }
  
  obj_prop = LE_TO_HOST_32(att_data_p);
  
  if((obj_prop & OBJ_PROP_WRITE) == 0)
  {
    /* Object is not writable. */
    return -3;
  }
  
  if(GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjSizeValueHdl, &att_data_p, &att_data_length) != BLE_STATUS_SUCCESS)
  {
    return -2;
  }  
  if(att_data_length != 8)
  {
    return -1;
  }
  
  curr_size = LE_TO_HOST_32(att_data_p);
  alloc_size = LE_TO_HOST_32(att_data_p + 4);
  
  APP_DBG_MSG("Curr size: %d Alloc size: %d\n", curr_size, alloc_size);
  
  if((obj_data_length > alloc_size) &
     ((obj_prop & OBJ_PROP_APPEND) == 0))
  {
    /* Not possible to append data. TODO: need to check also OTS feature? */
    return -3;
  }
  
  if((obj_data_length < curr_size) & !OTPClientContext.truncate &
     ((obj_prop & OBJ_PROP_PATCH) == 0))
  {
    /* Not possible to patch data. TODO: need to check also OTS feature? */
    return -3;
  }
  
  oacp_packet[0] = OACP_OPCODE_WRITE;
  HOST_TO_LE_32(&oacp_packet[1], 0); /* Offset */
  HOST_TO_LE_32(&oacp_packet[5], obj_data_length);  
  if((obj_data_length < curr_size) & OTPClientContext.truncate)
  {
    oacp_packet[9] = OACP_WRITE_MODE_TRUNCATE;
  }
  else
  {
    oacp_packet[9] = 0; /* Mode (truncate off) */
  }
  
  ret = GATT_CLIENT_Write_Char(OTPClientContext.OTSHandleContext_p->ObjActionCPValueHdl, oacp_packet, sizeof(oacp_packet));
  if(ret != BLE_STATUS_SUCCESS)
  {
    return -2;
  }    
  ret = waitOACPResponse();
  if(ret < 0)
  {
    return -5;
  }
  
  /* Everything is ready to send data */
  while(obj_data_index < obj_data_length)
  {
    uint16_t sdu_length;
    uint16_t remaining_data;
    
    remaining_data = obj_data_length - obj_data_index;
      
    sdu_length = MIN(remaining_data, OTPClientContext.peer_mtu);
    
    ret = aci_l2cap_cos_sdu_data_transmit(OTPClientContext.connection_handle, OTPClientContext.cid, sdu_length, (uint8_t *)&obj_data[obj_data_index]);
    
    APP_DBG_MSG("aci_l2cap_cos_sdu_data_transmit 0x%02X\n", ret);
    
    if(ret == BLE_STATUS_SUCCESS)
    {
      obj_data_index += sdu_length;
    }
    else if(ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      //Wait for credits
      ret = waitL2CAPEvent(OTP_EVENT_L2CAP_CREDITS);
      if(ret < 0)
      {
        return -5;
      }
    }
  }
  
  if(OTPClientContext.OTSHandleContext_p->ObjLastModifiedValueHdl != 0x0000 &&
     OTPClientContext.OTSHandleContext_p->ObjLastModifiedProp & BLE_GATT_SRV_CHAR_PROP_WRITE)
  {
    uint8_t *data_p;
    uint16_t data_length;
    /* If the Object Last-Modified characteristic is exposed and supports the Write property, the Object Client shall update the value
       of the Object Last-Modified characteristic when the object is modified. If the Object Client does not have access to a valid
       date and time to set the object last-modified metadata, the fields of the Object Last-Modified characteristic for which the
       Object Client does not have valid data shall be written with a 0 to show that valid data is not available. */
    
    /* Read characteristic value to know the size. */
    if(GATT_CLIENT_Read_Char(OTPClientContext.OTSHandleContext_p->ObjLastModifiedValueHdl, &data_p, &data_length) != BLE_STATUS_SUCCESS)
    {
      return -2;
    }
    
    /* Set value to 0. */    
    memset(data_p, 0, data_length);    
    
    if(GATT_CLIENT_Write_Char(OTPClientContext.OTSHandleContext_p->ObjLastModifiedValueHdl, data_p, data_length) != BLE_STATUS_SUCCESS)
    {
      return -2;
    }
  }
  
  return 0;
}

int OTP_CLIENT_ChannelClose(void)
{
  int ret;
  
  ret = aci_l2cap_cos_disconnect_req(OTPClientContext.connection_handle, OTPClientContext.cid);
  if(ret != BLE_STATUS_SUCCESS)
  {
    return -2;
  }
  
  ret = waitL2CAPEvent(OTP_EVENT_DISCONNECTION);
  if(ret < 0)
  {
    return -1;
  }
  
  return 0;
}

void OTP_CLIENT_OLCPIndication(uint8_t *data_p, uint16_t data_length)
{
  APP_DBG_MSG("OTP_CLIENT_OLCPIndication\n");
  
  if(data_length > sizeof(OTPClientContext.CP_response) ||
     data_p[0] != OLCP_OPCODE_RESPONSE)
  {
    /* Incorrect response */
    return;
  }
  
  memcpy(OTPClientContext.CP_response, data_p, data_length);
  OTPClientContext.CP_response_length = data_length;
  
  OTPClientContext.event = OTP_EVENT_OLCP_RESPONSE;
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_OTP_EVENT);
}

void OTP_CLIENT_OACPIndication(uint8_t *data_p, uint16_t data_length)
{
  APP_DBG_MSG("OTP_CLIENT_OACPIndication\n");
  
  if(data_length > sizeof(OTPClientContext.CP_response) ||
     data_p[0] != OACP_OPCODE_RESPONSE)
  {
    /* Incorrect response */
    return;
  }
  
  memcpy(OTPClientContext.CP_response, data_p, data_length);
  OTPClientContext.CP_response_length = data_length;
  
  OTPClientContext.event = OTP_EVENT_OACP_RESPONSE;
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_OTP_EVENT);
}

//TODO: pass single parameters instead of event for better portability?
void OTP_CLIENT_L2CAPConnectionResp(aci_l2cap_cos_connection_resp_event_rp0 *event)
{
  if(event->Connection_Handle != OTPClientContext.connection_handle)
    return;
  
  if(event->Result != L2CAP_CONN_SUCCESSFUL)
  {
    OTPClientContext.event = OTP_EVENT_L2CAP_ERROR;
    UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_OTP_EVENT);
    return;
  }
  
  OTPClientContext.cid = event->CID[0];
  OTPClientContext.peer_mtu = event->Peer_MTU;
  
  OTPClientContext.event = OTP_EVENT_L2CAP_OPEN;
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_OTP_EVENT);
}

void OTP_CLIENT_L2CAPCreditsReceived(void)
{
  OTPClientContext.event = OTP_EVENT_L2CAP_CREDITS;
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_OTP_EVENT);
}

static void OTP_CLIENT_Process(void)
{
  switch(OTPClientContext.state)
  {    
  default:
    break;
  }
  
}


