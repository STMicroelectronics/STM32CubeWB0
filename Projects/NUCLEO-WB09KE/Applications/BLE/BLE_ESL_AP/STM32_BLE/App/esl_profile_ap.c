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
#include "gatt_client_app.h"    
#include "stm32_seq.h"
#include "app_ble.h"    

#define MAX_ESL_PAYLOAD_SIZE                    (48U)
#define EAD_MIC_SIZE                            (4U)
#define EAD_RANDOMZER_SIZE                      (5U)
#define MAX_ADV_PAYLOAD                         (MAX_ESL_PAYLOAD_SIZE + 2 + EAD_RANDOMZER_SIZE + EAD_MIC_SIZE + 2)

typedef struct {
  uint16_t conn_handle;
  uint8_t group_id;
  uint8_t esl_id;
  resp_cb_t resp_cb;
  uint8_t cmd[0];  
} cmd_ECP_buff_t;
    
typedef struct {
  tListNode node;
  uint8_t retransmissions;
  resp_cb_t resp_cb;
  uint8_t cmd[0];  
} cmd_buff_t;

/* List of ESL bonded to AP */
typedef struct {
  tListNode cmd_queue;
  uint8_t adv_packet_len;
  uint8_t adv_packet[MAX_ADV_PAYLOAD];
} esl_group_info_t;

esl_group_info_t esl_group_info[MAX_GROUPS];

cmd_ECP_buff_t* cmd_ECP_buff;

uint8_t esl_id_sync_pck; 

uint8_t old_group_id, old_esl_id, new_group_id, new_esl_id = 0x00;

bool bUpdatingTransition = false;

extern uint16_t esl_address;

void ESL_AP_Init(void)
{  
  for(int i = 0; i < MAX_GROUPS; i++)
  {
    LST_init_head(&esl_group_info[i].cmd_queue);
  }
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

// to prepare cmd packet on Updating state and send command by ECP char
static uint8_t prepare_cmd_ECP_buff(uint8_t cmd_opcode, uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb)
{
  uint8_t cmd_length;
  
  if(group_id >= MAX_GROUPS)
    return 0;
  
  cmd_length = GET_LENGTH_FROM_OPCODE(cmd_opcode);
  
  if (cmd_ECP_buff != NULL)
    free(cmd_ECP_buff);
  
  cmd_ECP_buff = malloc(sizeof(cmd_ECP_buff_t)+cmd_length);
  
  if(cmd_ECP_buff == NULL)
    return 0;
  
  cmd_ECP_buff->group_id = group_id;
  cmd_ECP_buff->esl_id = esl_id;
  
  cmd_ECP_buff->resp_cb = resp_cb;
  
  cmd_ECP_buff->cmd[0] = cmd_opcode;
  cmd_ECP_buff->cmd[1] = esl_id;
  
  return cmd_length;
}

/* If bResponse is true the command wait for an ESL response
   else the command has no response */
static uint8_t ESL_AP_SendCmdByECP(uint8_t cmd_length, bool bResponse)
{
  tBleStatus ble_status;
  
  ble_status = ESL_AP_write_ECP(cmd_ECP_buff->cmd, cmd_length, &cmd_ECP_buff->conn_handle, bResponse);
  if (ble_status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> ESL_AP_write_ECP - fail, result: 0x%02X\n", ble_status);
  }
  else
  {
    APP_DBG_MSG("==>> Success: ESL_AP_write_ECP\n");
  }     
  
  return ble_status;
}

void ECP_respCB(uint16_t connHandle, uint8_t *current_data_resp_p)
{
  if (connHandle == cmd_ECP_buff->conn_handle)
  {
    cmd_ECP_buff->resp_cb(cmd_ECP_buff->group_id, cmd_ECP_buff->esl_id, current_data_resp_p);
  }
}

uint8_t ESL_AP_command(uint8_t cmd_opcode, uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_length;
  esl_bonded_t* esl_node;

  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node == NULL)  
    return 2;
  
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state
    cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
    if(cmd_buff == NULL)
      return 2;
  } 
  else
  {   
    // to prepare cmd packet on Updating state, because ESL is connected to AP
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
       
    if(cmd_ECP_buff == NULL)
      return 1;
    
    // to send command by ECP char
    if (esl_id == BRC_ESL_ID)
      return 1;
    else
      ESL_AP_SendCmdByECP(cmd_length, true);
  }  
  return 0;  
}

