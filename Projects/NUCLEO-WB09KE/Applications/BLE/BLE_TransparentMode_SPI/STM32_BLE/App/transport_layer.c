/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file    tramsport_layer.c
* @author  GPM WBL Application Team
* @brief   Transport layer file
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
#include "main.h"
#include "stm32wb0x.h"
#include "fifo.h"
#include "compiler.h"
#include "transport_layer.h"
#include "dtm_cmd_db.h"
#include "stm32wb0x_hal_dma.h"
#include "stm32wb0x_hal_uart.h"
#include "hci_parser.h"
#include "dtm_preprocess_events.h"
#include "app_common.h"
#include "app_ble.h"

/* Private typedef -----------------------------------------------------------*/\

typedef PACKED(struct) event_lost_register_s {
  uint8_t event_lost;
  uint8_t event_register[5];
  uint64_t event_lost_code;
} event_lost_register_t;

/* Private define ------------------------------------------------------------*/

#define MAX_EVENT_SIZE  (536)

#if defined(STM32WB07) || defined(STM32WB06) || defined(STM32WB09)
#define COMMAND_BUFFER_SIZE  (536 + 4)
#define EVENT_BUFFER_SIZE    2300
#elif defined(STM32WB05)
#define COMMAND_BUFFER_SIZE  265        /* Decrease buffer size for reducing RAM footprint */
#define EVENT_BUFFER_SIZE    1024
#endif

#define FIFO_ALIGNMENT       4
#define FIFO_VAR_LEN_ITEM_MAX_SIZE (MAX_EVENT_SIZE)

#define DMA_CH_UART_TX          LL_DMA_CHANNEL_1
#define DMA_CH_UART_RX          LL_DMA_CHANNEL_2

#define HCI_RESET_OPCODE        0x0C03

#define LEGACY_ADV_OPCODE_LOW  0x2006 // Lowest opcode for legacy advertising commands
#define LEGACY_ADV_OPCODE_HIGH 0x200D // Highest opcode for legacy advertising commands

#define EXTENDED_ADV_OPCODE_LOW  0x2036 // Lowest opcode for extended advertising commands
#define EXTENDED_ADV_OPCODE_HIGH 0x204A // Highest opcode for extended advertising commands

// Opcodes of commands that returns command status instead of command complete events
#define HCI_LE_CREATE_CONNECTION_OPCODE                 0x200D
#define HCI_LE_EXTENDED_CREATE_CONNECTION_OPCODE        0x2043
#define HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_OPCODE  0x2044

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

ALIGN(2) static uint8_t event_buffer[EVENT_BUFFER_SIZE + FIFO_VAR_LEN_ITEM_MAX_SIZE];
static uint8_t command_buffer[COMMAND_BUFFER_SIZE];
ALIGN(2) uint8_t command_fifo_buffer_tmp[COMMAND_BUFFER_SIZE];
static circular_fifo_t event_fifo, command_fifo;
uint16_t command_fifo_dma_len;

static event_lost_register_t event_lost_register;

#ifdef DEBUG_DTM
DebugLabel debug_buf[DEBUG_ARRAY_LEN] = {EMPTY,};
uint32_t debug_cnt = 0;
#endif

#define SPI_HEADER_LEN  (uint8_t)(4)    /* Indeed the header len is 5 due to load of dummy from FIFO */
#define SPI_CTRL_WRITE  (uint8_t)(0x0A)
#define SPI_CTRL_READ   (uint8_t)(0x0B)

SpiProtoType spi_proto_state = SPI_PROT_INIT_STATE;

/* Store first 4 bytes replaced with spi header during send event procedure */
uint8_t event_fifo_header_restore[4] = {0,0,0,0};
uint8_t restore_flag = 0;

/* Flag to signal if a timoeout HEADER_NOT_RECEIVED happens */
static uint8_t header_timeout = 0;

ALIGN(2) uint8_t buff_dma[MAX_EVENT_SIZE + SPI_HEADER_LEN + 1];

/* External variables --------------------------------------------------------*/

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef handle_DMA_Channel2;

/* Private function prototypes -----------------------------------------------*/

static void enqueue_event(circular_fifo_t *fifo, uint16_t buff_len1, const uint8_t *buff_evt1, uint16_t buff_len2, const uint8_t *buff_evt2, int8_t overflow_index);
void DMA_Rearm(uint32_t dma_channel, uint32_t buffer, uint32_t size);

