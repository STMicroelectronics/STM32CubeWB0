/**
  ******************************************************************************
  * @file           : updater.c
  * @brief          : main file
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

/* Includes ------------------------------------------------------------------*/
#include "updater.h"
#include "updater_boot.h"
#include "transparent_mode_config.h"
#include "ble_status.h"
#include "compiler.h"
#include "stm32wb0x_ll_spi.h"
#include "stm32wb0x_ll_bus.h"
#include "stm32wb0x_ll_cortex.h"
#include "stm32wb0x_ll_rcc.h"
#include "stm32wb0x_ll_system.h"
#include "stm32wb0x_ll_utils.h"
#include "stm32wb0x_ll_pwr.h"
#include "stm32wb0x_ll_gpio.h"
#include "stm32wb0x_ll_dma.h"
#include "system_stm32wb0x.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
  WAITING_TYPE,
  WAITING_HEADER,
  WAITING_PAYLOAD
}hci_state;

typedef PACKED(struct) _hci_cmd_hdr{
  uint8_t   type;
  uint16_t	opcode;
  uint8_t	param_len;
} hci_cmd_hdr;

typedef PACKED(struct) _hci_cmd_ext_hdr{
  uint8_t   type;
  uint16_t	opcode;
  uint16_t	param_len;
} hci_cmd_ext_hdr;

typedef  PACKED(struct) devConfigS  {
  uint8_t  HS_crystal;
  uint8_t  LS_source;
  uint8_t  SMPS_management;
  uint8_t  Reserved;
  uint16_t HS_startup_time;
  uint16_t SlaveSCA;
  uint8_t  MasterSCA;
  uint32_t max_conn_event_length;
  uint8_t  Test_mode;
} devConfig_t;

/* Private define ------------------------------------------------------------*/
#ifdef UART_INTERFACE
#define CMD_BUFF_OFFSET 1
#define EVT_BUFF_OFFSET 0
#endif
#ifdef SPI_INTERFACE
#define CMD_BUFF_OFFSET 6
#define EVT_BUFF_OFFSET 5
#endif

/* SPI protocol variables & definitions */
#define SPI_HEADER_LEN          (uint8_t)(5)
#define SPI_CTRL_WRITE          (uint8_t)(0x0A)
#define SPI_CTRL_READ           (uint8_t)(0x0B)

#define UPDATER_BUF_SIZE        ((uint16_t)64)
#define UPDATER_VERSION         ((uint8_t)12)
#define BLUEFLAG_FAILED_OP      ((uint8_t)0x4A)

#define CMD_UPDATER_REBOOT              ((uint8_t)0x21)
#define CMD_UPDATER_GET_VERSION         ((uint8_t)0x22)
#define CMD_UPDATER_GET_BUFSIZE         ((uint8_t)0x23)
#define CMD_UPDATER_ERASE_BLUEFLAG      ((uint8_t)0x24)
#define CMD_UPDATER_RESET_BLUEFLAG      ((uint8_t)0x25)
#define CMD_UPDATER_ERASE_SECTOR        ((uint8_t)0x26)
#define CMD_UPDATER_PROG_DATA_BLOCK     ((uint8_t)0x27)
#define CMD_UPDATER_READ_DATA_BLOCK     ((uint8_t)0x28)
#define CMD_UPDATER_CALC_CRC            ((uint8_t)0x29)
#define CMD_UPDATER_READ_HW_VERS        ((uint8_t)0x2A)

#define COMMAND_COMPLETE_EVENT          ((uint8_t)0x0E)
#define COMMAND_STATUS_EVENT            ((uint8_t)0x0F)

#define DMA_IDLE        0
#define DMA_IN_PROGRESS 1

/* HCI Packet types */
#define HCI_COMMAND_PKT		0x01
#define HCI_ACLDATA_PKT		0x02
#define HCI_SCODATA_PKT		0x03
#define HCI_EVENT_PKT		0x04
#define HCI_COMMAND_EXT_PKT	0x81
#define HCI_EVENT_EXT_PKT	0x82
#define HCI_VENDOR_PKT		0xFF

#define HCI_TYPE_OFFSET                 0
#define HCI_VENDOR_CMDCODE_OFFSET       1
#define HCI_VENDOR_LEN_OFFSET           2
#define HCI_VENDOR_PARAM_OFFSET         4

