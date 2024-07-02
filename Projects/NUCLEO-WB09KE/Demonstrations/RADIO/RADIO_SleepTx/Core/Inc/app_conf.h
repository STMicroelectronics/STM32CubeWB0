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

/**
 * Define to 1 if LSE is used, otherwise set it to 0.
 */
#define CFG_LSCLK_LSE                           (0)

/******************************************************************************
 * Application Config
 ******************************************************************************/
/**< generic parameters ******************************************************/

/**
 * Define Tx Power Mode
 */
#define CFG_TX_POWER_MODE                 (0) /* Tx normal power mode */

/**
 * Define Tx Power
 */
#define CFG_TX_POWER                      (0x18) /* 0x18 <=> 0 dBm */
/* USER CODE BEGIN Generic_Parameters */

/* USER CODE END Generic_Parameters */

/**< specific parameters */
/*****************************************************/

/* USER CODE BEGIN Specific_Parameters */

/* USER CODE END Specific_Parameters */

/******************************************************************************
 * Low Power
 *
 *  When CFG_FULL_LOW_POWER is set to 1, the system is configured in full
 *  low power mode. It means that all what can have an impact on the consumptions
 *  are powered down.(For instance LED, Access to Debugger, Etc.)
 *
 ******************************************************************************/

#define CFG_FULL_LOW_POWER       (0)

#define CFG_LPM_SUPPORTED        (1)

/**
 * Low Power configuration
 */
#if (CFG_FULL_LOW_POWER == 1)
  #undef CFG_LPM_SUPPORTED
  #define CFG_LPM_SUPPORTED      (1)
#endif /* CFG_FULL_LOW_POWER */

/* USER CODE BEGIN Low_Power 0 */

/* USER CODE END Low_Power 0 */

/**
 * Supported requester to the MCU Low Power Manager - can be increased up  to 32
 * It list a bit mapping of all user of the Low Power Manager
 */
typedef enum
{
  CFG_LPM_APP,
  /* USER CODE BEGIN CFG_LPM_Id_t */

  /* USER CODE END CFG_LPM_Id_t */
} CFG_LPM_Id_t;

/* USER CODE BEGIN Low_Power 1 */

/* USER CODE END Low_Power 1 */


/*****************************************************************************
 * Traces
 * Enable or Disable traces in application
 * When CFG_DEBUG_TRACE is set, traces are activated
 *
 * Note : Refer to utilities_conf.h file in order to details
 *        the level of traces : CFG_DEBUG_TRACE_FULL or CFG_DEBUG_TRACE_LIGHT
 *****************************************************************************/
/**
 * Enable or disable debug prints.
 */
#define CFG_DEBUG_APP_TRACE             (1)

/**
 * Use or not advanced trace module. UART interrupts to be enabled.
 */
#define CFG_DEBUG_APP_ADV_TRACE         (0)

#define ADV_TRACE_TIMESTAMP_ENABLE      (0)

#if (CFG_DEBUG_APP_TRACE == 0)
#undef CFG_DEBUG_APP_ADV_TRACE
#define CFG_DEBUG_APP_ADV_TRACE         (0)
#endif


#if (CFG_DEBUG_APP_ADV_TRACE != 0)

#include "stm32_adv_trace.h"

#define APP_DBG(...)                                      \
{                                                                 \
  UTIL_ADV_TRACE_COND_FSend(VLEVEL_L, ~0x0, ADV_TRACE_TIMESTAMP_ENABLE, __VA_ARGS__); \
}
#else
#define APP_DBG(...) printf(__VA_ARGS__)
#endif

#if (CFG_DEBUG_APP_TRACE != 0)
#include <stdio.h>
#define APP_DBG_MSG             APP_DBG
#else
#define APP_DBG_MSG(...)
#endif

/* USER CODE BEGIN Traces */

/* USER CODE END Traces */


/******************************************************************************
 * RT GPIO debug module configuration
 ******************************************************************************/

#define RT_DEBUG_GPIO_MODULE         (0)


/* USER CODE BEGIN Defines */

/* USER CODE END Defines */

#endif /*APP_CONF_H */
