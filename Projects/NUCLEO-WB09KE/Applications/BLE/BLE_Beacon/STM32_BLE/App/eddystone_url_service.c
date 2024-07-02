/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    eddystone_url_service.c
  * @author  GPM WBL Application Team
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
#include "ble.h"
#include "eddystone_beacon.h"
#include "eddystone_url_service.h"

/* Exported types ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT (16)
#define URL_LENGTH_OFFSET (7)
#define URL_OFFSET (14)

/* Private variables ---------------------------------------------------------*/
static uint8_t Url[]     = PHYSICAL_WEB_URL;
  
uint8_t url_service_data[24] = /* It also includes AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST and  AD_TYPE_FLAGS */
  {
    2,                                                                       /*< Length. */
    AD_TYPE_FLAGS,                                                           /*< Flags data type value. */
    (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED), /*< BLE general discoverable, without BR/EDR support. */
    3,                                                                       /*< Length. */
    AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST,                                     /*< Complete list of 16-bit Service UUIDs data type value. */
    0xAA, 0xFE,
    6,                                                                       /*< Length. */
    AD_TYPE_16_BIT_UUID_SERVICE_DATA,                                        /*< Service Data data type value. */
    0xAA, 0xFE,                                                              /*< 16-bit Eddystone UUID. */
    0x10,                                                                    /*< URL frame type. */
    CALIBRATED_TX_POWER_AT_0_M,                                              /*< Ranging data. */
    URL_PREFIX,                                                              /*< URL Scheme Prefix is http://www. */
    0x00,                                                                    /*< URL */
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  };

/* Exported constants --------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
tBleStatus EddystoneURL_Init(void)
{
  tBleStatus ret;
  uint16_t AdvertisingInterval = (ADVERTISING_INTERVAL_IN_MS * ADVERTISING_INTERVAL_INCREMENT / 10);
  Advertising_Set_Parameters_t Advertising_Set_Parameters;

  url_service_data[URL_LENGTH_OFFSET] = url_service_data[URL_LENGTH_OFFSET] + sizeof(Url) - 1;
  
  for (uint8_t i = 0; i < url_service_data[URL_LENGTH_OFFSET]; ++i)
  {
    url_service_data[URL_OFFSET + i] = Url[i];
  }
  
  /* Set advertising configuration for legacy advertising */  
  ret = aci_gap_set_advertising_configuration(ADV_URL_HANDLE, 
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
    APP_DBG_MSG("Error in aci_gap_set_advertising_configuration() 0x%02x\r\n", ret);
    return ret;
  }

  ret = aci_gap_set_advertising_data(ADV_URL_HANDLE, ADV_COMPLETE_DATA, sizeof(url_service_data), url_service_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_advertising_data() 0x%02x\r\n", ret);
    return ret; 
  }

  Advertising_Set_Parameters.Advertising_Handle = ADV_URL_HANDLE;
  Advertising_Set_Parameters.Duration = 0;
  Advertising_Set_Parameters.Max_Extended_Advertising_Events = 0;
  
  /* Enable advertising */
  ret = aci_gap_set_advertising_enable(1, 1, &Advertising_Set_Parameters);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_advertising_enable() 0x%02x\r\n", ret);
    return ret; 
  }

  return ret;
}


void EddystoneURL_Process(void)
{
   EddystoneURL_Init();
}
