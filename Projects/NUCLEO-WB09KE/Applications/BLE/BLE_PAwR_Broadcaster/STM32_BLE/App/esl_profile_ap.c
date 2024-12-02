/**
  ******************************************************************************
  * @file    esl_profile_ap.c
  * @author  GPM WBL Application Team
  * @brief   Implementation of ESL commands management.
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
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "ble.h"
#include "esl_profile_ap.h"
#include "app_common.h"
#include "stm_list.h"


typedef struct {
  tListNode node;
  //bool sending;
  uint8_t retransmissions;
  resp_cb_t resp_cb;
  uint8_t cmd[0];  
} cmd_buff_t;

typedef struct {
  tListNode cmd_queue;
  uint8_t adv_packet_len;
  uint8_t adv_packet[MAX_ESL_PAYLOAD_SIZE + 2]; /* 2 bytes needed for AD length and type. */
} esl_group_info_t;

esl_group_info_t esl_group_info[MAX_GROUPS];


uint8_t get_esl_id_from_response(uint8_t subevent, uint8_t response_slot, uint8_t *esl_id_p);

void ESL_AP_Init(void)
{  
  for(int i = 0; i < MAX_GROUPS; i++)
  {
    LST_init_head(&esl_group_info[i].cmd_queue);
  }
}

uint8_t ESL_AP_cmd_ping(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_PING;
  uint8_t cmd_length;
  
  if(esl_id == BRC_ESL_ID)
    return 1;
  
  if(group_id >= MAX_GROUPS)
    return 2;
  
  cmd_length = GET_LENGTH_FROM_OPCODE(cmd_opcode);
  
  cmd_buff = malloc(sizeof(cmd_buff_t)+cmd_length);
  
  if(cmd_buff == NULL)
    return 1;
  
  LST_insert_tail(&esl_group_info[group_id].cmd_queue, &cmd_buff->node);
  
  //cmd_buff->sending = false;
  cmd_buff->resp_cb = resp_cb;
  
  if(esl_id == BRC_ESL_ID)
  {
    cmd_buff->retransmissions = BRC_RETRANSMISSIONS;
  }
  else
  {
    cmd_buff->retransmissions = UNC_RETRANSMISSIONS;
  }
  
  cmd_buff->cmd[0] = cmd_opcode;
  cmd_buff->cmd[1] = esl_id;
  
  return 0;  
}

static void * prepare_cmd_buff(uint8_t cmd_opcode, uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb)
{
  uint8_t cmd_length;
  cmd_buff_t * cmd_buff;
  
  if(group_id >= MAX_GROUPS)
    return NULL;
  
  cmd_length = GET_LENGTH_FROM_OPCODE(cmd_opcode);
  
  cmd_buff = malloc(sizeof(cmd_buff_t)+cmd_length);
  
  if(cmd_buff == NULL)
    return NULL;
  
  LST_insert_tail(&esl_group_info[group_id].cmd_queue, &cmd_buff->node);
  
  cmd_buff->resp_cb = resp_cb;
  
  if(esl_id == BRC_ESL_ID)
  {
    cmd_buff->retransmissions = BRC_RETRANSMISSIONS;
  }
  else
  {
    cmd_buff->retransmissions = UNC_RETRANSMISSIONS;
  }
  
  cmd_buff->cmd[0] = cmd_opcode;
  cmd_buff->cmd[1] = esl_id;
  
  return cmd_buff;
}

uint8_t ESL_AP_cmd_led_control(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t led_index, uint8_t on_off)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_LED_CONTROL;
  
  cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
  
  if(cmd_buff == NULL)
    return 1;
  
  cmd_buff->cmd[2] = led_index;
  cmd_buff->cmd[3] = 0; /* rgb component. Not used.  */  
  memset(cmd_buff->cmd + 4, 0, 7); /* Flashing pattern. Ignored for the moment. */
  if(on_off == 0)
  {
    /* LED off */
    cmd_buff->cmd[11] = 0;
    cmd_buff->cmd[12] = 0;
  }
  else if(on_off == 1)
  {
    /* LED on */
    HOST_TO_LE_16(&cmd_buff->cmd[11], 0x0001);
  }
  else
  {
    /* TBR: standard values needs to be used. Now value 2 is used for the sake of simplicity. */
    HOST_TO_LE_16(&cmd_buff->cmd[11], 0x0002);
  }
  
  return 0;  
}

/* Can send a maximum of 11 characters. */
uint8_t ESL_AP_cmd_txt(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, const char *txt)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_VS_TXT;
  uint8_t cmd_length;
  
  cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
  
  if(cmd_buff == NULL)
    return 1;
  
  cmd_length = GET_LENGTH_FROM_OPCODE(cmd_opcode);
  
  memset(&cmd_buff->cmd[2], 0, cmd_length-2);
  memcpy(&cmd_buff->cmd[2], txt, MIN(strlen(txt), cmd_length-2));
  
  return 0;  
}

