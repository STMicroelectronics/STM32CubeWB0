/**
  ******************************************************************************
  * @file    esl_app.c
  * @author  GPM WBL Application Team
  * @brief   Implementation of callbacks needed by ESL profile
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

#include "esl_app.h"
#include "esl_profile.h"
#include "stm32wb0x.h"
#include "stm32wb0x_nucleo.h"
#include "app_common.h"
#include "stm32wb0x_ll_adc.h"
    
extern ADC_HandleTypeDef hadc;

static void led_blink(void *arg);
static void led_on_timeout_cb(void *arg);

#define LED_OFF                                     0
#define LED_ON                                      1

#define MAX_TXT_LENGTH                           (11U)

typedef struct
{
  uint16_t int_part;
  uint8_t fract_part;  
}price_t;

typedef struct
{
  char curr_txt[MAX_TXT_LENGTH + 1];
  char next_txt[MAX_TXT_LENGTH + 1];
  price_t curr_price;
  price_t next_price;  
  uint8_t curr_img_index;
  uint8_t next_img_index;
} ESL_APP_Context_t;

ESL_APP_Context_t ESL_APP_Context = 
{
  .curr_txt = {0},
  .next_txt = {0},
  .curr_price = {0},
  .next_price = {0},
  .curr_img_index = 0xFF, // Set to 0xFF so that if a watchdog reset occurs, the icon idx can be set again to 0.
  .next_img_index = 0xFF,
};

VTIMER_HandleType led_on_timer_h = {
  .callback = led_on_timeout_cb,
};
VTIMER_HandleType led_blink_timer_h = {
  .callback = led_blink,
};
uint8_t led_state = LED_OFF;

static void led_on_timeout_cb(void *arg)
{
  BSP_LED_Off(LD1);
  APP_DBG_MSG("LED OFF\n");
}

static void led_blink(void *arg)
{  
  if(led_state == LED_OFF)
  {
    BSP_LED_On(LD1);
    led_state = LED_ON;
    HAL_RADIO_TIMER_StartVirtualTimer(&led_blink_timer_h, LED_BLINK_ON_TIME_MS);
  }
  else if(led_state == LED_ON)
  {
    BSP_LED_Off(LD1);
    led_state = LED_OFF;
    HAL_RADIO_TIMER_StartVirtualTimer(&led_blink_timer_h, LED_BLINK_OFF_TIME_MS);
  }  
}

uint8_t ESL_APP_LEDControlCmdCB(uint16_t led_repeat)
{
  if(led_repeat == 0)
  {
    BSP_LED_Off(LD1);
    APP_DBG_MSG("LED OFF\n");
    HAL_RADIO_TIMER_StopVirtualTimer(&led_blink_timer_h);
    HAL_RADIO_TIMER_StopVirtualTimer(&led_on_timer_h);
  }
  else if(led_repeat == 1)
  {
    BSP_LED_On(LD1);
    APP_DBG_MSG("LED ON\n");
    HAL_RADIO_TIMER_StopVirtualTimer(&led_blink_timer_h);
    HAL_RADIO_TIMER_StopVirtualTimer(&led_on_timer_h);
    HAL_RADIO_TIMER_StartVirtualTimer(&led_on_timer_h, LED_ON_TIMEOUT_SECONDS*1000);
  }
  /* Special value, out of standard, used for blinking LED. Need to be
    done with flashing pattern */
  else if(led_repeat == 2)
  {
    APP_DBG_MSG("LED BLINK\n");
    HAL_RADIO_TIMER_StopVirtualTimer(&led_on_timer_h);
    HAL_RADIO_TIMER_StopVirtualTimer(&led_blink_timer_h);
    led_blink(NULL);
  }
  else
  {
    return ERROR_INVALID_PARAMETERS;
  } 
  
  return 0;
}

uint8_t ESL_APP_TxtVsCmdCB(uint8_t txt_length, char *txt_p)
{
  memset(ESL_APP_Context.next_txt, 0, sizeof(ESL_APP_Context.next_txt));
  
  /* Here use MIN only for better robustness. */
  memcpy(ESL_APP_Context.next_txt, txt_p, MIN(sizeof(ESL_APP_Context.next_txt) - 1, txt_length));
  
  ESL_APP_ProcessRequest();
  
  return 0;
}

uint8_t ESL_APP_PriceVsCmdCB(uint16_t int_part, uint8_t fract_part)
{
  ESL_APP_Context.next_price.int_part = int_part;
  ESL_APP_Context.next_price.fract_part = fract_part;
  
  ESL_APP_ProcessRequest();
  
  return 0;
}

uint8_t ESL_APP_DisplayImageCmdCB(uint8_t index)
{
  ESL_APP_Context.next_img_index = index;
  
  ESL_APP_ProcessRequest();
  
  return 0;
}

uint8_t ESL_APP_SensorDataCmdCB(uint8_t sensor_index, uint8_t *data_p, uint8_t *data_length_p)
{
  uint32_t adc_val = 0;
  uint16_t batt_voltage = 0;
      
  *data_length_p = 0;  
  
  HAL_ADC_Start(&hadc);
  
  if(HAL_ADC_PollForConversion(&hadc, 3) != HAL_OK)
  {
    APP_DBG_MSG("Sensor read timeout\n");
    
    return ERROR_RETRY;
  }
  
  adc_val = HAL_ADC_GetValue(&hadc);
  
  batt_voltage = __LL_ADC_CALC_DATA_TO_VOLTAGE(LL_ADC_VIN_RANGE_3V6, adc_val, LL_ADC_DS_DATA_WIDTH_16_BIT);
  
  HOST_TO_LE_16(data_p, batt_voltage);
  *data_length_p = 2;
  
  HAL_ADC_Stop(&hadc);
  
  return 0;
}

void ESL_APP_Process(void)
{
  
}
