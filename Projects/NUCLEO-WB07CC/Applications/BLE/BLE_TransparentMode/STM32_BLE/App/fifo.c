/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fifo.c
  * @author  VMA RF Application Team
  * @version V1.0.0
  * @date    July-2015
  * @brief   FIFO management sources
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
#include "fifo.h"
#include "osal.h"

#define MIN(a, b) ((a <= b) ? (a) : (b))
#define MAX(a, b) ((a < b) ? (b) : (a))
#define FIFO_ALIGN(value, alignment) ((value + alignment - 1) & ~(alignment - 1))
#define ADVANCE_QUEUE(index, size, max_size) ((index + size) % max_size) /* (((index + size) >= max_size) ? 0 : (index + size))*/
#define ROLLBACK_QUEUE(index, size, max_size) ((index)>=(size)?((index) - (size)):((max_size)+(index)-(size)))
#define FIFO_ALIGNMENT fifo->alignment
#define FIFO_GET_SIZE(fifo) ((fifo->tail>=fifo->head) ? (fifo->tail - fifo->head) : (fifo->max_size - (fifo->head - fifo->tail)))
#define VAR_LEN_ITEM_SIZE_LENGTH 2

/**
* @brief  Initialize a circular fifo specfiyng also elements alignment
* The buffer allocated memory should max_size+maximum length of element,
* so that no wrapping occurs and each element is made of only linear buffer segments.
* @retval None
*/

void fifo_init(circular_fifo_t *fifo, uint16_t max_size, uint8_t  *buffer, uint8_t alignment)
{
  fifo->tail = fifo->head = 0;
  fifo->max_size = max_size;
  fifo->buffer = buffer;
  fifo->alignment = alignment;
}
/**
* @brief  Return number of bytes held in the FIFO
* @retval None
*/

uint16_t fifo_size(circular_fifo_t *fifo)
{
  return FIFO_GET_SIZE(fifo);
}
void fifo_flush(circular_fifo_t *fifo)
{
  fifo->tail = fifo->head = 0;
}

static uint8_t _fifo_put(circular_fifo_t *fifo, uint16_t size, const uint8_t  *buffer, uint16_t index)
{
  uint16_t size_aligned = FIFO_ALIGN(size, FIFO_ALIGNMENT);
  if ((FIFO_GET_SIZE(fifo) + size_aligned) < fifo->max_size) { /* <= */
    Osal_MemCpy(&fifo->buffer[index], buffer, size);
    fifo->tail = ADVANCE_QUEUE(fifo->tail, size_aligned, fifo->max_size);
    return 0;
  }
  return 1;
}

//static uint8_t fifo_put_no_wrap(circular_fifo_t *fifo, uint16_t size, uint8_t  *buffer)
//{
//  uint16_t index = (fifo->tail == 0) ? fifo->max_size : fifo->tail;
//  return (_fifo_put(fifo, size, buffer, index));
//}

/**
* @brief  Put the buffer in the fifo (no wrapping)
* @retval None
*/
uint8_t fifo_put(circular_fifo_t *fifo, uint16_t size, const uint8_t  *buffer)
{
  return (_fifo_put(fifo, size, buffer, fifo->tail));
}

static uint8_t _fifo_get(circular_fifo_t *fifo, uint16_t size, uint8_t  *buffer, uint16_t index)
{
  uint16_t size_aligned = FIFO_ALIGN(size, FIFO_ALIGNMENT);
  if (FIFO_GET_SIZE(fifo) >= size_aligned) {
    Osal_MemCpy(buffer, &fifo->buffer[index], size);
    fifo->head = ADVANCE_QUEUE(fifo->head, size_aligned, fifo->max_size);

    return 0;
  }
  return 1;
}

static uint8_t fifo_get_no_wrap(circular_fifo_t *fifo, uint16_t size, uint8_t  *buffer)
{
  uint16_t index = (fifo->head == 0) ? fifo->max_size : fifo->head;
  return (_fifo_get(fifo, size, buffer, index));
}