uint8_t ESL_AP_cmd_price(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint16_t val_int, uint8_t val_fract)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_VS_PRICE;
  
  cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
  
  if(cmd_buff == NULL)
    return 1;
  
  HOST_TO_LE_16(&cmd_buff->cmd[2],val_int);
  cmd_buff->cmd[4] = val_fract;
  
  return 0;  
}

uint8_t ESL_AP_cmd_read_sensor_data(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t sensor_index)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_READ_SENSOR_DATA;
  
  cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
  
  if(cmd_buff == NULL)
    return 1;
  
  cmd_buff->cmd[2] = sensor_index;
  
  return 0;  
}

uint8_t ESL_AP_cmd_display_image(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_DISPLAY_IMG;
  
  cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
  
  if(cmd_buff == NULL)
    return 1;
  
  cmd_buff->cmd[2] = display_index;
  cmd_buff->cmd[3] = image_index;
  
  return 0;  
}

void ESL_AP_SubeventDataRequest(uint8_t subevent)
{
  tBleStatus ble_status;
  tListNode *list_head_p;
  cmd_buff_t* current_node_p;
  cmd_buff_t* node_to_remove_p = NULL;
  esl_group_info_t *esl_group_info_p;
  uint8_t curr_payload_len;
  Subevent_Data_Ptr_Parameters_t Subevent_Data_Ptr_Parameters;
  uint8_t cmd_length;
  uint8_t num_cmd = 0;
  uint8_t esl_payload[MAX_ESL_PAYLOAD_SIZE];
  
  /* subevent is the Group ID */
  
  if(subevent >= MAX_GROUPS)
    return;
  
  esl_group_info_p = &esl_group_info[subevent];  
  
  /* First byte of synchronization packet for commands is the Group ID. */
  esl_payload[0] = subevent;
  curr_payload_len = 1;
  
  list_head_p = &esl_group_info_p->cmd_queue;
    
  current_node_p = (cmd_buff_t*)list_head_p->next;
  
  while(&current_node_p->node != list_head_p)
  {
    num_cmd++;
    if(num_cmd > PAWR_NUM_RESPONSE_SLOTS && current_node_p->cmd[ESL_ID_CMD_OFFSET] != BRC_ESL_ID)
    {
      /* We do not send a unicast command if we cannot receive a response for it.
         However we can send a broadcast command regardless of the number of
         response slots, because they have no response. */
      break;
    }
    
    cmd_length = GET_LENGTH_FROM_OPCODE(current_node_p->cmd[0]);
    
    if(curr_payload_len + cmd_length > MAX_ESL_PAYLOAD_SIZE)
      break;
    
    memcpy(&esl_payload[curr_payload_len], current_node_p->cmd, cmd_length);
    curr_payload_len += cmd_length;
    
    //current_node_p->sending = true;
    
    /* Update retransmission count and remove them from
      queue if count has reached 0. */
    if(current_node_p->retransmissions == 0)
    {
      LST_remove_node(&current_node_p->node);
      node_to_remove_p = current_node_p;
    }
    else 
    {
      current_node_p->retransmissions--;
    }
    
    LST_get_next_node(&current_node_p->node, (tListNode **)&current_node_p);
    
    if(node_to_remove_p != NULL)
    {
      free(node_to_remove_p);
      node_to_remove_p = NULL;
    } 
  }
  
  if(curr_payload_len == 1)
  {
    /* No commands to be sent. */
    esl_group_info_p->adv_packet_len = 0;
    
    return;
  }
  
  esl_group_info_p->adv_packet[0] = curr_payload_len + 1; /* One byte for AD type. */
  esl_group_info_p->adv_packet[1] = AD_TYPE_ELECTRONIC_SHELF_LABEL;
  memcpy(&esl_group_info_p->adv_packet[2], esl_payload, curr_payload_len);
  
  esl_group_info_p->adv_packet_len = curr_payload_len + 2;
  
  Subevent_Data_Ptr_Parameters.Subevent = subevent;
  //TBR: calculate the used response slots depending on the current commands.
  Subevent_Data_Ptr_Parameters.Response_Slot_Start = 0;
  Subevent_Data_Ptr_Parameters.Response_Slot_Count = PAWR_NUM_RESPONSE_SLOTS;
  Subevent_Data_Ptr_Parameters.Subevent_Data_Length = esl_group_info_p->adv_packet_len;
  Subevent_Data_Ptr_Parameters.Subevent_Data = esl_group_info_p->adv_packet;
  
  ble_status = ll_set_periodic_advertising_subevent_data_ptr(0, 1, &Subevent_Data_Ptr_Parameters);  
  if (ble_status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> ll_set_periodic_advertising_subevent_data_ptr - fail, result: 0x%02X\n", ble_status);
  }
  else
  {
    APP_DBG_MSG("==>> Success: ll_set_periodic_advertising_subevent_data_ptr\n");
  }        
}

