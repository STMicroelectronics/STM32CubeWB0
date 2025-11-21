/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    MC_CentralPeripheral_app.h
  * @author  MCD Application Team
  * @brief   Header for MC_CentralPeripheral_app.c
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
#ifndef MC_CENTRALPERIPHERAL_APP_H
#define MC_CENTRALPERIPHERAL_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  MC_CENTRALPERIPHERAL_CENTR_CONN_HANDLE_EVT,
  MC_CENTRALPERIPHERAL_PERIPH_CONN_HANDLE_EVT,
  MC_CENTRALPERIPHERAL_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */
  MC_CENTRALPERIPHERAL_PAIRING_ERROR_HANDLE_EVT,
  MC_CENTRALPERIPHERAL_PAIRING_SUCCESS_HANDLE_EVT,
  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  MC_CENTRALPERIPHERAL_LAST_EVT,
} MC_CENTRALPERIPHERAL_APP_OpcodeNotificationEvt_t;

typedef struct
{
  MC_CENTRALPERIPHERAL_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN MC_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t */

  /* USER CODE END MC_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t */
} MC_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#if (CFG_BLE_NUM_CLIENT_CONTEXTS + CFG_BLE_PERIPHERAL_HANDLES_MAX) > CFG_NUM_RADIO_TASKS

#error "The total number of central and peripheral devices that can be managed in the application depends on the number of radio tasks configured in the radio driver. Each connection, whether with a central or peripheral device, or even to have advertising enabled, requires a radio task. Current configuration: CFG_BLE_NUM_CLIENT_CONTEXTS, CFG_BLE_PERIPHERAL_HANDLES_MAX, CFG_NUM_RADIO_TASKS"

#endif


/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void MC_CENTRALPERIPHERAL_APP_Init(void);
void MC_CENTRALPERIPHERAL_APP_EvtRx(MC_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EF */
int APP_Context_canStartAdvertising(void);
int APP_Context_canStartScanning(void);
int APP_Context_getActiveConnectionCountCentral(void);
int APP_Context_getActiveConnectionCountPeripheral(void);
/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*MC_CENTRALPERIPHERAL_APP_H */