/* Private functions ---------------------------------------------------------*/

/* "If, since the last power-on or reset, the Host has ever issued a legacy
  advertising command and then issues an extended advertising command, or
  has ever issued an extended advertising command and then issues a legacy
  advertising command, the Controller shall return the error code Command
  Disallowed (0x0C)."
  This function returns 1 if an error has to be given. */
static uint8_t check_legacy_extended_call(uint16_t opcode, uint8_t *buffer_out)
{
  static uint8_t legacy_cmd_issued = FALSE, extended_cmd_issued =  FALSE;
  uint8_t allowed = TRUE;

  if(opcode >= LEGACY_ADV_OPCODE_LOW && opcode <= LEGACY_ADV_OPCODE_HIGH)
  {
    if(extended_cmd_issued)
    {
      allowed = FALSE; // Error
    }
    else
    {
      legacy_cmd_issued = TRUE;
      allowed = TRUE; // OK
    }
  }
  else if(opcode >= EXTENDED_ADV_OPCODE_LOW && opcode <= EXTENDED_ADV_OPCODE_HIGH)
  {
    if(legacy_cmd_issued)
    {      
      allowed = FALSE; // Error
    }
    else
    {
      extended_cmd_issued = TRUE;
      allowed = TRUE; // OK
    }
  }

  if(!allowed){
    if(opcode == HCI_LE_CREATE_CONNECTION_OPCODE ||
       opcode == HCI_LE_EXTENDED_CREATE_CONNECTION_OPCODE||
       opcode == HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_OPCODE)
    {
      buffer_out[0] = 0x04;
      buffer_out[1] = 0x0F;
      buffer_out[2] = 0x04;
      buffer_out[3] = 0x0C;
      buffer_out[4] = 0x01;
      HOST_TO_LE_16(buffer_out+5,opcode);
      return 7;
    }
    else
    {
      buffer_out[0] = 0x04;
      buffer_out[1] = 0x0E;
      buffer_out[2] = 0x04;
      buffer_out[3] = 0x01;
      HOST_TO_LE_16(buffer_out+4,opcode);
      buffer_out[6] = 0x0C;
      return 7;
    }
  }

  return 0;
}

/* Process Commands */
uint16_t process_command(uint16_t op_code, uint8_t *buffer_in, uint16_t buffer_in_length, uint8_t *buffer_out, uint16_t buffer_out_max_length)
{
  uint32_t i;
  uint16_t ret_val;

  ret_val = check_legacy_extended_call(op_code, buffer_out);
  if(ret_val != 0)
  {
    return ret_val;
  }

  for (i = 0; hci_command_table[i].opcode != 0; i++)
  {
    if (op_code == hci_command_table[i].opcode)
    {
      ret_val = hci_command_table[i].execute(buffer_in, buffer_in_length, buffer_out, buffer_out_max_length);
      /* add get crash handler */
      return ret_val;
    }
  }

  // Unknown command length
  buffer_out[0] = 0x04;
  buffer_out[1] = 0x0F;
  buffer_out[2] = 0x04;
  buffer_out[3] = 0x01;
  buffer_out[4] = 0x01;
  HOST_TO_LE_16(buffer_out+5, op_code);
  return 7;
}

/**
* @brief  Transport Layer Init.
*	  Init the transport layer.
* @param  None
* @retval None
*/
void transport_layer_init(void)
{
  /* Queue index init */
  fifo_init(&event_fifo, EVENT_BUFFER_SIZE, event_buffer, FIFO_ALIGNMENT);
  fifo_init(&command_fifo, COMMAND_BUFFER_SIZE, command_buffer, FIFO_ALIGNMENT);

  /* event_lost_register init */
  event_lost_register.event_lost = 0;
  event_lost_register.event_register[0] = 0x04;
  event_lost_register.event_register[1] = 0xFF;
  event_lost_register.event_register[2] = 0x0A;
  event_lost_register.event_register[3] = 0x02;
  event_lost_register.event_register[4] = 0x00;
  event_lost_register.event_lost_code = 0;
  
  SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_STATE);
}
  