uint8_t fifo_get(circular_fifo_t *fifo, uint16_t size, uint8_t  *buffer)
{
  return (_fifo_get(fifo, size, buffer, fifo->head));
}

void fifo_roll_back(circular_fifo_t *fifo, uint16_t size)
{
      uint16_t size_aligned = FIFO_ALIGN(size, FIFO_ALIGNMENT);
      uint16_t var_le_item_size_aligned = FIFO_ALIGN(VAR_LEN_ITEM_SIZE_LENGTH, FIFO_ALIGNMENT);
      fifo->head = ROLLBACK_QUEUE(fifo->head, size_aligned+var_le_item_size_aligned, fifo->max_size);
}

uint8_t fifo_put_var_len_item(circular_fifo_t *fifo, uint16_t size1, const uint8_t  *buffer1, uint16_t size2, const uint8_t  *buffer2)
{
  uint8_t ret_val = 0;
  uint16_t size = size1 + size2;
  uint16_t size_aligned = FIFO_ALIGN(size, FIFO_ALIGNMENT);
  uint8_t  length_size_aligned = FIFO_ALIGN(VAR_LEN_ITEM_SIZE_LENGTH, FIFO_ALIGNMENT);

  if ((FIFO_GET_SIZE(fifo) + size_aligned + length_size_aligned) < fifo->max_size) {
    Osal_MemCpy(&fifo->buffer[fifo->tail], &size, VAR_LEN_ITEM_SIZE_LENGTH);

    Osal_MemCpy(&fifo->buffer[fifo->tail + length_size_aligned], buffer1, size1);
    Osal_MemCpy(&fifo->buffer[fifo->tail + length_size_aligned + size1], buffer2, size2);

    fifo->tail = ADVANCE_QUEUE(fifo->tail, size_aligned + length_size_aligned, fifo->max_size);

  } else {
    ret_val = 1;
  }
  return ret_val;
}

uint8_t fifo_discard(circular_fifo_t *fifo, uint16_t size)
{
  uint16_t size_aligned = FIFO_ALIGN(size, FIFO_ALIGNMENT);
  if (FIFO_GET_SIZE(fifo) >= size_aligned) {
    fifo->head = ADVANCE_QUEUE(fifo->head, size_aligned, fifo->max_size);
    return 0;
  }
  return 1;
}

uint8_t fifo_get_ptr(circular_fifo_t *fifo, uint16_t size, uint8_t **ptr)
{
  uint16_t size_aligned = FIFO_ALIGN(size, FIFO_ALIGNMENT);
  if (FIFO_GET_SIZE(fifo) >= size_aligned) {
    *ptr = &fifo->buffer[fifo->head];
    return 0;
  }
  return 1;
}

uint8_t fifo_get_var_len_item(circular_fifo_t *fifo, uint16_t *size, uint8_t  *buffer)
{
  uint8_t ret_val = fifo_get(fifo, VAR_LEN_ITEM_SIZE_LENGTH, (uint8_t *) size);
  if (ret_val == 0) {
    ret_val = fifo_get_no_wrap(fifo, *size, buffer);
  }
  return ret_val;
}

uint8_t fifo_get_ptr_var_len_item(circular_fifo_t *fifo, uint16_t *size, uint8_t  **ptr)
{
  uint8_t ret_val = fifo_get_ptr(fifo, VAR_LEN_ITEM_SIZE_LENGTH, ptr);
  if (ret_val == 0)  {
    *size = *((uint16_t *) *ptr);
  }
  return ret_val;
}
uint8_t fifo_discard_var_len_item(circular_fifo_t *fifo)
{
  uint16_t size;
  uint8_t ret_val = fifo_get(fifo, VAR_LEN_ITEM_SIZE_LENGTH, (uint8_t *) &size);
  if (ret_val == 0) {
    ret_val = fifo_discard(fifo, size);
  }
  return ret_val;
}
