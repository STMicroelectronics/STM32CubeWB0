/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    radio_ota.c
  * @author  GPM WBL Application Team
  * @brief   RADIO ota utility
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

/* Includes ------------------------------------------------------------------*/
#include "radio_ota.h"
#include "main.h"

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef enum {
  OTA_CONNECTION = 0,
  OTA_SIZE = 1,
  OTA_START = 2,
  OTA_DATAREQ = 3,
  OTA_GETDATA = 4,
  OTA_SENDATA = 4,
  OTA_FLASHDATA = 5,
  OTA_COMPLETE = 6,
  OTA_ONGOING = 7,
} ota_state_machine_t;

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

#define TOKEN_RELEASE 0
#define TOKEN_TAKEN_YMODEM   1
#define TOKEN_TAKEN_OTA      2

#define YMODEM_START 0xFF

#define YMODEM_SIZE  0
#define YMODEM_LOAD  1
#define YMODEM_WAIT  2
#define YMODEM_CLOSE 4
#define YMODEM_COMPLETE 5
#define YMODEM_ABORT 9

#define BYTE_IN_FRAME   16 //64


/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/

/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rx_ota_buffer[MAX_PACKET_LENGTH], tx_ota_buffer[MAX_PACKET_LENGTH];
static volatile uint16_t seq_num = 0;
static volatile uint16_t seq_num_max = 0;
static volatile uint8_t last_frame = 0;
static volatile uint8_t app_token = TOKEN_RELEASE;
static volatile uint8_t ymodem_state_machine_g = YMODEM_SIZE;
static volatile ota_state_machine_t ota_state_machine_g = OTA_CONNECTION;
static volatile uint32_t data_len = 0;
static volatile uint8_t bootloadingCompleted = 0;
static volatile uint8_t bootloadingCompleted_end = 0;
static uint32_t image_size;

uint8_t image[1024+8];
uint8_t image_tmp[BYTE_IN_FRAME+2+2+2];

extern uint8_t rx_buffer[];
extern uint32_t rx_buffer_size;
extern uint8_t *rx_buffer_ptr;
extern uint8_t *rx_buffer_tail_ptr;

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

/* USER CODE BEGIN FD */

/* USER CODE END FD */


/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */



/**
  * @brief  Process input routine used for handle the YMODEM data.
  * @param  data_buffer The array with the data received
  * @param  Nb_bytes The number of bytes received in the data_buffer array
  * @retval None.
  */
void processInputData(uint8_t* data_buffer, uint16_t Nb_bytes)
{
  volatile int i;
  if (rx_buffer_size == 0) {
    rx_buffer_ptr = rx_buffer;
    rx_buffer_tail_ptr = rx_buffer_ptr;
  }

  for (i = 0; i < Nb_bytes; i++) {
    if( (rx_buffer_tail_ptr-rx_buffer) >= RX_BUFFER_SIZE){
      // Buffer is full
      return;
    }
    *rx_buffer_tail_ptr++ = data_buffer[i];
  }
  rx_buffer_size += Nb_bytes;
}

/**
  * @brief  Callback used during CONNECTION state.
  * @param  None.
  * @retval None.
  */
uint8_t OTA_ConnectionCallback(ActionPacket* p, ActionPacket* next)
{
  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0) 
    {
      if(p->data[0] == HEADER_CONNECTION) 
      {
        ota_state_machine_g = OTA_SIZE;
      }
      else
      {
        PRINTF(">> OTA_ConnectionCallback != %02x\r\n",p->data[0]);
        ota_state_machine_g = OTA_CONNECTION;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) 
    {
      ota_state_machine_g = OTA_CONNECTION;
    }
  }
  return TRUE;
}


/**
  * @brief  Callback used during SIZE state.
  * @param  None.
  * @retval None.
  */