static void transport_layer_receive_data(void)
{  
  static uint8_t data[4];
  
  restore_flag = 0;  
  command_fifo_dma_len = (command_fifo.max_size - fifo_size(&command_fifo));
  
  data[0] = (uint8_t)command_fifo_dma_len;
  data[1] = (uint8_t)(command_fifo_dma_len>>8);
  data[2] = 0;
  data[3] = 0;
  
  buff_dma[0] = 0xFF;
  memcpy(&buff_dma[1], data, SPI_HEADER_LEN);
    
  DEBUG_NOTES(RECEIVE_DATA);
  DMA_Rearm(DMA_CH_SPI_RX, (uint32_t)command_fifo_buffer_tmp, command_fifo_dma_len);
  DMA_Rearm(DMA_CH_SPI_TX, (uint32_t)buff_dma, SPI_HEADER_LEN+1);
  
}

static void transport_layer_send_data(uint8_t *data, uint16_t data_length)
{
  restore_flag = 1;
  
  
  event_fifo_header_restore[0] = data[0];
  event_fifo_header_restore[1] = data[1];
  event_fifo_header_restore[2] = data[2];
  event_fifo_header_restore[3] = data[3];
  
  command_fifo_dma_len = (command_fifo.max_size - fifo_size(&command_fifo));
  
  data[0] = (uint8_t)command_fifo_dma_len;
  data[1] = (uint8_t)(command_fifo_dma_len>>8);
  data[2] = (uint8_t)data_length;
  data[3] = (uint8_t)(data_length>>8);
  
  buff_dma[0] = 0xFF;
  memcpy(&buff_dma[1], data, data_length+SPI_HEADER_LEN);
  
  LL_SPI_ReceiveData16(SPI);
  LL_SPI_ReceiveData16(SPI);
  
  DEBUG_NOTES(SEND_DATA);
  
  DMA_Rearm(DMA_CH_SPI_RX, (uint32_t)command_fifo_buffer_tmp, command_fifo_dma_len);
  DMA_Rearm(DMA_CH_SPI_TX, (uint32_t)buff_dma, data_length+SPI_HEADER_LEN+1); 
}

