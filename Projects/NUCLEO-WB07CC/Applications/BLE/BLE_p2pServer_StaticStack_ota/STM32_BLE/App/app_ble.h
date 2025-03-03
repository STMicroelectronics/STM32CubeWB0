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
  APP_BLE_ADV_FAST,
  APP_BLE_ADV_LP,
/* USER CODE BEGIN ConnStatus_t */

/* USER CODE END ConnStatus_t */
} APP_BLE_ConnStatus_t;

typedef enum
{
  PROC_GAP_GEN_PHY_TOGGLE,
  PROC_GAP_GEN_CONN_TERMINATE,
  PROC_GATT_EXCHANGE_CONFIG,
  /* USER CODE BEGIN ProcGapGeneralId_t*/

  /* USER CODE END ProcGapGeneralId_t */
}ProcGapGeneralId_t;

typedef enum
{
  PROC_GAP_PERIPH_ADVERTISE_START_LP,
  PROC_GAP_PERIPH_ADVERTISE_START_FAST,
  PROC_GAP_PERIPH_ADVERTISE_STOP,
  PROC_GAP_PERIPH_ADVERTISE_DATA_UPDATE,
  PROC_GAP_PERIPH_CONN_PARAM_UPDATE,
  PROC_GAP_PERIPH_CONN_TERMINATE,

  PROC_GAP_PERIPH_SET_BROADCAST_MODE,
  /* USER CODE BEGIN ProcGapPeripheralId_t */

  /* USER CODE END ProcGapPeripheralId_t */
}ProcGapPeripheralId_t;

typedef enum
{
  PROC_GAP_CENTRAL_SCAN_START,
  PROC_GAP_CENTRAL_SCAN_TERMINATE,
  /* USER CODE BEGIN ProcGapCentralId_t */

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
  * FIRMWARE ID 
  */
#define  FW_ID_P2P_SERVER       0x83
#define  FW_ID_P2P_ROUTER       0x85
#define  FW_ID_COC_PERIPH       0x87
#define  FW_ID_DT_SERVER        0x88
#define  FW_ID_HEART_RATE       0x89
#define  FW_ID_HEALTH_THERMO    0x8A
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
APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status(void);
void APP_BLE_Procedure_Gap_General(ProcGapGeneralId_t ProcGapGeneralId);
void APP_BLE_Procedure_Gap_Peripheral(ProcGapPeripheralId_t ProcGapPeripheralId);

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
