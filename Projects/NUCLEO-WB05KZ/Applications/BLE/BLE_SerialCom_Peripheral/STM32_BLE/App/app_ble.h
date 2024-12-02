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
/* Maximum size of L2CAP SDUs that can be received. It must be at least 64 for
  enhanced credit based mode. */
#define MAX_SDU_SIZE                     64

typedef struct
{
  uint16_t Conn_Handle;
  uint16_t Peer_Max_Transmission_Unit;
  uint16_t SPSM;
  uint16_t Channel_ID;
}BleCoCContext_t;

typedef enum
{
  BLE_CONN_HANDLE_EVT,
  BLE_DISCON_HANDLE_EVT,
  BLE_CONN_UPDATE_EVT,
  L2CAP_DATA_RECEIVED,
  EXCHANGE_ATT_MTU,
  BLE_SEND_DATA,
} CoC_APP_Opcode_Notification_evt_t;

typedef struct
{
  CoC_APP_Opcode_Notification_evt_t     CoC_Evt_Opcode;
  uint16_t                              ConnectionHandle;
  uint16_t                              DataLength;
  uint8_t                               DataTable[MAX_SDU_SIZE];
} COC_APP_ConnHandle_Not_evt_t;
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
extern BleCoCContext_t BleCoCContextPeriph;
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
void COC_PERIPH_APP_Notification( COC_APP_ConnHandle_Not_evt_t *pNotification );
void PeriphSendData( void );
void BLE_SVC_L2CAP_Conn_Update(uint16_t ConnectionHandle, uint16_t conn1, uint16_t conn2);
void VTimer_Process(void);
void VTimer_Process_Schedule(void);
void NVM_Process(void);
void NVM_Process_Schedule(void);
/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*APP_BLE_H */
