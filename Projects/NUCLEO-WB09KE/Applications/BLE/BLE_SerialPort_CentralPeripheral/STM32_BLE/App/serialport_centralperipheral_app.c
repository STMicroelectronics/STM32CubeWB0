/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SerialPort_CentralPeripheral_app.c
  * @author  MCD Application Team
  * @brief   SerialPort_CentralPeripheral_app application definition.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "app_ble.h"
#include "ble.h"
#include "serialport_centralperipheral_app.h"
#include "serialport_centralperipheral.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gatt_client_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Tx_NOTIFICATION_OFF,
  Tx_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  SERIALPORT_CENTRALPERIPHERAL_APP_SENDINFORMATION_LAST
} SERIALPORT_CENTRALPERIPHERAL_APP_SendInformation_t;

typedef struct
{
  SERIALPORT_CENTRALPERIPHERAL_APP_SendInformation_t     Tx_Notification_Status;
  uint16_t SERIALPORT_CENTRALPERIPHERAL_periph_connHdl;
  /* USER CODE BEGIN Service1_APP_Context_t */

  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} SERIALPORT_CENTRALPERIPHERAL_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFUARTRX_SIZE         20
#define BUFFSPTX_SIZE           BUFFUARTRX_SIZE
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static SERIALPORT_CENTRALPERIPHERAL_APP_Context_t SERIALPORT_CENTRALPERIPHERAL_APP_Context;

uint8_t a_SERIALPORT_CENTRALPERIPHERAL_UpdateCharData[247];

/* USER CODE BEGIN PV */
static uint8_t a_buffUartRx[BUFFUARTRX_SIZE];
static volatile uint8_t buffUartRxIndex;
static uint8_t a_buffSpTx[BUFFSPTX_SIZE];
static volatile uint8_t buffSpTxLen;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SERIALPORT_CENTRALPERIPHERAL_Tx_SendNotification(void);

