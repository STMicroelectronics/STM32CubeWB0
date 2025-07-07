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
#include "ble.h"
#include <stdbool.h>
#include "stm_list.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  GATT_CLIENT_APP_IDLE,
  GATT_CLIENT_APP_CONNECTED,
  GATT_CLIENT_APP_DISCONNECTING,
  GATT_CLIENT_APP_DISCONN_COMPLETE,
  GATT_CLIENT_APP_DISCOVER_SERVICES,
  GATT_CLIENT_APP_DISCOVER_CHARACS,
  GATT_CLIENT_APP_DISCOVER_WRITE_DESC,
  GATT_CLIENT_APP_DISCOVER_NOTIFICATION_CHAR_DESC,
  GATT_CLIENT_APP_ENABLE_NOTIFICATION_DESC,
  GATT_CLIENT_APP_DISABLE_NOTIFICATION_DESC,
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

typedef enum
{
  PROC_GATT_DISC_ALL_PRIMARY_SERVICES,
  PROC_GATT_DISC_ALL_CHARS,
  PROC_GATT_DISC_ALL_DESCS,
  PROC_GATT_ENABLE_ALL_NOTIFICATIONS,
  /* USER CODE BEGIN ProcGattId_t*/

  /* USER CODE END ProcGattId_t */
}ProcGattId_t;

typedef struct
{
  GATT_CLIENT_APP_Conn_Opcode_t          ConnOpcode;
  uint16_t                              ConnHdl;
}GATT_CLIENT_APP_ConnHandle_Notif_evt_t;


/* USER CODE BEGIN ET */

typedef enum
{
  ESL_STATE_UNASSOCIATED,
  ESL_STATE_CONFIGURING,
  ESL_STATE_SYNCHRONIZED,
  ESL_STATE_UPDATING,
  ESL_STATE_UNSYNCHRONIZED,
  
} ESL_PROFILE_State_t;

typedef struct
{
  uint8_t Session_Key[16];
  uint8_t IV[8];
}ESL_PROFILE_KeyMaterial_t;

typedef struct ESL_Profile_Context_t_s {
  
  uint16_t Conn_Handle;
  uint8_t Peer_Address[6];
  uint8_t Peer_Address_Type;
  
  /* State of ESL bonded to AP */
  ESL_PROFILE_State_t state;
  
  /** ESL Address characteristic
   *  Values:
   *  - ESL_ID: 8 bits value (from 0 to 7)
   *  - Group_ID: 7 bits value (from 8 to 14)
   *  - RFU: 1 bit (15)
   */
  uint16_t esl_address;
  
  /** ESL Response Key Material characteristic
   */  
  ESL_PROFILE_KeyMaterial_t esl_resp_key_material;

} ESL_Profile_Context_t;

/* If ESL is on Configuring state, AP may configure the ESL by writing new values 
   to the writable characteristics. This array contains characteristics values */
typedef struct {
  tListNode esl_queue;
  ESL_Profile_Context_t esl_info;
} esl_bonded_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define ESL_SERVICE_UUID                                                (0x1857)
#define ESL_ADDRESS_UUID                                                (0x2BF6)
#define AP_SYNC_KEY_MATERIAL_UUID                                       (0x2BF7)
#define ESL_RESP_KEY_MATERIAL_UUID                                      (0x2BF8)
#define ESL_CURR_ABS_TIME_UUID                                          (0x2BF9)
#define ESL_CONTROL_POINT_UUID                                          (0x2BFE)
#define ESL_DISPLAY_INFO_UUID                                           (0x2BFA)
#define ESL_IMAGE_INFO_UUID                                             (0x2BFB)
#define ESL_SENSOR_INFO_UUID                                            (0x2BFC)
#define ESL_LED_INFO_UUID                                               (0x2BFD)