/**
* @brief  Advance transport layer state machine
* @param  None
* @retval Desired sleep level
*/
void transport_layer_tick(void)
{
  uint8_t buffer[COMMAND_BUFFER_SIZE], buffer_out[FIFO_VAR_LEN_ITEM_MAX_SIZE];
  uint16_t len;
  uint16_t size = 0;
  uint32_t tick_start_val;  
  
  if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_HOST_REQ_STATE))
  {
    DEBUG_NOTES(PARSE_HOST_REQ);    
    transport_layer_receive_data();          
    LL_DMA_EnableChannel(DMA1, DMA_CH_SPI_RX);
    LL_DMA_EnableChannel(DMA1, DMA_CH_SPI_TX);
    SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
    /* Give authorization to send data. */
    LL_GPIO_SetOutputPin(IRQ_GPIO_Port, IRQ_Pin);
    DEBUG_NOTES(IRQ_RISE);
  }
  /* Event queue */
  else if ((fifo_size(&event_fifo) > 0) && (SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE) || SPI_STATE_CHECK(SPI_PROT_SLEEP_STATE)))
  {
    uint8_t *ptr;
    if (fifo_get_ptr_var_len_item(&event_fifo, &size, &ptr) == 0) 
    {
      SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_EVENT_PEND_STATE);      
      if (!header_timeout)
        transport_layer_send_data(ptr, size);
      header_timeout = 0;
      /* Signal data availability. */
      LL_GPIO_SetOutputPin(IRQ_GPIO_Port, IRQ_Pin);
      DEBUG_NOTES(IRQ_RISE);
    }
  }
  
  if(SPI_STATE_CHECK(SPI_PROT_WAITING_HEADER_STATE))
  {
    tick_start_val = HAL_GetTick();
  }
  
  while(SPI_STATE_CHECK(SPI_PROT_WAITING_HEADER_STATE))
  {    
    volatile uint16_t tmp_spi_dma_len = LL_DMA_GetDataLength(DMA1, DMA_CH_SPI_RX);
    tmp_spi_dma_len = (command_fifo_dma_len - tmp_spi_dma_len);
    
    if(tmp_spi_dma_len > 4)
    {
      SPI_STATE_TRANSACTION(SPI_PROT_WAITING_DATA_STATE);
      /* Host has begun to send data. Reset signal that we used to give authorization to send data. */
      LL_GPIO_ResetOutputPin(IRQ_GPIO_Port, IRQ_Pin);
      DEBUG_NOTES(IRQ_FALL);
      break;
    }
    /* Exit after 100 ms */
    uint32_t tick_val = HAL_GetTick();
    if( tick_val - tick_start_val > 100)
    {
      header_timeout = 1;
      DEBUG_NOTES(HEADER_NOT_RECEIVED);
      
      SPI_STATE_TRANSACTION(SPI_PROT_TRANS_COMPLETE_STATE);
      LL_GPIO_ResetOutputPin(IRQ_GPIO_Port, IRQ_Pin);    /* Release SPI communication request */
      DEBUG_NOTES(IRQ_FALL);
      
      if(restore_flag)
      {
        DEBUG_NOTES(ADVANCE_DMA_RESTORE);
        event_fifo.buffer[event_fifo.head] = event_fifo_header_restore[0];
        event_fifo.buffer[event_fifo.head+1] = event_fifo_header_restore[1];
        event_fifo.buffer[event_fifo.head+2] = event_fifo_header_restore[2];
        event_fifo.buffer[event_fifo.head+3] = event_fifo_header_restore[3];
      }
      SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_STATE);  
      
      break;
    }
  }

  /* Command FIFO */
  if (fifo_size(&command_fifo) > 0)
  {
    uint16_t opcode;
    uint8_t offset;

    fifo_get_var_len_item(&command_fifo, &size, buffer);
    /*  */
    if(buffer[0] == HCI_COMMAND_PKT)
    {
      hci_cmd_hdr *hdr = (hci_cmd_hdr *)buffer;
      opcode = hdr->opcode;
      offset = sizeof(hci_cmd_hdr);
    }
    else if(buffer[0] == HCI_COMMAND_EXT_PKT)
    {
      hci_cmd_ext_hdr *hdr = (hci_cmd_ext_hdr *)buffer;
      opcode = hdr->opcode;
      offset = sizeof(hci_cmd_ext_hdr);
    }
    else 
    {
      /* Unknown packet type */
      fifo_flush(&command_fifo);
      return;
    }

    if(opcode == HCI_RESET_OPCODE)
    {
      TL_ResetReqCallback();
    }

    len=process_command(opcode, buffer+offset, size-offset, buffer_out, sizeof(buffer_out));

#if (BUFFER_CMDS_ON_BUSY == 1)
    uint8_t status_offset = (buffer_out[1] == 0x0E) ? 6 : 3; /* 0x0E: command complete, 0x0F: command status */
    /* Apply command buffering in case of CONTROLLER BUSY error with the exception of the
     * aci_l2cap_connection_parameter_update_resp command (see  req_pbs #990070)
     */
    if ((*(buffer_out+status_offset) != BLE_ERROR_CONTROLLER_BUSY) || (opcode == 0xfd82))
    {
      DEBUG_NOTES(COMMAND_PROCESSED);
      /* Set user events back to normal queue */
      send_event(buffer_out, len, 1);
      fifo_flush(&command_fifo);
    }
    else
    {
      fifo_roll_back(&command_fifo, size);
    }
#else
    DEBUG_NOTES(COMMAND_PROCESSED);
    /* Set user events back to normal queue */
    send_event(buffer_out, len, 1);
    fifo_flush(&command_fifo);
#endif
    
    BLEStack_Process_Schedule();
  }

  if(event_lost_register.event_lost==1) 
  {
    if (fifo_put_var_len_item(&event_fifo, 13, event_lost_register.event_register,0,NULL) == 0) 
    {
      event_lost_register.event_lost = 0;
      event_lost_register.event_lost_code = 0;
    }
  }
  
}

void command_received(uint8_t *cmd, uint16_t len)
{
  fifo_put_var_len_item(&command_fifo, len, cmd, 0, NULL);
  TL_ProcessReqCallback();
}

void enqueue_event(circular_fifo_t *fifo, uint16_t buff_len1, const uint8_t *buff_evt1, uint16_t buff_len2, const uint8_t *buff_evt2, int8_t overflow_index)
{
  if (fifo_put_var_len_item(fifo, buff_len1, buff_evt1, buff_len2, buff_evt2) != 0)
  {
    /* Event queue overflow */
    if ((overflow_index >=0) && (overflow_index < 64))
    {
      event_lost_register.event_lost = 1;
      event_lost_register.event_lost_code |= (1 << overflow_index);
    }
    else 
    {
      /* assert */
    }
  }
  TL_ProcessReqCallback();
}

