/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    esl_app.h
  * @author  GPM WBL Application Team
  * @brief   Header file for ESL APP profile.
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
#ifndef ESL_APP_H
#define ESL_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ble_status.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

/* USER CODE BEGIN ET */
typedef enum
{
  ESL_STATE_UNASSOCIATED,
  ESL_STATE_CONFIGURING,
  ESL_STATE_SYNCHRONIZED,
  ESL_STATE_UPDATING,
  ESL_STATE_UNSYNCHRONIZED,
} ESL_APP_State_t;

typedef enum
{
  ESL_LED_INACTIVE,
  ESL_LED_ACTIVE,
} ESL_APP_LEDState_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* Error codes for Error Response (vendor-specific errors from 0xF0 to 0xFF). */
#define ERROR_UNSPECIFIED                       (0x01)
#define ERROR_INVALID_OPCODE                    (0x02)
#define ERROR_INVALID_STATE                     (0x03)
#define ERROR_INVALID_IMAGE_INDEX               (0x04)
#define ERROR_IMAGE_NOT_AVAILABLE               (0x05)
#define ERROR_INVALID_PARAMETERS                (0x06)
#define ERROR_CAPACITY_LIMIT                    (0x07)
#define ERROR_INSUFFICIENT_BATTERY              (0x08)
#define ERROR_INSUFFICIENT_RESOURCES            (0x09)
#define ERROR_RETRY                             (0x0A)
#define ERROR_QUEUE_FULL                        (0x0B)
#define ERROR_IMPLAUSIBLE_ABSOLUTE_TIME         (0x0C)

/* Basic state response flags */
#define BASIC_STATE_SERVICE_NEEDED_BIT          (0x01)
#define BASIC_STATE_SYNCHRONIZED_BIT            (0x02)
#define BASIC_STATE_ACTIVE_LED_BIT              (0x04)
#define BASIC_STATE_PENDING_LED_UPDATE_BIT      (0x08)
#define BASIC_STATE_PENDING_DISPLAY_UPDATE_BIT  (0x10)

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void ESL_APP_Init(void);
/* USER CODE BEGIN EF */
void ESL_APP_ConnectionComplete(uint16_t connection_handle, uint16_t sync_handle, uint8_t Peer_Address_Type, uint8_t Peer_Address[6]);
void ESL_APP_PairingComplete(uint16_t connection_handle);
void ESL_APP_DisconnectionComplete(uint16_t connection_handle);
void ESL_APP_SyncLost(void);
void ESL_APP_SyncInfoReceived(uint16_t sync_handle);
void ESL_APP_AdvPayloadReceived(uint16_t pa_event, uint8_t *p_adv_data, uint8_t size);
uint8_t ESL_APP_SetESLAddress(uint16_t address);
void ESL_APP_SetAPSyncKeyMaterial(uint8_t key_material[24]);
void ESL_APP_SetESLResponseKeyMaterial(uint8_t key_material[24]);
void ESL_APP_SetCurrentAbsoluteTime(uint32_t curr_absolute_time);
void ESL_APP_ControlPointReceived(uint8_t *p_cmd, uint8_t size);
uint8_t ESL_APP_ConfiguringOrUpdatingState(void);
void ESL_APP_PairingRequest(uint16_t connHandle);
int ESL_APP_GetAddress(uint8_t *group_id_p, uint8_t *esl_id_p);
uint8_t ESL_APP_SetBasicStateBitmap(uint8_t basic_resp_bit);
void ESL_APP_ResetBasicStateBitmap(uint8_t basic_resp_bit);
void ESL_APP_SetLEDState(uint8_t index, ESL_APP_LEDState_t led_state);
void ESL_APP_CmdProcessRequestCB(void);
void ESL_APP_CmdProcess(void);

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*ESL_APP_H */
