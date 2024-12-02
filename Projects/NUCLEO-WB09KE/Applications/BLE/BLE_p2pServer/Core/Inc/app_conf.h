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
#define CFG_LSCLK_LSE                       (1)

/******************************************************************************
 * Application Config
 ******************************************************************************/
/**< generic parameters ******************************************************/
/**
 * Define Tx Power Mode
 */
#define CFG_TX_POWER_MODE                   (0) /* Tx normal power mode */

/**
 * Define Tx Power
 */
#define CFG_TX_POWER                        (0x18) /* 0x18 <=> 0 dBm */

/**
 * Define Advertising parameters
 */
#define CFG_PUBLIC_BD_ADDRESS               (0x0280E1AAABAC)
#define CFG_BD_ADDRESS_TYPE                 HCI_ADDR_STATIC_RANDOM_ADDR
#define CFG_BLE_PRIVACY_ENABLED             (0)

#define ADV_INTERVAL_MIN                    (0x0080)
#define ADV_INTERVAL_MAX                    (0x00A0)
#define ADV_LP_INTERVAL_MIN                 (0x0640)
#define ADV_LP_INTERVAL_MAX                 (0x0FA0)
#define ADV_TYPE                            (HCI_ADV_EVENT_PROP_LEGACY|HCI_ADV_EVENT_PROP_CONNECTABLE|HCI_ADV_EVENT_PROP_SCANNABLE)
#define ADV_FILTER                          HCI_ADV_FILTER_NONE

/**
 * Define IO Authentication
 */
#define CFG_BONDING_MODE                    (1)
#define CFG_FIXED_PIN                       (111111)
#define CFG_ENCRYPTION_KEY_SIZE_MAX         (16)
#define CFG_ENCRYPTION_KEY_SIZE_MIN         (8)

/**
 * Define IO capabilities
 */
#define CFG_IO_CAPABILITY                   GAP_IO_CAP_DISPLAY_YES_NO

/**
 * Define MITM modes
 */
#define CFG_MITM_PROTECTION                 GAP_MITM_PROTECTION_REQUIRED

/**
 * Define Secure Connections Support
 */
#define CFG_SC_SUPPORT                      GAP_SC_OPTIONAL

/**
 * Define Keypress Notification Support
 */
#define CFG_KEYPRESS_NOTIFICATION_SUPPORT   GAP_KEYPRESS_NOT_SUPPORTED

/**
 * Appearance of device set into BLE GAP
 */
#define CFG_GAP_APPEARANCE                  (GAP_APPEARANCE_UNKNOWN)

/* USER CODE BEGIN Generic_Parameters */

/* USER CODE END Generic_Parameters */

/**< specific parameters */
/*****************************************************/

/* USER CODE BEGIN Specific_Parameters */

/* USER CODE END Specific_Parameters */

/******************************************************************************
 * BLE Stack initialization parameters
 ******************************************************************************/

/**
 * Maximum number of simultaneous radio tasks. Radio controller supports up to
 * 128 simultaneous radio tasks, but actual usable max value depends on the
 * available RAM.
 */
#define CFG_BLE_NUM_RADIO_TASKS                         (CFG_NUM_RADIO_TASKS)

/**
 * Maximum number of Attributes that can be stored in the GATT database.
 */
#define CFG_BLE_NUM_GATT_ATTRIBUTES                     (6)

/**
 * Maximum number of concurrent Client's Procedures. This value must be less
 * than or equal to the maximum number of supported links (CFG_BLE_NUM_RADIO_TASKS).
 */
#define CFG_BLE_NUM_OF_CONCURRENT_GATT_CLIENT_PROC      (2)

/**
 * Maximum supported ATT MTU size [23-1020].
 */
#define CFG_BLE_ATT_MTU_MAX                             (247)

/**
 * Maximum duration of the connection event in system time units (625/256 us =~
 * 2.44 us) when the device is in Peripheral role [0-0xFFFFFFFF].
 */
#define CFG_BLE_CONN_EVENT_LENGTH_MAX                   (0xFFFFFFFF)

/**
 * Sleep clock accuracy (ppm).
 */
#if CFG_LSCLK_LSE
/* Change this value according to accuracy of low speed crystal (ppm). */
#define CFG_BLE_SLEEP_CLOCK_ACCURACY                    (100)
#else
/* This value should be kept to 500 ppm when using LSI. */
#define CFG_BLE_SLEEP_CLOCK_ACCURACY                    (500)
#endif

/**
 * Number of extra memory blocks, in addition to the minimum required for the
 * supported links.
 */
#define CFG_BLE_MBLOCK_COUNT_MARGIN                     (0)

/**
 * Maximum number of simultaneous EATT active channels. It must be less than or
 * equal to CFG_BLE_COC_NBR_MAX.
 */
#define CFG_BLE_NUM_EATT_CHANNELS                       (0)

