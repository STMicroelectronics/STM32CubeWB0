/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adv_ext_app.c
  * @author  MCD Application Team
  * @brief   Advertising Extension Application
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
#include "adv_ext_app.h"
#include "main.h"
#include "app_common.h"
#include "ble.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t enable;
  Advertising_Set_Parameters_t adv_set;  
  uint8_t* data;
  uint16_t data_len;
  uint8_t sid;
  uint16_t property;
  uint16_t interval_min;
  uint16_t interval_max;
  int8_t tx_power;
  uint8_t adv_channels;
  uint8_t peer_address_type;
  uint8_t* p_peer_address;
  char username[30];
} Adv_Set_Param_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
#define ADV_SET_COUNT                  (4)
#define DATA_SLICE_SIZE                (251)
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#if (CFG_BLE_NUM_RADIO_TASKS < CFG_BLE_NUM_ADV_SETS)
#error "Parameter CFG_BLE_NUM_RADIO_TASKS must be above CFG_BLE_NUM_ADV_SETS to be able to use all adv sets."
#endif
#if (CFG_BLE_NUM_ADV_SETS < ADV_SET_COUNT)
#error "Parameter CFG_BLE_NUM_ADV_SETS must be greater than ADV_SET_COUNT."
#endif
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
Adv_Set_Param_t adv_set_param[ADV_SET_COUNT];
static uint8_t a_ext_adv_data_1[200];
static uint8_t a_ext_adv_data_2[200];
static uint8_t a_p2p_legacy_buff[31];
static uint8_t a_p2p_extended_buff[31];
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static uint8_t ADV_EXT_Build_data(Adv_Set_Param_t *adv_param);
static uint8_t ADV_EXT_Set_data(Adv_Set_Param_t *adv_param);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/
extern RNG_HandleTypeDef hrng;
extern uint8_t a_AdvData[31];
/* USER CODE BEGIN EV */