uint8_t OTA_SizeCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t retry = 0;

  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
    {
      if(p->data[0] == HEADER_SIZE)
      {
        ota_state_machine_g = OTA_START;
        retry = 0;
      }
      else
      {
        PRINTF(">> OTA_SizeCallback != %02x\r\n",p->data[0]);
        ota_state_machine_g = OTA_SIZE;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0))
    {
      ota_state_machine_g = OTA_SIZE;
      retry++;
    }
  }
  /* Max number of Re-Try is handled raising a failure */
  if(retry > MAX_RETRY)
  {
    PRINTF(">> OTA_SizeCallback MAX RETRY\r\n");
    ota_state_machine_g = OTA_CONNECTION;
    retry = 0;
  }
  return TRUE;
}


/**
  * @brief  Callback used during START state.
  * @param  None.
  * @retval None.
  */
uint8_t OTA_StartCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t rx_valid = 0, retry = 0;

  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
    {
      if(p->data[0] == HEADER_START)
      {
        rx_valid = 1;
      }
      else
      {
        PRINTF(">> OTA_StartCallback != %02x\r\n",p->data[0]);
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0))
    {
      ota_state_machine_g = OTA_START;
      retry++;
    }
  }
  else
  { /* ACK transmission completed */
    if(rx_valid)
    {
      rx_valid = 0;
      retry = 0;
      ota_state_machine_g = OTA_DATAREQ;
    }
    else {
      ota_state_machine_g = OTA_START;
    }
  }
  /* Max number of Re-Try is handled raising a failure */
  if(retry > MAX_RETRY)
  {
    PRINTF(">> OTA_StartCallback MAX RETRY\r\n");
    ota_state_machine_g = OTA_CONNECTION;
    retry = 0;
  }
  return TRUE;
}


/**
  * @brief  Callback used during DATA REQUEST state.
  * @param  None.
  * @retval None.
  */
uint8_t OTA_DataRequestCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t rx_valid = 0, retry = 0;

  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
    {
      if(p->data[0] == HEADER_DATAREQ)
      {
        if(p->data[1] == 2)
        {
          rx_valid = 1;
          seq_num = (((uint16_t)p->data[2])<<8) | ((uint16_t)p->data[3]);
        }
        else
        {
          PRINTF(">> OTA_DataRequestCallback != %d LENGTH WRONG\r\n", p->data[1]);
          BSP_LED_Off(LD1);
          while(1);
        }
      }
      /* It have received the ACK packet from GETDATA state */
      else if(p->data[0] == HEADER_SENDATA)
      {
        PRINTF(">> OTA_DataRequestCallback get HEADER_SENDATA\r\n");
      }
      else
      {
        PRINTF(">> OTA_DataRequestCallback != %02x\r\n", p->data[0]);
        while(1);
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) {
      ota_state_machine_g = OTA_DATAREQ;
      retry++;
    }
  }
  else 
  { /* ACK transmission completed */
    if(rx_valid) 
    {
      retry = 0;
      rx_valid = 0;
      ota_state_machine_g = OTA_SENDATA;
    }
    else 
    { /* Received a HEADER_SENDATA */
      ota_state_machine_g = OTA_DATAREQ;
    }
  }
  if(retry > MAX_RETRY)
  { /* create a reset function */
    retry = 0;
    // Handle the crash of the connection, once the Ymodem is opened (send abort on ymodem channel) */
    PRINTF(">> OTA_DataRequestCallback MAX RETRY\r\n");
    //    ota_state_machine_g = OTA_CONNECTION;
    //    seq_num = 0;
  }
  return TRUE;
}


/**
  * @brief  Callback used during SEND DATA state.
  * @param  None.
  * @retval None.
  */