#define LL_FLASH_FLAG_CMDDONE             FLASH_IRQRAW_CMDDONE_RIS     /*!< FLASH command done flag    */
#define LL_FLASH_FLAG_CMDSTART            FLASH_IRQRAW_CMDSTART_RIS    /*!< FLASH command started flag */
#define LL_FLASH_FLAG_CMDERR              FLASH_IRQRAW_CMDERR_RIS      /*!< FLASH command error flag   */
#define LL_FLASH_FLAG_ILLCMD              FLASH_IRQRAW_ILLCMD_RIS      /*!< FLASH illegal command flag */
#define LL_FLASH_SIZE_MASK                 FLASH_FLASH_SIZE_FLASH_SIZE
#define LL_FLASH_SIZE                      (((*(uint32_t *)FLASHSIZE_BASE & LL_FLASH_SIZE_MASK) + 1) << 2U)
#define LL_FLASH_START_ADDR                NVM_BASE
#define LL_FLASH_END_ADDR                  (LL_FLASH_START_ADDR + LL_FLASH_SIZE - 1U)

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static uint32_t updater_calc_crc(uint32_t address, uint32_t nr_of_sector);
static void DMA_Rearm(uint32_t dma_channel, uint32_t buffer, uint32_t size);


/* Private functions ---------------------------------------------------------*/

// x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 +
// x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + x^0
#define CRC_POLY        0x04C11DB7      // the poly without the x^32

static uint32_t updater_calc_crc(uint32_t address, uint32_t nr_of_sector)
{
  uint32_t nr_of_bytes;
  uint32_t i, j, a1;
  uint32_t crc;
  
  nr_of_bytes = nr_of_sector * 0x800;
  crc = 0;
  for (i = 0; i < nr_of_bytes; i += 4) {
    crc = crc ^ (*((uint32_t *)(address + i)));
    for (j = 0; j < 32; j ++) {
      a1 = (crc >> 31) & 0x1;
      crc = (crc << 1) ^ (a1 * CRC_POLY);
    }
  }
  return crc;
}


static void DMA_Rearm(uint32_t dma_channel, uint32_t buffer, uint32_t size)
{
  /* Disable DMA Channel */
  LL_DMA_DisableChannel(DMA1, dma_channel);
  
  /* Rearm the DMA transfer */
#ifdef UART_INTERFACE
  LL_DMA_ConfigAddresses(DMA1, dma_channel, buffer, LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT), LL_DMA_DIRECTION_MEMORY_TO_PERIPH); // PER UART, DA CONTROLLARE
#endif
  