static void Led_cmd_buff(uint8_t *cmd, uint8_t led_index, uint8_t led_component, uint64_t flash_pattern, uint8_t off_period, uint8_t on_period, uint16_t repeat) 
{
  cmd[2] = led_index;
  /* if the LED is a monochrome LED, the value of the Color fields shall be ignored.*/
  cmd[3] = led_component; /* RGB and Brightness component. Not used.  */                          
  
  //Flashing pattern
  memcpy(&cmd[4], &flash_pattern, 5);
  //Flashing off period
  cmd[9] = off_period;
  //Flashing on period
  cmd[10] = on_period;
  
  //Repeat type and duration
  memcpy(&cmd[11], &repeat, 2);
}

uint8_t ESL_AP_cmd_led_control(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t led_index, uint8_t led_component, uint64_t flash_pattern, uint8_t off_period, uint8_t on_period, uint16_t repeat)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_LED_CONTROL;
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node == NULL)  
    return 2;
  
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)    
  {   
    // to prepare cmd packet on Synchronized state
    cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    Led_cmd_buff(cmd_buff->cmd, led_index, led_component, flash_pattern, off_period, on_period, repeat);
  }
  else
  {  
    // to prepare cmd packet on Updating state
    uint8_t cmd_length;
    
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_ECP_buff == NULL)
      return 1;
    
    Led_cmd_buff(cmd_ECP_buff->cmd, led_index, led_component, flash_pattern, off_period, on_period, repeat);
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  
  
  return 0;  
}

/* Can send a maximum of 11 characters. */
uint8_t ESL_AP_cmd_txt(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, const char *txt)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_VS_TXT;
  uint8_t cmd_length;
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node == NULL)  
    return 2;
  
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state  
    cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_length = GET_LENGTH_FROM_OPCODE(cmd_opcode);
    
    memset(&cmd_buff->cmd[2], 0, cmd_length-2);
    memcpy(&cmd_buff->cmd[2], txt, MIN(strlen(txt), cmd_length-2));
  }
  else
  {
    // to prepare cmd packet on Updating state
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
     
    if(cmd_ECP_buff == NULL)
      return 1;  
    
    memset(&cmd_ECP_buff->cmd[2], 0, cmd_length-2);
    memcpy(&cmd_ECP_buff->cmd[2], txt, MIN(strlen(txt), cmd_length-2));
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  
  
  return 0;  
}

uint8_t ESL_AP_cmd_price(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint16_t val_int, uint8_t val_fract)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_VS_PRICE;
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node == NULL)  
    return 2;
  
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state  
    cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    HOST_TO_LE_16(&cmd_buff->cmd[2],val_int);
    cmd_buff->cmd[4] = val_fract;
  }
  else
  { 
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
     
    if(cmd_ECP_buff == NULL)
      return 1;
    
    HOST_TO_LE_16(&cmd_ECP_buff->cmd[2],val_int);
    cmd_ECP_buff->cmd[4] = val_fract;
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }
  
  return 0;  
}

uint8_t ESL_AP_cmd_read_sensor_data(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t sensor_index)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_READ_SENSOR_DATA;
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node == NULL)  
    return 2;
  
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)  
  {   
    // to prepare cmd packet on Synchronized state  
    cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_buff->cmd[2] = sensor_index;
  }
  else
  {
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
      
    if(cmd_ECP_buff == NULL)
      return 1;
    
    cmd_ECP_buff->cmd[2] = sensor_index;
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  
  
  return 0;  
}

uint8_t ESL_AP_cmd_display_image(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_DISPLAY_IMG;
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node == NULL)  
    return 2;
  
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state  
    cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_buff->cmd[2] = display_index;
    cmd_buff->cmd[3] = image_index;
  }
  else
  {
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
  
    if(cmd_ECP_buff == NULL)
      return 1;
    
    cmd_ECP_buff->cmd[2] = display_index;
    cmd_ECP_buff->cmd[3] = image_index;  
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  

  return 0;  
}

uint8_t ESL_AP_cmd_led_timed_control(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, 
                                     uint8_t led_index, uint8_t led_component, 
                                     uint64_t flash_pattern, uint8_t off_period, 
                                     uint8_t on_period, uint16_t repeat, 
                                     uint32_t absolute_time)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_LED_TIMED_CONTROL;
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node == NULL)  
    return 2;
  
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)    
  {   
    // to prepare cmd packet on Synchronized state
    cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    Led_cmd_buff(cmd_buff->cmd, led_index, led_component, flash_pattern, off_period, on_period, repeat);
    HOST_TO_LE_32(&cmd_buff->cmd[13], absolute_time);
  }
  else
  {  
    // to prepare cmd packet on Updating state
    uint8_t cmd_length;
    
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
   
    if(cmd_ECP_buff == NULL)
      return 1;
    
    Led_cmd_buff(cmd_ECP_buff->cmd, led_index, led_component, flash_pattern, off_period, on_period, repeat);
    HOST_TO_LE_32(&cmd_ECP_buff->cmd[13], absolute_time);
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);
  }  

  return 0;  
}

