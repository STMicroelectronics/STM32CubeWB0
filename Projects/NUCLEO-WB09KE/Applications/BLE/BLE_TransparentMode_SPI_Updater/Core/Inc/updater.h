/**
  ******************************************************************************
  * @file           : Updater.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UPDATER_H
#define UPDATER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32wb0x.h"
#include "stm32wb0x_hal.h"
#include "system_stm32wb0x.h"
#include "stm32wb0x_ll_usart.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  SPI_PROT_INIT_STATE = 0,                  /* Initialization phase         */
  SPI_PROT_CONFIGURED_STATE,                /* Configuration phase          */
  SPI_PROT_SLEEP_STATE,                     /* Sleep phase                  */
  SPI_PROT_CONFIGURED_HOST_REQ_STATE,       /* Host request phase           */
  SPI_PROT_CONFIGURED_EVENT_PEND_STATE,     /* Event pending phase          */
  SPI_PROT_WAITING_HEADER_STATE,            /* Waiting header phase         */
  SPI_PROT_HEADER_RECEIVED_STATE,           /* Header received phase        */
  SPI_PROT_WAITING_DATA_STATE,              /* Waiting data phase           */
  SPI_PROT_TRANS_COMPLETE_STATE,            /* Transaction complete phase   */
} SpiProtoType;

/* Exported constants --------------------------------------------------------*/
#define BUFFER_SIZE  (264) /* 255 length + 4 ble header + 5 SPI protocol header */

/* Exported macro ------------------------------------------------------------*/
#define SPI_STATE_TRANSACTION(NEWSTATE)        spi_proto_state = NEWSTATE
#define SPI_STATE_CHECK(STATE)                (spi_proto_state==STATE)
#define SPI_STATE_FROM(STATE)                 (spi_proto_state>=STATE)

/** @brief Macro that returns a 32-bit value from a buffer where the value is stored in Little Endian Format */
#define LE_TO_HOST_32(ptr)   (uint32_t) ( ((uint32_t)*((uint8_t *)(ptr))) | \
                                           ((uint32_t)*((uint8_t *)(ptr) + 1) << 8)  | \
                                           ((uint32_t)*((uint8_t *)(ptr) + 2) << 16) | \
                                           ((uint32_t)*((uint8_t *)(ptr) + 3) << 24) )

#define BLUE_FLAG_RAM_RESET             (0x01010101)
#define BLUE_FLAG_RESET                 (0x00000000)
#define BLUE_FLAG_SET                   (0x424C5545)

#define TM_SPI_DMA_CH_TX           (LL_DMA_CHANNEL_3)       /* SPI TX DMA channels */
#define TM_SPI_DMA_CH_RX           (LL_DMA_CHANNEL_1)       /* SPI RX DMA channels */

/* Exported functions ------------------------------------------------------- */
void updater_init(void);
void updater(uint8_t reset_event);

#endif /* UPDATER_H */
