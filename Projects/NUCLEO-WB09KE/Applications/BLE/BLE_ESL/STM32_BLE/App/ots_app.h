/**
  ******************************************************************************
  * @file    ots_app.h
  * @author  MCD Application Team
  * @brief   Header for ots_app.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OTS_APP_H
#define OTS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
  
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Flash area for files starts 16 KB before NVMDB (NVMDB uses 4KB) */
#define OTS_FLASH_STORAGE_START_ADDRESS   (_MEMORY_FLASH_END_ - (2 + 8) * _MEMORY_BYTES_PER_PAGE_ + 1)
#define OTS_FLASH_STORAGE_END_ADDRESS     (_MEMORY_FLASH_END_ - 2 * _MEMORY_BYTES_PER_PAGE_ + 1)
  
/* Object size: 200 x 200 pixels black and white image */
#define OBJ_ALLOC_SIZE                                                      5000
#define OBJ_HEADER_SIZE                                                        4

/* Exported functions ------------------------------------------------------- */
  
void OTS_APP_Init(void);

void OTS_APP_L2CAPChannelOpened(uint16_t conn_handle, uint16_t cid);

void OTS_APP_L2CAPChannelClosed(void);

void OTS_APP_L2CAPDataReceived(uint16_t sdu_length, uint8_t *sdu_data);

#ifdef __cplusplus
}
#endif

#endif /* OTS_APP_H */
