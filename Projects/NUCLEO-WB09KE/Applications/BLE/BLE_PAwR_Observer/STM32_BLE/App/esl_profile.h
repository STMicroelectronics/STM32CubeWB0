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

#ifndef GROUP_ID
#define GROUP_ID                        0x00
#endif

#ifndef ESL_ID
#define ESL_ID                          0x00
#endif
    
#define LED_ON_TIMEOUT_SECONDS          60
#define LED_BLINK_ON_TIME_MS            10
#define LED_BLINK_OFF_TIME_MS           1000

void ESL_PROFILE_Init(void);
void ESL_PROFILE_ConnectionComplete(uint16_t connection_handle);
void ESL_PROFILE_SyncInfoReceived(uint16_t sync_handle);
void ESL_PROFILE_AdvPayloadReceived(uint16_t pa_event, uint8_t *p_adv_data, uint8_t size);
void ESL_PROFILE_UpdateDisplay(void);
void ESL_PROFILE_UpdateDisplayReqCB(void);
void ESL_PROFILE_SensorDataReqCB(uint8_t sensor_index, uint8_t *data_p, uint8_t *data_length_p);

#endif /* ESL_PROFILE_H */