uint8_t ESL_AP_cmd_display_timed_image(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index, uint32_t absolute_time)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_DISPLAY_TIMED_IMG;
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node == NULL)  
    return 2;
  
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)
  { 
    // to prepare cmd packet on Synchronized state  
    cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
    
    if(cmd_buff == NULL)
      return 1;
    
    cmd_buff->cmd[2] = display_index;
    cmd_buff->cmd[3] = image_index;
    HOST_TO_LE_32(&cmd_buff->cmd[4], absolute_time);
  }
  else
  {
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
  
    if(cmd_ECP_buff == NULL)
      return 1;
    
    cmd_ECP_buff->cmd[2] = display_index;
    cmd_ECP_buff->cmd[3] = image_index;
    HOST_TO_LE_32(&cmd_ECP_buff->cmd[4], absolute_time);
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, true);  
  }
 
  return 0;  
}

uint8_t ESL_AP_cmd_factory_reset(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb)
{
  uint8_t cmd_opcode = ESL_CMD_FACTORY_RESET;
  esl_bonded_t* esl_node;

  /* The Factory Reset command is reserved for use in the Configuring state and  
     the Updating state, the AP shall not send the Factory Reset command to an 
     ESL that is in the Synchronized state. */ 
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id); 
  if (esl_node == NULL)  
    return 2;

  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) == 0)
  { 
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
  
    if(cmd_ECP_buff == NULL)
      return 1;
    
    // to send command by ECP char
    ESL_AP_SendCmdByECP(cmd_length, false);   //No response
    return 0;
  } 
  
  return  1;
}

uint8_t ESL_AP_cmd_update_complete(uint8_t group_id, uint8_t esl_id)
{
  uint8_t cmd_opcode = ESL_CMD_UPDATE_COMPLETE;
  esl_bonded_t* esl_node;

  /* The Update Complete command is reserved for use in the Configuring state and  
     the Updating state. */ 
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id); 
  if (esl_node == NULL)  
    return 2;

  /* Update complete can be sent only if ESL is connected. */
  if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) == 0)
  { 
    // to prepare cmd packet on Updating state  
    uint8_t cmd_length;
    
    cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, NULL);
  
    if(cmd_ECP_buff == NULL)
      return 1;
    
    // to send command by ECP char
    if (ESL_AP_SendCmdByECP(cmd_length, false) == BLE_STATUS_SUCCESS)   //No response 
    {  
      bUpdatingTransition = false;
      /* After the Update Complete command sending, the AP shall start the PAST procedure */  
      //Start PAST procedure (call "periodic_sync_info_transfer")
      UTIL_SEQ_SetTask( 1U << CFG_TASK_START_INFO_TRANSFER, CFG_SEQ_PRIO_0);
      esl_node->esl_info.state = ESL_STATE_SYNCHRONIZED;
      APP_DBG_MSG("Synchronized State transition \n ");
      return 0;
    }  
  } 
  return  1;
}

