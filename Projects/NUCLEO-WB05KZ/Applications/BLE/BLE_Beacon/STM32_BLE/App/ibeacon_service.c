/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ibeacon_service.c
  * @author  GPM WBL Application Team
  * @brief   Bluetooth LE ibeacon application
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
#include "ibeacon.h"
#include "ibeacon_service.h"
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
} IBeacon_InitTypeDef;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT (16)
uint8_t ibeacon_data[30];
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/

static tBleStatus IBeacon_Init(IBeacon_InitTypeDef *IBeacon_Init)
{
/* USER CODE BEGIN IBeacon_Init_1 */

/* USER CODE END IBeacon_Init_1 */

  tBleStatus ret = BLE_STATUS_SUCCESS;
  uint16_t AdvertisingInterval = (IBeacon_Init->AdvertisingInterval * ADVERTISING_INTERVAL_INCREMENT / 10);
  Advertising_Set_Parameters_t Advertising_Set_Parameters;
  
  ibeacon_data[0] = 2;                                                                       /*< Length. */
  ibeacon_data[1] = AD_TYPE_FLAGS;                                                           /*< Flags data type value. */
  ibeacon_data[2] = (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED);  /*< BLE general discoverable, without BR/EDR support. */
  ibeacon_data[3] = 26;                                                                      /*< Length. */
  ibeacon_data[4] = AD_TYPE_MANUFACTURER_SPECIFIC_DATA;                                      /*< Manufacturer Specific Data data type value. */
  ibeacon_data[5] = 0x4C;
  ibeacon_data[6] = 0x00;
  ibeacon_data[7] = 0x02;
  ibeacon_data[8] = 0x15;                                                  /*< 32-bit Manufacturer Data. */
  memcpy(&ibeacon_data[9], IBeacon_Init->UuID, 16);
  ibeacon_data[25] = IBeacon_Init->MajorID[0],                                                /*< 2-byte Major. */
  ibeacon_data[26] = IBeacon_Init->MajorID[1],
  ibeacon_data[27] = IBeacon_Init->MinorID[0],                                                /*< 2-byte Minor. */
  ibeacon_data[28] = IBeacon_Init->MinorID[1],
  ibeacon_data[29] = IBeacon_Init->CalibratedTxPower,                                         /*< Ranging data. */

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
  
  ret = aci_gap_set_advertising_data(0, ADV_COMPLETE_DATA, sizeof(ibeacon_data), ibeacon_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_advertising_data() 0x%02x\r\n", ret);
    return ret; 
  }
  
  Advertising_Set_Parameters.Advertising_Handle = 0;
  Advertising_Set_Parameters.Duration = 0;
  Advertising_Set_Parameters.Max_Extended_Advertising_Events = 0;

  /* Enable advertising */
  ret = aci_gap_set_advertising_enable(1, 1, &Advertising_Set_Parameters);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("Error in aci_gap_set_advertising_enable() 0x%02x\r\n", ret);
    return ret; 
  }

/* USER CODE BEGIN IBeacon_Init_2 */

/* USER CODE END IBeacon_Init_2 */
  return ret;
}

void IBeacon_Process(void)
{
/* USER CODE BEGIN IBeacon_Process_1 */

/* USER CODE END IBeacon_Process_1 */
  /* Default ibeacon */
  uint8_t UuID[]    = { UUID };
  uint8_t MajorID[] = { MAJOR_ID };
  uint8_t MinorID[] = { MINOR_ID };

  IBeacon_InitTypeDef IBeacon_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .UuID                = UuID,
    .MajorID             = MajorID,
    .MinorID             = MinorID,
    .CalibratedTxPower   = CALIBRATED_TX_POWER_AT_1_M
  };

  IBeacon_Init(&IBeacon_InitStruct);
/* USER CODE BEGIN IBeacon_Process_2 */

/* USER CODE END IBeacon_Process_2 */
}
/* USER CODE BEGIN FD */

/* USER CODE END FD */