#ifdef SPI_INTERFACE
  if (dma_channel == TM_SPI_DMA_CH_TX) {
    LL_DMA_ConfigAddresses(DMA1, dma_channel, buffer, LL_SPI_DMA_GetRegAddr(TM_SPI), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  }
  else {
    LL_DMA_ConfigAddresses(DMA1, dma_channel, LL_SPI_DMA_GetRegAddr(TM_SPI), buffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  }
#endif
  
  LL_DMA_SetDataLength(DMA1, dma_channel, size);
  
  /* Enable DMA Channel */
  LL_DMA_EnableChannel(DMA1, dma_channel);
  
}



/**
* @brief  updater Init.
* @param  None
* @retval None
*/
void updater_init(void)
{
#ifdef SPI_INTERFACE
  DTM_SystemInit();
  
  /* Initialize the GPIOs associated to the SPI port */
  TM_GPIO_CLK();
  
  /* SPI MISO */
  LL_GPIO_SetPinMode(TM_SPI_MISO_GPIO_PORT, TM_SPI_MISO_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TM_SPI_MISO_GPIO_PORT, TM_SPI_MISO_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TM_SPI_MISO_GPIO_PORT, TM_SPI_MISO_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  TM_SET_AF_MISO_PIN();
  
  /* SPI MOSI */
  LL_GPIO_SetPinMode(TM_SPI_MOSI_GPIO_PORT, TM_SPI_MOSI_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TM_SPI_MOSI_GPIO_PORT, TM_SPI_MOSI_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TM_SPI_MOSI_GPIO_PORT, TM_SPI_MOSI_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  TM_SET_AF_MOSI_PIN();
  
  /* SPI CLK */
  LL_GPIO_SetPinMode(TM_SPI_SCK_GPIO_PORT, TM_SPI_SCK_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TM_SPI_SCK_GPIO_PORT, TM_SPI_SCK_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TM_SPI_SCK_GPIO_PORT, TM_SPI_SCK_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  TM_SET_AF_SCK_PIN();
  
  /* SPI CS */
  LL_GPIO_SetPinMode(TM_SPI_CS_GPIO_PORT, TM_SPI_CS_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TM_SPI_CS_GPIO_PORT, TM_SPI_CS_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TM_SPI_CS_GPIO_PORT, TM_SPI_CS_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  TM_SET_AF_CS_PIN();
  
  /* SPI IRQ */
  LL_GPIO_ResetOutputPin(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN);
  LL_GPIO_SetPinMode(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinSpeed(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN, LL_GPIO_PULL_DOWN);
  HAL_PWREx_EnableGPIOPullDown(TM_SPI_IRQ_PWR_GPIO_PORT, TM_SPI_IRQ_PWR_GPIO_PORT);
  
  
  /* Initialize the SPI clock */
  TM_SPI_CLK();
  
  /* SPI parameter configuration*/
  LL_SPI_SetMode(TM_SPI, LL_SPI_MODE_SLAVE);
  LL_SPI_SetStandard(TM_SPI, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_SetTransferDirection(TM_SPI, LL_SPI_FULL_DUPLEX);
  LL_SPI_SetClockPolarity(TM_SPI, LL_SPI_POLARITY_HIGH);
  LL_SPI_SetClockPhase(TM_SPI, LL_SPI_PHASE_2EDGE);
  LL_SPI_SetNSSMode(TM_SPI, LL_SPI_NSS_HARD_INPUT);
  LL_SPI_SetDataWidth(TM_SPI, LL_SPI_DATAWIDTH_8BIT);
  LL_SPI_SetTransferBitOrder(TM_SPI, LL_SPI_MSB_FIRST);
  LL_SPI_DisableCRC(TM_SPI);
  LL_SPI_DisableNSSPulseMgt(TM_SPI);
  
  /* Configure the SPI RX FIFO threshold to 1 byte */
  LL_SPI_SetRxFIFOThreshold(TM_SPI, LL_SPI_RX_FIFO_TH_QUARTER);
  
  /* Enable the SPI */
  LL_SPI_Enable(TM_SPI);
  
  /* Initialize the DMA clock */
  __HAL_RCC_DMA_CLK_ENABLE();
  
  /* DMA parameter configuration*/
  LL_DMA_SetDataTransferDirection(DMA1, TM_SPI_DMA_CH_TX, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_SetMode(DMA1, TM_SPI_DMA_CH_TX, LL_DMA_MODE_NORMAL);
  LL_DMA_SetPeriphIncMode(DMA1, TM_SPI_DMA_CH_TX, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, TM_SPI_DMA_CH_TX, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, TM_SPI_DMA_CH_TX, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA1, TM_SPI_DMA_CH_TX, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_SetPeriphRequest(DMA1, TM_SPI_DMA_CH_TX, TM_SPI_DMA_REQ_TX);
  LL_DMA_SetChannelPriorityLevel(DMA1, TM_SPI_DMA_CH_TX, LL_DMA_PRIORITY_MEDIUM);
  
  
  LL_DMA_SetDataTransferDirection(DMA1, TM_SPI_DMA_CH_RX, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetMode(DMA1, TM_SPI_DMA_CH_RX, LL_DMA_MODE_NORMAL);
  LL_DMA_SetPeriphIncMode(DMA1, TM_SPI_DMA_CH_RX, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, TM_SPI_DMA_CH_RX, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, TM_SPI_DMA_CH_RX, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA1, TM_SPI_DMA_CH_RX, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_SetPeriphRequest(DMA1, TM_SPI_DMA_CH_RX, TM_SPI_DMA_REQ_RX);
  LL_DMA_SetChannelPriorityLevel(DMA1, TM_SPI_DMA_CH_RX, LL_DMA_PRIORITY_HIGH);
  
  /* Enable DMAReq_Tx/Rx */
  LL_SPI_EnableDMAReq_TX(TM_SPI);
  LL_SPI_EnableDMAReq_RX(TM_SPI);
  __disable_irq();
#endif
  
#ifdef UART_INTERFACE
  DTM_SystemInit();
  __HAL_RCC_USART1_CLK_ENABLE();
  
    TM_UART_GPIO_CLK();
  
  /* Configure GPIO pins for UART. */
  LL_GPIO_SetPinMode(TM_UART_TX_GPIO_PORT, TM_UART_TX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TM_UART_TX_GPIO_PORT, TM_UART_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TM_UART_TX_GPIO_PORT, TM_UART_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TM_UART_TX_GPIO_PORT, TM_UART_TX_PIN, LL_GPIO_PULL_UP);
  TM_SET_AF_UART_TX_PIN();
  
  LL_GPIO_SetPinMode(TM_UART_RX_GPIO_PORT, TM_UART_RX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TM_UART_RX_GPIO_PORT, TM_UART_RX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TM_UART_RX_GPIO_PORT, TM_UART_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TM_UART_RX_GPIO_PORT, TM_UART_RX_PIN, LL_GPIO_PULL_UP);
  TM_SET_AF_UART_RX_PIN()
  
  LL_USART_SetBaudRate(USART1, 16000000, LL_USART_PRESCALER_DIV1, LL_USART_OVERSAMPLING_16, 921600);
  LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
  LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
  LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);
  
  LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_EnableFIFO(USART1);
  
  LL_USART_ConfigAsyncMode(USART1);
  
  /* Enable UART */
  LL_USART_Enable(USART1);
  
  /* DMA controller clock enable */
  __HAL_RCC_DMA_CLK_ENABLE();
  
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_USART1_TX);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_HIGH);
  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_BYTE);
  
  /* Enable UART_DMAReq_Tx */
  LL_USART_EnableDMAReq_TX(USART1);
#endif
}


typedef uint32_t (*RomWriteFunction)(uint32_t, uint8_t*, uint32_t);
typedef uint32_t (*RomEraseFunction)(uint32_t, uint32_t);

/* Flash Controller defines ---------------------------------------------------*/
#define FLASH_WORD_LEN       4
#define FLASH_BURST_DATA     4
#define ERR_WRITE_FAILED       5
#define ERR_VERIFY_FAILED      7

uint32_t flashWrite(uint32_t address, uint8_t *data,
                    uint32_t writeLength)
{
  uint32_t index, totalLen, totalWord, totalBurstWord;
  uint8_t flash_word[4], i;
  
  // Burst Write if applicable
  LL_FLASH_EnableBurstWrite(FLASH);
  totalWord = writeLength/FLASH_WORD_LEN;
  totalBurstWord = totalWord/FLASH_BURST_DATA;
  for (index=0; index<totalBurstWord; index++) {
    // Clear the IRQ flags
    LL_FLASH_ClearFlag(FLASH, LL_FLASH_FLAG_CMDDONE|LL_FLASH_FLAG_CMDSTART|LL_FLASH_FLAG_CMDERR|LL_FLASH_FLAG_ILLCMD);
    
    // Load the flash address to write
    FLASH->ADDRESS = ((((address + (index*FLASH_WORD_LEN*FLASH_BURST_DATA)) - LL_FLASH_START_ADDR) >> 2) & LL_FLASH_SIZE_MASK);

    // Prepare and load the data to flash
    FLASH->DATA0 = LE_TO_HOST_32(data);
    data += 4;
    FLASH->DATA1 = LE_TO_HOST_32(data);
    data += 4;
    FLASH->DATA2 = LE_TO_HOST_32(data);
    data += 4;
    FLASH->DATA3 = LE_TO_HOST_32(data);
    data += 4;   
    
    // Flash Burst write command
    FLASH->COMMAND = FLASH_CMD_BURSTWRITE;
    
    // Wait the end of the flash write command
    while ((LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDDONE) == 0) && (LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDERR) == 0));
    
    // Verify if the data is written in a correct way
    if (LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDERR))
      return ERR_WRITE_FAILED;
  }
  
  // Total length multiple of FLASH_WORD_LEN
  writeLength -= totalBurstWord * FLASH_WORD_LEN*FLASH_BURST_DATA;
  totalLen = writeLength;
  if (totalLen%FLASH_WORD_LEN)
    totalLen += FLASH_WORD_LEN - (totalLen%FLASH_WORD_LEN);
  address += (totalBurstWord * FLASH_WORD_LEN*FLASH_BURST_DATA);
  
  for (index=0; index<totalLen; index+=FLASH_WORD_LEN) {
    for (i=0; i<FLASH_WORD_LEN; i++) {
      if ((index+i) < writeLength)
        flash_word[i] = data[index+i];
      else
        flash_word[i] = 0xFF;
    }
    
    // Clear the IRQ flags
    LL_FLASH_ClearFlag(FLASH, LL_FLASH_FLAG_CMDDONE|LL_FLASH_FLAG_CMDSTART|LL_FLASH_FLAG_CMDERR|LL_FLASH_FLAG_ILLCMD);
    
    // Load the flash address to write
    FLASH->ADDRESS = ((((address + index) - LL_FLASH_START_ADDR) >> 2) & LL_FLASH_SIZE_MASK);
    
    // Prepare and load the data to flash
    FLASH->DATA0 = LE_TO_HOST_32(flash_word);
    
    // Flash write command
    FLASH->COMMAND = FLASH_CMD_WRITE;
    
    // Wait the end of the flash write command
    while ((LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDDONE) == 0) && (LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDERR) == 0));
    
    // Verify if the data is written in a correct way
    if (LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDERR))
      return ERR_WRITE_FAILED;
  }
  
  return SUCCESS;
}

uint32_t flashErase(uint32_t eraseType, uint32_t address)
{
  uint8_t command = FLASH_CMD_ERASE_PAGES;
    
  // Clear the IRQ flags
  LL_FLASH_ClearFlag(FLASH, LL_FLASH_FLAG_CMDDONE|LL_FLASH_FLAG_CMDSTART|LL_FLASH_FLAG_CMDERR|LL_FLASH_FLAG_ILLCMD);
  
  // Load the flash address to erase
  FLASH->ADDRESS = (((address - LL_FLASH_START_ADDR) >> 2) & LL_FLASH_SIZE_MASK);
  
  // Flash erase command
  FLASH->COMMAND = command;
  
  // Wait the end of the flash erase command
  while ((LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDDONE) == 0) && (LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDERR) == 0));
  
  // Verify if the flash is erased
  if (LL_FLASH_GetFlag(FLASH, LL_FLASH_FLAG_CMDERR))
    return ERR_VERIFY_FAILED;

  return SUCCESS;
}