/* USER CODE BEGIN PFP */
static void SERIALPORT_SERVER_APP_Terminal_Init(void);
void Send_Data_Over_BLE(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void SERIALPORT_CENTRALPERIPHERAL_Notification(SERIALPORT_CENTRALPERIPHERAL_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case SERIALPORT_CENTRALPERIPHERAL_TX_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_ENABLED_EVT */
      SERIALPORT_CENTRALPERIPHERAL_APP_Context.Tx_Notification_Status = Tx_NOTIFICATION_ON;
      APP_DBG_MSG("-- APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n");
      /* USER CODE END Service1Char1_NOTIFY_ENABLED_EVT */
      break;

    case SERIALPORT_CENTRALPERIPHERAL_TX_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_DISABLED_EVT */
      SERIALPORT_CENTRALPERIPHERAL_APP_Context.Tx_Notification_Status = Tx_NOTIFICATION_OFF;
      APP_DBG_MSG("-- APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
      APP_DBG_MSG(" \n");
      /* USER CODE END Service1Char1_NOTIFY_DISABLED_EVT */
      break;

    case SERIALPORT_CENTRALPERIPHERAL_RX_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char2_WRITE_NO_RESP_EVT */

      /* USER CODE END Service1Char2_WRITE_NO_RESP_EVT */
      break;

    case SERIALPORT_CENTRALPERIPHERAL_RX_WRITE_EVT:
      /* USER CODE BEGIN Service1Char2_WRITE_EVT */

      /* USER CODE END Service1Char2_WRITE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_Notification_default */

      /* USER CODE END Service1_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service1_Notification_2 */

  /* USER CODE END Service1_Notification_2 */
  return;
}

void SERIALPORT_CENTRALPERIPHERAL_APP_EvtRx(SERIALPORT_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case SERIALPORT_CENTRALPERIPHERAL_CENTR_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CENTR_CONN_HANDLE_EVT */
      SERIALPORT_CENTRALPERIPHERAL_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;

      
      /* USER CODE END Service1_APP_CENTR_CONN_HANDLE_EVT */
      break;

    case SERIALPORT_CENTRALPERIPHERAL_PERIPH_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_PERIPH_CONN_HANDLE_EVT */
      SERIALPORT_CENTRALPERIPHERAL_APP_Context.SERIALPORT_CENTRALPERIPHERAL_periph_connHdl = p_Notification->ConnectionHandle;

      /* USER CODE END Service1_APP_PERIPH_CONN_HANDLE_EVT */
      break;

    case SERIALPORT_CENTRALPERIPHERAL_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      SERIALPORT_CENTRALPERIPHERAL_APP_Context.ConnectionHandle = 0xFFFF;
      SERIALPORT_CENTRALPERIPHERAL_APP_Context.SERIALPORT_CENTRALPERIPHERAL_periph_connHdl = 0xFFFF;

      /* USER CODE END Service1_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_APP_EvtRx_default */

      /* USER CODE END Service1_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service1_APP_EvtRx_2 */

  /* USER CODE END Service1_APP_EvtRx_2 */

  return;
}

void SERIALPORT_CENTRALPERIPHERAL_APP_Init(void)
{
  SERIALPORT_CENTRALPERIPHERAL_Init();

  /* USER CODE BEGIN Service1_APP_Init */

  SERIALPORT_CENTRALPERIPHERAL_APP_Context.ConnectionHandle = 0xFFFF;
  SERIALPORT_CENTRALPERIPHERAL_APP_Context.SERIALPORT_CENTRALPERIPHERAL_periph_connHdl = 0xFFFF;
  
  SERIALPORT_CENTRALPERIPHERAL_APP_Context.Tx_Notification_Status= Tx_NOTIFICATION_OFF;
  
  UTIL_SEQ_RegTask(1U << CFG_TASK_SERIALPORT_TX_REQ_ID, UTIL_SEQ_RFU, Send_Data_Over_BLE);
  
  SERIALPORT_SERVER_APP_Terminal_Init();
  
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
/**
  * @brief  This function initialize terminal
  * @param  None
  * @retval None
  */
static void SERIALPORT_SERVER_APP_Terminal_Init(void)
{ 

  return;
}

void UartRxCpltCallback(uint8_t * pdata, uint16_t size)
{
 
  uint8_t byte_received;
    
  if(size == 1)
  {
    byte_received = pdata[0];
        
    if(buffUartRxIndex < sizeof(a_buffUartRx))
    {
      a_buffUartRx[buffUartRxIndex++] = byte_received;
    }
    else
    {
      buffUartRxIndex = 0;
    }
    
    if( (byte_received == '\n') || (buffUartRxIndex >= sizeof(a_buffUartRx)))
    {
      memcpy(&a_buffSpTx[0], &a_buffUartRx[0], buffUartRxIndex);
      buffSpTxLen = buffUartRxIndex;
      buffUartRxIndex = 0;
      
      if(SERIALPORT_CENTRALPERIPHERAL_APP_Context.SERIALPORT_CENTRALPERIPHERAL_periph_connHdl != 0xFFFF)
      {
        /* Peripheral */
        UTIL_SEQ_SetTask(1U << CFG_TASK_SERIALPORT_TX_REQ_ID, CFG_SEQ_PRIO_0);
      }
      else
      {        
        if(SERIALPORT_CENTRALPERIPHERAL_APP_Context.ConnectionHandle != 0xFFFF)
        {
          /* Central */
          SerialPort_Client_Write_Char__set_params(a_buffSpTx, buffSpTxLen);
          UTIL_SEQ_SetTask(1 << CFG_TASK_WRITE_RX_CHAR_ID, CFG_SEQ_PRIO_0);
        }
      }
    }    
  }
  else
  {
    
  }
  return; 
}


/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void SERIALPORT_CENTRALPERIPHERAL_Tx_SendNotification(void) /* Property Notification */
{
  SERIALPORT_CENTRALPERIPHERAL_APP_SendInformation_t notification_on_off = Tx_NOTIFICATION_OFF;
  SERIALPORT_CENTRALPERIPHERAL_Data_t serialport_centralperipheral_notification_data;

  serialport_centralperipheral_notification_data.p_Payload = (uint8_t*)a_SERIALPORT_CENTRALPERIPHERAL_UpdateCharData;
  serialport_centralperipheral_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_NS_1*/

  /* USER CODE END Service1Char1_NS_1*/

  if (notification_on_off != Tx_NOTIFICATION_OFF && SERIALPORT_CENTRALPERIPHERAL_APP_Context.SERIALPORT_CENTRALPERIPHERAL_periph_connHdl != 0xFFFF)
  {
    SERIALPORT_CENTRALPERIPHERAL_NotifyValue(SERIALPORT_CENTRALPERIPHERAL_TX, &serialport_centralperipheral_notification_data, SERIALPORT_CENTRALPERIPHERAL_APP_Context.SERIALPORT_CENTRALPERIPHERAL_periph_connHdl);
  }

  /* USER CODE BEGIN Service1Char1_NS_Last*/

  /* USER CODE END Service1Char1_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
void Send_Data_Over_BLE(void)
{
  if(SERIALPORT_CENTRALPERIPHERAL_APP_Context.SERIALPORT_CENTRALPERIPHERAL_periph_connHdl != 0xFFFF)
  {
    SERIALPORT_CENTRALPERIPHERAL_Data_t TxBuffer;

    TxBuffer.p_Payload = a_buffSpTx;
    TxBuffer.Length = buffSpTxLen;
    SERIALPORT_CENTRALPERIPHERAL_UpdateValue(SERIALPORT_CENTRALPERIPHERAL_TX, &TxBuffer);
    SERIALPORT_CENTRALPERIPHERAL_NotifyValue(SERIALPORT_CENTRALPERIPHERAL_TX, 
                                             &TxBuffer, 
                                             SERIALPORT_CENTRALPERIPHERAL_APP_Context.SERIALPORT_CENTRALPERIPHERAL_periph_connHdl);  
    BLEStack_Process_Schedule();
  }
  return;
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
