/**
  ******************************************************************************
  * @file    esl_profile.c
  * @author  GPM WBL Application Team
  * @brief   Implementation of ESL packet management.
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

#include <stdint.h>
#include <string.h>
#include "stm32wb0x.h"
#include "ble.h"
#include "esl_profile.h"
#include "stm32wb0x_nucleo.h"
#include "app_common.h"

#define MAX_ESL_PAYLOAD_SIZE                    (48U)

#define AD_TYPE_ELECTRONIC_SHELF_LABEL          (0x34)

/* Codes for Commands */
#define ESL_CMD_PING                            (0x00)
#define ESL_CMD_READ_SENSOR_DATA                (0x10)
#define ESL_CMD_DISPLAY_IMG                     (0x20)
#define ESL_CMD_LED_CONTROL                     (0xB0)
/* Code for vendor-specific commands */
#define ESL_CMD_VS_PRICE                        (0x3F)
#define ESL_CMD_VS_TXT                          (0xBF)

#define ESL_RESP_ERROR                          (0x00)
#define ESL_RESP_LED_STATE                      (0x01)
#define ESL_RESP_BASIC_STATE                    (0x10)
#define ESL_RESP_DISPLAY_STATE                  (0x11)
#define ESL_RESP_SENSOR_VALUE_TAG_NIBBLE        (0x0E) // This is only the value of the Tag nibble. The Length is variable
#define ESL_RESP_VS_OK                          (0x0F)

/* Basic state response flags */
#define BASIC_STATE_SERVICE_NEEDED_BIT          (0x01)
#define BASIC_STATE_SYNCHRONIZED_BIT            (0x02)
#define BASIC_STATE_ACTIVE_LED_BIT              (0x04)
#define BASIC_STATE_PENDING_LED_UPDATE_BIT      (0x08)
#define BASIC_STATE_PENDING_DISPLAY_UPDATE_BIT  (0x10)

#define BRC_ESL_ID                              (0xFF)

#define MAX_TXT_LENGTH                           (11U)

#define GET_PARAM_LENGTH_FROM_OPCODE(opcode)          ((((opcode) & 0xF0) >> 4) + 1)

#define GET_LENGTH_FROM_OPCODE(opcode)                (GET_PARAM_LENGTH_FROM_OPCODE(opcode) + 1)

#define SET_LENGTH_TO_OPCODE(tag, param_length)       (((tag) & 0x0F) | ((((param_length) - 1) & 0x0F) << 4))


typedef struct
{
  uint16_t conn_handle;
  uint16_t sync_handle;
  uint8_t group_id;
  uint8_t esl_id;
  uint16_t esl_state;
  uint8_t a_resp[MAX_ESL_PAYLOAD_SIZE+2];
} ESL_PROFILE_Context_t;

ESL_PROFILE_Context_t ESL_PROFILE_Context = 
{
  .group_id = GROUP_ID,
  .esl_id = ESL_ID,
  .esl_state = 0,
};

static void synch_packet_received(uint16_t pa_event, uint8_t *p_esl_data, uint8_t size);
static void send_resp(uint16_t pa_event, uint8_t resp_slot, uint8_t *p_esl_resp, uint8_t resp_size);

void ESL_PROFILE_Init(void)
{
  tBleStatus ret;
  ret = hci_le_set_default_periodic_advertising_sync_transfer_parameters(0x01, /* Mode: reports disabled */
                                                                         0x0000, /* Skip */
                                                                         1000, /* Sync_Timeout */
                                                                         0); /* CTE_Type*/
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("hci_le_set_default_periodic_advertising_sync_transfer_parameters failure: reason=0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("==>> hci_le_set_default_periodic_advertising_sync_transfer_parameters : Success\n");
  }  
}

void ESL_PROFILE_ConnectionComplete(uint16_t connection_handle)
{
  ESL_PROFILE_Context.conn_handle = connection_handle;
}

