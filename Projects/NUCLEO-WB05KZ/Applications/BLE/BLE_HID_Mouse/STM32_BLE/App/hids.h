/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HIDS.h
  * @author  MCD Application Team
  * @brief   Header for HIDS.c
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
#ifndef HIDS_H
#define HIDS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ble_status.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */
#define EXTERNAL_REPORT_NUMBER 1
#define INCLUDED_SERVICE_NUMBER 1
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  HIDS_HII,
  HIDS_HCP,
  HIDS_REM,
  HIDS_INPUTREP,

  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */

  HIDS_CHAROPCODE_LAST
} HIDS_CharOpcode_t;

typedef enum
{
  HIDS_HII_READ_EVT,
  HIDS_HCP_WRITE_NO_RESP_EVT,
  HIDS_REM_READ_EVT,
  HIDS_INPUTREP_READ_EVT,
  HIDS_INPUTREP_WRITE_EVT,
  HIDS_INPUTREP_NOTIFY_ENABLED_EVT,
  HIDS_INPUTREP_NOTIFY_DISABLED_EVT,

  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */

  HIDS_BOOT_REQUEST_EVT
} HIDS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */

} HIDS_Data_t;

typedef struct
{
  HIDS_OpcodeEvt_t       EvtOpcode;
  HIDS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;

  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */

} HIDS_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef struct reportS {
  /* Report ID */
  uint8_t ID;
  /* Report Type: 0x01 Input, 0x02 Output, 0x03 Feature */
  uint8_t type;
} report_Type;

typedef struct includeSerS {
  /* Service UUID to be included */
  uint16_t uuid;
  /* Start handle of the service to be included.
   *  If the service to include is BATTERY Service
   *  This data is not required
   */
  uint16_t start_handle;
  /* End handle of the service to be included.
   *  If the service to include is BATTERY Service
   *  This data is not required
   */
  uint16_t end_handle;
} includeSer_Type;

/* HID Service Specification  */
typedef struct hidServiceS {
  /* TRUE if the device supports the 
   *  boot protocol mode. FALSE otherwise
   */
  uint8_t bootSupport;
  /* Report Descriptor length */
  uint8_t reportDescLen;
  /* Report Descriptor */
  uint8_t *reportDesc;
  /* TRUE if the device supports the 
   *  Report characteristic. FALSE otherwise
   */
  uint8_t reportSupport;
  /* Number of report characteristics. */
  uint8_t num_reports;
  /* Pointer to an array describing the Report Reference Descriptors */
  report_Type *reportReferenceDesc;
  /* TRUE if the HID device is a keyboard that 
   *  supports boot protocol mode.
   *  FALSE otherwise.
   */
  uint8_t isBootDevKeyboard;
  /* TRUE if the HID device is a mouse that supports
   *  boot protocol mode.
   *  FALSE otherwise.
   */
  uint8_t isBootDevMouse;
  /* TRUE if the HID Service has external report.
   *  FALSE otherwise.
   */
  uint8_t externalReportEnabled;
  /* External reports UUID 
   *  referenced in report Map
   */
  uint16_t externalReport[EXTERNAL_REPORT_NUMBER];
  /* TRUE if the HID service has included services.
   *  FALSE otherwise.
   */
  uint8_t includedServiceEnabled;
  /* Included services UUID */
  includeSer_Type includedService[INCLUDED_SERVICE_NUMBER];
  /* HID Information Characteristic value The format is:
   * - 2 bytes USB HID specification implemented by HID Device
   * - 1 byte Country Code
   * - 1 byte Bit0: RemoteWake, Bit1: Normally Connectable
   */
  uint8_t informationCharac[4];
} hidService_Type;

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
void HIDS_Init(void);
void HIDS_Notification(HIDS_NotificationEvt_t *p_Notification);
tBleStatus HIDS_UpdateValue(HIDS_CharOpcode_t CharOpcode, HIDS_Data_t *pData);
tBleStatus HIDS_NotifyValue(HIDS_CharOpcode_t CharOpcode, HIDS_Data_t *pData, uint16_t ConnectionHandle);
/* USER CODE BEGIN EF */

/* USER CODE END EF */

#ifdef __cplusplus
}
#endif

#endif /*HIDS_H */