#define OBJECT_TRANSFER_SERVICE_UUID                                    (0x1825)
#define OTS_FEATURE_UUID                                                (0x2ABD)
#define OBJECT_NAME_UUID                                                (0x2ABE)
#define OBJECT_TYPE_UUID                                                (0x2ABF)
#define OBJECT_SIZE_UUID                                                (0x2AC0)
#define OBJECT_FIRST_CREATED_UUID                                       (0x2AC1)
#define OBJECT_LAST_MODIFIED_UUID                                       (0x2AC2)
#define OBJECT_ID_UUID                                                  (0x2AC3)
#define OBJECT_PROPERTIES_UUID                                          (0x2AC4)
#define OBJECT_ACTION_CONTROL_POINT_UUID                                (0x2AC5)
#define OBJECT_LIST_CONTROL_POINT_UUID                                  (0x2AC6)
#define OBJECT_LIST_FILTER_POINT_UUID                                   (0x2AC7)
#define OBJECT_CHANGED_UUID                                             (0x2AC8)

#define CFG_MAX_NUM_CONNECTED_SERVERS                                          1

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern ESL_PROFILE_KeyMaterial_t ap_sync_key_material_config_value;
/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ---------------------------------------------*/
void GATT_CLIENT_APP_Init(void);
uint8_t GATT_CLIENT_APP_Procedure_Gatt(uint8_t index, ProcGattId_t GattProcId);
void GATT_CLIENT_APP_Notification(GATT_CLIENT_APP_ConnHandle_Notif_evt_t *p_Notif);
uint8_t GATT_CLIENT_APP_Get_State(uint8_t index);
void GATT_CLIENT_APP_Discover_services(uint8_t index);
/* USER CODE BEGIN EFP */
void ESL_AP_List_Init(void);
bool ESL_AP_Insert_ESL_In_List(uint16_t Conn_Handle, uint8_t Peer_Address[6], uint8_t Peer_Address_Type, uint16_t esl_address);
void ESL_AP_Remove_ESL_from_List(esl_bonded_t * esl_node);
esl_bonded_t* Search_by_Peer_address_In_List(uint8_t Peer_Address[6], uint8_t Peer_Address_Type);
esl_bonded_t* Search_by_ESL_address_In_List(uint16_t esl_address);
esl_bonded_t* Search_by_Conn_Handle_In_List(uint16_t conn_handle);
void display_all_ESL_bonded(tListNode *head);
void Update_Info_to_ESL_queue(esl_bonded_t* esl_node, ESL_Profile_Context_t new_info, ESL_PROFILE_KeyMaterial_t new_ap_sync_key_material);
uint16_t ESL_AP_return_ESL_address(uint8_t group_id, uint8_t esl_id);
esl_bonded_t* ESL_AP_return_ESL_bonded(uint8_t group_id, uint8_t esl_id);

uint8_t ESL_AP_write_ECP(uint8_t* cmd, uint8_t len_cmd, uint16_t* conn_handle, bool bResponse);
uint8_t ESL_AP_send_Update_Complete_cmd(uint16_t esl_address);
esl_bonded_t* ESL_AP_return_ESL_to_Update(void);
void set_ECP_Failed(bool bValue);
void ESL_APP_DisconnectionComplete(uint16_t conn_handle);
void ESL_APP_ReconnectionStateTransition(uint8_t Peer_Address[6], uint8_t Peer_Address_Type);

uint8_t ESL_APP_Read_All_Info_Chars(void);
uint8_t ESL_APP_Read_Info_Char(uint16_t ValueHdl);

uint8_t ESL_APP_Read_Display_Info_Chars(void);
uint8_t ESL_APP_Read_Image_Info_Chars(void);
uint8_t ESL_APP_Read_Sensor_Info_Chars(void);
uint8_t ESL_APP_Read_Led_Info_Chars(void);
uint8_t ESL_APP_Clear_Security_DB(void);

tBleStatus GATT_CLIENT_Read_Char(uint16_t ValueHdl, uint8_t **data_p, uint16_t *data_length_p);
tBleStatus GATT_CLIENT_Write_Char(uint16_t ValueHdl, uint8_t *data, uint16_t data_length);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*GATT_CLIENT_APP_H */
