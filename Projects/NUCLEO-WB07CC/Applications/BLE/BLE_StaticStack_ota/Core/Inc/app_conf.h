/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_conf.h
  * @author  MCD Application Team
  * @brief   Application configuration file for STM32WPAN Middleware.
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
#ifndef APP_CONF_H
#define APP_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32wb0x.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/******************************************************************************
 * Application Config
 ******************************************************************************/
/**< generic parameters ******************************************************/

/* USER CODE BEGIN Generic_Parameters */

/* USER CODE END Generic_Parameters */

/**< specific parameters */
/*****************************************************/

/* USER CODE BEGIN Specific_Parameters */

/* USER CODE END Specific_Parameters */

/******************************************************************************
 * BLE Stack modularity options
 ******************************************************************************/
#define CFG_BLE_CONTROLLER_SCAN_ENABLED                   (0U)
#define CFG_BLE_CONTROLLER_PRIVACY_ENABLED                (0U)
#define CFG_BLE_SECURE_CONNECTIONS_ENABLED                (1U)
#define CFG_BLE_CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED  (1U)
#define CFG_BLE_CONTROLLER_2M_CODED_PHY_ENABLED           (1U)
#define CFG_BLE_CONTROLLER_EXT_ADV_SCAN_ENABLED           (0U)
#define CFG_BLE_L2CAP_COS_ENABLED                         (0U)
#define CFG_BLE_CONTROLLER_PERIODIC_ADV_ENABLED           (0U)
#define CFG_BLE_CONTROLLER_PERIODIC_ADV_WR_ENABLED        (0U)
#define CFG_BLE_CONTROLLER_CTE_ENABLED                    (0U)
#define CFG_BLE_CONTROLLER_POWER_CONTROL_ENABLED          (0U)
#define CFG_BLE_CONNECTION_ENABLED                        (1U)
#define CFG_BLE_CONTROLLER_CHAN_CLASS_ENABLED             (0U)
#define CFG_BLE_CONTROLLER_BIS_ENABLED                    (0U)
#define CFG_BLE_CONNECTION_SUBRATING_ENABLED              (0U)
#define CFG_BLE_CONTROLLER_CIS_ENABLED                    (0U)

/* USER CODE BEGIN Defines */

/* USER CODE END Defines */

#endif /*APP_CONF_H */