/**
 * Maximum number of channels in LE Credit Based Flow Control mode [0-255].
 * This number must be greater than or equal to CFG_BLE_NUM_EATT_CHANNELS.
 */
#define CFG_BLE_COC_NBR_MAX                             (1)

/**
 * The maximum size of payload data in octets that the L2CAP layer entity is
 * capable of accepting [0-1024].
 */
#define CFG_BLE_COC_MPS_MAX                             (247)

/**
 * Maximum number of Advertising Data Sets, if Advertising Extension Feature is
 * enabled.
 */
#define CFG_BLE_NUM_ADV_SETS                            (1)

/**
 * Maximum number of Periodic Advertising with Responses subevents.
 */
#define CFG_BLE_NUM_PAWR_SUBEVENTS                      (16)

/**
 * Maximum number of subevent data that can be queued in the controller.
 */
#define CFG_BLE_PAWR_SUBEVENT_DATA_COUNT_MAX            (8U)

/**
 * Maximum number of slots for scanning on the secondary advertising channel,
 * if Advertising Extension Feature is enabled.
 */
#define CFG_BLE_NUM_AUX_SCAN_SLOTS                      (0)

/**
 * Maximum number of slots for synchronizing to a periodic advertising train,
 * if Periodic Advertising and Synchronizing Feature is enabled.
 */
#define CFG_BLE_NUM_SYNC_SLOTS                          (0)

/**
 * Two's logarithm of Filter Accept, Resolving and Advertiser list size.
 */
#define CFG_BLE_FILTER_ACCEPT_LIST_SIZE_LOG2            (3)

/**
 * Maximum number of Antenna IDs in the antenna pattern used in CTE connection
 * oriented mode.
 */
#define CFG_BLE_NUM_CTE_ANTENNA_IDS_MAX                 (0)

/**
 * Maximum number of IQ samples in the buffer used in CTE connection oriented mode.
 */
#define CFG_BLE_NUM_CTE_IQ_SAMPLES_MAX                  (0)

/**
 * Maximum number of slots for synchronizing to a Broadcast Isochronous Group.
 */
#define CFG_BLE_NUM_SYNC_BIG_MAX                        (1U)

/**
 * Maximum number of slots for synchronizing to a Broadcast Isochronous Stream.
 */
#define CFG_BLE_NUM_SYNC_BIS_MAX                        (2U)

/**
 * Maximum number of slots for broadcasting a Broadcast Isochronous Group.
 */
#define CFG_BLE_NUM_BRC_BIG_MAX                         (1U)

/**
 * Maximum number of slots for broadcasting a Broadcast Isochronous Stream.
 */
#define CFG_BLE_NUM_BRC_BIS_MAX                         (2U)

/**
 * Maximum number of Connected Isochronous Groups.
 */
#define CFG_BLE_NUM_CIG_MAX                             (2U)

/**
 * Maximum number of Connected Isochronous Streams.
 */
#define CFG_BLE_NUM_CIS_MAX                             (2U)

/**
 * Size of the internal FIFO used for critical controller events produced by the
 * ISR (e.g. rx data packets).
 */
#define CFG_BLE_ISR0_FIFO_SIZE                          (256)

/**
 * Size of the internal FIFO used for non-critical controller events produced by
 * the ISR (e.g. advertising or IQ sampling reports).
 */
#define CFG_BLE_ISR1_FIFO_SIZE                          (768)

/**
 * Size of the internal FIFO used for controller and host events produced
 * outside the ISR.
 */
#define CFG_BLE_USER_FIFO_SIZE                          (1024)

/**
 * If 1, Peripheral Preferred Connection Parameters Characteristic is added in GAP service.
 */
#define CFG_BLE_GAP_PERIPH_PREF_CONN_PARAM_CHARACTERISTIC  (0)

/**
 * If 1, Encrypted Key Material Characteristic is added in GAP service.
 */
#define CFG_BLE_GAP_ENCRYPTED_KEY_MATERIAL_CHARACTERISTIC  (0)

/**
 * Number of allocated memory blocks used for packet allocation.
 * The use of BLE_STACK_MBLOCKS_CALC macro is suggested to calculate the minimum
 * number of memory blocks for a given number of supported links and ATT MTU.
 */
#define CFG_BLE_MBLOCKS_COUNT           (BLE_STACK_MBLOCKS_CALC(CFG_BLE_ATT_MTU_MAX, CFG_BLE_NUM_RADIO_TASKS, CFG_BLE_NUM_EATT_CHANNELS) + CFG_BLE_MBLOCK_COUNT_MARGIN)

/**
 * Macro to calculate the RAM needed by the stack according the number of links,
 * memory blocks, advertising data sets and all the other initialization
 * parameters.
 */
