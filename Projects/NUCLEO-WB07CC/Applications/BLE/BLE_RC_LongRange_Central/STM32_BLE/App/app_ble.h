/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ble.h
  * @author  MCD Application Team
  * @brief   Header for ble application
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_BLE_H
#define APP_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_events.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  APP_BLE_IDLE,
  APP_BLE_LP_CONNECTING,
  APP_BLE_CONNECTED_SERVER,
  APP_BLE_CONNECTED_CLIENT,
  APP_BLE_SCANNING,
  APP_BLE_CONNECTED,
  APP_BLE_DISCOVERING_SERVICES,
  APP_BLE_DISCOVERING_CHARACS,
/* USER CODE BEGIN ConnStatus_t */

/* USER CODE END ConnStatus_t */
} APP_BLE_ConnStatus_t;

typedef enum
{
  PROC_GAP_GEN_PHY_TOGGLE,
  PROC_GAP_GEN_CONN_TERMINATE,
  PROC_GATT_EXCHANGE_CONFIG,
  /* USER CODE BEGIN ProcGapGeneralId_t*/
  PROC_GAP_GEN_PHY_TOGGLE_CODED,
  /* USER CODE END ProcGapGeneralId_t */
}ProcGapGeneralId_t;

typedef enum
{
  PROC_GAP_CENTRAL_SCAN_START,
  PROC_GAP_CENTRAL_SCAN_TERMINATE,
  /* USER CODE BEGIN ProcGapCentralId_t */
  PROC_GAP_CENTRAL_SCAN_START_CODED_PHY,
  PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS,
  PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_1M_PHY,
  PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_CODED_PHY,
  /* USER CODE END ProcGapCentralId_t */
}ProcGapCentralId_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/** 
  * ST Manufacturer ID (2 bytes: least significant and most significant bytes).
  */
#define ST_MANUF_ID_LSB         0x30
#define ST_MANUF_ID_MSB         0x00

/** 
  * BlueSTSDK Version
  */
#define  BLUESTSDK_V1           0x01
#define  BLUESTSDK_V2           0x02

/** 
  * BOARD ID 
  */
#define  BOARD_ID_NUCLEO_WB0    0x8D

/** 
  * FIRMWARE ID / Data 
  */
#define  FW_ID_RC_PERIPHERAL    0x23
#define  FW_ID_RC_PERIPHERAL_D1 0x20
#define  FW_ID_RC_PERIPHERAL_D2 0x21
#define  FW_ID_RC_PERIPHERAL_D3 0x21

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
#define SCAN_WIN_MS(x) ((uint16_t)((x)/0.625f))
#define SCAN_INT_MS(x) ((uint16_t)((x)/0.625f))
#define CONN_INT_MS(x) ((uint16_t)((x)/1.25f))
#define CONN_SUP_TIMEOUT_MS(x) ((uint16_t)((x)/10.0f))
#define CONN_CE_LENGTH_MS(x) ((uint16_t)((x)/0.625f))
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
void ModulesInit(void);
void BLE_Init(void);
void APP_BLE_Init(void);
APP_BLE_ConnStatus_t APP_BLE_Get_Client_Connection_Status(uint16_t Connection_Handle);
void APP_BLE_Procedure_Gap_General(ProcGapGeneralId_t ProcGapGeneralId);
void APP_BLE_Procedure_Gap_Central(ProcGapCentralId_t ProcGapCentralId);

/* USER CODE BEGIN EF */
void BLEStack_Process_Schedule(void);
void VTimer_Process(void);
void VTimer_Process_Schedule(void);
void NVM_Process(void);
void NVM_Process_Schedule(void);
void BLEEVT_App_Notification(const hci_pckt *hci_pckt);
/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*APP_BLE_H */
