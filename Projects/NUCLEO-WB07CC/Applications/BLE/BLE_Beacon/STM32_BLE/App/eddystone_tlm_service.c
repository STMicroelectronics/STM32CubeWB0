/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    eddystone_tlm_service.c
  * @author  MCD Application Team
  * @brief   
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
#include "app_common.h"
#include "app_ble.h"
#include "ble.h"
#include "eddystone_beacon.h"
#include "eddystone_url_service.h"
#include "eddystone_uid_service.h"
#include "system_stm32wb0x.h"
#include "eddystone_tlm_service.h"

/* Exported types ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT (16)
#define TLM_ADV_RATE                   (5)

#define TLM_VERSION                    (0)
#define BATTERY_VOLTAGE                (3000)
#define BEACON_TEMPERATURE             (10000)
#define UPTIME                         (2000000)
#define ADVERTISING_COUNT              (3000000)

#define BATTERY_VOLTAGE_OFFSET         (13)
#define BEACON_TEMPERATURE_OFFSET      (15)
#define ADVERTISING_COUNT_OFFSET       (17)
#define UPTIME_OFFSET                  (21)

/* Private variables ---------------------------------------------------------*/

uint8_t tlm_service_data[] =
  {
    2,                                                                       /*< Length. */
    AD_TYPE_FLAGS,                                                           /*< Flags data type value. */
    (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED), /*< BLE general discoverable, without BR/EDR support. */
    3,                                                                       /*< Length. */
    AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST,                                     /*< Complete list of 16-bit Service UUIDs data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    17,                                                                      /*< Length. */
    AD_TYPE_16_BIT_UUID_SERVICE_DATA,                                                    /*< Service Data data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    0x20,                                                                    /*< TLM frame type. */
    TLM_VERSION,                                                                    /*< TLM version. */
    0x00,                                                                    /*< Battery voltage. */
    0x00,
    0x00,                                                                    /*< Beacon temperature. */
    0x00,
    0x00,                                                                    /*< Advertising PDU count. */
    0x00,
    0x00,
    0x00,
    0x00,                                                                     /*< Time since power-on or reboot. */
    0x00,
    0x00,
    0x00
  };
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static tBleStatus EddystoneTLM_Init(void)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  uint16_t AdvertisingInterval = (ADVERTISING_INTERVAL_IN_MS * TLM_ADV_RATE * ADVERTISING_INTERVAL_INCREMENT/ 10);
  Advertising_Set_Parameters_t Advertising_Set_Parameters;
  
  /* Set advertising configuration for legacy advertising */  
  ret = aci_gap_set_advertising_configuration(ADV_TLM_HANDLE, 
                                              GAP_MODE_GENERAL_DISCOVERABLE,
                                              HCI_ADV_EVENT_PROP_LEGACY,
                                              AdvertisingInterval, 
                                              AdvertisingInterval,
                                              HCI_ADV_CH_ALL,
                                              CFG_BD_ADDRESS_TYPE,
                                              NULL, /* No peer address */
                                              HCI_ADV_FILTER_NONE,
                                              0, /* 0 dBm */
                                              HCI_PHY_LE_1M, /* Primary advertising PHY */
                                              0, /* 0 skips */
                                              HCI_PHY_LE_1M, /* Secondary advertising PHY. Not used with legacy advertising. */
                                              0, /* SID */
                                              0 /* No scan request notifications */);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("TLM: Error in aci_gap_set_advertising_configuration() 0x%02x\r\n", ret);
    return ret;
  }

  ret = aci_gap_set_advertising_data(ADV_TLM_HANDLE, ADV_COMPLETE_DATA, sizeof(tlm_service_data), tlm_service_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("TLM: Error in aci_gap_set_advertising_data() 0x%02x\r\n", ret);
    return ret; 
  }

  Advertising_Set_Parameters.Advertising_Handle = ADV_TLM_HANDLE;
  Advertising_Set_Parameters.Duration = 0;
  Advertising_Set_Parameters.Max_Extended_Advertising_Events = 0;
  
    /* Enable advertising */
  ret = aci_gap_set_advertising_enable(1, 1, &Advertising_Set_Parameters);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("TLM: Error in aci_gap_set_advertising_enable() 0x%02x\r\n", ret);
    return ret; 
  }

  return ret;
}


/* Exported functions --------------------------------------------------------*/
void EddystoneTLM_Process(void)
{  
  tlm_service_data[BATTERY_VOLTAGE_OFFSET]      = (BATTERY_VOLTAGE & 0xFF00) >> 8;        /*< Battery voltage. */
  tlm_service_data[BATTERY_VOLTAGE_OFFSET+1]    = (BATTERY_VOLTAGE & 0x00FF); 
  tlm_service_data[BEACON_TEMPERATURE_OFFSET]   = (BEACON_TEMPERATURE & 0xFF00) >> 8;     /*< Beacon temperature. */
  tlm_service_data[BEACON_TEMPERATURE_OFFSET+1] = (BEACON_TEMPERATURE & 0x00FF); 
  tlm_service_data[ADVERTISING_COUNT_OFFSET]    = (ADVERTISING_COUNT & 0xFF000000) >> 24; /*< Advertising PDU count. */
  tlm_service_data[ADVERTISING_COUNT_OFFSET+1]  = (ADVERTISING_COUNT & 0x00FF0000) >> 16; 
  tlm_service_data[ADVERTISING_COUNT_OFFSET+2]  = (ADVERTISING_COUNT & 0x0000FF00) >> 8;
  tlm_service_data[ADVERTISING_COUNT_OFFSET+3]  = (ADVERTISING_COUNT & 0x000000FF);
    
  tlm_service_data[UPTIME_OFFSET]               = (UPTIME  & 0xFF000000) >> 24;           /*< Time since power-on or reboot. */
  tlm_service_data[UPTIME_OFFSET+1]             = (UPTIME  & 0x00FF0000) >> 16;
  tlm_service_data[UPTIME_OFFSET+2]             = (UPTIME  & 0x0000FF00) >> 8;
  tlm_service_data[UPTIME_OFFSET+3]             = (UPTIME  & 0x000000FF);
      
  /*  URL (or UID) advertise */
  EddystoneURL_Init();
  
 
  /* TLM advertise */
  EddystoneTLM_Init();
  
}

