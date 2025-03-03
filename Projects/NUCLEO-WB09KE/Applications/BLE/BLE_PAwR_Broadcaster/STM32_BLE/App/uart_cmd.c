/**
  ******************************************************************************
  * @file    uart_cmd.c
  * @author  GPM WBL Application Team
  * @brief   Implementation of uart commands for ESL.
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uart_cmd.h"
#include "esl_profile_ap.h"
#include "app_debug.h"
#include "app_common.h"

#define C_SIZE_CMD_STRING       256U

static uint8_t CommandString[C_SIZE_CMD_STRING];
static volatile uint16_t indexReceiveChar = 0;
static uint8_t echo_mode = 0;
static volatile uint8_t buff_lock = 0;

void UART_CMD_DataReceived(uint8_t * pRxDataBuff, uint16_t nDataSize)
{
  /* nDataSize always 1 in current implementation. */
  
  if(buff_lock)
  {
    /* Previous command still need to be processed. This should not happen. */
    return;
  }  
  
  /* Filling buffer and wait for '\r' char */
  if (indexReceiveChar < C_SIZE_CMD_STRING - 1)
  {
    if(echo_mode)
    {
      putchar(*pRxDataBuff);
    }
    if (*pRxDataBuff == '\r')
    {
      CommandString[indexReceiveChar] = '\0';
      buff_lock = 1;
        
      //APP_DBG_MSG("received %s\n", CommandString);
      
      UART_CMD_ProcessRequestCB();
    }
    else
    {
      CommandString[indexReceiveChar++] = *pRxDataBuff;
    }
  }
}

static void print_esl_resp(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t resp_length;
  
  resp_length =  GET_LENGTH_FROM_OPCODE(resp[0]);
  
  APP_DBG_MSG("Response from ESL (%d, %d): ", group_id, esl_id);
  
  for(int i = 0; i < resp_length; i++)
  {
    APP_DBG_MSG("0x%02X ", resp[i]);
  }
  
  APP_DBG_MSG("\n");
}

static void ping_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status = 0;
  uint16_t basic_state = 0;  
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_BASIC_STATE)
  {
    basic_state = LE_TO_HOST_16(&resp[1]);    
  }
  else
  {
    status = 1;
  }
  
  printf("+STATE:%02X,%02X,%d,%d\r\n", group_id, esl_id, status, basic_state);
}

static void led_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_LED_STATE)
  {
    status = 0;
  }
  else
  {
    status = 1;
  }
  
  printf("+LED:%02X,%02X,%d\r\n", group_id, esl_id, status);
}

static void txt_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_VS_OK)
  {
    status = 0;
  }
  else
  {
    status = 1;
  }
  
  printf("+TXT:%02X,%02X,%d\r\n", group_id, esl_id, status);
}

static void price_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_VS_OK)
  {
    status = 0;
  }
  else
  {
    status = 1;
  }
  
  printf("+PRICE:%02X,%02X,%d\r\n", group_id, esl_id, status);
}

static void batt_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;  
  uint8_t resp_param_length;
  uint16_t batt_voltage = 0;
  uint8_t sensor_index;
  
  resp_param_length =  GET_PARAM_LENGTH_FROM_OPCODE(resp[0]);
  sensor_index = resp[1];
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp_param_length != 3 || sensor_index != 0)
  {
    status = 1;
    goto fail;
  }
  
  if((resp[0] & 0x0F) == ESL_RESP_SENSOR_VALUE_TAG_NIBBLE)
  {
    status = 0;
    batt_voltage = LE_TO_HOST_16(&resp[2]);    
  }
  else
  {
    status = 1;
  }
  
fail:
  printf("+BATT:%02X,%02X,%d,%d\r\n", group_id, esl_id, status, batt_voltage);
}

static void img_resp_cb(uint8_t group_id, uint8_t esl_id, uint8_t *resp)
{
  uint8_t status;
  uint8_t img_index = 0;
  
  print_esl_resp(group_id, esl_id, resp);
  
  if(resp[0] == ESL_RESP_DISPLAY_STATE)
  {
    status = 0;
    img_index = resp[2];
  }
  else
  {
    status = 1;
  }
  
  printf("+IMG:%02X,%02X,%d,%d\r\n", group_id, esl_id, status, img_index);
}

static int parse_cmd(void)
{
  uint32_t group_id, esl_id;
  
  if(strncasecmp((char *)CommandString, "ATE", 3) == 0)
  {
    uint32_t echo = 1;
    
    sscanf((char*)CommandString + 3, "%x", &echo);
    if(echo > 1)
    {
      return 1;
    }
    else
    {
      echo_mode = echo;
      
      return 0;
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+PING=", 8) == 0)
  {
    int ret;
    
    ret = sscanf((char*)CommandString + 8, "%x,%x", &group_id, &esl_id);
    if(ret != 2 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_cmd_ping(group_id, esl_id, ping_resp_cb) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if(strncasecmp((char *)CommandString, "AT+IMG=", 7) == 0)
  {
    uint32_t image_index;
    int ret;
    
    ret = sscanf((char*)CommandString + 7, "%x,%x,%x", &group_id, &esl_id, &image_index);
    if(ret != 3 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_cmd_display_image(group_id, esl_id, img_resp_cb, 0, image_index) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if (strncasecmp((char *)CommandString, "AT+LED=", 7) == 0)
  {
    uint32_t led_status;
    int ret;
    
    ret = sscanf((char*)CommandString + 7, "%x,%x,%u", &group_id, &esl_id, &led_status);
    if(ret != 3 || group_id >= MAX_GROUPS || led_status > 2)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_cmd_led_control(group_id, esl_id, led_resp_cb, 0, led_status) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if (strncasecmp((char *)CommandString, "AT+TXT=", 7) == 0)
  {
    char text[61];
    int ret;
    
    ret = sscanf((char*)CommandString + 7, "%x,%x,%[^\t\r\n]", &group_id, &esl_id, text);
    if(ret != 3 || group_id >= MAX_GROUPS)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_cmd_txt(group_id, esl_id, txt_resp_cb, text) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if (strncasecmp((char *)CommandString, "AT+PRICE=", 9) == 0)
  {
    uint32_t val_int, val_fract;
    int ret;
    
    ret = sscanf((char*)CommandString + 9, "%x,%x,%u.%u", &group_id, &esl_id, &val_int, &val_fract);
    if(ret != 4 || group_id >= MAX_GROUPS || val_int > 999 || val_fract > 99)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_cmd_price(group_id, esl_id, price_resp_cb, val_int, val_fract) == 0)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else if (strncasecmp((char *)CommandString, "AT+BATT=", 8) == 0)
  {
    int ret;
    
    ret = sscanf((char*)CommandString + 8, "%x,%x", &group_id, &esl_id);
    if(ret != 2 || group_id >= MAX_GROUPS || esl_id == 0xFF)
    {
      return 1;
    }
    else
    {
      if(ESL_AP_cmd_read_sensor_data(group_id, esl_id, batt_resp_cb, 0) == 0)
      {
        return 0;
      }
      else
      {      
        return 1;
      }
    }
  }
  
  return 1;
  
}

void UART_CMD_Process(void)
{
  if(parse_cmd() == 0)
  {
    printf("OK\r\n");
  }
  else
  {
    printf("ERROR\r\n");
  }
  
  indexReceiveChar = 0;
  
  buff_lock = 0;
}
