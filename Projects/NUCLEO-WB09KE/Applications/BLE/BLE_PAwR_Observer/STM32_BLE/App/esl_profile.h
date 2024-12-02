/**
  ******************************************************************************
  * @file    esl_profile.h
  * @author  GPM WBL Application Team
  * @brief   Header file for ESL profile.
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
#ifndef ESL_PROFILE_H
#define ESL_PROFILE_H

#include <stdint.h>

#ifndef GROUP_ID
#define GROUP_ID                        0x00
#endif

#ifndef ESL_ID
#define ESL_ID                          0x00
#endif


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



void ESL_PROFILE_Init(void);
void ESL_PROFILE_ConnectionComplete(uint16_t connection_handle);
void ESL_PROFILE_SyncInfoReceived(uint16_t sync_handle);
void ESL_PROFILE_AdvPayloadReceived(uint16_t pa_event, uint8_t *p_adv_data, uint8_t size);

/* Callbacks to be implemented. */

uint8_t ESL_APP_LEDControlCmdCB(uint16_t led_repeat);
uint8_t ESL_APP_SensorDataCmdCB(uint8_t sensor_index, uint8_t *data_p, uint8_t *data_length_p);
uint8_t ESL_APP_TxtVsCmdCB(uint8_t txt_length, char *txt_p);
uint8_t ESL_APP_PriceVsCmdCB(uint16_t int_part, uint8_t fract_part);
uint8_t ESL_APP_DisplayImageCmdCB(uint8_t index);

#endif /* ESL_PROFILE_H */