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

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

#ifdef DEBUG
static uint32_t imageBase = APP_WITH_OTA_SERVICE_ADDRESS;
#endif
static uint16_t page_currently_written = 0;
uint8_t rx_buffer[MAX_PACKET_LENGTH], tx_buffer[MAX_PACKET_LENGTH];
uint32_t app_size = 0;

ALIGN(4) uint8_t page_buffer[2048] = {0,}; /* consider only 32 byte of data except the last one */
uint16_t page_size = 0;

static uint16_t seq_num = 0;
uint16_t seq_num_max = 0;

ota_state_machine_t ota_state_machine_g = OTA_CONNECTION;

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
* @retval tmpAppAddress: the application base address where to jump
*
* @note The API code could be subject to change in future releases.
*/
uint32_t OTA_Check_Application_Tags_Value(void)
{
  uint32_t tmpAppAddress = 0;
  
  /* If OTA_SERVICE_FLASH[16] IS OTA_SERVICE_MNG_TAG  AND  APP_FLASH[16] IS OTA_IN_PROGRESS_TAG OR OTA_INVALID_OLD_TAG ==> GO TO OTA SERVICE APP */
  if (((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_IN_PROGRESS_TAG))|| /* 10 */
      ((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_INVALID_OLD_TAG))) /* 11 */
  {
    /* Jump to OTA Service Manager Application */
    tmpAppAddress = APP_OTA_SERVICE_ADDRESS;
  }
  /* If OTA_SERVICE_FLASH[16] IS OTA_SERVICE_MNG_TAG  AND  APP_FLASH[16] IS OTA_VALID_TAG ==> GO TO APP */
  else if ((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_VALID_TAG)) /* 12 */
  {
    /* Jump to Application using OTA Service Manager */
    tmpAppAddress = APP_WITH_OTA_SERVICE_ADDRESS;
  }
  
  return tmpAppAddress;
}

void OTA_Set_Application_Tag_Value(uint32_t address,uint32_t Data)
{
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET, Data);
}



/**
  * @brief  It erases destination flash erase before starting OTA upgrade session.
  * @param  None.
  * @retval None.
  */
void OTA_Erase_Flash(uint16_t startNumber, uint16_t endNumber)
{
  FLASH_EraseInitTypeDef p_erase_init;
  uint32_t page_error;
  
  p_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
  p_erase_init.Page = startNumber;
  p_erase_init.NbPages = endNumber-startNumber+1;
  
  HAL_FLASHEx_Erase(&p_erase_init, &page_error);
}

/**
  * @brief  Callback used during CONNECTION state.
  * @param  None.
  * @retval None.
  */
uint8_t OTA_ConnectionCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t rx_valid = 0;

  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0) 
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0) 
    {
      /* check the reception of a connection header otherwise do not go to app_size state */
      if(p->data[0] == HEADER_CONNECTION) 
      {
        rx_valid = 1;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) {
      ota_state_machine_g = OTA_CONNECTION;
    }
  }
  else 
  { /* Transmit complete */
    if(rx_valid) 
    {
      rx_valid = 0;
      ota_state_machine_g = OTA_SIZE;
    }
    else {
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
  static volatile uint8_t rx_valid = 0, retry = 0;

  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
   {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0)
     {
      if(p->data[0] == HEADER_SIZE) 
      {
        if(p->data[1] == 4) 
        {
          rx_valid = 1;
          app_size = 0;
          for(uint8_t i=0; i<4; i++) 
          {
            app_size |= ((uint32_t)(p->data[2+i]))<<((3-i)*8);
          }
        }
      }
      else 
      {
        retry++;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) 
    {
      ota_state_machine_g = OTA_SIZE;
      retry++;
    }
  }
  else 
  { /* Transmit complete */
    if(rx_valid) 
    {
      retry = 0;
      rx_valid = 0;
      ota_state_machine_g = OTA_START;
    }
    else 
    {
      ota_state_machine_g = OTA_SIZE;
    }
  }
  if(retry > MAX_RETRY) 
  {
    retry = 0;
    ota_state_machine_g = OTA_CONNECTION;
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
  static volatile uint8_t retry = 0;
  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0) 
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0) 
    {
      if(p->data[0] == HEADER_START) 
      {
        retry = 0;
        ota_state_machine_g = OTA_DATAREQ;
      }
      else 
      {
        ota_state_machine_g = OTA_START;
        retry++;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) 
    {
      ota_state_machine_g = OTA_START;
      retry++;
    }
  }
  if(retry > MAX_RETRY) 
  {
    retry = 0;
    ota_state_machine_g = OTA_CONNECTION;
  }
  return TRUE;
}


