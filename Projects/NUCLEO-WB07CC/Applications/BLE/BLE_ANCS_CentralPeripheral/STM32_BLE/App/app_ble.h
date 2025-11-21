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
  APP_BLE_SCANNING,
/* USER CODE BEGIN ConnStatus_t */
  APP_BLE_WAIT_START_ENC,
  APP_BLE_SECURITY_REQ_STATE,
  APP_BLE_CHECK_NOTIFICATION_STATE,
  APP_BLE_GET_ATTR_STATE,
  APP_BLE_PERFORM_NOTIFICATION_ACTION_STATE,
  APP_BLE_GET_NOTIF_ATTR_STATE,
  APP_BLE_CONNECTED_SERVER_IDLE,
  APP_BLE_GET_APP_ATTR_STATE,
  APP_BLE_BONDED_STATE,
  APP_BLE_ANCS_STATE_NOT_CONFIGURED,
  APP_BLE_ANCS_STATE_IDLE,
  APP_BLE_ANCS_STATE_DISC_SERVICE,
  APP_BLE_ANCS_STATE_WAITING_DISC_SERVICE,
  APP_BLE_ANCS_STATE_DISC_NOTIFICATION,
  APP_BLE_ANCS_STATE_WAITING_DISC_NOTIFICATION,
  APP_BLE_ANCS_STATE_DISC_CONTROL_POINT,
  APP_BLE_ANCS_STATE_WAITING_DISC_CONTROL_POINT,
  APP_BLE_ANCS_STATE_DISC_DATA_SOURCE,
  APP_BLE_ANCS_STATE_WAITING_DISC_DATA_SOURCE,
  APP_BLE_ANCS_STATE_DISC_CTRLPOINT_EXTENDED_PROP,
  APP_BLE_ANCS_STATE_WAITING_DISC_CTRLPOINT_EXTENDED_PROP,
  APP_BLE_ANCS_STATE_READ_CTRLPOINT_EXTENDED_PROP_VALUE,
  APP_BLE_ANCS_STATE_WAITING_READ_CTRLPOINT_EXTENDED_PROP_VALUE,
  APP_BLE_ANCS_STATE_SUBSCRIBE_NOTIFICATION_SOURCE,
  APP_BLE_ANCS_STATE_WAITING_SUBSCRIBE_NOTIFICATION_SOURCE,
  APP_BLE_ANCS_STATE_SUBSCRIBE_DATA_SOURCE,
  APP_BLE_ANCS_STATE_WAITING_SUBSCRIBE_DATA_SOURCE,
  APP_BLE_ANCS_STATE_CMD_ONGOING,
  APP_BLE_ANCS_COMPLETE_DISCOVER,
  APP_BLE_ANCS_STATE_WAITING_DISC_SERVICE_SUCCESS,
  APP_BLE_ANCS_STATE_WAITING_DISC_NOTIFICATION_SUCCESS,
  APP_BLE_ANCS_STATE_WAITING_DISC_CONTROL_POINT_SUCCESS,
  APP_BLE_ANCS_STATE_WAITING_DISC_DATA_SOURCE_SUCCESS
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
  PROC_GAP_PERIPH_ADVERTISE_START_FAST_ONLY_CONNECTABLE,
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