uint8_t OTA_SendDataCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t retry = 0;

  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
    {
      retry = 0;
      if(p->data[0] == HEADER_SENDATA)
      {
        if(last_frame)
        {
          BSP_LED_Off(LD1);
          ota_state_machine_g = OTA_COMPLETE;
        }
        else
        {
          ota_state_machine_g = OTA_DATAREQ;
        }
      }
      else if(p->data[0] == HEADER_DATAREQ)
      {
        if(p->data[1] == 2)
        {
          seq_num = (((uint16_t)p->data[2])<<8) | ((uint16_t)p->data[3]);
          ota_state_machine_g = OTA_SENDATA;
        }
        else
        {
          PRINTF(">> OTA_DataRequestCallback != %d LENGTH WRONG\r\n", p->data[1]);
          BSP_LED_Off(LD1);
          while(1);
        }
      }
      else
      {
        PRINTF(">> OTA_SendDataCallback != %02x\r\n", p->data[0]);
        ota_state_machine_g = OTA_CONNECTION;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0))
    {
      ota_state_machine_g = OTA_SENDATA;
      retry++;
    }
  }
  if(retry > MAX_RETRY)
  {
    PRINTF(">> OTA_SendDataCallback MAX RETRY\r\n");
    retry = 0;
    ota_state_machine_g = OTA_DATAREQ;
  }
  return TRUE;
}


/**
  * @brief  Initialize the OTA state machine
  * @param  None.
  * @retval uint8_t return value.
  */
uint8_t OTA_Init(void)
{
  ota_state_machine_g = OTA_CONNECTION;

  return 0;
}

/**
  * @brief  State machine routine for OTA
  * @param  None.
  * @retval uint8_t return value.
  */
static volatile uint32_t debugger[3+1];
uint8_t OTA_Tick()
{
  static uint32_t ota_pkt_idx = 0;
  static uint32_t ota_pkt_size = 0;
  uint8_t ret;

  /* Manage the token between the two state machines */
  if(app_token == TOKEN_TAKEN_YMODEM)
    return 0;
  else
    app_token = TOKEN_TAKEN_OTA;

  if(ota_state_machine_g == OTA_CONNECTION)
  {
    PRINTF("OTA_CONNECTION\r\n");

    /* state setting */
    last_frame = 0;
    seq_num = 0;
    seq_num_max = image_size / BYTE_IN_FRAME;
    if(image_size%BYTE_IN_FRAME)
    {
      seq_num_max++;
    }

    /* frame setting */
    tx_ota_buffer[0] = HEADER_CONNECTION;
    tx_ota_buffer[1] = 0;

    debugger[3] = ota_state_machine_g;
    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(150);

    BSP_LED_On(LD1);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_ota_buffer, rx_ota_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_ConnectionCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_SIZE)
  {
    PRINTF("OTA_SIZE\r\n");

    /* frame setting */
    tx_ota_buffer[0] = HEADER_SIZE;
    tx_ota_buffer[1] = 4;
    tx_ota_buffer[2] = (uint8_t)(image_size>>24);
    tx_ota_buffer[3] = (uint8_t)(image_size>>16);
    tx_ota_buffer[4] = (uint8_t)(image_size>>8);
    tx_ota_buffer[5] = (uint8_t)image_size;

    debugger[3] = ota_state_machine_g;
    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_ota_buffer, rx_ota_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_SizeCallback);
    if(ret != SUCCESS_0)
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_START)
  {
    PRINTF("OTA_START\r\n");

    /* frame setting */
    tx_ota_buffer[0] = HEADER_START;
    tx_ota_buffer[1] = 0;

    debugger[3] = ota_state_machine_g;
    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_ota_buffer, tx_ota_buffer, RX_TIMEOUT_DATA, MAX_LL_PACKET_LENGTH, OTA_StartCallback);
    if(ret != SUCCESS_0)
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_DATAREQ)
  {
    PRINTF("OTA_DATAREQ %d\r\n", seq_num);

    /* Manage the token between the two state machines */
    if(ymodem_state_machine_g == YMODEM_LOAD)
    {
      app_token = TOKEN_RELEASE;
      return 0;
    }

    /* frame setting */
    tx_ota_buffer[0] = HEADER_DATAREQ;
    tx_ota_buffer[1] = 0;
    debugger[3] = ota_state_machine_g;
    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_ota_buffer, tx_ota_buffer, RX_TIMEOUT_DATA, MAX_LL_PACKET_LENGTH, OTA_DataRequestCallback);
    if(ret != SUCCESS_0)
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_SENDATA)
  {
    PRINTF("OTA_SENDATA %d\r\n", seq_num);

    /* pointer  of buffer */
    ota_pkt_idx = (seq_num * BYTE_IN_FRAME);

    /* size of buffer */
    if((ota_pkt_idx + BYTE_IN_FRAME) <= data_len)
    {
      debugger[0]++;
      ota_pkt_size = BYTE_IN_FRAME + 2; /* +2 because the sequence number is added */
    }
    else if((ota_pkt_idx + 1) > data_len)
    {
      debugger[1]++;
      /* last frame already sent */
      ota_state_machine_g = OTA_DATAREQ;
      if(ymodem_state_machine_g == YMODEM_CLOSE)
      {
        while(1);
      }
      app_token = TOKEN_RELEASE;
      return 0;
    }
    /* still a few byte to send, it is the last frame */
    else if((ota_pkt_idx + 1) <= data_len)
    {
      debugger[2]++;
      ota_pkt_size = (data_len - ota_pkt_idx) + 2; /* +2 because the sequence number is added */
      if(ymodem_state_machine_g == YMODEM_CLOSE)
        last_frame = 1;
    }
    else
    {
      while(1); /* not possible ? */
    }

    /* copy the buffer */
    for(uint8_t i = 0; i < ota_pkt_size; i++)
    {
      image_tmp[4+i] = image[ota_pkt_idx%1024 + i];
    }

    image_tmp[0] = HEADER_SENDATA;
    image_tmp[1] = ota_pkt_size;
    image_tmp[2] = (uint8_t)(seq_num>>8);
    image_tmp[3] = (uint8_t)seq_num;

    debugger[3] = ota_state_machine_g;
    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, (uint8_t*)image_tmp, rx_ota_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_SendDataCallback);
    if(ret != SUCCESS_0)
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_COMPLETE)
  {
    PRINTF("OTA_COMPLETE\r\n");

    PRINTF("image %d\r\n",image_size);
    ota_state_machine_g = OTA_CONNECTION;
    last_frame = 0;
    seq_num = 0;
    app_token = TOKEN_RELEASE;
  }

  if (bootloadingCompleted)
  {
    bootloadingCompleted = 0;
  }

  return (bootloadingCompleted_end);
}