uint8_t a_peeraddr[8];

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/
uint8_t ADV_EXT_Config(void)
{
  tBleStatus status = BLE_STATUS_INVALID_PARAMS;
  uint16_t data_cpt;
  uint8_t i;

/* Extended SCANNABLE */
  i = 0;
  adv_set_param[i].enable = 1;
  adv_set_param[i].data = a_ext_adv_data_1;
  adv_set_param[i].data_len = sizeof(a_ext_adv_data_1);
  strcpy(adv_set_param[i].username, "extended_scannable");
  adv_set_param[i].sid = i;
  adv_set_param[i].interval_min = 200;
  adv_set_param[i].interval_max = 300;
  adv_set_param[i].tx_power = CFG_ADV_TX_POWER_DBM;
  adv_set_param[i].adv_channels = HCI_ADV_CH_ALL;
  adv_set_param[i].property = HCI_ADV_EVENT_PROP_SCANNABLE;
  adv_set_param[i].peer_address_type = HCI_ADDR_PUBLIC;
  adv_set_param[i].p_peer_address = &a_peeraddr[0];
  adv_set_param[i].adv_set.Advertising_Handle = i;
  adv_set_param[i].adv_set.Duration = 0;
  adv_set_param[i].adv_set.Max_Extended_Advertising_Events = 0;  
  
  status = ADV_EXT_Build_data(&adv_set_param[i]);
  if (status == BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Success: ADV_EXT_Build_data\n");
  }
  else
  {
    APP_DBG_MSG("  Fail   : ADV_EXT_Build_data, result: 0x%02X\n", status);
  }

/* Extended NO SCANNABLE-NO CONNECTABLE */
  i = 1;
  adv_set_param[i].enable = 1;
  adv_set_param[i].data = a_ext_adv_data_2;
  adv_set_param[i].data_len = sizeof(a_ext_adv_data_2);
  strcpy(adv_set_param[i].username, "ext_noscan_noconn");
  adv_set_param[i].sid = i;
  adv_set_param[i].interval_min = 200;
  adv_set_param[i].interval_max = 300;
  adv_set_param[i].tx_power = CFG_ADV_TX_POWER_DBM;
  adv_set_param[i].adv_channels = HCI_ADV_CH_ALL;
  adv_set_param[i].property = HCI_ADV_EVENT_PROP_NONE;
  adv_set_param[i].peer_address_type = HCI_ADDR_PUBLIC;
  adv_set_param[i].p_peer_address = &a_peeraddr[0];
  adv_set_param[i].adv_set.Advertising_Handle = i;
  adv_set_param[i].adv_set.Duration = 0;
  adv_set_param[i].adv_set.Max_Extended_Advertising_Events = 0;
  
  status = ADV_EXT_Build_data(&adv_set_param[i]);
  if (status == BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Success: ADV_EXT_Build_data\n");
  }
  else
  {
    APP_DBG_MSG("  Fail   : ADV_EXT_Build_data, result: 0x%02X\n", status);
  }

/* Extended CONNECTABLE with p2pClient_Ext device */
  data_cpt = 0;
  memcpy(&a_p2p_extended_buff[0], &a_AdvData[0], sizeof(a_AdvData));
  data_cpt += sizeof(a_AdvData);
  
  i = 2;
  adv_set_param[i].enable = 1;
  adv_set_param[i].data = &a_p2p_extended_buff[0];
  adv_set_param[i].data_len = data_cpt;
  strcpy(adv_set_param[i].username, "EXT_P2P");
  adv_set_param[i].sid = i;
  adv_set_param[i].interval_min = 200;
  adv_set_param[i].interval_max = 300;
  adv_set_param[i].tx_power = CFG_ADV_TX_POWER_DBM;
  adv_set_param[i].adv_channels = HCI_ADV_CH_ALL;
  adv_set_param[i].property = HCI_ADV_EVENT_PROP_CONNECTABLE;
  adv_set_param[i].peer_address_type = HCI_ADDR_PUBLIC;
  adv_set_param[i].p_peer_address = &a_peeraddr[0];
  adv_set_param[i].adv_set.Advertising_Handle = i;
  adv_set_param[i].adv_set.Duration = 0;
  adv_set_param[i].adv_set.Max_Extended_Advertising_Events = 0;

/* Legacy */
  data_cpt = 0;

  memcpy(&a_p2p_legacy_buff[data_cpt], &a_AdvData[0], sizeof(a_AdvData));
  data_cpt += sizeof(a_AdvData);
  
  i = 3;
  adv_set_param[i].enable = 1;
  adv_set_param[i].data = &a_p2p_legacy_buff[0];
  adv_set_param[i].data_len = data_cpt;
  strcpy(adv_set_param[i].username, "LEGACY");
  adv_set_param[i].sid = i;
  adv_set_param[i].interval_min = 200;
  adv_set_param[i].interval_max = 300;
  adv_set_param[i].tx_power = CFG_ADV_TX_POWER_DBM;
  adv_set_param[i].adv_channels = HCI_ADV_CH_ALL;
  adv_set_param[i].property = HCI_ADV_EVENT_PROP_CONNECTABLE | HCI_ADV_EVENT_PROP_LEGACY | HCI_ADV_EVENT_PROP_SCANNABLE;
  adv_set_param[i].peer_address_type = HCI_ADDR_PUBLIC;
  adv_set_param[i].p_peer_address = &a_peeraddr[0];
  adv_set_param[i].adv_set.Advertising_Handle = i;
  adv_set_param[i].adv_set.Duration = 0;
  adv_set_param[i].adv_set.Max_Extended_Advertising_Events = 0;

  return 0;
}