/**
* @brief  Advance transport layer state machine
* @param  None
* @retval Desired sleep level
*/
void updater(uint8_t reset_event)
{
  uint8_t event_buffer[BUFFER_SIZE];  
  uint8_t command_buffer[BUFFER_SIZE];
  volatile uint8_t command_pending = 0;
  volatile uint8_t event_pending = 0;
  volatile uint8_t reset_pending = 0;
  uint16_t event_buffer_len = 0;
  volatile uint16_t hci_pckt_len = 0;
//  uint8_t aci_header_size;
  
#ifdef SPI_INTERFACE  
  uint8_t cs_prev = SET;
  SpiProtoType spi_proto_state = SPI_PROT_CONFIGURED_STATE;
  
  event_buffer[5] = 0x04;
  event_buffer[6] = 0xFF;
  event_buffer[7] = 0x03;
  event_buffer[8] = 0x01;
  event_buffer[9] = 0x00;
  event_buffer[10] = reset_event;
#endif

#ifdef UART_INTERFACE
  hci_state state = WAITING_TYPE;    
  uint16_t collected_payload_len = 0;
  uint16_t payload_len;
  uint16_t header_len;
  
  uint8_t dma_state = DMA_IDLE;
  
  event_buffer[0] = 0x04;
  event_buffer[1] = 0xFF;
  event_buffer[2] = 0x03;
  event_buffer[3] = 0x01;
  event_buffer[4] = 0x00;
  event_buffer[5] = reset_event;
#endif
  event_buffer_len = 6;
  event_pending = 1;
  
  
  while(1) {
    /* Check reset pending */
#ifdef SPI_INTERFACE
    if (event_pending==0 && reset_pending) {
#endif

#ifdef UART_INTERFACE
    if (event_pending==0 && reset_pending && dma_state == DMA_IDLE) {
      while(LL_USART_IsActiveFlag_TXE_TXFNF(USART1) == RESET);
      while (LL_USART_IsActiveFlag_RXNE_RXFNE(USART1) != RESET);
      while(LL_USART_IsActiveFlag_TC(USART1) == RESET);
#endif
      NVIC_SystemReset();
    }

#ifdef SPI_INTERFACE
      /* CS pin falling edge - start SPI communication */
      if((LL_GPIO_IsInputPinSet(TM_SPI_CS_GPIO_PORT, TM_SPI_CS_PIN) == 0) && (cs_prev==SET)) {
        cs_prev = RESET;

        if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE)) {
          SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_HOST_REQ_STATE);
        }
      }
      /* CS pin rising edge - close SPI communication */
      else if((LL_GPIO_IsInputPinSet(TM_SPI_CS_GPIO_PORT, TM_SPI_CS_PIN) != 0) && (cs_prev==RESET)) {
        cs_prev = SET;
        
        if(SPI_STATE_FROM(SPI_PROT_CONFIGURED_EVENT_PEND_STATE)) {
          LL_GPIO_ResetOutputPin(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN);
          
          /* Pass the number of data received in fifo_command */
          uint16_t rx_buffer_len = (BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, TM_SPI_DMA_CH_RX) );
          if(rx_buffer_len>5) {
            
            /* check ctrl field from command buffer */  
            if(command_buffer[0] == SPI_CTRL_WRITE) {
              command_pending = 1;
            }
            else if(command_buffer[0] == SPI_CTRL_READ) {
              event_pending = 0;
            }
          }
          SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_STATE);  
        }
      }
      
      if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_HOST_REQ_STATE)) {
        uint8_t data[5] = {0xFF, (uint8_t)BUFFER_SIZE, (uint8_t)(BUFFER_SIZE>>8), 0, 0};      
        DMA_Rearm(TM_SPI_DMA_CH_RX, (uint32_t)command_buffer, BUFFER_SIZE);
        DMA_Rearm(TM_SPI_DMA_CH_TX, (uint32_t)data, SPI_HEADER_LEN);
        SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
        LL_GPIO_SetOutputPin(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN);
      }
      /* Event queue */
      else if (event_pending!=0 && (SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE)) ) {
        SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
        event_buffer[0] = 0xFF;
        event_buffer[1] = (uint8_t)BUFFER_SIZE;
        event_buffer[2] = (uint8_t)(BUFFER_SIZE>>8);
        event_buffer[3] = (uint8_t)event_buffer_len;
        event_buffer[4] = (uint8_t)(event_buffer_len>>8);
        
        DMA_Rearm(TM_SPI_DMA_CH_RX, (uint32_t)command_buffer, BUFFER_SIZE);
        DMA_Rearm(TM_SPI_DMA_CH_TX, (uint32_t)event_buffer, event_buffer_len+SPI_HEADER_LEN);
        
        LL_GPIO_SetOutputPin(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN);
      }
      
      while(SPI_STATE_CHECK(SPI_PROT_WAITING_HEADER_STATE)) {
        volatile uint16_t tmp_spi_dma_len = LL_DMA_GetDataLength(DMA1, TM_SPI_DMA_CH_RX);
        tmp_spi_dma_len = BUFFER_SIZE - tmp_spi_dma_len;
        if( tmp_spi_dma_len >4) {
          LL_GPIO_ResetOutputPin(TM_SPI_IRQ_GPIO_PORT, TM_SPI_IRQ_PIN);
          SPI_STATE_TRANSACTION(SPI_PROT_WAITING_DATA_STATE);
          break;
        }
      }
