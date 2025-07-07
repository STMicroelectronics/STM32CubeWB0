/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ots.h
  * @author  GPAM Application Team
  * @brief   Header file for ots.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef OTS_H
#define OTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_status.h"

/* Exported defines ----------------------------------------------------------*/
#define OBJECT_ID_SIZE                                                6

/* Result codes for OACP responses */
#define OACP_RESULT_SUCCESS                                         0x01
#define OACP_RESULT_NOT_SUPPORTED                                   0x02
#define OACP_RESULT_INVALID_PARAM                                   0x03
#define OACP_RESULT_INSUFF_RESOURCES                                0x04
#define OACP_RESULT_INVALID_OBJ                                     0x05
#define OACP_RESULT_CHANNEL_UNAVAILABLE                             0x06
#define OACP_RESULT_UNSUPPORTED_TYPE                                0x07
#define OACP_RESULT_PROC_NOT_PERMITTED                              0x08
#define OACP_RESULT_OBJ_LOCKED                                      0x09
#define OACP_RESULT_OPERATION_FAILED                                0x0A
  
  
/* Op Codes for OLCP procedures */
#define OLCP_OPCODE_FIRST                                           0x01
#define OLCP_OPCODE_LAST                                            0x02
#define OLCP_OPCODE_PREVIOUS                                        0x03
#define OLCP_OPCODE_NEXT                                            0x04
#define OLCP_OPCODE_GOTO                                            0x05
#define OLCP_OPCODE_ORDER                                           0x06
#define OLCP_OPCODE_REQUEST_NUM_OBJ                                 0x07
#define OLCP_OPCODE_CLEAR_MARKING                                   0x08
#define OLCP_OPCODE_RESPONSE                                        0x70
  
/* Result codes for OLCP responses */
#define OLCP_RESULT_SUCCESS                                         0x01
#define OLCP_RESULT_NOT_SUPPORTED                                   0x02
#define OLCP_RESULT_INVALID_PARAM                                   0x03
#define OLCP_RESULT_OPERATION_FAILED                                0x04
#define OLCP_RESULT_OUT_OF_BOUNDS                                   0x05
#define OLCP_RESULT_TOO_MANY_OBJ                                    0x06
#define OLCP_RESULT_NO_OBJ                                          0x07
#define OLCP_RESULT_OBJ_ID_NOT_FOUND                                0x08
  
/* Write mode bits */
#define WRITE_MODE_TRUNCATE_MASK                                    0x02  

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* External variables --------------------------------------------------------*/
  
extern ble_gatt_srv_def_t ots_service_def;

/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
  
void OTS_Init(void);
  
/* Functions to be implemented by application outside of the service */

void OTS_GetCurrentObjName(char **name_p);
/*uuid_type: 0 for 16-bit UUIDs, 1 for 128-bit UUIDS */
void OTS_GetCurrentObjType(uint8_t *uuid_type_p, uint8_t **uuid_p);
void OTS_GetCurrentObjSize(uint32_t *current_size_p, uint32_t *allocated_size_p);
void OTS_GetCurrentObjID(uint8_t **id_p);
void OTS_GetCurrentObjProp(uint32_t *prop_p);
uint8_t OTS_OACPWrite(uint32_t offset, uint32_t length, uint8_t mode);
uint8_t OTS_OLCP(uint8_t op_code);

#ifdef __cplusplus
}
#endif

#endif /*OTS_H */
