/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ymodem.c
  * @author  AMS VMA RF Application team
  * @version V1.0.0
  * @date    19-March-2015
  * @brief   This file provides all the software functions related to the ymodem
  *          protocol.
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

/** @addtogroup ymodem
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ymodem.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))
/* Private variables ---------------------------------------------------------*/
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint32_t rx_buffer_size = 0;
uint8_t *rx_buffer_ptr;
uint8_t *rx_buffer_tail_ptr;
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN EFP */
uint32_t Str2Int(uint8_t *inputstr, uint32_t *intnum);
/* USER CODE END EFP */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *        -1: Timeout
  */
static  int32_t Receive_Byte (uint8_t *c, uint32_t timeout)
{
  struct timer t;

  /* Minimum timeout is 1. */
  if(timeout == 0)
    timeout = 1;

  Timer_Set(&t, timeout);

  while (1)
  {
    if (rx_buffer_size > 0) {
       __disable_irq();
       *c = *rx_buffer_ptr++;
      rx_buffer_size--;
      __enable_irq();
      return 0;
    }
    if(Timer_Expired(&t)){
      break;
    }
  }
  return -1;
}

/**
  * @brief  Send a byte
  * @param  c: Character
  * @retval 0: Byte sent
  */
static uint32_t Send_Byte (uint8_t c)
{
  putchar(c);
  return 0;
}

/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  * @param  timeout
  *     0: end of transmission
  *    -1: abort by sender
  *    >0: packet length
  * @retval 0: normally return
  *        -1: timeout or packet error
  *         1: abort by user
  */
static int32_t Receive_Packet (uint8_t *data, int32_t *length, uint32_t timeout)
{
  uint16_t i, packet_size;
  uint8_t c;
  *length = 0;
  if (Receive_Byte(&c, timeout) != 0)
  {
    return -1;
  }
  switch (c)
  {
    case SOH:
      packet_size = PACKET_SIZE;
      break;
    case STX:
      packet_size = PACKET_1K_SIZE;
      break;
    case EOT:
      return 0;
    case CA:
      if ((Receive_Byte(&c, timeout) == 0) && (c == CA))
      {
        *length = -1;
        return 0;
      }
      else
      {
        return -1;
      }
    case ABORT1:
    case ABORT2:
      return 1;
    default:
      return -1;
  }
  *data = c;
  for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)
  {
    if (Receive_Byte(data + i, timeout) != 0)
    {
      return -1;
    }
  }
  if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
  {
    return -1;
  }
  *length = packet_size;
  return 0;
}


/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The integer value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, uint32_t *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}

/**
 * @brief  Abort Ymode session
 * @retval None
 */
void Ymodem_Abort (void)
{
  Send_Byte(CA);
  Send_Byte(CA);
}

/**
 * @brief  Ack received packet
 * @retval None
 */
void Ymodem_SendAck (void)
{
  Send_Byte(ACK);
}
/**
 * @brief  Receive a file using the ymodem protocol.
 * @param  buf: Address of the first byte.
 * @retval The size of the file.
 */
void Ymodem_Init (void)
{
  rx_buffer_size = 0;
}

//static uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
/**
 * @brief  Receive a file using the ymodem protocol.
 * @param  buf: Address of the buffer.
 * @param  buf_size: size of buf in bytes
 * @param  *size: file size in bytes (return parameter)
 * @param  packets_received: number of packet to be received (0 means filename)
 * @retval The size of the file.
 */
int32_t Ymodem_Receive (uint8_t *buf, uint32_t buf_size, uint32_t *size, uint32_t  packets_received)
{
  uint8_t *file_ptr, *filesize_ptr;
  int32_t i, packet_length, errors=0, return_value=YMODEM_CONTINUE, done=0;
  uint8_t *packet_data = buf;

  while (!done) {
    switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))	{
    case 0:
      errors = 0;
      switch (packet_length) {
	/* Abort by sender */
      case - 1:
	Send_Byte(ACK);
	return_value = YMODEM_ABORTED;
        done = 1;
        break;
	/* End of transmission */
      case 0:
	Send_Byte(ACK);
	return_value = YMODEM_DONE;
        done = 1;
        break;
	/* Normal packet */
      default:
	if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff)) {
	  //Send_Byte(NAK);
	}
	else {
	  if (packets_received == 0) {
	    /* Filename packet */
	    if (packet_data[PACKET_HEADER] != 0) {
	      /* Filename packet has valid data */
	      for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
        {
          /* Skip filename */
		      file_ptr++;
	      }
        /* Record start of filesize string */
        filesize_ptr = file_ptr+1;
	      for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
        {
		      file_ptr++;
	      }
	      *file_ptr = '\0'; // Add termination char */
        /* Convert to integer */
	      Str2Int(filesize_ptr, size);

	      //Send_Byte(ACK);
	      //Send_Byte(CRC16);

	    }
	    /* Filename packet is empty, end session */
	    else {
	      Send_Byte(ACK);
	      return_value = YMODEM_NO_FILE;
              done = 1;
	      break;
	    }
	  }  else {
	    /* Data packet */
            {
              int i;
              for (i = 0; i < packet_length; i++)
                buf[i] = buf[i+PACKET_HEADER];
            }
	    //memcpy(buf, packet_data + PACKET_HEADER, packet_length);
	    *size = packet_length;

	  }
	  packets_received ++;
          return_value = YMODEM_CONTINUE;
          done = 1;
	}
      }
      break;
    case 1:
      /* ABORT1 or ABORT2 */
      Send_Byte(CA);
      Send_Byte(CA);
      return_value = YMODEM_ABORTED;
      done = 1;
      break;
    default:
      if (packets_received > 0) {
	errors++;
      }
      if (errors > MAX_ERRORS) {
	Send_Byte(CA);
	Send_Byte(CA);
	return_value = YMODEM_TOO_MANY_ERRORS;
        done = 1;
      } else {
        Send_Byte(CRC16);
      }
      break;
    }
  }
  return return_value;
}

/**
 * @}
 */

/*******************(C)COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