void ESL_PROFILE_SyncInfoReceived(uint16_t sync_handle)
{
  tBleStatus ret;
    
  ESL_PROFILE_Context.sync_handle = sync_handle;
  
  ret = hci_le_set_periodic_sync_subevent(sync_handle,
                                          0, /* Periodic_Advertising_Properties */
                                          1, /* Num_Subevents */
                                          &ESL_PROFILE_Context.group_id);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("hci_le_set_periodic_sync_subevent failure: reason=0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("==>> hci_le_set_periodic_sync_subevent : Success\n");
  }
  
  ret = hci_le_set_periodic_advertising_receive_enable(sync_handle,
                                                       1 /* Enable*/);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("hci_le_set_periodic_advertising_receive_enable failure: reason=0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("==>> hci_le_set_periodic_advertising_receive_enable : Success\n");
  }
  
  ret = aci_gap_terminate(ESL_PROFILE_Context.conn_handle, BLE_ERROR_TERMINATED_REMOTE_USER);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gap_terminate failure: reason=0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("==>> aci_gap_terminate : Success\n");
  }
    
}

void ESL_PROFILE_AdvPayloadReceived(uint16_t pa_event, uint8_t *p_adv_data, uint8_t size)
{
  uint16_t i = 0;
  uint8_t ad_length, ad_type;
  
  while(i < size)
  {
    ad_length = p_adv_data[i];
    ad_type = p_adv_data[i + 1];
      
    switch (ad_type)
    {
    case AD_TYPE_ELECTRONIC_SHELF_LABEL:
      {        
        synch_packet_received(pa_event, &p_adv_data[i + 2], ad_length - 1);
      }
      break;
    default:
      break;
    }
    i += ad_length + 1; /* increment the iterator to go on next element */
  }
  
  APP_DBG_MSG("End of ESL_PROFILE_AdvPayloadReceived\n");
}

