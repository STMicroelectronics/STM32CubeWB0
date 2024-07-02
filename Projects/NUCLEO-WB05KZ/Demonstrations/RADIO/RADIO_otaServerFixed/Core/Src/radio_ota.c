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


/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern const unsigned char image[];
extern uint32_t IMAGE_SIZE;
uint32_t image_size;
uint8_t rx_ota_buffer[MAX_PACKET_LENGTH], tx_buffer[MAX_PACKET_LENGTH];
static uint16_t seq_num = 0;
uint16_t seq_num_max = 0;
uint8_t last_frame = 0;
ota_state_machine_t ota_state_machine_g = OTA_CONNECTION;
uint8_t image_tmp[BYTE_IN_FRAME+2+2+2];

static uint8_t bootloadingCompleted = 0;
static uint8_t bootloadingCompleted_end = 0;

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
  { /* Transmit complete */
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
        retry = MAX_RETRY+1;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) {
      ota_state_machine_g = OTA_DATAREQ;
      retry++;
    }
  }
  else 
  { /* Transmit complete */
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
    image_size = IMAGE_SIZE;
    seq_num = 0;
    PRINTF(">> OTA_DataRequestCallback MAX RETRY\r\n");
    ota_state_machine_g = OTA_CONNECTION;
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
uint8_t OTA_Tick()
{
  uint8_t ret;
  
  if(ota_state_machine_g == OTA_CONNECTION) 
  {
    //PRINTF("OTA_CONNECTION\r\n");
    image_size = IMAGE_SIZE;
    last_frame = 0;
    seq_num = 0;
    tx_buffer[0] = HEADER_CONNECTION;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_ota_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_ConnectionCallback);
    if(ret != SUCCESS_0) 
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }
  else if(ota_state_machine_g == OTA_SIZE) 
  {
    PRINTF("OTA_SIZE %d\r\n", image_size);

    /* frame setting */
    tx_buffer[0] = HEADER_SIZE;
    tx_buffer[1] = 4;
    tx_buffer[2] = (uint8_t)(image_size>>24);
    tx_buffer[3] = (uint8_t)(image_size>>16);
    tx_buffer[4] = (uint8_t)(image_size>>8);
    tx_buffer[5] = (uint8_t)image_size;

    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_ota_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_SizeCallback);
    if(ret != SUCCESS_0) 
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_START) 
  {
    printf("OTA_START\r\n");
    BSP_LED_On(LD2);
    
    /* frame setting */
    tx_buffer[0] = HEADER_START;
    tx_buffer[1] = 0;

    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_ota_buffer, tx_buffer, RX_TIMEOUT_DATA, MAX_LL_PACKET_LENGTH, OTA_StartCallback);
    if(ret != SUCCESS_0) 
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_DATAREQ) 
  {
    PRINTF("OTA_DATAREQ %d\r\n", seq_num);

    /* frame setting */
    tx_buffer[0] = HEADER_DATAREQ;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_ota_buffer, tx_buffer, RX_TIMEOUT_DATA, MAX_LL_PACKET_LENGTH, OTA_DataRequestCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_SENDATA) {
    PRINTF("OTA_SENDATA %d\r\n", seq_num);
    BSP_LED_Toggle(LD1);

    if(seq_num == (seq_num_max - 1)) {
      last_frame = 1;
    }

    /* Here the packet to send with data must be defined */
    image_tmp[0] = HEADER_SENDATA;
    image_tmp[2] = (uint8_t)(seq_num>>8);
    image_tmp[3] = (uint8_t)seq_num;

    if(last_frame) {
      image_tmp[1] = (image_size - BYTE_IN_FRAME*seq_num) + 2;
    }
    else 
    {
      image_tmp[1] = (BYTE_IN_FRAME+2);
    }

    for(uint8_t i = 0; i < image_tmp[1]; i++)
    {
      image_tmp[4+i] = image[seq_num*BYTE_IN_FRAME + i];
    }

    ota_state_machine_g = OTA_ONGOING;
    HAL_RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, image_tmp, rx_ota_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_SendDataCallback);
    if(ret != SUCCESS_0) 
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_COMPLETE) 
  {
    printf("OTA_COMPLETE\r\n");
    PRINTF("image %d\r\n",image_size);
    BSP_LED_Off(LD1);
    BSP_LED_Off(LD2);
    ota_state_machine_g = OTA_CONNECTION;
    image_size = IMAGE_SIZE;
    last_frame = 0;
    seq_num = 0;
  }


  if (bootloadingCompleted)
  {
    bootloadingCompleted = 0;
  }

  return (bootloadingCompleted_end);
}

/* USER CODE END FD_LOCAL_FUNCTIONS */


