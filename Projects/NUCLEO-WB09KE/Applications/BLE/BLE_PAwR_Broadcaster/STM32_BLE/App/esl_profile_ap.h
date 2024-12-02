/**
  ******************************************************************************
  * @file    esl_profile_ap.h
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
#ifndef ESL_PROFILE_AP_H
#define ESL_PROFILE_AP_H



#define MAX_GROUPS                              PAWR_NUM_SUBEVENTS
#define MAX_ESL_PAYLOAD_SIZE                    (48U)


/* AD Type */
#define AD_TYPE_ELECTRONIC_SHELF_LABEL          (0x34)

/* Codes for Commands */
#define ESL_CMD_PING                            (0x00)
#define ESL_CMD_READ_SENSOR_DATA                (0x10)
#define ESL_CMD_DISPLAY_IMG                     (0x20)
#define ESL_CMD_LED_CONTROL                     (0xB0)
/* Code for vendor-specific commands */
#define ESL_CMD_VS_PRICE                        (0x3F)
#define ESL_CMD_VS_TXT                          (0xBF)

/* Codes for Responses */
#define ESL_RESP_ERROR                          (0x00)
#define ESL_RESP_LED_STATE                      (0x01)
#define ESL_RESP_BASIC_STATE                    (0x10)
#define ESL_RESP_DISPLAY_STATE                  (0x11)
#define ESL_RESP_SENSOR_VALUE_TAG_NIBBLE        (0x0E) // This is only the value of the Tag nibble. The Length is variable
#define ESL_RESP_VS_OK                          (0x0F)

/* Basic state response flags */
#define BASIC_STATE_SERVICE_NEEDED_BIT          (0x01)
#define BASIC_STATE_SYNCHRONIZED_BIT            (0x02)
#define BASIC_STATE_ACTIVE_LED_BIT              (0x04)
#define BASIC_STATE_PENDING_LED_UPDATE_BIT      (0x08)
#define BASIC_STATE_PENDING_DISPLAY_UPDATE_BIT  (0x10)

/* Error codes for Error Response */
#define ERROR_INVALID_OPCODE                    (0x02)
#define ERROR_INVALID_PARAMETERS                (0x06)

/* Offset in the command where to find ESL ID */
#define ESL_ID_CMD_OFFSET                       (1)

#define BRC_ESL_ID                              (0xFF)

#define BRC_RETRANSMISSIONS                     (5)
#define UNC_RETRANSMISSIONS                     (10)


#define GET_PARAM_LENGTH_FROM_OPCODE(opcode)          ((((opcode) & 0xF0) >> 4) + 1)

#define GET_LENGTH_FROM_OPCODE(opcode)                (GET_PARAM_LENGTH_FROM_OPCODE(opcode) + 1)


typedef void (*resp_cb_t)(uint8_t group_id, uint8_t esl_id, uint8_t *resp);


void ESL_AP_Init(void);

uint8_t ESL_AP_cmd_ping(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb);
uint8_t ESL_AP_cmd_led_control(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t led_index, uint8_t on_off);
uint8_t ESL_AP_cmd_txt(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, const char *txt);
uint8_t ESL_AP_cmd_price(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint16_t val_int, uint8_t val_fract);
uint8_t ESL_AP_cmd_read_sensor_data(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t sensor_index);
uint8_t ESL_AP_cmd_display_image(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index);

void ESL_AP_SubeventDataRequest(uint8_t subevent);

void ESL_AP_ResponseReport(uint8_t subevent, uint8_t response_slot, uint8_t data_length, uint8_t *data);


#endif /* ESL_PROFILE_AP_H */
