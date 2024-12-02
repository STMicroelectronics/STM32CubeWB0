/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    beacon_aoa_tag_service.c
  * @author  GPM WBL Application Team
  * @brief   Bluetooth bracon aoa tag application
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
#include <string.h>
#include "ble_stack.h"
#include "ble_const.h"
#include "beacon_aoa_tag.h"
#include "beacon_aoa_tag_service.h"
#include "app_conf.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef ------------------------------------------------------------*/

typedef struct
{
    uint16_t AdvertisingInterval;/*!< Specifies the desired advertising interval. */
    uint8_t * UuID;              /*!< Specifies the 16-byte UUID to which the beacon belongs. */
    uint8_t * MajorID;           /*!< Specifies the 2-byte beacon identifying information. */
    uint8_t * MinorID;           /*!< Specifies the 2-byte beacon identifying information. */
    uint8_t CalibratedTxPower;   /*!< Specifies the power at 1m. */
} Beacon_AoA_Tag_InitTypeDef;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Advertising interval for legacy advertising (0.625 ms units) 
  For Beacon AoA Tag this should be set to 100 ms. */
#define LEGACY_ADV_INTERVAL     160  /* 100 ms */
/* Advertising interval for extended advertising (0.625 ms units) */

/* If periodic advertising is used. Reduce advertising rate to save power. */
#define EXT_ADV_INTERVAL        1600 /* 1 s */

/* Advertising interval for periodic advertising (1.25 ms units) */
#define PERIODIC_ADV_INTERVAL   240 /* 300 ms*/

/* PHY used in extended advertising events. One between: HCI_TX_PHY_LE_1M,
  HCI_TX_PHY_LE_2M and HCI_TX_PHY_LE_CODED (HCI_TX_PHY_LE_CODED not possible for direction finding).  */
#define EXT_ADV_PHY HCI_TX_PHY_LE_1M


/* Parameters for Direction Finding */
#define CTE_LENGTH                  20
#define CTE_TYPE                    0   /* 0: AoA, 1: AoD with 1 us slots, 2: AoD with 2 us slots */
#define CTE_COUNT                   4
#define SWITCHING_PATTERN_LENGTH    0 /* Ignored for AoA */
#define ANTENNA_IDS                 NULL

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT (16)
uint8_t beacon_aoa_tag_data[30];
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/