void send_event(const uint8_t *buffer_out, uint16_t buffer_out_length, int8_t overflow_index)
{
  if(buffer_out_length != 0)
  {
    DEBUG_NOTES(ENQUEUE_EVENT);
    enqueue_event(&event_fifo, buffer_out_length, buffer_out, 0, NULL, overflow_index);
  }
}

void send_event_2buffers(const uint8_t *buffer_out1, uint16_t buffer_out_length1, const  uint8_t *buffer_out2, uint16_t buffer_out_length2, int8_t overflow_index)
{
  if(buffer_out_length1 != 0)
  {
    DEBUG_NOTES(ENQUEUE_EVENT);
    enqueue_event(&event_fifo, buffer_out_length1, buffer_out1, buffer_out_length2, buffer_out2, overflow_index);
  }
}

void advance_spi_dma(uint16_t rx_buffer_len)
{
  uint8_t spi_command;
  
  if(restore_flag) 
  {
    DEBUG_NOTES(ADVANCE_DMA_RESTORE);
    event_fifo.buffer[event_fifo.head] = event_fifo_header_restore[0];
    event_fifo.buffer[event_fifo.head+1] = event_fifo_header_restore[1];
    event_fifo.buffer[event_fifo.head+2] = event_fifo_header_restore[2];
    event_fifo.buffer[event_fifo.head+3] = event_fifo_header_restore[3];
  }
  if(rx_buffer_len>5)
  {
    /* get ctrl field from command buffer */  
    spi_command = command_fifo_buffer_tmp[0];
    
    if(spi_command == SPI_CTRL_WRITE)
    {
      DEBUG_NOTES(ADVANCE_DMA_WRITE);
      uint16_t real_len = rx_buffer_len - 5;
      hci_input(&command_fifo_buffer_tmp[5], real_len);
      command_fifo_buffer_tmp[5] = 0;
      command_fifo_buffer_tmp[0] = 0;
    }
    else if(spi_command == SPI_CTRL_READ)
    {
      DEBUG_NOTES(ADVANCE_DMA_READ);
      fifo_discard_var_len_item(&event_fifo);
      DEBUG_NOTES(ADVANCE_DMA_DISCARD);
      TL_ProcessReqCallback(); /* This is to check if there are other events to process. */
    }
  }
  SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_STATE); 
}

void DMA_Rearm(uint32_t dma_channel, uint32_t buffer, uint32_t size)
{
  /* Disable DMA Channel */
  LL_DMA_DisableChannel(DMA1, dma_channel);
  
  LL_SPI_ReceiveData16(SPI);
  LL_SPI_ReceiveData16(SPI);
  
  /* Rearm the DMA transfer */
  if (dma_channel == DMA_CH_SPI_TX)
  {
    LL_DMA_ConfigAddresses(DMA1, dma_channel, buffer, LL_SPI_DMA_GetRegAddr(SPI), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  }
  else 
  {
    LL_DMA_ConfigAddresses(DMA1, dma_channel, LL_SPI_DMA_GetRegAddr(SPI), buffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  }
  
  LL_DMA_SetDataLength(DMA1, dma_channel, size);
  
  /* Enable DMA Channel */
  LL_DMA_EnableChannel(DMA1, dma_channel);
}

void SPI_FIFO_Flush(void)
{
  LL_SPI_DeInit(SPI);
  
  __HAL_RCC_DMA_FORCE_RESET();
  __HAL_RCC_DMA_RELEASE_RESET();
  
  MX_SPI3_Init();
  
  command_fifo_dma_len = 0;
}

void TL_CSHandler(void)
{
  /* CS pin rising edge - close SPI communication
     Temporary workaround for a bug in stm32_lpm_if.c: value of CS line is checked because
     TL_CSHandler may be called either by HAL_PWR_WKUPx_Callback() or by CS_IRQHandler().
     Call to LL_GPIO_IsInputPinSet may be removed when fixed.  */
  if(LL_EXTI_IsEnabledRisingTrig(CS_EXTI_LINE) && LL_GPIO_IsInputPinSet(CS_GPIO_Port, CS_Pin) == 1)
  {
    LL_GPIO_ResetOutputPin(IRQ_GPIO_Port, IRQ_Pin);
    DEBUG_NOTES(IRQ_FALL);
    
    DEBUG_NOTES(GPIO_CS_RISING);
    LL_EXTI_DisableRisingTrig(CS_EXTI_LINE); /* Set trigger to falling. */
    
    if(SPI_STATE_FROM(SPI_PROT_CONFIGURED_EVENT_PEND_STATE))
    {      
      SPI_STATE_TRANSACTION(SPI_PROT_TRANS_COMPLETE_STATE);
      
      /* Pass the number of data received in fifo_command */
      uint16_t tmp_spi_dma_len;
      tmp_spi_dma_len = (command_fifo_dma_len - LL_DMA_GetDataLength(DMA1, DMA_CH_SPI_RX));
      advance_spi_dma(tmp_spi_dma_len);
      
      if(LL_SPI_GetTxFIFOLevel(SPI) != LL_SPI_TX_FIFO_EMPTY)
      {
        SPI_FIFO_Flush();
        DEBUG_NOTES(TXFIFO_NE);
      }
    }
  }
  /* CS pin falling edge - start SPI communication */
  else if( LL_GPIO_IsInputPinSet(CS_GPIO_Port, CS_Pin) == 0)
  {
    DEBUG_NOTES(GPIO_CS_FALLING);
    LL_EXTI_EnableRisingTrig(CS_EXTI_LINE);
    
    if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_EVENT_PEND_STATE))
    {
      SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
      TL_ProcessReqCallback();
    }
    else if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE))
    {
      SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_HOST_REQ_STATE);
      TL_ProcessReqCallback();
    }
  }
}