uint8_t ADV_EXT_Start(void)
{
  tBleStatus status = BLE_STATUS_INVALID_PARAMS;
  uint8_t adv_set_count_supported, loop, adv_set_count;
  
  APP_DBG_MSG("--\nBegin to configure and start advertising sets defined in the application : \n" );
  
  status = hci_le_read_number_of_supported_advertising_sets(&adv_set_count_supported);
  if (status == BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Number of advertising sets supported : %d\n" ,adv_set_count_supported);
  }
  else
  {
    Error_Handler();
  }
  
  adv_set_count = 0;
  for (loop = 0 ; loop < ADV_SET_COUNT ; loop++)
  {
    if (adv_set_param[loop].enable != 0)
    {
      APP_DBG_MSG("--\n");
      adv_set_count++;
      if(adv_set_count > adv_set_count_supported)
      {
        APP_DBG_MSG("Advertising set count limit has been reached ! please update parameters app_conf.h file\n");
      }
      APP_DBG_MSG("  Adv name     : %s\n", (char*)adv_set_param[loop].username );
      APP_DBG_MSG("  SID          : %d\n", adv_set_param[loop].sid);
      APP_DBG_MSG("  Data length  : %d\n", adv_set_param[loop].data_len );
      
      status = aci_gap_set_advertising_configuration(adv_set_param[loop].adv_set.Advertising_Handle,
                                                     GAP_MODE_GENERAL_DISCOVERABLE,
                                                     adv_set_param[loop].property,
                                                     adv_set_param[loop].interval_min,
                                                     adv_set_param[loop].interval_max,
                                                     adv_set_param[loop].adv_channels,
                                                     adv_set_param[loop].peer_address_type,
                                                     adv_set_param[loop].p_peer_address,
                                                     HCI_ADV_FILTER_NONE,
                                                     (uint8_t)adv_set_param[loop].tx_power,
                                                     HCI_ADV_PHY_LE_1M, /* Primary advertising PHY */
                                                     0, /* 0 skips */
                                                     HCI_ADV_PHY_LE_1M, /* Secondary advertising PHY. Not used with legacy advertising. */
                                                     adv_set_param[loop].sid, /* SID */
                                                     0 /* Scan request notifications */);
      if (status == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Success: aci_gap_set_advertising_configuration\n");
      }
      else
      {
        APP_DBG_MSG("  Fail   : aci_gap_set_advertising_configuration, result: 0x%02X\n", status);
      }
    
      status = ADV_EXT_Set_data(&adv_set_param[loop]);
      if (status == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Success: aci_gap_set_advertising_data/aci_gap_set_scan_response_data\n");
      }
      else
      {
        APP_DBG_MSG("  Fail   : aci_gap_set_advertising_data/aci_gap_set_scan_response_data, result: 0x%02X\n", status);
      }
      
      status = aci_gap_set_advertising_enable(1, 1, &adv_set_param[loop].adv_set);
      
      if (status == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  Success: aci_gap_set_advertising_enable\n");
      }
      else
      {
        APP_DBG_MSG("  Fail   : aci_gap_set_advertising_enable, result: 0x%02X\n", status);
      }
    }
  }
    
  return status;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

static uint8_t ADV_EXT_Set_data(Adv_Set_Param_t *adv_param)
{
  uint8_t status;
  
  if (adv_param->data_len > 1650)
  {
    status = 1;
  }
  else
  {      
    if (adv_param->property == HCI_ADV_EVENT_PROP_SCANNABLE )
    {
      status = aci_gap_set_scan_response_data(adv_param->adv_set.Advertising_Handle,
                                              adv_param->data_len,
                                              adv_param->data);
    }
    else
    {
      status = aci_gap_set_advertising_data(adv_param->adv_set.Advertising_Handle,
                                            ADV_COMPLETE_DATA,
                                            adv_param->data_len,
                                            adv_param->data);
    }
  }
  return status;
}

static uint8_t ADV_EXT_Build_data(Adv_Set_Param_t *adv_param)
{
  uint16_t data_cpt, cpt;
  int16_t manuf_cpt;
  uint8_t i, status = 0;
  
  data_cpt = 0;
  memset(&adv_param->data[0], 0, adv_param->data_len);
  
  adv_param->data[data_cpt++] = 0x02;
  adv_param->data[data_cpt++] = AD_TYPE_FLAGS;
  adv_param->data[data_cpt++] = FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED;

  adv_param->data[data_cpt++] = strlen(adv_param->username) + 1;
  adv_param->data[data_cpt++] = AD_TYPE_COMPLETE_LOCAL_NAME;
  memcpy(&adv_param->data[data_cpt], &adv_param->username[0], strlen(adv_param->username));
  data_cpt += strlen(adv_param->username);
  
  manuf_cpt = (adv_param->data_len - data_cpt);
  if(manuf_cpt <= 2)
  {
    status = 1;
  }
  else
  {
    while (manuf_cpt > 0)
    {
      if (manuf_cpt >= 0xFE)
      {
        cpt = 0xFE - 2;
      }
      else
      {
        cpt = manuf_cpt - 2;
      }
      adv_param->data[data_cpt++] = cpt + 1;
      adv_param->data[data_cpt++] = AD_TYPE_MANUFACTURER_SPECIFIC_DATA;
      adv_param->data[data_cpt++] = 0x30; /* STMicroelectronis company ID */
      cpt--;
      adv_param->data[data_cpt++] = 0x00;    
      cpt--;
      for (i = 0 ; i < cpt; i++)
      {
        adv_param->data[data_cpt + i] = i;
      }
      data_cpt += cpt;
      
      manuf_cpt = (adv_param->data_len - data_cpt);
    }
  }
  
  return status;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
