/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    eddystone_uid_service.h
  * @author  MCD Application Team
  * @brief   
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDDYSTONE_UID_SERVICE_H
#define EDDYSTONE_UID_SERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define ADV_UID_HANDLE (0)
/* Exported Macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
tBleStatus EddystoneUID_Init(void);
void EddystoneUID_Process(void);

#ifdef __cplusplus
}
#endif

#endif /* EDDYSTONE_UID_SERVICE_H */