#endif

#ifdef UART_INTERFACE
      while(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1) != RESET) {
        /* The Interrupt flag is cleared from the FIFO manager */    
        uint8_t byte = LL_USART_ReceiveData8(USART1);
        
        if(state == WAITING_TYPE)
          hci_pckt_len = 0;
        
        command_buffer[hci_pckt_len++] = byte;        
        
        if(state == WAITING_TYPE){
          
          state = WAITING_HEADER;
          
          if(byte == HCI_COMMAND_PKT){
            header_len = 4;
          }
          else if(byte == HCI_COMMAND_EXT_PKT){
            header_len = 5;
          }
          else {
            state = WAITING_TYPE;        
          }
        }
        else if(state == WAITING_HEADER){
          if(hci_pckt_len == header_len){
            // The entire header has been received
            uint8_t pckt_type = command_buffer[0];
            collected_payload_len = 0;
            payload_len = 0;
            
            if(pckt_type == HCI_COMMAND_PKT){
              hci_cmd_hdr *hdr = (hci_cmd_hdr *)command_buffer;
              payload_len = hdr->param_len;
            }
            else if(pckt_type == HCI_COMMAND_EXT_PKT){
              hci_cmd_ext_hdr *hdr = (hci_cmd_ext_hdr *)command_buffer;
              payload_len = hdr->param_len;
            }
            if(payload_len == 0){
              state = WAITING_TYPE;
              command_pending = 1;
            }
            else if(payload_len > 255){
              state = WAITING_TYPE; // Packet is too big                        
            }
            else {
              state = WAITING_PAYLOAD;                      
            }
          }      
        }
        else if(state == WAITING_PAYLOAD){
          collected_payload_len++;
          if(collected_payload_len >= payload_len){
            state = WAITING_TYPE;
            command_pending = 1;
          }      
        }
      }