/**
  * @brief  State machine routine for YMODEM
  * @param  None.
  * @retval uint8_t return value.
  */
int32_t OTA_ymodem_tick(void)
{
  int32_t ret = 0;
  static volatile uint32_t packet = 0;
  uint32_t ymodem_data_len = 0;
  volatile uint32_t tmp_size;
  uint8_t tmp_buff[1024+8];

  if(app_token == TOKEN_TAKEN_OTA)
    return 0;
  app_token = TOKEN_TAKEN_YMODEM;

  /* Get file name and size */
  if(ymodem_state_machine_g == YMODEM_SIZE)
  {
    ret = Ymodem_Receive ((uint8_t *)tmp_buff, 0, (uint32_t *) &image_size, packet++);
    if (ret != YMODEM_CONTINUE)
    {
      ymodem_state_machine_g = YMODEM_ABORT;
    }
    else
    {
      ymodem_state_machine_g = YMODEM_LOAD;
      app_token = TOKEN_RELEASE;
    }
  }

  else if(ymodem_state_machine_g == YMODEM_LOAD)
  {

    /* Answer to the previous Receive */

    Ymodem_SendAck();

    ret = Ymodem_Receive (image, 0, (uint32_t *) &ymodem_data_len, packet++);
    if (ret != YMODEM_CONTINUE)
    {
      ymodem_state_machine_g = YMODEM_ABORT;
      return ret;
    }
    else {
      app_token = TOKEN_RELEASE;
    }

    if((ymodem_data_len+data_len) > image_size)
    {
      ymodem_state_machine_g = YMODEM_CLOSE;

      /* Answer to the previous Receive */
      Ymodem_SendAck();
      data_len = image_size;
    }
    else
    {
      data_len += ymodem_data_len;
      ymodem_state_machine_g = YMODEM_WAIT;
    }
  }

  else if(ymodem_state_machine_g == YMODEM_WAIT)
  {
    /* Empty state waiting for OTA action. Still data with ymodem. */
    ymodem_state_machine_g = YMODEM_LOAD;
  }

  else if(ymodem_state_machine_g == YMODEM_CLOSE)
  {
    ymodem_state_machine_g = YMODEM_COMPLETE;

    /* Close Ymodem session */
    ret = Ymodem_Receive (tmp_buff, 0, (uint32_t *) &tmp_size, packet++);
    if (ret != YMODEM_DONE)
    {
      Ymodem_Abort();
    }
    Ymodem_SendAck();
  }

  else if(ymodem_state_machine_g == YMODEM_COMPLETE)
  {

  }

  else if(ymodem_state_machine_g == YMODEM_ABORT)
  {
    Ymodem_Abort();
    /* Reset all the states and variables or reset the system */
  }
  return 0;
}


