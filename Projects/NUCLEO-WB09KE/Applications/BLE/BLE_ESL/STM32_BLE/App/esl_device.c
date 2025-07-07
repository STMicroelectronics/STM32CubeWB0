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
#include "esl_device.h"
#include "stm32wb0x.h"
#include "stm32wb0x_nucleo.h"
#include "app_common.h"
#include "time_ref.h"
#include "app_ble.h"
#include "led_flash.h"

/* Property ID used for Sensor Type, as specified in Mesh Device Properties.
   Present Input Voltage:
   This property represents the most recently measured input voltage for the device.
   Characteristic: Voltage
   Characteristic is described in GATT Specification Supplement.
   Unit is volt with a resolution of 1/64V.   */
#define PRESENT_INPUT_VOLTAGE_PROP_ID       0x0059

bool bFactoryReset = false;

extern ADC_HandleTypeDef hadc;

/* The ESL Display Information characteristic value consists of an array of one or more Display Data:
    - Width (uint16) = 0x01F4  (500 pixels)
    - Height (uint16) = 0x0064 (100 pixels)
    - Display_Type (uint8) = 0x0B (Full RGB) */
uint8_t ESL_Display_Info[] = {0xF4, 0x01, 0x64, 0x00, 0x0B};

/* ESL Sensor Information Characteristic fields values for one sensor (battery):
   - Size = 0x00
   - Sensor_Type: Present Input Voltage Property ID  */
uint8_t ESL_Sensor_info[] = {0x00, PRESENT_INPUT_VOLTAGE_PROP_ID & 0xFF, (PRESENT_INPUT_VOLTAGE_PROP_ID >> 8) & 0xFF};

/* The ESL LED Information characteristic is an array of one or more octets in 
   which each octet represents an LED that is supported by the ESL.
   We have 3 monochrome leds:
    - index 0: LED Blue  01110000 = 0x70 
    - index 1: LED Green 01001100 = 0x4C 
    - index 2: LED Red   01000011 = 0x43  */ 
uint8_t ESL_LED_info[] = {0x70, 0x4c, 0x43};


#if NUM_LEDS
void ESL_DEVICE_LEDControlCmdCB(uint8_t led_index, uint8_t led_RGB_Brigthness, uint8_t led_flash_pattern[5], uint8_t off_period, uint8_t on_period, uint16_t led_repeat)
{
  Led_TypeDef Led;
  uint8_t repeat_type = led_repeat & 0x0001;        //the first bit of led_repeat
  uint16_t repeat_duration = led_repeat >> 1;       //other 15 bits of led_repeat
   
  if (led_index == 0)
  {
    Led = LD1;
  }
  else if (led_index == 1)
  {
    Led = LD2;
  }
  else if (led_index == 2)
  {
    Led = LD3;
  }
  else
  {
    /* This should not happen. */
    return;
  }
  
  APP_DBG_MSG("LED Index: 0x%02x \n", led_index);
  /* For now led_RGB_Brigthness component is not used, in fact: 
     if the LED selected by the Index value is a monochrome LED, the value 
     of the Color fields shall be ignored upon receipt.*/
  APP_DBG_MSG("LED Component: 0x%02x [Brightness: %d]\n", led_RGB_Brigthness, (led_RGB_Brigthness >> 6)); 
  /* Flashing pattern is ignored for the moment, because Repeats_Duration = 0 */
  APP_DBG_MSG("LED Flashing Pattern: 0x%02x%02x%02x%02x%02x \n", led_flash_pattern[4], 
                                                                 led_flash_pattern[3],
                                                                 led_flash_pattern[2],
                                                                 led_flash_pattern[1],
                                                                 led_flash_pattern[0]);
  APP_DBG_MSG("LED Flashing Pattern OFF Period: 0x%02x \n", off_period);
  APP_DBG_MSG("LED Flashing Pattern ON Period:  0x%02x \n", on_period);
  
  APP_DBG_MSG("LED Repeats:  0x%04x [Repeat type: %d - Repeat duration: 0x%04x]\n", 
              led_repeat, repeat_type, repeat_duration);
  
  //to implement LED brightness
  
  if(repeat_duration == 0)
  { 
    /* Special Value:
    If Repeats_Duration = 0, the Flashing_Pattern field shall be ignored
    and the Repeat_Type field shall have the following interpretation:
    � If Repeat_Type = 0, then the LED shall be turned off continuously.
    � If Repeat_Type = 1, then the LED shall be turned on continuously. */
    LED_FLASH_Stop(led_index);
    
	if(repeat_type == 0)
	{
	  BSP_LED_Off(Led);
	  APP_DBG_MSG("LED OFF\n");
	  ESL_APP_SetLEDState(led_index, ESL_LED_INACTIVE);
    }
	else // repeat_type == 1
	{
	  BSP_LED_On(Led);
	  APP_DBG_MSG("LED ON\n");
	  ESL_APP_SetLEDState(led_index, ESL_LED_ACTIVE);
	}    
  }
  else
  {
    LED_FLASH_Start(led_index, led_flash_pattern, off_period, on_period, repeat_type, repeat_duration);
  }
}