uint8_t ESL_AP_cmd_refresh_display(uint8_t group_id, uint8_t esl_id, uint8_t display_index, resp_cb_t resp_cb)
{
  cmd_buff_t * cmd_buff;
  uint8_t cmd_opcode = ESL_CMD_REFRESH_IMG;
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(group_id, esl_id);   
  if (esl_node != NULL)  
  {
    if (memcmp(get_bleAppContext_address(), esl_node->esl_info.Peer_Address, BD_ADDR_SIZE) != 0)  
    {   
      // to prepare cmd packet on Synchronized state  
      cmd_buff = prepare_cmd_buff(cmd_opcode, group_id, esl_id, resp_cb);
      
      if(cmd_buff == NULL)
        return 1;
      
      cmd_buff->cmd[2] = display_index;
    }
    else
    {
      // to prepare cmd packet on Updating state  
      uint8_t cmd_length;
      
      cmd_length = prepare_cmd_ECP_buff(cmd_opcode, group_id, esl_id, resp_cb);
        
      if(cmd_ECP_buff == NULL)
        return 1;
      
      cmd_ECP_buff->cmd[2] = display_index;
      // to send command by ECP char
      ESL_AP_SendCmdByECP(cmd_length, true);  
    } 
  }
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
  ALIGN(4) uint8_t esl_payload_tag[MAX_ESL_PAYLOAD_SIZE + 2]; /* 2 extra bytes needed for ESL AD type and length */
  uint8_t esl_id;
  
  /* subevent is the Group ID */
  
  if(subevent >= MAX_GROUPS)
    return;
  
  esl_group_info_p = &esl_group_info[subevent];  
 
  esl_payload_tag[1] = AD_TYPE_ELECTRONIC_SHELF_LABEL;
  
  /* First byte of synchronization packet for commands is the Group ID. */
  /* Group_ID with value N shall be trasmitted in a PAwR subevent with number N */
  esl_payload_tag[2] = subevent; 
  curr_payload_len = 3; 
  
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
    esl_id = current_node_p->cmd[ESL_ID_CMD_OFFSET];
        
    cmd_length = GET_LENGTH_FROM_OPCODE(current_node_p->cmd[0]);
    
    if(curr_payload_len + cmd_length > MAX_ESL_PAYLOAD_SIZE)
      break;
    
    memcpy(&esl_payload_tag[curr_payload_len], current_node_p->cmd, cmd_length);
    curr_payload_len += cmd_length;
    
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
  
  if(curr_payload_len == 3)
  {
    /* No commands to be sent. */
    esl_group_info_p->adv_packet_len = 0;
    
    return;
  }
  
  esl_payload_tag[0] = curr_payload_len - 1;    /* One byte for ESL tag (AD type). */    
  
  esl_id_sync_pck = esl_id;
  
  /*
  APP_DBG_MSG("Packet:\n"); 
  int i;
  for(i=0; i<curr_payload_len-1; i++)
  {
    APP_DBG_MSG("0x%02X - ", esl_payload_tag[i]);
  }  
  APP_DBG_MSG("0x%02X\n", esl_payload_tag[i]); */
  
  /* The "ap_sync_key_material" info is the same for all ESLs */
  ble_status = aci_gap_encrypt_adv_data(ap_sync_key_material_config_value.Session_Key,
                                        ap_sync_key_material_config_value.IV,
                                        curr_payload_len,
                                        (uint32_t*)esl_payload_tag,
                                        &esl_group_info_p->adv_packet[2]); 

  esl_group_info_p->adv_packet[0] = curr_payload_len + EAD_MIC_SIZE + EAD_RANDOMZER_SIZE + 1;
  esl_group_info_p->adv_packet[1] = AD_TYPE_ENCRYPTED_ADVERTISING_DATA;
  esl_group_info_p->adv_packet_len = esl_group_info_p->adv_packet[0] + 1;
  
    
  Subevent_Data_Ptr_Parameters.Subevent = subevent;
  Subevent_Data_Ptr_Parameters.Response_Slot_Start = 0;
  // to calculate the used response slots (Response_Slot_Count) depending on the current commands.
  Subevent_Data_Ptr_Parameters.Response_Slot_Count = PAWR_NUM_RESPONSE_SLOTS;
  Subevent_Data_Ptr_Parameters.Subevent_Data_Length = esl_group_info_p->adv_packet_len ;
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
  uint8_t *current_data_resp_p;
  uint8_t resp_length;

  ALIGN(4) uint8_t decrypted_data[MAX_ESL_PAYLOAD_SIZE];
  uint8_t decrypted_data_length;
  uint8_t encrypted_data_length;
  tBleStatus ret;
  
  esl_bonded_t* esl_node; 

  /* subevent correspond to group_id */
   
  if(subevent >= MAX_GROUPS)
    return;
  
  /* We assume only one AD type is present */
  if(data[1] != AD_TYPE_ENCRYPTED_ADVERTISING_DATA)
    return;
  
  /* ADV data decryption */
  /* ADV packet length - 2 for Len and ED Tag (see Fig 5.1 ESL profile spec) */
  encrypted_data_length = data_length - 2; 
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/ 
  esl_node = ESL_AP_return_ESL_bonded(subevent, esl_id_sync_pck);
  if (esl_node == NULL)
  {  
    return;
  }  
  /* take the "esl_resp_key_material" info by esl_node */  
  ret = aci_gap_decrypt_adv_data(esl_node->esl_info.esl_resp_key_material.Session_Key,
                                 esl_node->esl_info.esl_resp_key_material.IV,
                                 encrypted_data_length,
                                 &data[2], //Encrypted data on ADV packet 
                                 (uint32_t *)decrypted_data);
  
  if(ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Decryption fail.\n");
    return;
  }
  
  decrypted_data_length = decrypted_data[0] + 1;

  if((decrypted_data_length != encrypted_data_length - EAD_RANDOMZER_SIZE - EAD_MIC_SIZE) ||
     (decrypted_data[1] != AD_TYPE_ELECTRONIC_SHELF_LABEL))
  {
    return;
  }  
 
  /* Inspect sent commands and link them to the responses. */
  esl_group_info_p = &esl_group_info[subevent];
  
  list_head_p = &esl_group_info_p->cmd_queue;
    
  current_node_p = (cmd_buff_t*)list_head_p->next;
  
  current_data_resp_p = decrypted_data + 2; /* Skip first 2 bytes for AD length and type. */
  
  while(&current_node_p->node != list_head_p && current_data_resp_p < decrypted_data + decrypted_data_length)
  {
    if(current_node_p->cmd[ESL_ID_CMD_OFFSET] == esl_id_sync_pck)
    {
      if(current_node_p->resp_cb != NULL)
      {
        current_node_p->resp_cb(subevent, esl_id_sync_pck, current_data_resp_p);
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

uint8_t ESL_AP_cmd_reconfig_esl_address(uint8_t group_id, uint8_t esl_id, uint8_t n_group_id, uint8_t n_esl_id)
{
  if((group_id >= MAX_GROUPS) || (n_group_id >= MAX_GROUPS))
    return 1;
  old_group_id = group_id;
  old_esl_id = esl_id;
  new_group_id = n_group_id;
  new_esl_id = n_esl_id;
   
  bUpdatingTransition = false;
  
  //Set the AP status 
  UTIL_SEQ_SetTask( 1u << CFG_TASK_UPDATING_STATE_TRANSITION, CFG_SEQ_PRIO_0);
  return 0;
}  

uint8_t ESL_AP_cmd_updating_state(uint8_t group_id, uint8_t esl_id)
{
  if(group_id >= MAX_GROUPS)
    return 1;
  old_group_id = group_id;
  old_esl_id = esl_id;
   
  bUpdatingTransition = true;
  //Set the AP status 
  UTIL_SEQ_SetTask( 1u << CFG_TASK_UPDATING_STATE_TRANSITION, CFG_SEQ_PRIO_0);
  return 0;
} 

//Task for CFG_TASK_UPDATING_STATE_TRANSITION
void ESL_AP_UpdatingStateTransition(void)
{
  esl_bonded_t* esl_node;
  
  /* Return an ESL bonded to AP given the Group_ID and ESL_ID*/
  esl_node = ESL_AP_return_ESL_bonded(old_group_id, old_esl_id);   
  if (esl_node != NULL)
  {
    APP_DBG_MSG("!!! ESL_AP_UpdatingStateTransition - ESL state: %d \n", esl_node->esl_info.state);
    if (esl_node->esl_info.state == ESL_STATE_SYNCHRONIZED)
    {  
      if (!bUpdatingTransition)
      {
        set_AP_Status(ESL_AP_UPDATING_ESL_ADDRESS);
      }  
      /* To transition an ESL from the Synchronized state to the Updating state, 
         the AP shall use the Periodic Advertising Connection procedure.
         When the AP connects with the ESL, the ESL transitions to the Updating state. */
      create_periodic_advertising_connection(old_group_id, esl_node->esl_info.Peer_Address, esl_node->esl_info.Peer_Address_Type);  

      esl_node->esl_info.state = ESL_STATE_UPDATING;  // to this is done after connection complete event. Remove.
      APP_DBG_MSG("Updating State transition\n");
    }
  }  
  else
    APP_DBG_MSG("!!!!  ESL_AP_UpdatingStateTransition - esl_node == NULL\n");
}

uint16_t ESL_AP_New_esl_address(void)
{
  return ESL_AP_return_ESL_address(new_group_id, new_esl_id);
}

esl_bonded_t* ESL_AP_return_ESL_to_Update(void)
{
  return ESL_AP_return_ESL_bonded(old_group_id, old_esl_id);
}


uint8_t ESL_AP_cmd_provisioning(uint8_t addr_type, uint8_t address[6], uint8_t group_id, uint8_t esl_id)
{
  if(group_id >= MAX_GROUPS)
    return 1;
  
  //to check if group_id and esl_id is already assigned to another ESL
  esl_address = (group_id << 8) | esl_id;
  if (Search_by_ESL_address_In_List(esl_address) != NULL)
  {  
    APP_DBG_MSG("ESL Address (0x%04X) already assigned!\n", esl_address);
    return 1;
  }
  
  set_bleAppContext_address(addr_type, address);
  
  // Connect to ESL address  
  UTIL_SEQ_SetTask(1u << CFG_TASK_CONN_DEV_ID, CFG_SEQ_PRIO_0);

  return 0;
} 