/**
  * @brief  Callback used during NOT START state.
  * @param  None.
  * @retval None.
  */
uint8_t OTA_NotStartCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t retry = 0;
  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0) 
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0) 
    {
      if(p->data[0] == HEADER_NOTSTART) 
      {
        retry = 0;
        ota_state_machine_g = OTA_CONNECTION;
      }
      else {
        ota_state_machine_g = OTA_START;
        retry++;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) 
    {
      ota_state_machine_g = OTA_START;
      retry++;
    }
  }
  if(retry > MAX_RETRY) 
  {
    retry = 0;
    ota_state_machine_g = OTA_CONNECTION;
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
  static volatile uint8_t retry = 0;

  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0)
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0) 
    {
      if(p->data[0] == HEADER_DATAREQ) 
      {
        retry = 0;
        ota_state_machine_g = OTA_GETDATA;
      }
      else 
      {
        ota_state_machine_g = OTA_DATAREQ;
        retry++;
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) 
    {
      ota_state_machine_g = OTA_DATAREQ;
      retry++;
    }
  }
  if(retry > 250) 
  { /* retry used during "connection" it lasts ~ 300 ms, then disconnect */
    PRINTF(">> OTA_DataRequestCallback MAX_RETRY\r\n\n");
//    ota_state_machine_g = OTA_CONNECTION;
    retry = 0;
  }
  return TRUE;
}


/**
  * @brief  Callback used during GET DATA state.
  * @param  None.
  * @retval None.
  */
uint8_t OTA_GetDataCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t rx_valid = 0, retry = 0;
  volatile uint16_t sequence_number_received = 0;

  /* received a packet */
  if( (p->status & BLUE_STATUSREG_PREVTRANSMIT) == 0) 
  {
    if((p->status & BLUE_INTERRUPT1REG_RCVOK) != 0) 
    {
      if(p->data[0] == HEADER_GETDATA) 
      {
        rx_valid = 1;
        if(p->data[1] > 2) 
        {
          sequence_number_received = (((uint16_t)p->data[2])<<8) | ((uint16_t)p->data[3]);
          if(seq_num == sequence_number_received) 
          {
            for(uint8_t i=0; i< (p->data[1]-2); i++) 
            {
              page_buffer[page_size+i] = p->data[4+i];
            }
            page_size += p->data[1] - 2;
            seq_num++;
          }
        }
      }
    }
    else if(((p->status & BLUE_INTERRUPT1REG_RCVTIMEOUT) != 0) || ((p->status & BLUE_INTERRUPT1REG_RCVCRCERR) != 0)) 
    {
      ota_state_machine_g = OTA_GETDATA;
      retry++;
    }
  }
  else 
  { /* Transmit complete */
    if(rx_valid) 
    {
      retry = 0;
      rx_valid = 0;
      ota_state_machine_g = OTA_FLASHDATA;
    }
    else 
    {
      ota_state_machine_g = OTA_GETDATA;
    }
  }
  if(retry > MAX_RETRY) {
    retry = 0;
    ota_state_machine_g = OTA_DATAREQ;
  }
  return TRUE;
}

/**
  * @brief  It jumps to the new upgraded application
  * @param  None
  * @retval None
  *
  * @note The API code could be subject to change in future releases.
  */

void OTA_Jump_To_New_Application(void)
{
  /* Reset manager will take care of running the new application */
  NVIC_SystemReset();
}


/**
  * @brief  Initialize the OTA state machine
  * @param  None.
  * @retval uint8_t return value.
  */
uint8_t OTA_Init()
{
  ota_state_machine_g = OTA_CONNECTION;

  return 0;
}

/**
  * @brief  State machine routine
  * @param  None.
  * @retval uint8_t return value.
  */