/**
* Define mapping of OTA messages in SRAM
*/
#define CFG_OTA_REBOOT_VAL_MSG            RAM_VR.OTAActivation[0]
#define CFG_OTA_START_SECTOR_IDX_VAL_MSG  RAM_VR.OTAActivation[1]
#define CFG_OTA_NBR_OF_SECTOR_VAL_MSG     RAM_VR.OTAActivation[2]

/**
* @brief  It checks the runtime operation type and set the related OTA tags
*         for handling the proper jumping to the valid application.
* @param  None
* @retval None
*
* @note The API code could be subject to change in future releases.
*/
void OTA_Check_ServiceManager_Operation(void)
{
    if (RAM_VR.OTAActivation[0]  == OTA_APP_SWITCH_OP_CODE_GO_TO_OTA_SERVICE_MANAGER) //Go to OTA Service manager 0xB0
    {
      /* Set Invalid valid tag x OTA Application with OTA Service Manager  for allowing jumping to OTA Service manager */
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, APP_WITH_OTA_SERVICE_ADDRESS + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET, OTA_INVALID_OLD_TAG);
  
      /* Reset Service Manager ram location */
      RAM_VR.OTAActivation[0]  = 0; //OTA_INVALID_OLD_TAG;
    }
}

/**
* @brief  It defines the valid application address where to jump
*         by checking the OTA application validity tags for the lower and
*         higher applications
* @param  None.
* @retval appaddress: the application base address where to jump
*
* @note The API code could be subject to change in future releases.
*/
uint32_t OTA_Check_Application_Tags_Value(void)
{
  uint32_t appAddress = 0;
  
  /* If OTA_SERVICE_FLASH[16] IS OTA_SERVICE_MNG_TAG  AND  APP_FLASH[16] IS OTA_IN_PROGRESS_TAG OR OTA_INVALID_OLD_TAG ==> GO TO OTA SERVICE APP */
  if (((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_IN_PROGRESS_TAG))|| /* 10 */
      ((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_INVALID_OLD_TAG))) /* 11 */
  {
    /* Jump to OTA Service Manager Application */
    appAddress = APP_OTA_SERVICE_ADDRESS;
  }
  /* If OTA_SERVICE_FLASH[16] IS OTA_SERVICE_MNG_TAG  AND  APP_FLASH[16] IS OTA_VALID_TAG ==> GO TO APP */
  else if ((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_VALID_TAG)) /* 12 */
  {
    /* Jump to Application using OTA Service Manager */
    appAddress = APP_WITH_OTA_SERVICE_ADDRESS;
  }
  
  return appAddress;
}

void OTA_Set_Application_Tag_Value(uint32_t address,uint32_t Data)
{
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET, Data);
}

/* USER CODE END FD_LOCAL_FUNCTIONS */


