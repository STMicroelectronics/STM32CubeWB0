/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gatt_client_app.h
  * @author  MCD Application Team
  * @brief   Header for gatt_client_app.c module
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
#ifndef GATT_CLIENT_APP_H
#define GATT_CLIENT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  GATT_CLIENT_APP_DISCONNECTED,
  GATT_CLIENT_APP_CONNECTED,
  GATT_CLIENT_APP_DISCOVER_SERVICES,
  /* USER CODE BEGIN GATT_CLIENT_APP_State_t*/

  /* USER CODE END GATT_CLIENT_APP_State_t */
}GATT_CLIENT_APP_State_t;

typedef enum
{
  PEER_CONN_HANDLE_EVT,
  PEER_DISCON_HANDLE_EVT,
  /* USER CODE BEGIN GATT_CLIENT_APP_Conn_Opcode_t*/

  /* USER CODE END GATT_CLIENT_APP_Conn_Opcode_t */
}GATT_CLIENT_APP_Conn_Opcode_t;

typedef struct
{
  GATT_CLIENT_APP_Conn_Opcode_t          ConnOpcode;
  uint16_t                              ConnHdl;
}GATT_CLIENT_APP_ConnHandle_Notif_evt_t;
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define CFG_BLE_NUM_CLIENT_CONTEXTS                           (2)
/* USER CODE BEGIN EC */
#define MC_PERIPHERAL_UUID_16BIT                                        (0xF130)
#define LED_C_UUID_16BIT                                                (0xF131)
#define SWITCH_C_UUID_16BIT                                             (0xF132)


/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
void GATT_CLIENT_APP_Init(void);
void GATT_CLIENT_APP_Notification(GATT_CLIENT_APP_ConnHandle_Notif_evt_t *p_Notif);
void GATT_CLIENT_APP_Discover_services(uint16_t connection_handle);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*GATT_CLIENT_APP_H */