uint8_t OTA_Tick()
{
  uint8_t ret;

  if(ota_state_machine_g == OTA_CONNECTION) 
  {
    //PRINTF("OTA_CONNECTION\r\n");
    app_size = 0;
    page_size = 0;
    seq_num = 0;
    seq_num_max = 0;
    page_currently_written = 0;
    tx_buffer[0] = HEADER_CONNECTION;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    //HAL_RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_buffer, tx_buffer, RX_TIMEOUT_DATA, MAX_LL_PACKET_LENGTH, OTA_ConnectionCallback);
    if(ret != SUCCESS_0) 
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_SIZE) 
  {
    BSP_LED_On(LD2);
    PRINTF("OTA_SIZE\r\n");
    tx_buffer[0] = HEADER_SIZE;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    //HAL_RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_buffer, tx_buffer, RX_TIMEOUT_DATA, MAX_LL_PACKET_LENGTH, OTA_SizeCallback);
    if(ret != SUCCESS_0) 
    {
      PRINTF("ERROR %d\r\n",ret);
    }
  }

  else if(ota_state_machine_g == OTA_START) 
  {
    if(app_size<=SM_APP_SIZE) 
    { /* Check if the new application fit the Flash memory */
      //PRINTF("OK for 0x%08X %.2f KB max %.2f KB\r\n", app_size, ((float)app_size)/1024.0, ((float)SM_APP_SIZE)/1024.0);
      PRINTF("OK for 0x%08X %d.%02d KB max %d.%02d KB\r\n", app_size, PRINT_INT(app_size/1024.0),PRINT_FLOAT(app_size/1024.0), PRINT_INT(SM_APP_SIZE/1024.0),PRINT_FLOAT(SM_APP_SIZE/1024.0)) ;
      printf("OTA_START\r\n");
      tx_buffer[0] = HEADER_START;
      tx_buffer[1] = 0;

      seq_num_max = app_size / BYTE_IN_FRAME;
      if(app_size%BYTE_IN_FRAME) 
      {
        seq_num_max++;
      }

      ota_state_machine_g = OTA_ONGOING;
      //HAL_RADIO_SetBackToBackTime(300);
      ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_StartCallback);
      if(ret != SUCCESS_0) 
      {
        PRINTF("ERROR %d\r\n",ret);
      }
    }
    /* TO BE DONE */
    else 
    { /* If the new image does not fit the Flash memory */
      //PRINTF("NOT ok for 0x%08X %.2f KB max %.2f KB\r\n", app_size, ((float)app_size)/1024.0, ((float)SM_APP_SIZE)/1024.0);
      PRINTF("NOT OK for 0x%08X %d.%02d KB max %d.%02d KB\r\n", app_size, PRINT_INT(app_size/1024.0),PRINT_FLOAT(app_size/1024.0), PRINT_INT(SM_APP_SIZE/1024.0),PRINT_FLOAT(SM_APP_SIZE/1024.0)) ;

      PRINTF("OTA_NOTSTART\r\n");
      tx_buffer[0] = HEADER_NOTSTART;
      tx_buffer[1] = 0;
      ota_state_machine_g = OTA_ONGOING;
      //HAL_RADIO_SetBackToBackTime(300);
      ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_NotStartCallback);
      if(ret != SUCCESS_0) {
        PRINTF("ERROR %d\r\n",ret);
      }
    }
  }

  else if(ota_state_machine_g == OTA_DATAREQ) 
  {
    if(seq_num < seq_num_max) 
    {
      PRINTF("OTA_DATAREQ %d/%d app_size %d\r\n", seq_num, seq_num_max, app_size);
      tx_buffer[0] = HEADER_DATAREQ;
      tx_buffer[1] = 2;
      tx_buffer[2] = (uint8_t)(seq_num>>8);
      tx_buffer[3] = (uint8_t)seq_num;
      ota_state_machine_g = OTA_ONGOING;
      //HAL_RADIO_SetBackToBackTime(300);
      ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_buffer, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, OTA_DataRequestCallback);
      if(ret != SUCCESS_0) {
        PRINTF("ERROR %d\r\n",ret);
      }
    }
    else 
    {
      ota_state_machine_g = OTA_FLASHDATA;
//      ota_state_machine_g = OTA_COMPLETE;
    }
  }

  else if(ota_state_machine_g == OTA_GETDATA) {
//    PRINTF("OTA_GETDATA %d page_size %d\r\n", seq_num, page_size);
    tx_buffer[0] = HEADER_GETDATA;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    //HAL_RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_buffer, tx_buffer, RX_TIMEOUT_DATA, MAX_LL_PACKET_LENGTH, OTA_GetDataCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }

  }

  else if(ota_state_machine_g == OTA_FLASHDATA) 
  {
//    PRINTF("OTA_FLASHDATA\r\n");
    PRINTF("OTA_FD %d/%d app_size %d ps %d\r\n", seq_num, seq_num_max, app_size,page_size);
    /* New data available */
    if((page_size == _MEMORY_BYTES_PER_PAGE_) || ((app_size - page_size) == 0)) {

      /* FLASH LOAD _MEMORY_BYTES_PER_PAGE_ */

      /* in which address ? must be calculated my free address plus size after write op */
      int app_size_tmp = app_size - page_size;
      if(app_size_tmp < 0) 
      {
        while(1);
      }

      uint16_t i;

      i=0;
      while(i<page_size) 
      {
        BSP_LED_Toggle(LD2);
        if( (i+16) <= page_size) 
        {
          if(page_currently_written==0 && i==16) 
          {
            page_buffer[16] = 0xFF;
            page_buffer[17] = 0xFF;
            page_buffer[18] = 0xFF;
            page_buffer[19] = 0xFF;
          }
//          LL_FLASH_ProgramBurst(FLASH, APP_WITH_OTA_SERVICE_ADDRESS + page_currently_written*2048 + i, (uint32_t *)&page_buffer[i]);

          HAL_FLASH_Program(FLASH_TYPEPROGRAM_BURST, APP_WITH_OTA_SERVICE_ADDRESS + page_currently_written*2048 + i, (uint32_t)(page_buffer+i));
          i += 16;
        }
        else 
        {
//          LL_FLASH_Program(FLASH, APP_WITH_OTA_SERVICE_ADDRESS + page_currently_written*2048 + i, *(uint32_t *)&page_buffer[i]);

          HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, APP_WITH_OTA_SERVICE_ADDRESS + page_currently_written*2048 + i, *(uint32_t *)&page_buffer[i]);
          i += 4;
        }
      }
      page_currently_written++;

      /* erase data buffer */
      for(uint16_t i=0; i<page_size; i++) 
      {
        page_buffer[i] = 0;
      }

      if(app_size_tmp >= 0) 
      {
        app_size -= page_size;
      }
      page_size = 0;
      //PRINTF("app_size 0x%08X %.2f KB\r\n", app_size, ((float)app_size)/1024.0);
      PRINTF("app_size 0x%08X %d.%02d KB\r\n", app_size, PRINT_INT(app_size/1024.0),PRINT_FLOAT(app_size/1024.0));
    }
    if(app_size) 
    {
      ota_state_machine_g = OTA_DATAREQ;
    }
    else 
    {
      ota_state_machine_g = OTA_COMPLETE;
      /* If the new image does not fit the Flash range */
      printf("OTA_COMPLETE\r\n");
      BSP_LED_Off(LD1);
      BSP_LED_Off(LD2);

      bootloadingCompleted = 1;
    }
  }

  if (bootloadingCompleted)
  {
    /* Set valid tag x the new application just successfully upgraded through OTA */
    OTA_Set_Application_Tag_Value(APP_WITH_OTA_SERVICE_ADDRESS, OTA_VALID_TAG);

    bootloadingCompleted = 0;
    bootloadingCompleted_end = 1;
    PRINTF("** Over The Air BLE  FW upgrade completed with success! *****************\r\n");
    PRINTF("** Application is JUMPING to new base address: 0x%08X *********************\r\n",(unsigned int)imageBase);
  }

  return (bootloadingCompleted_end);
}

ota_state_machine_t OTA_GetStatus(void)
{
  return ota_state_machine_g;
}

/* USER CODE END FD_LOCAL_FUNCTIONS */