static tBleStatus Beacon_AoA_Tag_Init(Beacon_AoA_Tag_InitTypeDef *Beacon_AoA_Tag_Init)
{
  uint8_t adv_sets = 0;
  tBleStatus ret = BLE_STATUS_SUCCESS;
  uint16_t AdvertisingInterval = (Beacon_AoA_Tag_Init->AdvertisingInterval * ADVERTISING_INTERVAL_INCREMENT / 10);
  Advertising_Set_Parameters_t Advertising_Set_Parameters[2];
  
  beacon_aoa_tag_data[0] = 2;                                                                       /*< Length. */
  beacon_aoa_tag_data[1] = AD_TYPE_FLAGS;                                                           /*< Flags data type value. */
  beacon_aoa_tag_data[2] = (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED);  /*< BLE general discoverable, without BR/EDR support. */
  beacon_aoa_tag_data[3] = 26;                                                                      /*< Length. */
  beacon_aoa_tag_data[4] = AD_TYPE_MANUFACTURER_SPECIFIC_DATA;                                      /*< Manufacturer Specific Data data type value. */
  beacon_aoa_tag_data[5] = 0x4C;
  beacon_aoa_tag_data[6] = 0x00;
  beacon_aoa_tag_data[7] = 0x02;
  beacon_aoa_tag_data[8] = 0x15;                                                  /*< 32-bit Manufacturer Data. */
  memcpy(&beacon_aoa_tag_data[9], Beacon_AoA_Tag_Init->UuID, 16);
  beacon_aoa_tag_data[25] = Beacon_AoA_Tag_Init->MajorID[0],                                                /*< 2-byte Major. */
  beacon_aoa_tag_data[26] = Beacon_AoA_Tag_Init->MajorID[1],
  beacon_aoa_tag_data[27] = Beacon_AoA_Tag_Init->MinorID[0],                                                /*< 2-byte Minor. */
  beacon_aoa_tag_data[28] = Beacon_AoA_Tag_Init->MinorID[1],
  beacon_aoa_tag_data[29] = Beacon_AoA_Tag_Init->CalibratedTxPower,                                         /*< Ranging data. */

  /* Set advertising configuration for legacy advertising */  
  ret = aci_gap_set_advertising_configuration(0, 
                                              GAP_MODE_GENERAL_DISCOVERABLE,
                                              ADV_TYPE,
                                              AdvertisingInterval, 
                                              AdvertisingInterval,
                                              HCI_ADV_CH_ALL,
                                              HCI_ADDR_PUBLIC,  /* ignored param */
                                              NULL, /* No peer address */
                                              HCI_ADV_FILTER_NONE,
                                              6, /* 6 dBm */
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
  
  ret = aci_gap_set_advertising_data(0, ADV_COMPLETE_DATA, sizeof(beacon_aoa_tag_data), beacon_aoa_tag_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_advertising_data() 0x%02x\r\n", ret);
    return ret; 
  }
  
  Advertising_Set_Parameters[adv_sets].Advertising_Handle = 0;
  Advertising_Set_Parameters[adv_sets].Duration = 0;
  Advertising_Set_Parameters[adv_sets].Max_Extended_Advertising_Events = 0;
  
  adv_sets++;
  
  /* Set advertising configuration for extended advertising */  
  ret = aci_gap_set_advertising_configuration(1, GAP_MODE_GENERAL_DISCOVERABLE,
                                              HCI_ADV_EVENT_PROP_NONE,
                                              EXT_ADV_INTERVAL, EXT_ADV_INTERVAL,
                                              HCI_ADV_CH_ALL,
                                              0,NULL, /* No peer address */
                                              HCI_ADV_FILTER_NONE,
                                              6, /* 6 dBm */
                                              (EXT_ADV_PHY==HCI_TX_PHY_LE_2M)?HCI_TX_PHY_LE_1M:EXT_ADV_PHY, /* Primary advertising PHY */
                                              0, /* 0 skips */
                                              EXT_ADV_PHY, /* Secondary advertising PHY */
                                              0, /* SID */
                                              0 /* No scan request notifications */);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_advertising_configuration() 0x%02x\r\n", ret);
    return ret;
  }
  
  ret = aci_gap_set_advertising_data(1, ADV_COMPLETE_DATA, sizeof(beacon_aoa_tag_data), beacon_aoa_tag_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_advertising_data() 0x%02x\r\n", ret);
    return ret;
  }
  
  APP_DBG_MSG("Extended advertising configured\n");
  
  Advertising_Set_Parameters[adv_sets].Advertising_Handle = 1;
  Advertising_Set_Parameters[adv_sets].Duration = 0;
  Advertising_Set_Parameters[adv_sets].Max_Extended_Advertising_Events = 0;
  
  adv_sets++;
  
  /* Configure periodic advertising */
  ret = hci_le_set_periodic_advertising_parameters(1, PERIODIC_ADV_INTERVAL, PERIODIC_ADV_INTERVAL,  0 );
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in hci_le_set_periodic_advertising_parameters() 0x%02x\r\n", ret);
    return ret;
  }    

  /* Set periodic advertising data: beacon payload */

  ret = ll_set_periodic_advertising_data_ptr(1, 0x03, sizeof(beacon_aoa_tag_data), beacon_aoa_tag_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in ll_set_periodic_advertising_data_ptr() 0x%02x\r\n", ret);
    return ret;
  }
  
  ret = hci_le_set_connectionless_cte_transmit_parameters(1, CTE_LENGTH, CTE_TYPE, CTE_COUNT, SWITCHING_PATTERN_LENGTH, ANTENNA_IDS);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in hci_le_set_connectionless_cte_transmit_parameters() 0x%02x\r\n", ret);
    return ret;
  }
  
  ret = hci_le_set_connectionless_cte_transmit_enable(1, 1);
  
  APP_DBG_MSG("CTE configured\n");
  
  /* Enable periodic advertising */
  ret = hci_le_set_periodic_advertising_enable(ENABLE, 1);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_periodic_advertising_enable() 0x%02x\r\n", ret);
    return ret;
  }
            
  APP_DBG_MSG("Periodic advertising configured\n");
  
  /* Enable advertising */
  ret = aci_gap_set_advertising_enable(1, adv_sets, Advertising_Set_Parameters);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_advertising_enable() 0x%02x\r\n", ret);
    return ret; 
  }

  return ret;
}

void Beacon_AoA_Tag_Process(void)
{

  /* Default beacon aoa tag data */
  uint8_t UuID[]    = { UUID };
  uint8_t MajorID[] = { MAJOR_ID };
  uint8_t MinorID[] = { MINOR_ID };

  Beacon_AoA_Tag_InitTypeDef Beacon_AoA_Tag_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .UuID                = UuID,
    .MajorID             = MajorID,
    .MinorID             = MinorID,
    .CalibratedTxPower   = CALIBRATED_TX_POWER_AT_1_M
  };

  Beacon_AoA_Tag_Init(&Beacon_AoA_Tag_InitStruct);
}
/* USER CODE BEGIN FD */

/* USER CODE END FD */