#endif

      /* Command FIFO */
      if (command_pending==1 && (!reset_pending)) {
        RomWriteFunction romWriteFun = flashWrite;
        RomEraseFunction romEraseFun = flashErase;

        /* Set user events to temporary queue */
        uint8_t * buff = command_buffer+CMD_BUFF_OFFSET;
        uint8_t * params;
        
        if(command_buffer[CMD_BUFF_OFFSET-1] == HCI_COMMAND_PKT) {
          params = buff;
        }
        else {
          params = buff + 1;
        }
        
        event_buffer[EVT_BUFF_OFFSET+0] = 0x04;     // HCI_EVENT
        event_buffer[EVT_BUFF_OFFSET+1] = COMMAND_COMPLETE_EVENT;
        event_buffer[EVT_BUFF_OFFSET+2] = 0x04;     // number of bytes in the event
        event_buffer[EVT_BUFF_OFFSET+3] = 0x01;     // 1 HCI_CMD_SLOT 
        event_buffer[EVT_BUFF_OFFSET+4] = buff[0];     // OPCODE_LB
        event_buffer[EVT_BUFF_OFFSET+5] = buff[1];     // OPCODE_HB
        event_buffer[EVT_BUFF_OFFSET+6] = 0x00;     // reason code: command successful
        
        event_pending = 1;
        
        if(buff[1] == 0xFC) {
          uint32_t word_tmp;
          
          if(buff[2]>=4) {
            /* Get the address */
            word_tmp  = ((uint32_t)params[3]);
            word_tmp |= (((uint32_t)params[4]) << 8);
            word_tmp |= (((uint32_t)params[5]) << 16);
            word_tmp |= (((uint32_t)params[6]) << 24);
          }
          
          switch(buff[0]) {
          case CMD_UPDATER_REBOOT:
            reset_pending = 1;
            break;
            
          case CMD_UPDATER_GET_VERSION:
            event_buffer[EVT_BUFF_OFFSET+2] = 0x05;             // overwrite return data length
            event_buffer[EVT_BUFF_OFFSET+7] = UPDATER_VERSION;
            break;
            
          case CMD_UPDATER_GET_BUFSIZE:
            event_buffer[EVT_BUFF_OFFSET+2] = 0x06;             // overwrite return data length
            event_buffer[EVT_BUFF_OFFSET+7] = (uint8_t)(UPDATER_BUF_SIZE);
            event_buffer[EVT_BUFF_OFFSET+8] = (uint8_t)(UPDATER_BUF_SIZE >> 8);
            break;
            
          case CMD_UPDATER_ERASE_BLUEFLAG:
            word_tmp = BLUE_FLAG_RESET;
            if(romWriteFun(BLUE_FLAG_FLASH_BASE_ADDRESS, (uint8_t *)&word_tmp, 4) == 0) {
              event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
            }
            else {
              event_buffer[EVT_BUFF_OFFSET+6] = BLUEFLAG_FAILED_OP;
            }
            break;
          case CMD_UPDATER_RESET_BLUEFLAG:
            /* Write the blue flag */
            word_tmp = BLUE_FLAG_SET;
            if(romWriteFun(BLUE_FLAG_FLASH_BASE_ADDRESS, (uint8_t *)&word_tmp, 4) == 0) {
              event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
            }
            else {
              event_buffer[EVT_BUFF_OFFSET+6] = BLUEFLAG_FAILED_OP;
            }
            break;
            
          case CMD_UPDATER_READ_HW_VERS:
            event_buffer[EVT_BUFF_OFFSET+2] = 0x05;
            event_buffer[EVT_BUFF_OFFSET+7] = (uint8_t)(SYSCFG->DIE_ID);
            break;
            
          case CMD_UPDATER_ERASE_SECTOR:
            
            /* Verify the address is inside the Flash range and avoid the first page used by the updater itself */
            event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_NVM_ERASE_FAILED;
            if(word_tmp>=DTM_APP_ADDR && (word_tmp&0x3FF) == 0) {
              
              if(romEraseFun(0x02, word_tmp)==0) {
                event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
              }          
            }          
            break;
            
          case CMD_UPDATER_PROG_DATA_BLOCK:
            /* Verify the address is inside the Flash range */
            event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_NVM_WRITE_FAILED;
            if(word_tmp>=DTM_APP_ADDR) {
              
              uint32_t length;            
              uint32_t length_a, length_b;
              length = (uint16_t)params[7];
              length |= ((uint16_t)params[8])<<8;
              
              uint8_t misalign_word;
              misalign_word = word_tmp & 3;
              
              if(misalign_word) {
                word_tmp -= misalign_word; // move back the address to align to a word
                length += misalign_word;   // increase the number of byte to write
                
                for(uint8_t i=0;i<misalign_word;i++) {
                  params[9-misalign_word+i] = *(uint8_t *)(word_tmp+i);
                }
              }            
              length_b = length & 0x3;
              length_a = length & ~0x3;
              
              if(length_a) {              
                if(romWriteFun(word_tmp, params + 9-misalign_word, length_a) == 0) {
                  event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
                  word_tmp += length_a;                
                }
                else {
                  break;
                }
              }
              
              if(length_b) {
                /*read and write */
                for(uint8_t i=0;i<4;i++) {
                  if(i>length_b)
                    params[9-misalign_word+length_a+i] = *(uint8_t *)word_tmp;
                }
                /* word write */
                if(romWriteFun(word_tmp, params + 9-misalign_word+length_a, 4) == 0) {
                  event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;                
                }
                else {
                  event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_NVM_WRITE_FAILED;
                }
              }
            }
            break;
            
          case CMD_UPDATER_READ_DATA_BLOCK:
            
            /* Verify the address is inside the Flash range */
            if(word_tmp>=DTM_APP_ADDR) {
              
              uint32_t length;
              length = (uint16_t)params[7];
              length |= (((uint16_t)params[8])<<8);
              
              for (uint32_t i = 0; i < length; i ++) {
                event_buffer[EVT_BUFF_OFFSET+7+i] = *((uint8_t *)word_tmp);
                word_tmp++;
              }
              event_buffer[EVT_BUFF_OFFSET+2] = 0x04 + length;
              event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
            }
            else {
              event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_NVM_READ_FAILED;
            }
            break;
            
          case CMD_UPDATER_CALC_CRC:
            event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_NVM_READ_FAILED;
            event_buffer[EVT_BUFF_OFFSET+2] = 0x08;
            event_buffer[EVT_BUFF_OFFSET+7] = 0xFF;
            event_buffer[EVT_BUFF_OFFSET+8] = 0xFF;
            event_buffer[EVT_BUFF_OFFSET+9] = 0xFF;
            event_buffer[EVT_BUFF_OFFSET+10] = 0xFF;

            //          if(word_tmp>=DTM_APP_ADDR && word_tmp<=_MEMORY_FLASH_END_) {
            if(word_tmp>=DTM_APP_ADDR) {
              event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
              
              uint32_t crc;
              uint32_t length;            
              length = (uint32_t)params[7];            
              crc = updater_calc_crc(word_tmp, length);
              
              event_buffer[EVT_BUFF_OFFSET+7] = (uint8_t)crc;
              event_buffer[EVT_BUFF_OFFSET+8] = (uint8_t)(crc >> 8);
              event_buffer[EVT_BUFF_OFFSET+9] = (uint8_t)(crc >> 16);
              event_buffer[EVT_BUFF_OFFSET+10] =(uint8_t)(crc >> 24);
            }
            break;
            
          default:
            event_buffer[EVT_BUFF_OFFSET+1] = COMMAND_STATUS_EVENT;          
            event_buffer[EVT_BUFF_OFFSET+3] = BLE_ERROR_UNKNOWN_HCI_COMMAND;
            event_buffer[EVT_BUFF_OFFSET+4] = 0x01;     // 1 HCI_CMD_SLOT 
            event_buffer[EVT_BUFF_OFFSET+5] = buff[0];     // OPCODE_LB
            event_buffer[EVT_BUFF_OFFSET+6] = buff[1];     // OPCODE_HB
            break;
          }
        }
        else {
          event_buffer[EVT_BUFF_OFFSET+1] = COMMAND_STATUS_EVENT;          
          event_buffer[EVT_BUFF_OFFSET+3] = BLE_ERROR_UNKNOWN_HCI_COMMAND;
          event_buffer[EVT_BUFF_OFFSET+4] = 0x01;     // 1 HCI_CMD_SLOT 
          event_buffer[EVT_BUFF_OFFSET+5] = buff[0];     // OPCODE_LB
          event_buffer[EVT_BUFF_OFFSET+6] = buff[1];     // OPCODE_HB
        }
        
        if(event_pending == 1) {
          if(event_buffer[EVT_BUFF_OFFSET+2]>4)
            event_buffer_len = (7 + event_buffer[EVT_BUFF_OFFSET+2] - 4);
          else
            event_buffer_len = 7;
        }
        command_pending = 0;
      }
      
#ifdef UART_INTERFACE
      /* Event queue */
      if (event_pending!=0 && (dma_state == DMA_IDLE)) {
        if (dma_state == DMA_IDLE) {
          dma_state = DMA_IN_PROGRESS;
          DMA_Rearm(LL_DMA_CHANNEL_1, (uint32_t)event_buffer, event_buffer_len);
          event_pending = 0;
        }
      }

      if(LL_DMA_IsActiveFlag_TC1(DMA1) != RESET) {
        //      DMA->IFCR = DMA_FLAG_TC_UART_TX;
        LL_DMA_ClearFlag_TC1(DMA1);
        dma_state = DMA_IDLE;
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
      }
#endif
    }
  }
 