#if (CFG_LPM_SUPPORTED == 1)
void HAL_PWR_WKUPx_Callback(uint32_t wakeupIOs)
{  
  if (wakeupIOs & CS_WAKEUP_PIN)
  {
    TL_CSHandler();
  }
}
#endif

PowerSaveLevels TL_PowerSaveLevelCheck(void)
{ 
  if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_EVENT_PEND_STATE) || SPI_STATE_CHECK(SPI_PROT_WAITING_DATA_STATE))
  {    
    return POWER_SAVE_LEVEL_CPU_HALT;
  }  
  else 
  {
    return POWER_SAVE_LEVEL_STOP;
  }
}

__weak void TL_ProcessReqCallback(void){}

__weak void TL_ResetReqCallback(void){}

void BLE_STACK_Event(hci_pckt *hci_pckt, uint16_t length)
{
  uint16_t i;
  int ret = 0;

  if(hci_pckt->type == HCI_EVENT_PKT)
  {
    hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

    if(event_pckt->evt == EVT_LE_META_EVENT)
    {
      evt_le_meta_event *evt = (evt_le_meta_event *)event_pckt->data;

      for (i = 0; hci_le_meta_events_table[i].evt_code != 0; i++)
      {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code)
        {
          ret = hci_le_meta_events_table[i].process((void *)evt->data);
          goto send_event;
        }
      }
    }
    else if(event_pckt->evt != HCI_VENDOR_PKT)
    {
      for (i = 0; hci_events_table[i].evt_code != 0; i++)
      {
        if (event_pckt->evt == hci_events_table[i].evt_code)
        {
          ret = hci_events_table[i].process(event_pckt->data);
          goto send_event;
        }
      }
    }
  }
  if(hci_pckt->type == HCI_EVENT_PKT || hci_pckt->type == HCI_EVENT_EXT_PKT)
  {
    uint8_t   evt_code;
    uint8_t  *evt_payload;

    if(hci_pckt->type == HCI_EVENT_PKT)
    {
      hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
      evt_code = event_pckt->evt;
      evt_payload = event_pckt->data;
    }
    else
    {
      hci_event_ext_pckt *event_pckt = (hci_event_ext_pckt*)hci_pckt->data;
      evt_code = event_pckt->evt;
      evt_payload = event_pckt->data;
    }

    if(evt_code == HCI_VENDOR_PKT)
    {
      evt_blue_aci *blue_evt = (evt_blue_aci *)evt_payload;

      for (i = 0; hci_vendor_specific_events_table[i].evt_code != 0; i++)
      {
        if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code)
        {
          ret = hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
          break;
        }
      }
    }
  }

send_event:

  if(ret == 0)
  {
    send_event((uint8_t *)hci_pckt, length, -1);
  }
}

