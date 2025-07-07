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
#define AD_TYPE_ENCRYPTED_ADVERTISING_DATA      (0x31)

/* Codes for Commands */
#define ESL_CMD_PING                            (0x00)
#define ESL_CMD_READ_SENSOR_DATA                (0x10)
#define ESL_CMD_DISPLAY_IMG                     (0x20)
#define ESL_CMD_LED_CONTROL                     (0xB0)

#define ESL_CMD_UNASSOCIATE_FROM_AP             (0x01)
#define ESL_CMD_SERVICE_RESET                   (0x02)
#define ESL_CMD_FACTORY_RESET                   (0x03)
#define ESL_CMD_REFRESH_IMG                     (0x11)
#define ESL_CMD_DISPLAY_TIMED_IMG               (0x60)
#define ESL_CMD_LED_TIMED_CONTROL               (0xF0)

/* Requests that the ESL return to the Synchronized state once synchronized */
#define ESL_CMD_UPDATE_COMPLETE                 (0x04)
    
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

#define MAX_NUM_ESL_DEVICES     4

typedef void (*resp_cb_t)(uint8_t group_id, uint8_t esl_id, uint8_t *resp);

void ESL_AP_Init(void);

uint8_t ESL_AP_command(uint8_t cmd_opcode, uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb);

uint8_t ESL_AP_cmd_led_control(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t led_index, uint8_t led_component, uint64_t flash_pattern, uint8_t off_period, uint8_t on_period, uint16_t repeat);  

uint8_t ESL_AP_cmd_txt(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, const char *txt);
uint8_t ESL_AP_cmd_price(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint16_t val_int, uint8_t val_fract);
uint8_t ESL_AP_cmd_read_sensor_data(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t sensor_index);
uint8_t ESL_AP_cmd_display_image(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index);

uint8_t ESL_AP_cmd_led_timed_control(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t led_index, uint8_t led_component, uint64_t flash_pattern, uint8_t off_period, uint8_t on_period, uint16_t repeat, uint32_t absolute_time);
uint8_t ESL_AP_cmd_display_timed_image(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb, uint8_t display_index, uint8_t image_index, uint32_t absolute_time);
uint8_t ESL_AP_cmd_factory_reset(uint8_t group_id, uint8_t esl_id, resp_cb_t resp_cb);
uint8_t ESL_AP_cmd_update_complete(uint8_t group_id, uint8_t esl_id);
uint8_t ESL_AP_cmd_refresh_display(uint8_t group_id, uint8_t esl_id, uint8_t display_index, resp_cb_t resp_cb);

void ESL_AP_SubeventDataRequest(uint8_t subevent);

void ESL_AP_ResponseReport(uint8_t subevent, uint8_t response_slot, uint8_t data_length, uint8_t *data);

uint8_t ESL_AP_cmd_reconfig_esl_address(uint8_t group_id, uint8_t esl_id, uint8_t new_group_id, uint8_t new_esl_id);
uint16_t ESL_AP_New_esl_address(void);

uint8_t ESL_AP_cmd_updating_state(uint8_t group_id, uint8_t esl_id);
void ESL_AP_UpdatingStateTransition(void);

uint8_t ESL_AP_cmd_provisioning(uint8_t addr_type, uint8_t address[6], uint8_t group_id, uint8_t esl_id);

void ECP_respCB(uint16_t connHandle, uint8_t *current_data_resp_p);

uint8_t Scan_proc(void);
void set_bleAppContext_address(uint8_t address_type, uint8_t p_address[6]);
uint8_t* get_bleAppContext_address(void);

#endif /* ESL_PROFILE_AP_H */
