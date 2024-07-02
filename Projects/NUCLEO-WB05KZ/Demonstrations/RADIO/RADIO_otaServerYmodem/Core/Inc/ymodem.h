/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ymodem.h
  * @author  AMS VMA RF Application team
  * @version V1.0.0
  * @date    19-March-2015
  * @brief   This file provides all the software function headers of the ymodem.c
  *          file.
  ******************************************************************************
  * @attention
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __YMODEM_H_
#define __YMODEM_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#if defined(ENABLE_USB) || defined(ENABLE_UART_RX_DMA)
 #define RX_BUFFER_SIZE  (2048+8)
#else
 #define RX_BUFFER_SIZE  (128+8)
#endif
/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             500 // ms (0x100000)
#define MAX_ERRORS              (5)

#define YMODEM_ABORTED         0x01
#define YMODEM_CONTINUE        0x02
#define YMODEM_TOO_MANY_ERRORS 0x03
#define YMODEM_DONE            0x04
#define YMODEM_NO_FILE         0x05
#define YMODEM_INVALID_FILE_SIZE 0x06

/* Exported functions ------------------------------------------------------- */
void Ymodem_SendAck (void);
void Ymodem_Abort (void);
void Ymodem_Init (void);
int32_t Ymodem_Receive (uint8_t *buf, uint32_t buf_size, uint32_t *size, uint32_t packets_received);

#endif  /* __YMODEM_H_ */

/*******************(C)COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
