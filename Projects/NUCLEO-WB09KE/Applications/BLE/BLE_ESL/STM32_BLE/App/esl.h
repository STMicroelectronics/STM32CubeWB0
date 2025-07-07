/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    esl.h
  * @author  MCD Application Team
  * @brief   Header for esl_service.c
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
#ifndef ESL_H
#define ESL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_status.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  ESL_SERVICE_ADDR,
  ESL_SERVICE_SYNC_KEY_MATERIAL,
  ESL_SERVICE_RESP_KEY_MATERIAL,
  ESL_SERVICE_CURR_ABS_TIME,
  ESL_SERVICE_CONTROL_POINT,
  ESL_SERVICE_SENSOR_INFO,  
  ESL_SERVICE_LED_INFO,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  ESL_SERVICE_CHAROPCODE_LAST
} ESL_SERVICE_CharOpcode_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} ESL_SERVICE_Data_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void ESL_SERVICE_Init(void);
tBleStatus ESL_SERVICE_NotifyValue(ESL_SERVICE_CharOpcode_t CharOpcode, ESL_SERVICE_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*ESL_H */