static void synch_packet_received(uint16_t pa_event, uint8_t *p_esl_data, uint8_t size)
{
  uint8_t opcode;
  uint8_t group_id;
  uint8_t param_length;
  uint8_t esl_cmd_id;
  int8_t tlv_num = -1, relevant_cmd_tlv_num = -1;
  uint8_t *p_cmd;
  uint8_t resp_idx = 0;
  uint8_t esl_payload_resp[MAX_ESL_PAYLOAD_SIZE];
  
  group_id = p_esl_data[0] & 0x7F;
  
  if(group_id != ESL_PROFILE_Context.group_id)
    return;
  
  p_cmd = &p_esl_data[1];
  
  while(p_cmd < p_esl_data + size - 1) /* Shortest command is 2 bytes. */
  {
    opcode = p_cmd[0];
    param_length = ((p_cmd[0] & 0xF0) >> 4) + 1;
    esl_cmd_id = p_cmd[1]; /* First cmd parameter is always the ESL_ID */
    tlv_num += 1;
    
    if(esl_cmd_id == ESL_PROFILE_Context.esl_id || esl_cmd_id == BRC_ESL_ID)
    {
      /* Identify the relevant command to choose response slot. */
      if( esl_cmd_id != BRC_ESL_ID)
      {
        relevant_cmd_tlv_num = tlv_num;
      }
      
      switch(opcode)
      {
      case ESL_CMD_PING:
        {          
          if(esl_cmd_id != BRC_ESL_ID)
          {
            //TBR: to check if response exceeds ESL payload size.
            esl_payload_resp[resp_idx] = ESL_RESP_BASIC_STATE;
            HOST_TO_LE_16(&esl_payload_resp[resp_idx + 1], ESL_PROFILE_Context.esl_state);
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
        }
        break;
      case ESL_CMD_LED_CONTROL:
        {
          uint8_t led_index;
          uint16_t led_repeat; /* Repeat type and duration. */
          uint8_t ret;
          
          led_index = p_cmd[2];
          led_repeat = LE_TO_HOST_16(&p_cmd[11]);
          
          ret = ESL_APP_LEDControlCmdCB(led_repeat);
          
          if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
          {
            /* Error */
            esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
            esl_payload_resp[resp_idx + 1] = ret;
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
          
          if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
          {            
            //TBR: to check if response exceeds ESL payload size.
            esl_payload_resp[resp_idx] = ESL_RESP_LED_STATE;
            esl_payload_resp[resp_idx + 1] = led_index;
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
        }
        break;
      case ESL_CMD_READ_SENSOR_DATA:
        {
          uint8_t sensor_index;
          uint8_t sensor_data_length;
          uint8_t ret;
          
          if(esl_cmd_id == BRC_ESL_ID)
          {
            /* This command cannot have a broadcast address. Do not invoke callback. */
            break;
          }
          
          sensor_index = p_cmd[2];
          
          /* TBR: Need to use asynchronous response, since data from sensor may not be available immediately.
                   Check also for available space in response.  */
          ret = ESL_APP_SensorDataCmdCB(sensor_index, &esl_payload_resp[resp_idx + 2], &sensor_data_length);
          
          if(ret != 0)
          {
            esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
            esl_payload_resp[resp_idx + 1] = ret;
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
          
          if(ret == 0)
          {
            esl_payload_resp[resp_idx] = SET_LENGTH_TO_OPCODE(ESL_RESP_SENSOR_VALUE_TAG_NIBBLE, sensor_data_length + 1);            
            esl_payload_resp[resp_idx + 1] = sensor_index;            
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
        }
        break;
      case ESL_CMD_VS_TXT:
        {
          uint8_t ret;
          
          ret = ESL_APP_TxtVsCmdCB(param_length - 1, (char *)&p_cmd[2]);
          
          if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
          {
            esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
            esl_payload_resp[resp_idx + 1] = ret;
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
          
          if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
          {
            esl_payload_resp[resp_idx] = ESL_RESP_VS_OK;
            esl_payload_resp[resp_idx + 1] = 0; /* Not used. */
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);            
          }
          
        }
        break;
      case ESL_CMD_VS_PRICE:
        {
          uint8_t ret;
          
          ret = ESL_APP_PriceVsCmdCB(LE_TO_HOST_16(&p_cmd[2]), p_cmd[4]);
          
          if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
          {
            esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
            esl_payload_resp[resp_idx + 1] = ret;
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
          
          if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
          {
            esl_payload_resp[resp_idx] = ESL_RESP_VS_OK;
            esl_payload_resp[resp_idx + 1] = 0; /* Not used. */
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
          
        }
        break;
      case ESL_CMD_DISPLAY_IMG:
        {
          uint8_t ret;
          
          ret = ESL_APP_DisplayImageCmdCB(p_cmd[3]);
          
          if(ret != 0 && esl_cmd_id != BRC_ESL_ID)
          {
            esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
            esl_payload_resp[resp_idx + 1] = ret;
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
          
          if(ret == 0 && esl_cmd_id != BRC_ESL_ID)
          {
            esl_payload_resp[resp_idx] = ESL_RESP_DISPLAY_STATE;
            esl_payload_resp[resp_idx + 1] = p_cmd[2]; /* Display index */
            esl_payload_resp[resp_idx + 2] = p_cmd[3]; /* Image index */            
            resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
          }
          
        }
        break;
      default:
        esl_payload_resp[resp_idx] = ESL_RESP_ERROR;
        esl_payload_resp[resp_idx + 1] = ERROR_INVALID_OPCODE;
        resp_idx += GET_LENGTH_FROM_OPCODE(esl_payload_resp[resp_idx]);
        break;
      }
    }
    p_cmd += (param_length + 1); /* increment the iterator to go on next element*/
  }
  
  if(relevant_cmd_tlv_num >= 0)
  {
    /* Send the response */
    send_resp(pa_event, relevant_cmd_tlv_num, esl_payload_resp, resp_idx);
  }
  
}

static void send_resp(uint16_t pa_event, uint8_t resp_slot, uint8_t *p_esl_resp, uint8_t resp_size)
{
  tBleStatus status;
  
  ESL_PROFILE_Context.a_resp[0] = resp_size + 1;
  ESL_PROFILE_Context.a_resp[1] = AD_TYPE_ELECTRONIC_SHELF_LABEL;
  memcpy(ESL_PROFILE_Context.a_resp + 2, p_esl_resp, resp_size);
  
  status = ll_set_periodic_advertising_response_data_ptr(ESL_PROFILE_Context.sync_handle,
                                                         pa_event,
                                                         ESL_PROFILE_Context.group_id,
                                                         ESL_PROFILE_Context.group_id,
                                                         resp_slot,
                                                         resp_size + 2,
                                                         ESL_PROFILE_Context.a_resp);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("ll_set_periodic_advertising_response_data_ptr failure: reason=0x%02X, Sync_Handle 0x%04X, Request_Event %d, Subevent %d, Response_Slot %d\n", status,
                ESL_PROFILE_Context.sync_handle,
                pa_event,
                ESL_PROFILE_Context.group_id,
                resp_slot);
  }
  else
  {
    APP_DBG_MSG("==>> ll_set_periodic_advertising_response_data_ptr : Success\n");
  }
}