#define BLE_DYN_ALLOC_SIZE (BLE_STACK_TOTAL_BUFFER_SIZE(CFG_BLE_NUM_RADIO_TASKS,\
                                                        CFG_BLE_NUM_EATT_CHANNELS,\
                                                        CFG_BLE_NUM_GATT_ATTRIBUTES,\
                                                        CFG_BLE_NUM_OF_CONCURRENT_GATT_CLIENT_PROC,\
                                                        CFG_BLE_MBLOCKS_COUNT,\
                                                        CFG_BLE_NUM_ADV_SETS,\
                                                        CFG_BLE_NUM_PAWR_SUBEVENTS,\
                                                        CFG_BLE_PAWR_SUBEVENT_DATA_COUNT_MAX,\
                                                        CFG_BLE_NUM_AUX_SCAN_SLOTS,\
                                                        CFG_BLE_FILTER_ACCEPT_LIST_SIZE_LOG2,\
                                                        CFG_BLE_COC_NBR_MAX,\
                                                        CFG_BLE_NUM_SYNC_SLOTS,\
                                                        CFG_BLE_NUM_CTE_ANTENNA_IDS_MAX,\
                                                        CFG_BLE_NUM_CTE_IQ_SAMPLES_MAX,\
                                                        CFG_BLE_NUM_SYNC_BIG_MAX,\
                                                        CFG_BLE_NUM_BRC_BIG_MAX,\
                                                        CFG_BLE_NUM_SYNC_BIS_MAX,\
                                                        CFG_BLE_NUM_BRC_BIS_MAX,\
                                                        CFG_BLE_NUM_CIG_MAX,\
                                                        CFG_BLE_NUM_CIS_MAX,\
                                                        CFG_BLE_ISR0_FIFO_SIZE,\
                                                        CFG_BLE_ISR1_FIFO_SIZE,\
                                                        CFG_BLE_USER_FIFO_SIZE))

/* USER CODE BEGIN BLE_Stack */

/* USER CODE END BLE_Stack */

/******************************************************************************
 * BLE Stack modularity options
 ******************************************************************************/
#define CFG_BLE_CONTROLLER_SCAN_ENABLED                   (0U)
#define CFG_BLE_CONTROLLER_PRIVACY_ENABLED                (0U)
#define CFG_BLE_SECURE_CONNECTIONS_ENABLED                (1U)
#define CFG_BLE_CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED  (0U)
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

#if CFG_BLE_PRIVACY_ENABLED
#undef CFG_BLE_CONTROLLER_PRIVACY_ENABLED
#define CFG_BLE_CONTROLLER_PRIVACY_ENABLED                (1U)
#endif

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
 * Sequencer
 ******************************************************************************/

/**
 * These are the lists of task id registered to the sequencer
 * Each task id shall be in the range [0:31]
 */
typedef enum
{
  CFG_TASK_BLE_STACK,
  CFG_TASK_VTIMER,
  CFG_TASK_NVM,
  /* USER CODE BEGIN CFG_Task_Id_t */
  TASK_BUTTON_1,
  TASK_BUTTON_2,
  TASK_BUTTON_3,
  CFG_TASK_ADV_CANCEL_ID,
  CFG_TASK_SEND_NOTIF_ID,
  /* USER CODE END CFG_Task_Id_t */
  CFG_TASK_NBR,  /**< Shall be LAST in the list */
} CFG_Task_Id_t;

/* USER CODE BEGIN DEFINE_TASK */

/* USER CODE END DEFINE_TASK */

/**
 * This is the list of priority required by the application
 * Each Id shall be in the range 0..31
 */
typedef enum
{
  CFG_SEQ_PRIO_0,
  CFG_SEQ_PRIO_1,
  /* USER CODE BEGIN CFG_SEQ_Prio_Id_t */

  /* USER CODE END CFG_SEQ_Prio_Id_t */
  CFG_SEQ_PRIO_NBR
} CFG_SEQ_Prio_Id_t;

/**
 * This is a bit mapping over 32bits listing all events id supported in the application
 */
typedef enum
{
  CFG_IDLEEVT_PROC_GAP_COMPLETE,
  /* USER CODE BEGIN CFG_IdleEvt_Id_t */

  /* USER CODE END CFG_IdleEvt_Id_t */
} CFG_IdleEvt_Id_t;

/******************************************************************************
 * RT GPIO debug module configuration
 ******************************************************************************/

#define RT_DEBUG_GPIO_MODULE         (0)

/* USER CODE BEGIN Defines */
/**
 * User interaction
 * When CFG_LED_SUPPORTED is set, LEDS are activated if requested
 * When CFG_BUTTON_SUPPORTED is set, the push button are activated if requested
 */

#define CFG_LED_SUPPORTED                       (1)
#define CFG_BUTTON_SUPPORTED                    (1)

/**
 * If CFG_FULL_LOW_POWER is requested, make sure LED and debugger are disabled
 */
#if (CFG_FULL_LOW_POWER == 1)
  #undef  CFG_LED_SUPPORTED
  #define CFG_LED_SUPPORTED      (0)
#endif /* CFG_FULL_LOW_POWER */

/* USER CODE END Defines */

#endif /*APP_CONF_H */