void LED_FLASH_LEDChangeCB(uint8_t led_index, uint8_t led_state, bool duration_end)
{
  if(led_state == 0)
  {
    BSP_LED_Off((Led_TypeDef)led_index);
  }
  else
  {
    BSP_LED_On((Led_TypeDef)led_index);
  }
  
  APP_DBG_MSG("LED %d %s\n", led_index, led_state?"ON":"OFF");
  
  if(duration_end)
  {
    APP_DBG_MSG("LED inactive\n");
    ESL_APP_SetLEDState(led_index, ESL_LED_INACTIVE);
  }
}
    
#endif

uint8_t ESL_DEVICE_TxtVsCmdCB(uint8_t txt_length, char *txt_p)
{
  /* Vendor command to set text not implemented. */  
  return 0;
}

uint8_t ESL_DEVICE_PriceVsCmdCB(uint16_t int_part, uint8_t fract_part)
{
  /* Vendor command to set price not implemented. */  
  return 0;
}

#if NUM_DISPLAYS
void ESL_DEVICE_DisplayImageCmdCB(uint8_t display_index, uint8_t image_index)
{
  APP_DBG_MSG("ESL_DEVICE_DisplayImageCmdCB: display %d, image %d\n", display_index, image_index);
}

void ESL_DEVICE_RefreshDisplayCmdCB(uint8_t display_index, uint8_t *image_index_p)
{
}

#endif

#if NUM_SENSORS
uint8_t ESL_DEVICE_SensorDataCmdCB(uint8_t sensor_index, uint8_t *data_p, uint8_t *data_length_p)
{
  uint32_t adc_val = 0;
  uint16_t batt_voltage = 0;
  
  if(sensor_index != 0)
  {
    return ERROR_INVALID_PARAMETERS;
  }
      
  *data_length_p = 0;  
  
  HAL_ADC_Start(&hadc);
  
  if(HAL_ADC_PollForConversion(&hadc, 3) != HAL_OK)
  {
    APP_DBG_MSG("Sensor read timeout\n");
    
    return ERROR_RETRY;
  }
  
  adc_val = HAL_ADC_GetValue(&hadc);
  
  batt_voltage = __LL_ADC_CALC_DATA_TO_VOLTAGE(LL_ADC_VIN_RANGE_3V6, adc_val, LL_ADC_DS_DATA_WIDTH_16_BIT);
  
  /* Convert voltage in units of 1/64 volts. */
  batt_voltage = (uint32_t)batt_voltage * 64 / 1000;  
  
  HOST_TO_LE_16(data_p, batt_voltage);
  *data_length_p = 2;
  
  HAL_ADC_Stop(&hadc);
  
  return 0;
}
#endif

void ESL_DEVICE_ServiceResetCmdCB(void)
{
  /* Service Reset command received. 
     We reset Service Needed state if service is no more needed.
     We may keep Service Needed state if condition requiring service is still
     present.  */
  
  ESL_APP_Reset_Basic_State_Bitmap(BASIC_STATE_SERVICE_NEEDED_BIT);
}

void ESL_DEVICE_FactoryResetCB(void)
{
  HAL_NVIC_SystemReset();
}