void ESL_AP_ResponseReport(uint8_t subevent, uint8_t response_slot, uint8_t data_length, uint8_t *data)
{
  esl_group_info_t *esl_group_info_p;
  tListNode *list_head_p;
  cmd_buff_t* current_node_p;
  cmd_buff_t* node_to_remove_p = NULL;
  uint8_t esl_id;
  uint8_t *current_data_resp_p;
  uint8_t resp_length;
  
  if(subevent >= MAX_GROUPS)
    return;
  
  /* We assume only one AD type is present */
  if(data[1] != AD_TYPE_ELECTRONIC_SHELF_LABEL)
    return;
  
  if(get_esl_id_from_response(subevent, response_slot, &esl_id) != 0)
  {
    /* No corresponding ESL ID found for that response. This should not happen. */
    return;
  }  
  
  /* Inspect sent commands and link them to the responses. */
  esl_group_info_p = &esl_group_info[subevent];
  
  list_head_p = &esl_group_info_p->cmd_queue;
    
  current_node_p = (cmd_buff_t*)list_head_p->next;
  
  current_data_resp_p = data + 2; /* Skip first 2 bytes for AD length and type. */
  
  while(&current_node_p->node != list_head_p && current_data_resp_p < data + data_length)
  {
    if(current_node_p->cmd[ESL_ID_CMD_OFFSET] == esl_id)
    {
      if(current_node_p->resp_cb != NULL)
      {
        current_node_p->resp_cb(subevent, esl_id, current_data_resp_p);
      }
      
      /* Command has received a response. Remove from queue. */      
      LST_remove_node(&current_node_p->node);      
      node_to_remove_p = current_node_p;
      
      /* Pass to following response. */
      resp_length = GET_LENGTH_FROM_OPCODE(current_data_resp_p[0]);
      current_data_resp_p += resp_length;
    }
    
    LST_get_next_node(&current_node_p->node, (tListNode **)&current_node_p);
    
    if(node_to_remove_p != NULL)
    {
      free(node_to_remove_p);
      node_to_remove_p = NULL;
    }    
  }  
}

uint8_t get_esl_id_from_response(uint8_t subevent, uint8_t response_slot, uint8_t *esl_id_p)
{
  esl_group_info_t *esl_group_info_p;
  uint8_t *esl_payload_p;
  uint8_t esl_payload_len;
  uint8_t *cmd_p;
  int8_t cmd_idx = -1;
  uint8_t cmd_length;
  
  if(subevent >= MAX_GROUPS)
    return 1;
  
  /* Inspect last sent ESL payload. */
  esl_group_info_p = &esl_group_info[subevent];
  esl_payload_p = &esl_group_info_p->adv_packet[2]; /* Two bytes for AD length and type */
  esl_payload_len = esl_group_info_p->adv_packet_len - 2;
  
  cmd_p = &esl_payload_p[1];
  
  while(cmd_p < esl_payload_p + esl_payload_len)
  {
    cmd_idx++;
    
    if(cmd_idx == response_slot)
    {
      if(cmd_p[ESL_ID_CMD_OFFSET] == BRC_ESL_ID)
      {
        return 1;
      }
      
      *esl_id_p = cmd_p[ESL_ID_CMD_OFFSET];
      
      return 0;
    }    
    
    cmd_length = GET_LENGTH_FROM_OPCODE(cmd_p[0]);    
    cmd_p += cmd_length;
  }
  
  return 1;
}

// Not working because cmds are freed when response are received.
//uint8_t get_esl_id_from_response(uint8_t subevent, uint8_t response_slot, uint8_t *esl_id_p)
//{
//  
//  esl_group_info_t *esl_group_info_p;
//  tListNode *list_head_p;
//  cmd_buff_t* current_node_p;
//  int8_t cmd_idx = -1;
//  
//  /* Inspect last sent ESL payload. */
//  esl_group_info_p = &esl_group_info[subevent];
//  
//  list_head_p = &esl_group_info_p->cmd_queue;
//    
//  current_node_p = (cmd_buff_t*)list_head_p->next;
//  
//  /* Go to the cmd  */
//  
//  while(&current_node_p->node != list_head_p)
//  {
//    cmd_idx++;
//    
//    if(!current_node_p->sending)
//    {
//      /* This can happen if response slot is not as expected. */
//      return 1;
//    }
//    
//    if(cmd_idx == response_slot)
//    {
//      /* This is the command addressed to the ESL that sent the response. */
//      *esl_id_p = current_node_p->cmd[ESL_ID_CMD_OFFSET];
//      return 0;
//    }
//    
//    LST_get_next_node(&current_node_p->node, (tListNode **)&current_node_p);
//  }
//  
//  return 1;  
//}
