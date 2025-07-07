/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ble.c
  * @author  GPM WBL Application Team
  * @brief   BLE Application
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
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32wb0x.h"
#include "ble.h"
#include "gatt_profile.h"
#include "gap_profile.h"
#include "app_ble.h"
#include "stm32wb0x_hal_radio_timer.h"
#include "bleplat.h"
#include "nvm_db.h"
#include "blenvm.h"
#include "pka_manager.h"
#include "stm32_seq.h"
#include "gatt_client_app.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "esl_profile_ap.h"
#include "uart_cmd.h"
#include "crash_handler.h"
#include "otp_client.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/**
 * Security parameters structure
 */
typedef struct
{
  /**
   * IO capability of the device
   */
  uint8_t ioCapability;

  /**
   * Authentication requirement of the device
   * Man In the Middle protection required?
   */
  uint8_t mitm_mode;

  /**
   * bonding mode of the device
   */
  uint8_t bonding_mode;

  /**
   * minimum encryption key size requirement
   */
  uint8_t encryptionKeySizeMin;

  /**
   * maximum encryption key size requirement
   */
  uint8_t encryptionKeySizeMax;

  /**
   * this flag indicates whether the host has to initiate
   * the security, wait for pairing or does not have any security
   * requirements.
   * 0x00 : no security required
   * 0x01 : host should initiate security by sending the security
   *        request command
   * 0x02 : host need not send the clave security request but it
   * has to wait for paiirng to complete before doing any other
   * processing
   */
  uint8_t initiateSecurity;
  /* USER CODE BEGIN tSecurityParams*/

  /* USER CODE END tSecurityParams */
}SecurityParams_t;

/**
 * Global context contains all BLE common variables.
 */
typedef struct
{
  /**
   * security requirements of the host
   */
  SecurityParams_t bleSecurityParam;

  /**
   * gap service handle
   */
  uint16_t gapServiceHandle;

  /**
   * device name characteristic handle
   */
  uint16_t devNameCharHandle;

  /**
   * appearance characteristic handle
   */
  uint16_t appearanceCharHandle;

  /**
   * connection handle of the current active connection
   * When not in connection, the handle is set to 0xFFFF
   */
  uint16_t connectionHandle;
  /* USER CODE BEGIN BleGlobalContext_t*/

  /* USER CODE END BleGlobalContext_t */
}BleGlobalContext_t;

typedef struct
{
  BleGlobalContext_t BleApplicationContext_legacy;
  APP_BLE_ConnStatus_t Device_Connection_Status;
  /* USER CODE BEGIN PTD_1*/  
  ESL_AP_status_t AP_status;

  uint8_t deviceServerBdAddrType;
  uint8_t a_deviceServerBdAddr[BD_ADDR_SIZE];

  /* USER CODE END PTD_1 */
}BleApplicationContext_t;

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */
/* Private variables ---------------------------------------------------------*/

NO_INIT(uint32_t dyn_alloc_a[BLE_DYN_ALLOC_SIZE>>2]);

static BleApplicationContext_t bleAppContext;

static const char a_GapDeviceName[] = {'E', 'S', 'L', 'A', 'P'}; /* Gap Device Name */

/* USER CODE BEGIN PV */
uint16_t esl_address;

bool bGeneralConn = false;
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */
extern IWDG_HandleTypeDef hiwdg;
/* USER CODE END GV */

/* Private function prototypes -----------------------------------------------*/
static void connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Connection_Interval,
                                      uint16_t Peripheral_Latency,
                                      uint16_t Supervision_Timeout);

static void gap_cmd_resp_wait(void);
static void gap_cmd_resp_release(void);
/* USER CODE BEGIN PFP */
static void start_periodic_adv(void);
void periodic_sync_info_transfer(void);
void connection_setup(void);

static uint8_t analyse_adv_report(uint8_t adv_data_size, uint8_t *p_adv_data,
                                  uint8_t address_type, uint8_t *p_address);
static void Connect_Request(void);

static void start_general_connection(void);
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private functions ---------------------------------------------------------*/

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/* Functions Definition ------------------------------------------------------*/
void ModulesInit(void)
{
  BLENVM_Init();
  if (PKAMGR_Init() == PKAMGR_ERROR)
  {
    Error_Handler();
  }
}

void BLE_Init(void)
{
  uint8_t role;
  uint8_t privacy_type = 0;
  tBleStatus ret;
  uint16_t gatt_service_changed_handle;
  uint16_t gap_dev_name_char_handle;
  uint16_t gap_appearance_char_handle;
  uint16_t gap_periph_pref_conn_param_char_handle;
  uint8_t bd_address[6] = {0};
  uint8_t bd_address_len= 6;
  uint16_t appearance = CFG_GAP_APPEARANCE;

  BLE_STACK_InitTypeDef BLE_STACK_InitParams = {
    .BLEStartRamAddress = (uint8_t*)dyn_alloc_a,
    .TotalBufferSize = BLE_DYN_ALLOC_SIZE,
    .NumAttrRecords = CFG_BLE_NUM_GATT_ATTRIBUTES,
    .MaxNumOfClientProcs = CFG_BLE_NUM_OF_CONCURRENT_GATT_CLIENT_PROC,
    .NumOfRadioTasks = CFG_BLE_NUM_RADIO_TASKS,
    .NumOfEATTChannels = CFG_BLE_NUM_EATT_CHANNELS,
    .NumBlockCount = CFG_BLE_MBLOCKS_COUNT,
    .ATT_MTU = CFG_BLE_ATT_MTU_MAX,
    .MaxConnEventLength = CFG_BLE_CONN_EVENT_LENGTH_MAX,
    .SleepClockAccuracy = CFG_BLE_SLEEP_CLOCK_ACCURACY,
    .NumOfAdvDataSet = CFG_BLE_NUM_ADV_SETS,
    .NumOfSubeventsPAwR = CFG_BLE_NUM_PAWR_SUBEVENTS,
    .MaxPAwRSubeventDataCount = CFG_BLE_PAWR_SUBEVENT_DATA_COUNT_MAX,
    .NumOfAuxScanSlots = CFG_BLE_NUM_AUX_SCAN_SLOTS,
    .FilterAcceptListSizeLog2 = CFG_BLE_FILTER_ACCEPT_LIST_SIZE_LOG2,
    .L2CAP_MPS = CFG_BLE_COC_MPS_MAX,
    .L2CAP_NumChannels = CFG_BLE_COC_NBR_MAX,
    .NumOfSyncSlots = CFG_BLE_NUM_SYNC_SLOTS,
    .CTE_MaxNumAntennaIDs = CFG_BLE_NUM_CTE_ANTENNA_IDS_MAX,
    .CTE_MaxNumIQSamples = CFG_BLE_NUM_CTE_IQ_SAMPLES_MAX,
    .NumOfSyncBIG = CFG_BLE_NUM_SYNC_BIG_MAX,
    .NumOfBrcBIG = CFG_BLE_NUM_BRC_BIG_MAX,
    .NumOfSyncBIS = CFG_BLE_NUM_SYNC_BIS_MAX,
    .NumOfBrcBIS = CFG_BLE_NUM_BRC_BIS_MAX,
    .NumOfCIG = CFG_BLE_NUM_CIG_MAX,
    .NumOfCIS = CFG_BLE_NUM_CIS_MAX,
    .isr0_fifo_size = CFG_BLE_ISR0_FIFO_SIZE,
    .isr1_fifo_size = CFG_BLE_ISR1_FIFO_SIZE,
    .user_fifo_size = CFG_BLE_USER_FIFO_SIZE
  };

  /* Bluetooth LE stack init */
  ret = BLE_STACK_Init(&BLE_STACK_InitParams);
  if (ret != BLE_STATUS_SUCCESS) {
    APP_DBG_MSG("Error in BLE_STACK_Init() 0x%02x\r\n", ret);
    Error_Handler();
  }

#if (CFG_BD_ADDRESS_TYPE == HCI_ADDR_PUBLIC)

  bd_address[0] = (uint8_t)((CFG_PUBLIC_BD_ADDRESS & 0x0000000000FF));
  bd_address[1] = (uint8_t)((CFG_PUBLIC_BD_ADDRESS & 0x00000000FF00) >> 8);
  bd_address[2] = (uint8_t)((CFG_PUBLIC_BD_ADDRESS & 0x000000FF0000) >> 16);
  bd_address[3] = (uint8_t)((CFG_PUBLIC_BD_ADDRESS & 0x0000FF000000) >> 24);
  bd_address[4] = (uint8_t)((CFG_PUBLIC_BD_ADDRESS & 0x00FF00000000) >> 32);
  bd_address[5] = (uint8_t)((CFG_PUBLIC_BD_ADDRESS & 0xFF0000000000) >> 40);
  (void)bd_address_len;

  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bd_address);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_hal_write_config_data command - CONFIG_DATA_PUBADDR_OFFSET, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_hal_write_config_data command - CONFIG_DATA_PUBADDR_OFFSET\n");
  }
#endif

  /**
   * Set TX Power.
   */
  ret = aci_hal_set_tx_power_level(0, CFG_TX_POWER);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_hal_set_tx_power_level command, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_hal_set_tx_power_level command\n");
  }

  /**
   * Initialize GATT interface
   */
  ret = aci_gatt_srv_profile_init(GATT_INIT_SERVICE_CHANGED_BIT, &gatt_service_changed_handle);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_profile_init command, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_profile_init command\n");
  }

  /**
   * Initialize GAP interface
   */
  role = 0U;
  role |= GAP_CENTRAL_ROLE|GAP_BROADCASTER_ROLE;
  
#if CFG_BLE_PRIVACY_ENABLED
  privacy_type = 0x02;
#endif

/* USER CODE BEGIN Role_Mngt*/

/* USER CODE END Role_Mngt */

  ret = aci_gap_init(privacy_type, CFG_BD_ADDRESS_TYPE);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gap_init command, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gap_init command\n");
  }

  ret = aci_gap_profile_init(role, privacy_type,
                             &gap_dev_name_char_handle,
                             &gap_appearance_char_handle,
                             &gap_periph_pref_conn_param_char_handle);

#if (CFG_BD_ADDRESS_TYPE == HCI_ADDR_STATIC_RANDOM_ADDR)
  ret = aci_hal_read_config_data(CONFIG_DATA_STORED_STATIC_RANDOM_ADDRESS,
                                 &bd_address_len, bd_address);
  APP_DBG_MSG("  Static Random Bluetooth Address: %02x:%02x:%02x:%02x:%02x:%02x\n",bd_address[5],bd_address[4],bd_address[3],bd_address[2],bd_address[1],bd_address[0]);
#elif (CFG_BD_ADDRESS_TYPE == HCI_ADDR_PUBLIC)
  APP_DBG_MSG("  Public Bluetooth Address: %02x:%02x:%02x:%02x:%02x:%02x\n",bd_address[5],bd_address[4],bd_address[3],bd_address[2],bd_address[1],bd_address[0]);
#else
#error "Invalid CFG_BD_ADDRESS_TYPE"
#endif

  ret = Gap_profile_set_dev_name(0, sizeof(a_GapDeviceName), (uint8_t*)a_GapDeviceName);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : Gap_profile_set_dev_name - Device Name, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: Gap_profile_set_dev_name - Device Name\n");
  }

  ret = Gap_profile_set_appearance(0, sizeof(appearance), (uint8_t*)&appearance);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : Gap_profile_set_appearance - Appearance, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: Gap_profile_set_appearance - Appearance\n");
  }

  /**
   * Initialize IO capability
   */
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability = CFG_IO_CAPABILITY;
  ret = aci_gap_set_io_capability(bleAppContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gap_set_io_capability command, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gap_set_io_capability command\n");
  }

  /**
   * Initialize authentication
   */
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode             = CFG_MITM_PROTECTION;
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin  = CFG_ENCRYPTION_KEY_SIZE_MIN;
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax  = CFG_ENCRYPTION_KEY_SIZE_MAX;
  bleAppContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode          = CFG_BONDING_MODE;

  /* USER CODE BEGIN Ble_Hci_Gap_Gatt_Init_1*/

  /* USER CODE END Ble_Hci_Gap_Gatt_Init_1*/
  ret = aci_gap_set_security_requirements(bleAppContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode,
                                          bleAppContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode,
                                          CFG_SC_SUPPORT,
                                          CFG_KEYPRESS_NOTIFICATION_SUPPORT,
                                          bleAppContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin,
                                          bleAppContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax,
                                          GAP_PAIRING_RESP_NONE);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gap_set_security_requirements command, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gap_set_security_requirements command\n");
  }

  /**
   * Initialize Filter Accept List
   */
  if (bleAppContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode)
  {
    ret = aci_gap_configure_filter_accept_and_resolving_list(0x01);
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : aci_gap_configure_filter_accept_and_resolving_list command, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("  Success: aci_gap_configure_filter_accept_and_resolving_list command\n");
    }
  }
  
  uint8_t LE_Event_Mask[8] = {0xFF,0xFF,0xFF,0xFF,
    HCI_LE_EVT_MASK_BYTE4_PERIODIC_ADVERTISING_SUBEVENT_DATA_REQ|HCI_LE_EVT_MASK_BYTE4_PERIODIC_ADVERTISING_RESPONSE_REPORT,
    0xFF,0xFF,0xFF};  
  ret= aci_gap_set_le_event_mask(LE_Event_Mask);
  
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gap_set_le_event_mask command, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gap_set_le_event_mask command\n");
  }  

  APP_DBG_MSG("==>> End BLE_Init function\n");

}

void BLEStack_Process_Schedule(void)
{
  /* Keep BLE Stack Process priority low, since there are limited cases
     where stack wants to be rescheduled for busy waiting.  */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_BLE_STACK, CFG_SEQ_PRIO_1);
}
static void BLEStack_Process(void)
{
  APP_DEBUG_SIGNAL_SET(APP_STACK_PROCESS);
  BLE_STACK_Tick();

  APP_DEBUG_SIGNAL_RESET(APP_STACK_PROCESS);
}

void VTimer_Process(void)
{
  HAL_RADIO_TIMER_Tick();
}

void VTimer_Process_Schedule(void)
{
  UTIL_SEQ_SetTask( 1U << CFG_TASK_VTIMER, CFG_SEQ_PRIO_0);
}
void NVM_Process(void)
{
  NVMDB_Tick();
}

void NVM_Process_Schedule(void)
{
  UTIL_SEQ_SetTask( 1U << CFG_TASK_NVM, CFG_SEQ_PRIO_1);
}

/* Function called from RADIO_TIMER_TXRX_WKUP_IRQHandler() context. */
void HAL_RADIO_TIMER_TxRxWakeUpCallback(void)
{
  VTimer_Process_Schedule();
}

/* Function called from RADIO_TIMER_CPU_WKUP_IRQHandler() context. */
void HAL_RADIO_TIMER_CpuWakeUpCallback(void)
{
  VTimer_Process_Schedule();
}

/* Function called from RADIO_TXRX_IRQHandler() context. */
void HAL_RADIO_TxRxCallback(uint32_t flags)
{
  BLE_STACK_RadioHandler(flags);

  VTimer_Process_Schedule();
  NVM_Process_Schedule();
}

void UART_CMD_ProcessRequestCB(void)
{
  UTIL_SEQ_SetTask(1u << CFG_TASK_UART_CMD_PROCESS, CFG_SEQ_PRIO_0);  
}

void BLE_STACK_ProcessRequest(void)
{
  BLEStack_Process_Schedule();
}

/* Functions Definition ------------------------------------------------------*/
void APP_BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */

  /* USER CODE END APP_BLE_Init_1 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BLE_STACK, UTIL_SEQ_RFU, BLEStack_Process);
  UTIL_SEQ_RegTask(1U << CFG_TASK_VTIMER, UTIL_SEQ_RFU, VTimer_Process);
  UTIL_SEQ_RegTask(1U << CFG_TASK_NVM, UTIL_SEQ_RFU, NVM_Process);
  ModulesInit();

  /* Initialization of HCI & GATT & GAP layer */
  BLE_Init();
  
  /**
  * Initialize GATT Client Application
  */
  GATT_CLIENT_APP_Init();
  
  /* USER CODE BEGIN APP_BLE_Init_2 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_UART_CMD_PROCESS, UTIL_SEQ_RFU, UART_CMD_Process);
  UTIL_SEQ_RegTask(1u << CFG_TASK_CONN_DEV_ID, UTIL_SEQ_RFU, Connect_Request);
  UTIL_SEQ_RegTask(1U << CFG_TASK_START_INFO_TRANSFER, UTIL_SEQ_RFU, periodic_sync_info_transfer);
  UTIL_SEQ_RegTask(1U << CFG_TASK_CONN_SETUP, UTIL_SEQ_RFU, connection_setup);  
  UTIL_SEQ_RegTask(1U << CFG_TASK_UPDATING_STATE_TRANSITION, UTIL_SEQ_RFU, ESL_AP_UpdatingStateTransition);
  
  ESL_AP_Init();
  ESL_AP_List_Init();
  
  start_periodic_adv();
  
  set_AP_Status(ESL_AP_CONFIGURING_ESL);
  
  start_general_connection();
  
  /* USER CODE END APP_BLE_Init_2 */

  return;
}

void BLEEVT_App_Notification(const hci_pckt *hci_pckt)
{
  tBleStatus ret = BLE_STATUS_ERROR;
  hci_event_pckt    *p_event_pckt;
  hci_le_meta_event *p_meta_evt;
  void *event_data;

  UNUSED(ret);
  /* USER CODE BEGIN SVCCTL_App_Notification */
#ifndef DISABLE_WATCHDOG
  HAL_IWDG_Refresh(&hiwdg);
#endif
  /* USER CODE END SVCCTL_App_Notification */

  if(hci_pckt->type != HCI_EVENT_PKT_TYPE && hci_pckt->type != HCI_EVENT_EXT_PKT_TYPE)
  {
    /* Not an event */
    return;
  }

  p_event_pckt = (hci_event_pckt*)hci_pckt->data;

  if(hci_pckt->type == HCI_EVENT_PKT_TYPE){
    event_data = p_event_pckt->data;
  }
  else { /* hci_pckt->type == HCI_EVENT_EXT_PKT_TYPE */
    hci_event_ext_pckt *p_event_pckt = (hci_event_ext_pckt*)hci_pckt->data;
    event_data = p_event_pckt->data;
  }

  switch (p_event_pckt->evt) /* evt field is at same offset in hci_event_pckt and hci_event_ext_pckt */
  {
  case HCI_DISCONNECTION_COMPLETE_EVT_CODE:
    {
      hci_disconnection_complete_event_rp0 *p_disconnection_complete_event;
      p_disconnection_complete_event = (hci_disconnection_complete_event_rp0 *) p_event_pckt->data;

        /* USER CODE BEGIN EVT_DISCONN_COMPLETE_3 */

        /* USER CODE END EVT_DISCONN_COMPLETE_3 */

      if (p_disconnection_complete_event->Connection_Handle == bleAppContext.BleApplicationContext_legacy.connectionHandle)
      {
        bleAppContext.BleApplicationContext_legacy.connectionHandle = 0xFFFF;
        bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
        APP_DBG_MSG(">>== HCI_DISCONNECTION_COMPLETE_EVT_CODE\n");
        APP_DBG_MSG("     - Connection Handle:   0x%02X\n     - Reason:    0x%02X\n",
                    p_disconnection_complete_event->Connection_Handle,
                    p_disconnection_complete_event->Reason);

        /* USER CODE BEGIN EVT_DISCONN_COMPLETE_2 */        
        memset(bleAppContext.a_deviceServerBdAddr, 0, sizeof(bleAppContext.a_deviceServerBdAddr));
        
        GATT_CLIENT_APP_ConnHandle_Notif_evt_t notif;
        notif.ConnOpcode = PEER_DISCON_HANDLE_EVT;
        notif.ConnHdl = p_disconnection_complete_event->Connection_Handle;   
        GATT_CLIENT_APP_Notification(&notif);
        
        ESL_APP_DisconnectionComplete(p_disconnection_complete_event->Connection_Handle);
        
        /* In case a task was waiting for this event. */
        UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_READ_REMOTE_FEATURES_COMPLETE);
        
        start_general_connection();
        
        /* USER CODE END EVT_DISCONN_COMPLETE_2 */
      }
      gap_cmd_resp_release();
      /* USER CODE BEGIN EVT_DISCONN_COMPLETE_1 */

      /* USER CODE END EVT_DISCONN_COMPLETE_1 */
    }
    break;

  case HCI_LE_META_EVT_CODE:
    {
      p_meta_evt = (hci_le_meta_event*) p_event_pckt->data;
      /* USER CODE BEGIN EVT_LE_META_EVENT */

      /* USER CODE END EVT_LE_META_EVENT */
      switch (p_meta_evt->subevent)
      {
      case HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE:
        {
          hci_le_connection_update_complete_event_rp0 *p_conn_update_complete;
          p_conn_update_complete = (hci_le_connection_update_complete_event_rp0 *) p_meta_evt->data;
          APP_DBG_MSG(">>== HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE\n");
          APP_DBG_MSG("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout:   %d ms\n",
                      INT(p_conn_update_complete->Connection_Interval*1.25),
                      FRACTIONAL_2DIGITS(p_conn_update_complete->Connection_Interval*1.25),
                      p_conn_update_complete->Peripheral_Latency,
                      p_conn_update_complete->Supervision_Timeout*10);
          UNUSED(p_conn_update_complete);
          /* USER CODE BEGIN EVT_LE_CONN_UPDATE_COMPLETE */

          /* USER CODE END EVT_LE_CONN_UPDATE_COMPLETE */
        }
        break;
      case HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE:
        {
          hci_le_phy_update_complete_event_rp0 *p_le_phy_update_complete;
          p_le_phy_update_complete = (hci_le_phy_update_complete_event_rp0*)p_meta_evt->data;
          UNUSED(p_le_phy_update_complete);

          gap_cmd_resp_release();

          /* USER CODE BEGIN EVT_LE_PHY_UPDATE_COMPLETE */

          /* USER CODE END EVT_LE_PHY_UPDATE_COMPLETE */
        }
        break;
      case HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_enhanced_connection_complete_event_rp0 *p_enhanced_conn_complete;
          p_enhanced_conn_complete = (hci_le_enhanced_connection_complete_event_rp0 *) p_meta_evt->data;
          APP_DBG_MSG("HCI_LE_ENHANCED_CONNECTION_COMPLETE_SUBEVT_CODE\n");
          connection_complete_event(p_enhanced_conn_complete->Status,
                                    p_enhanced_conn_complete->Connection_Handle,
				                    p_enhanced_conn_complete->Role,
                                    p_enhanced_conn_complete->Peer_Address_Type,
                                    p_enhanced_conn_complete->Peer_Address,
                                    p_enhanced_conn_complete->Connection_Interval,
                                    p_enhanced_conn_complete->Peripheral_Latency,
                                    p_enhanced_conn_complete->Supervision_Timeout);
        }
        break;
      case HCI_LE_ENHANCED_CONNECTION_COMPLETE_V2_SUBEVT_CODE:
        {
          hci_le_enhanced_connection_complete_v2_event_rp0 *event;
          event = (hci_le_enhanced_connection_complete_v2_event_rp0 *) p_meta_evt->data;
          APP_DBG_MSG("HCI_LE_ENHANCED_CONNECTION_COMPLETE_V2_SUBEVT_CODE\n");
          connection_complete_event(event->Status,
                                    event->Connection_Handle,
                                    event->Role,
                                    event->Peer_Address_Type,
                                    event->Peer_Address,
                                    event->Connection_Interval,
                                    event->Peripheral_Latency,
                                    event->Supervision_Timeout);
        }
        break;        
      case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_connection_complete_event_rp0 *p_conn_complete;
          p_conn_complete = (hci_le_connection_complete_event_rp0 *) p_meta_evt->data;
          APP_DBG_MSG("HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE\n");
          connection_complete_event(p_conn_complete->Status,
                                    p_conn_complete->Connection_Handle,
                                    p_conn_complete->Role,
                                    p_conn_complete->Peer_Address_Type,
                                    p_conn_complete->Peer_Address,
                                    p_conn_complete->Connection_Interval,
                                    p_conn_complete->Peripheral_Latency,
                                    p_conn_complete->Supervision_Timeout);
        }
        break;
      case HCI_LE_ADVERTISING_REPORT_SUBEVT_CODE:
        {
          hci_le_advertising_report_event_rp0 *p_adv_report;
          p_adv_report = (hci_le_advertising_report_event_rp0 *) p_meta_evt->data;
          /* USER CODE BEGIN EVT_LE_ADVERTISING_REPORT */
       
          analyse_adv_report(p_adv_report->Advertising_Report.Data_Length, p_adv_report->Advertising_Report.Data_RSSI,
                             p_adv_report->Advertising_Report.Address_Type, p_adv_report->Advertising_Report.Address);
       
          /* USER CODE END EVT_LE_ADVERTISING_REPORT */
	  UNUSED(p_adv_report);

        }
        break;
      case HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE:
        {
          hci_le_extended_advertising_report_event_rp0 *p_ext_adv_report;
          p_ext_adv_report = (hci_le_extended_advertising_report_event_rp0 *) p_meta_evt->data;
          /* USER CODE BEGIN EVT_LE_EXT_ADVERTISING_REPORT */

          analyse_adv_report(p_ext_adv_report->Extended_Advertising_Report.Data_Length, p_ext_adv_report->Extended_Advertising_Report.Data,
                             p_ext_adv_report->Extended_Advertising_Report.Address_Type, p_ext_adv_report->Extended_Advertising_Report.Address);
        
          /* USER CODE END EVT_LE_EXT_ADVERTISING_REPORT */
          UNUSED(p_ext_adv_report);

        }
        break;          
      /* USER CODE BEGIN EVT_LE_META_EVENT_1 */
      case HCI_LE_READ_REMOTE_FEATURES_COMPLETE_SUBEVT_CODE:
        {
          hci_le_read_remote_features_complete_event_rp0 *p_read_remote_features_complete;
          p_read_remote_features_complete = (hci_le_read_remote_features_complete_event_rp0 *) p_meta_evt->data;
          (void)p_read_remote_features_complete;
          
          APP_DBG_MSG("LE_Features: ");
          for(int i = 0; i < 8; i++)
            APP_DBG_MSG("0x%02X ", p_read_remote_features_complete->LE_Features[i]);
          APP_DBG_MSG("\n");
          
          UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_READ_REMOTE_FEATURES_COMPLETE);
        }
        break;
      case HCI_LE_PERIODIC_ADVERTISING_SUBEVENT_DATA_REQUEST_SUBEVT_CODE:
        {
          hci_le_periodic_advertising_subevent_data_request_event_rp0 *p_periodic_adv_subevent_data_req;
          p_periodic_adv_subevent_data_req = (hci_le_periodic_advertising_subevent_data_request_event_rp0 *) p_meta_evt->data;
          
//          APP_DBG_MSG("PADV_DATA_REQUEST %d %d\n", p_periodic_adv_subevent_data_req->Subevent_Start, p_periodic_adv_subevent_data_req->Subevent_Data_Count);
          
          for(int i = p_periodic_adv_subevent_data_req->Subevent_Start; i < p_periodic_adv_subevent_data_req->Subevent_Start + p_periodic_adv_subevent_data_req->Subevent_Data_Count; i++)
          {
            ESL_AP_SubeventDataRequest(i);
          }
          
        }
        break;
      case HCI_LE_PERIODIC_ADVERTISING_RESPONSE_REPORT_SUBEVT_CODE:
        {
          hci_le_periodic_advertising_response_report_event_rp0 *p_periodic_adv_response_report;
          p_periodic_adv_response_report = (hci_le_periodic_advertising_response_report_event_rp0 *) p_meta_evt->data;
          
          packed_Periodic_Advertising_Response_t *periodic_adv_resp_p;
          
          APP_DBG_MSG("HCI_LE_PERIODIC_ADVERTISING_RESPONSE_REPORT\n");
          
          periodic_adv_resp_p = &p_periodic_adv_response_report->Periodic_Advertising_Response;
          
          for(int i = 0; i < p_periodic_adv_response_report->Num_Responses; i++)
          {
            ESL_AP_ResponseReport(p_periodic_adv_response_report->Subevent, periodic_adv_resp_p->Response_Slot, periodic_adv_resp_p->Data_Length, periodic_adv_resp_p->Data);
            periodic_adv_resp_p = periodic_adv_resp_p + sizeof(packed_Periodic_Advertising_Response_t) + periodic_adv_resp_p->Data_Length;
          }
          
        }
        break;

      /* USER CODE END EVT_LE_META_EVENT_1 */

      default:
        /* USER CODE BEGIN SUBEVENT_DEFAULT */
        APP_DBG_MSG("HCI_LE_META_EVT: 0x%02X\n", p_meta_evt->subevent);
        /* USER CODE END SUBEVENT_DEFAULT */
        break;
      }
    } /* HCI_LE_META_EVT_CODE */
    break;

  case HCI_VENDOR_EVT_CODE:
    {
      aci_blecore_event *p_blecore_evt = (aci_blecore_event*) event_data;
      /* USER CODE BEGIN EVT_VENDOR */

      /* USER CODE END EVT_VENDOR */
      switch (p_blecore_evt->ecode)
      {
        /* USER CODE BEGIN ecode */

        /* USER CODE END ecode */
      case ACI_L2CAP_CONNECTION_UPDATE_REQ_VSEVT_CODE:
        {
          aci_l2cap_connection_update_req_event_rp0 *p_l2cap_conn_update_req;
          p_l2cap_conn_update_req = (aci_l2cap_connection_update_req_event_rp0 *) p_blecore_evt->data;
          tBleStatus ret;
          uint8_t req_resp = 0x01;

          /* USER CODE BEGIN EVT_L2CAP_CONNECTION_UPDATE_REQ */

          /* USER CODE END EVT_L2CAP_CONNECTION_UPDATE_REQ */

          ret = aci_l2cap_connection_parameter_update_resp(p_l2cap_conn_update_req->Connection_Handle,
                                                           p_l2cap_conn_update_req->Connection_Interval_Min,
                                                           p_l2cap_conn_update_req->Connection_Interval_Max,
                                                           p_l2cap_conn_update_req->Max_Latency,
                                                           p_l2cap_conn_update_req->Timeout_Multiplier,
                                                           CONN_CE_LENGTH_MS(10),
                                                           CONN_CE_LENGTH_MS(10),
                                                           p_l2cap_conn_update_req->Identifier,
                                                           req_resp);
          if(ret != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("  Fail   : aci_l2cap_connection_parameter_update_resp command\n");
          }
          else
          {
            APP_DBG_MSG("  Success: aci_l2cap_connection_parameter_update_resp command\n");
          }

          /* USER CODE BEGIN EVT_L2CAP_CONNECTION_UPDATE_RESP */

          /* USER CODE END EVT_L2CAP_CONNECTION_UPDATE_RESP */
        }
        break;
      case ACI_GAP_PROC_COMPLETE_VSEVT_CODE:
        {
          APP_DBG_MSG(">>== ACI_GAP_PROC_COMPLETE_VSEVT_CODE\n");
          aci_gap_proc_complete_event_rp0 *p_gap_proc_complete;
          p_gap_proc_complete = (aci_gap_proc_complete_event_rp0*) p_blecore_evt->data;

          /* USER CODE BEGIN EVT_GAP_PROCEDURE_COMPLETE */
          if ((p_gap_proc_complete->Procedure_Code == GAP_GENERAL_DISCOVERY_PROC) &&
              (p_gap_proc_complete->Status == 0x00))
          {
            printf("+SCANEND\r\n");
          }
          /* USER CODE END EVT_GAP_PROCEDURE_COMPLETE */
        }
        break;
      case ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE:
        /* USER CODE BEGIN RADIO_ACTIVITY_EVENT*/
       
        /* USER CODE END RADIO_ACTIVITY_EVENT*/
        break;
      case ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE:
        {
          APP_DBG_MSG(">>== ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE\n");
          /* USER CODE BEGIN ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE*/
          
          /* USER CODE END ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE*/
        }
        break;
      case ACI_GAP_PASSKEY_REQ_VSEVT_CODE:
        {
          APP_DBG_MSG(">>== ACI_GAP_PASSKEY_REQ_VSEVT_CODE\n");

          ret = aci_gap_passkey_resp(bleAppContext.BleApplicationContext_legacy.connectionHandle, CFG_FIXED_PIN);
          if (ret != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("==>> aci_gap_passkey_resp : Fail, reason: 0x%02X\n", ret);
          }
          else
          {
            APP_DBG_MSG("==>> aci_gap_passkey_resp : Success\n");
          }
          /* USER CODE BEGIN ACI_GAP_PASSKEY_REQ_VSEVT_CODE*/
          
          /* USER CODE END ACI_GAP_PASSKEY_REQ_VSEVT_CODE*/
        }
        break;
      case ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE:
        {
          uint8_t confirm_value;
          APP_DBG_MSG(">>== ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE\n");
          APP_DBG_MSG("     - numeric_value = %d\n",
                      ((aci_gap_numeric_comparison_value_event_rp0 *)(p_blecore_evt->data))->Numeric_Value);
          APP_DBG_MSG("     - Hex_value = %x\n",
                      ((aci_gap_numeric_comparison_value_event_rp0 *)(p_blecore_evt->data))->Numeric_Value);

          /* Set confirm value to 1(YES) */
          confirm_value = 1;
          /* USER CODE BEGIN ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE_0*/
          
          /* USER CODE END ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE_0*/

          ret = aci_gap_numeric_comparison_value_confirm_yesno(bleAppContext.BleApplicationContext_legacy.connectionHandle, confirm_value);
          if (ret != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("==>> aci_gap_numeric_comparison_value_confirm_yesno : Fail, reason: 0x%02X\n", ret);
          }
          else
          {
            APP_DBG_MSG("==>> aci_gap_numeric_comparison_value_confirm_yesno : Success\n");
          }
          /* USER CODE BEGIN ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE*/
          
          /* USER CODE END ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE*/
        }
        break;
      case ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE:
        {
          APP_DBG_MSG(">>== ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE\n");
          aci_gap_pairing_complete_event_rp0 *p_pairing_complete;
          p_pairing_complete = (aci_gap_pairing_complete_event_rp0*)p_blecore_evt->data;

          if (p_pairing_complete->Status != 0)
          {
            APP_DBG_MSG("     - Pairing KO\n     - Status: 0x%02X\n     - Reason: 0x%02X\n",
                        p_pairing_complete->Status, p_pairing_complete->Reason);
          }
          else
          {
            APP_DBG_MSG("     - Pairing Success\n");
          }
          APP_DBG_MSG("\n");

          /* USER CODE BEGIN ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE*/
          
          /* If the AP establish a bond with an ESL and ESL is on Configuring state,
             the AP can configure ESL by writing some ESL Service characteristics  */
          if (p_pairing_complete->Status == SM_PAIRING_SUCCESS)
          {						
            if (ESL_AP_Insert_ESL_In_List(p_pairing_complete->Connection_Handle, bleAppContext.a_deviceServerBdAddr, bleAppContext.deviceServerBdAddrType, esl_address))
              UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);
          }
          else if (p_pairing_complete->Status == SM_ENCRYPTION_FAILED_MISSING_PEER_LTK)
          {
            esl_bonded_t* esl_dev;
            
            esl_dev = Search_by_Peer_address_In_List(bleAppContext.a_deviceServerBdAddr, bleAppContext.deviceServerBdAddrType);
            if(esl_dev != NULL)
            {
              ESL_AP_Remove_ESL_from_List(esl_dev);
            }     
            else
            {
              aci_gap_remove_bonded_device(bleAppContext.deviceServerBdAddrType, bleAppContext.a_deviceServerBdAddr); // to check if address is not an identity address
            }
          }
          
          if (p_pairing_complete->Status != SM_PAIRING_SUCCESS)
          {
            aci_gap_terminate(bleAppContext.BleApplicationContext_legacy.connectionHandle, BLE_ERROR_TERMINATED_REMOTE_USER);
          }
          
          /* USER CODE END ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE*/
        }
        break;
        /* USER CODE BEGIN EVT_VENDOR_1 */
        
      case ACI_L2CAP_COS_CONNECTION_RESP_VSEVT_CODE:
        {
          APP_DBG_MSG(">>== ACI_L2CAP_COS_CONNECTION_RESP_VSEVT_CODE\n");
          aci_l2cap_cos_connection_resp_event_rp0 *p_event;
          p_event = (aci_l2cap_cos_connection_resp_event_rp0 *) p_blecore_evt->data;
          
          OTP_CLIENT_L2CAPConnectionResp(p_event);
        }
        break;
      case ACI_L2CAP_COS_FLOW_CONTROL_CREDIT_VSEVT_CODE:
        {
          APP_DBG_MSG(">>== ACI_L2CAP_COS_FLOW_CONTROL_CREDIT_VSEVT_CODE\n");
          OTP_CLIENT_L2CAPCreditsReceived();
        }
        break;
      case ACI_L2CAP_COS_DISCONNECTION_COMPLETE_VSEVT_CODE:
        {
          APP_DBG_MSG(">>== ACI_L2CAP_COS_DISCONNECTION_COMPLETE_VSEVT_CODE\n");
          OTP_CLIENT_L2CAPDisconnectionComplete();
        }
        break;
      case ACI_HAL_FW_ERROR_VSEVT_CODE: 
        {
          aci_hal_fw_error_event_rp0 *event;
          event = (aci_hal_fw_error_event_rp0 *)p_blecore_evt->data;
          
          APP_DBG_MSG("ACI_HAL_FW_ERROR_VSEVT 0x%02X\n", event->FW_Error_Type);
          NVIC_SystemReset();
        }
        break;
        
      /* Opcode captured by client handler */
      case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
      case ACI_ATT_CLT_FIND_INFO_RESP_VSEVT_CODE:
      case ACI_ATT_CLT_READ_RESP_VSEVT_CODE:
      case ACI_GATT_CLT_PROC_COMPLETE_VSEVT_CODE:
        
        break;
        
        /* USER CODE END EVT_VENDOR_1 */
      default:
        /* USER CODE BEGIN EVT_VENDOR_DEFAULT */
        APP_DBG_MSG("HCI_VENDOR_EVT: 0x%04X\n", p_blecore_evt->ecode);
        /* USER CODE END EVT_VENDOR_DEFAULT */
        break;
      }
    } /* HCI_VENDOR_EVT_CODE */
    break;

  case HCI_HARDWARE_ERROR_EVT_CODE:
    {
      hci_hardware_error_event_rp0 *p_hci_hardware_error_event;
      p_hci_hardware_error_event = (hci_hardware_error_event_rp0*)p_event_pckt->data;
      
      APP_DBG_MSG("HCI_HARDWARE_ERROR_EVT 0x%02X\n", p_hci_hardware_error_event->Hardware_Code);
      
      if (p_hci_hardware_error_event->Hardware_Code <= 0x03)
      {
        HAL_Delay(5);
        NVIC_SystemReset();
      }
    }
    break;

    /* USER CODE BEGIN EVENT_PCKT */

    /* USER CODE END EVENT_PCKT */

  default:
    /* USER CODE BEGIN ECODE_DEFAULT*/
    APP_DBG_MSG("HCI_EVENT: 0x%02X\n", p_event_pckt->evt);
    /* USER CODE END ECODE_DEFAULT*/
    break;
  }
}

static void connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Peripheral_Latency,
                                      uint16_t Supervision_Timeout)
{
  /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE_1 */

  /* USER CODE END HCI_EVT_LE_CONN_COMPLETE_1 */
   APP_DBG_MSG(">>== hci_le_connection_complete_event - Connection handle: 0x%04X\n", Connection_Handle);
   APP_DBG_MSG("     - Connection established with @:%02x:%02x:%02x:%02x:%02x:%02x\n",
               Peer_Address[5],
               Peer_Address[4],
               Peer_Address[3],
               Peer_Address[2],
               Peer_Address[1],
               Peer_Address[0]);
   APP_DBG_MSG("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout: %d ms    - Status: 0x%02x\n",
               INT(Conn_Interval*1.25),
               FRACTIONAL_2DIGITS(Conn_Interval*1.25),
               Peripheral_Latency,
               Supervision_Timeout * 10,
               Status);
   if(Status != BLE_STATUS_SUCCESS)
    return;
   
   if (bleAppContext.Device_Connection_Status == APP_BLE_LP_CONNECTING)
   {
     /* Connection as client */
     bleAppContext.Device_Connection_Status = APP_BLE_CONNECTED_CLIENT;
   }
   else
   {
     /* Connection as server */
     bleAppContext.Device_Connection_Status = APP_BLE_CONNECTED_SERVER;
   }  
   bleAppContext.BleApplicationContext_legacy.connectionHandle = Connection_Handle;
   
   GATT_CLIENT_APP_ConnHandle_Notif_evt_t notif;
   notif.ConnOpcode = PEER_CONN_HANDLE_EVT;
   notif.ConnHdl = Connection_Handle;   
   GATT_CLIENT_APP_Notification(&notif);
   
   /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */
    
   UTIL_SEQ_SetTask( 1U << CFG_TASK_CONN_SETUP, CFG_SEQ_PRIO_0);
   
   bleAppContext.deviceServerBdAddrType = Peer_Address_Type;
   memcpy(bleAppContext.a_deviceServerBdAddr, Peer_Address, BD_ADDR_SIZE);
   
   set_ECP_Failed(false);
   
   ESL_APP_ReconnectionStateTransition(Peer_Address, Peer_Address_Type);
   
   /* USER CODE END HCI_EVT_LE_CONN_COMPLETE */

}/* end hci_le_connection_complete_event() */

/* USER CODE BEGIN EVT_VENDOR_2 */

/* USER CODE END EVT_VENDOR_2 */

APP_BLE_ConnStatus_t APP_BLE_Get_Client_Connection_Status(uint16_t Connection_Handle)
{
  APP_BLE_ConnStatus_t conn_status;

  if (bleAppContext.BleApplicationContext_legacy.connectionHandle == Connection_Handle)
  {
    conn_status = bleAppContext.Device_Connection_Status;
  }
  else
  {
    conn_status = APP_BLE_IDLE;
  }

  return conn_status;
}

void APP_BLE_Procedure_Gap_General(ProcGapGeneralId_t ProcGapGeneralId)
{
  tBleStatus status;

  switch(ProcGapGeneralId)
  {
#if (CFG_BLE_CONTROLLER_2M_CODED_PHY_ENABLED == 1)
    case PROC_GAP_GEN_PHY_TOGGLE:
    {
      uint8_t phy_tx, phy_rx;

      status = hci_le_read_phy(bleAppContext.BleApplicationContext_legacy.connectionHandle, &phy_tx, &phy_rx);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("hci_le_read_phy failure: reason=0x%02X\n",status);
      }
      else
      {
        APP_DBG_MSG("==>> hci_le_read_phy - Success\n");
        APP_DBG_MSG("==>> PHY Param  TX= %d, RX= %d\n", phy_tx, phy_rx);
        if ((phy_tx == HCI_TX_PHY_LE_2M) && (phy_rx == HCI_RX_PHY_LE_2M))
        {
          APP_DBG_MSG("==>> hci_le_set_phy PHY Param  TX= %d, RX= %d - ", HCI_TX_PHY_LE_1M, HCI_RX_PHY_LE_1M);
          status = hci_le_set_phy(bleAppContext.BleApplicationContext_legacy.connectionHandle, 0, HCI_TX_PHYS_LE_1M_PREF, HCI_RX_PHYS_LE_1M_PREF, 0);
          if (status != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("Fail\n");
          }
          else
          {
            APP_DBG_MSG("Success\n");
            gap_cmd_resp_wait();/* waiting for HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE */
          }
        }
        else
        {
          APP_DBG_MSG("==>> hci_le_set_phy PHY Param  TX= %d, RX= %d - ", HCI_TX_PHYS_LE_2M_PREF, HCI_RX_PHYS_LE_2M_PREF);
          status = hci_le_set_phy(bleAppContext.BleApplicationContext_legacy.connectionHandle, 0, HCI_TX_PHYS_LE_2M_PREF, HCI_RX_PHYS_LE_2M_PREF, 0);
          if (status != BLE_STATUS_SUCCESS)
          {
            APP_DBG_MSG("Fail\n");
          }
          else
          {
            APP_DBG_MSG("Success\n");
            gap_cmd_resp_wait();/* waiting for HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE */
          }
        }
      }
      break;
    }/* PROC_GAP_GEN_PHY_TOGGLE */
#endif
    case PROC_GAP_GEN_CONN_TERMINATE:
    {
      status = aci_gap_terminate(bleAppContext.BleApplicationContext_legacy.connectionHandle, BLE_ERROR_TERMINATED_REMOTE_USER);
      if (status != BLE_STATUS_SUCCESS)
      {
         APP_DBG_MSG("aci_gap_terminate failure: reason=0x%02X\n", status);
      }
      else
      {
        APP_DBG_MSG("==>> aci_gap_terminate : Success\n");
        gap_cmd_resp_wait();/* waiting for HCI_DISCONNECTION_COMPLETE_EVT_CODE */
      }
      break;
    }/* PROC_GAP_GEN_CONN_TERMINATE */
    case PROC_GATT_EXCHANGE_CONFIG:
    {
      status =aci_gatt_clt_exchange_config(bleAppContext.BleApplicationContext_legacy.connectionHandle);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("aci_gatt_clt_exchange_config failure: reason=0x%02X\n", status);
      }
      else
      {
        APP_DBG_MSG("==>> aci_gatt_clt_exchange_config : Success\n");
      }
      break;
    }
    /* USER CODE BEGIN GAP_GENERAL */

    /* USER CODE END GAP_GENERAL */
    default:
      break;
  }
  return;
}

void APP_BLE_Procedure_Gap_Central(ProcGapCentralId_t ProcGapCentralId)
{
  tBleStatus status;
  uint32_t paramA, paramB, paramC, paramD;

  UNUSED(paramA);
  UNUSED(paramB);
  UNUSED(paramC);
  UNUSED(paramD);

  /* First set parameters before calling ACI APIs, only if needed */
  switch(ProcGapCentralId)
  {
    case PROC_GAP_CENTRAL_SCAN_START:
    {
      paramA = SCAN_INT_MS(500);
      paramB = SCAN_WIN_MS(500);
      paramC = APP_BLE_SCANNING;

      /* USER CODE BEGIN PROC_GAP_CENTRAL_SCAN_START */

      /* USER CODE END PROC_GAP_CENTRAL_SCAN_START */
      break;
    }/* PROC_GAP_CENTRAL_SCAN_START */
    case PROC_GAP_CENTRAL_SCAN_TERMINATE:
    {
      paramA = 1;
      paramB = 1;
      paramC = APP_BLE_IDLE;

      /* USER CODE BEGIN PROC_GAP_CENTRAL_SCAN_TERMINATE */

      /* USER CODE END PROC_GAP_CENTRAL_SCAN_TERMINATE */
      break;
    }/* PROC_GAP_CENTRAL_SCAN_TERMINATE */
    default:
      break;
  }

  /* Call ACI APIs */
  switch(ProcGapCentralId)
  {
    case PROC_GAP_CENTRAL_SCAN_START:
    {
      status = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, 0x00, LE_1M_PHY_BIT, HCI_SCAN_TYPE_ACTIVE, paramA, paramB);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("aci_gap_set_scan_configuration - fail, result: 0x%02X\n", status);
      }
      else
      {
        APP_DBG_MSG("==>> aci_gap_set_scan_configuration - Success\n");
      }

      status = aci_gap_start_procedure (GAP_GENERAL_DISCOVERY_PROC,LE_1M_PHY_BIT,0,0);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("aci_gap_start_procedure - fail, result: 0x%02X\n", status);
      }
      else
      {
        bleAppContext.Device_Connection_Status = (APP_BLE_ConnStatus_t)paramC;
        APP_DBG_MSG("==>> aci_gap_start_procedure - Success\n");
      }
      break;
    }/* PROC_GAP_CENTRAL_SCAN_START */
    case PROC_GAP_CENTRAL_SCAN_TERMINATE:
    {
      status = aci_gap_terminate_proc(GAP_GENERAL_DISCOVERY_PROC);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("aci_gap_terminate_gap_proc - fail, result: 0x%02X\n",status);
      }
      else
      {
        bleAppContext.Device_Connection_Status = (APP_BLE_ConnStatus_t)paramC;
        APP_DBG_MSG("==>> aci_gap_terminate_gap_proc - Success\n");
      }
      break;
    }/* PROC_GAP_CENTRAL_SCAN_TERMINATE */

    default:
      break;
  }
  return;
}

static void gap_cmd_resp_release(void)
{
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_PROC_GAP_COMPLETE);
  return;
}

static void gap_cmd_resp_wait(void)
{
  UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_PROC_GAP_COMPLETE);
  return;
}
/* USER CODE BEGIN FD_LOCAL_FUNCTION */

static void start_periodic_adv(void)
{
  tBleStatus status;
  Advertising_Set_Parameters_t advertising_set_parameters;
  
  status = aci_gap_set_advertising_configuration(0, 
                                                 GAP_MODE_NON_DISCOVERABLE,
                                                 HCI_ADV_EVENT_PROP_NONE,
                                                 1600, 
                                                 1600,
                                                 HCI_ADV_CH_ALL,
                                                 0,
                                                 NULL, /* No peer address */
                                                 HCI_ADV_FILTER_NONE,
                                                 0, /* 0 dBm */
                                                 HCI_PHY_LE_1M, /* Primary advertising PHY */
                                                 0, /* 0 skips */
                                                 HCI_PHY_LE_1M, /* Secondary advertising PHY. Not used with legacy advertising. */
                                                 0, /* SID */
                                                 0 /* No scan request notifications */);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> aci_gap_set_advertising_configuration - fail, result: 0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> Success: aci_gap_set_advertising_configuration\n");
  }
  
  status = hci_le_set_periodic_advertising_parameters_v2(0,
                                                         (PAWR_INTERVAL_MS*100)/125,
                                                         (PAWR_INTERVAL_MS*100)/125,
                                                         0,
                                                         PAWR_NUM_SUBEVENTS,
                                                         (PAWR_SUBEVENT_INTERVAL_MS*100)/125,
                                                         (PAWR_RESPONSE_SLOT_DELAY_MS*100)/125,
                                                         PAWR_RESPONSE_SLOT_SPACING_US/125,
                                                         PAWR_NUM_RESPONSE_SLOTS);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> hci_le_set_periodic_advertising_parameters_v2 - fail, result: 0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> Success: hci_le_set_periodic_advertising_parameters_v2\n");
  }
  
  status = hci_le_set_periodic_advertising_enable(HCI_ENABLE, 0);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> hci_le_set_periodic_advertising_enable - fail, result: 0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> Success: hci_le_set_periodic_advertising_enable\n");
  }
  
  advertising_set_parameters.Advertising_Handle = 0;
  advertising_set_parameters.Duration = 0;
  advertising_set_parameters.Max_Extended_Advertising_Events = 0;
  
  status = hci_le_set_extended_advertising_enable(HCI_ENABLE, 1, &advertising_set_parameters);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> hci_le_set_extended_advertising_enable - fail, result: 0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> Success: hci_le_set_extended_advertising_enable\n");
  }
  
  status = hci_le_set_extended_advertising_enable(HCI_DISABLE, 1, &advertising_set_parameters); 
  
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> hci_le_set_extended_advertising_enable - fail, result: 0x%02X\n", status);
  }
}

static void start_general_connection(void)
{
  tBleStatus status;
  
  bGeneralConn = true;
 
  status = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, 0x00, LE_1M_PHY_BIT, HCI_SCAN_TYPE_PASSIVE, SCAN_INT_MS(500), SCAN_WIN_MS(500));
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gap_set_scan_configuration - fail, result: 0x%02X\n", status);
    Error_Handler();
  }
  else
  {
    APP_DBG_MSG("==>> aci_gap_set_scan_configuration - Success\n");
  }
  
  status = aci_gap_set_connection_configuration(LE_1M_PHY_BIT,
                                                (CONNECTION_INTERVAL_MS*100)/125, (CONNECTION_INTERVAL_MS*100)/125,
                                                0u,
                                                SUPERVISION_TIMEOUT_MS/10,
                                                2, 2);  
  if (status == BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> aci_gap_set_connection_configuration Success, result: 0x%02x\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> aci_gap_set_connection_configuration Failed, result: 0x%02x\n", status);
    Error_Handler();
  } 

  status = aci_gap_start_procedure(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC, LE_1M_PHY_BIT, 0, 0);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gap_start_procedure (general connection) - fail, result: 0x%02X\n", status);
    bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
    Error_Handler();
  }
  else
  {
    APP_DBG_MSG("==>> aci_gap_start_procedure (general connection) - Success\n");
    bleAppContext.Device_Connection_Status = APP_BLE_LP_CONNECTING;
  }  
}

/* Task for CFG_TASK_START_INFO_TRANSFER */
void periodic_sync_info_transfer(void)
{
  tBleStatus status;
  
  status = hci_le_periodic_advertising_set_info_transfer(bleAppContext.BleApplicationContext_legacy.connectionHandle, 0, 0); 
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("hci_le_periodic_advertising_set_info_transfer - fail, result: 0x%02X, conn handle: 0x%04X\n", status, bleAppContext.BleApplicationContext_legacy.connectionHandle);
    return;
  }
  else
  {
    APP_DBG_MSG("==>> hci_le_periodic_advertising_set_info_transfer - Success\n");
  }
}

void connection_setup(void)
{
   tBleStatus ret;
   
   UTIL_SEQ_ClrEvt(1 << CFG_IDLEEVT_READ_REMOTE_FEATURES_COMPLETE);
   ret = hci_le_read_remote_features(bleAppContext.BleApplicationContext_legacy.connectionHandle);
   APP_DBG_MSG("hci_le_read_remote_features - result: 0x%02X\n", ret);
   if (ret == BLE_ERROR_CONTROLLER_BUSY)
   {
     /* Retry later */
     UTIL_SEQ_SetTask( 1U << CFG_TASK_CONN_SETUP, CFG_SEQ_PRIO_0);
     
     return;
   }
   else if(ret != BLE_STATUS_SUCCESS)
   {
     APP_BLE_Procedure_Gap_General(PROC_GAP_GEN_CONN_TERMINATE);
     
     return;
   }   
   
   UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_READ_REMOTE_FEATURES_COMPLETE);
   //to check if remote device has wanted feature
   
   ret = aci_gap_set_security(bleAppContext.BleApplicationContext_legacy.connectionHandle, GAP_SECURITY_LEVEL_2, 0x00);
   if (ret != BLE_STATUS_SUCCESS)
   {
     APP_DBG_MSG("Pairing request cmd failure: 0x%02X\n", ret);
   }
   else
   {
     APP_DBG_MSG("Pairing request cmd success\n");
     
   }
}

void create_periodic_advertising_connection(uint8_t subevent, uint8_t Peer_Address[6], uint8_t Peer_Address_Type)
{ 
    tBleStatus ble_status;
    APP_DBG_MSG("!!!!! create_periodic_advertising_connection \n Peer_Address @:%02x:%02x:%02x:%02x:%02x:%02x\n",
               Peer_Address[5],
               Peer_Address[4],
               Peer_Address[3],
               Peer_Address[2],
               Peer_Address[1],
               Peer_Address[0]);
    APP_DBG_MSG("      Connection Interval:   %d ms\n  - Supervision Timeout: %d ms\n",
               ((CONNECTION_INTERVAL_MS*100)/125), 
               (SUPERVISION_TIMEOUT_MS/10) );

    aci_gap_terminate_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
    gap_cmd_resp_wait();
    bGeneralConn = false;
    
    ble_status = aci_gap_create_periodic_advertising_connection(0, 
                                                                subevent,
                                                                0x00,
                                                                0,
                                                                Peer_Address_Type,
                                                                Peer_Address,
                                                                (CONNECTION_INTERVAL_MS*100)/125, (CONNECTION_INTERVAL_MS*100)/125,
                                                                0,
                                                                SUPERVISION_TIMEOUT_MS/10,
                                                                2, 
                                                                2);  
  if (ble_status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gap_create_periodic_advertising_connection - fail, result: 0x%02X\n", ble_status);
    return;
  }
  else
  {
    APP_DBG_MSG("==>> aci_gap_create_periodic_advertising_connection - Success\n");
  }    
  bleAppContext.Device_Connection_Status = APP_BLE_LP_CONNECTING;
}     

void set_AP_Status(ESL_AP_status_t status)
{
  bleAppContext.AP_status = status;
}

ESL_AP_status_t get_AP_Status(void)
{
  return bleAppContext.AP_status;
}

static uint8_t analyse_adv_report(uint8_t adv_data_size, uint8_t *p_adv_data,
                                  uint8_t address_type, uint8_t *p_address)
{
  uint8_t found_status = 0;
  uint16_t i = 0;
  uint8_t ad_length, ad_type;

  if (bGeneralConn)
  {
    esl_bonded_t* esl_dev;
    
    esl_dev = Search_by_Peer_address_In_List(p_address, address_type);
    if(esl_dev != NULL)
    {
      APP_DBG_MSG("Connection to ESL already bonded: %02x:%02x:%02x:%02x:%02x:%02x\n",p_address[5],p_address[4],p_address[3],p_address[2],p_address[1],p_address[0]);
      //The ESL with p_address is already bonded --> the AP connect to this ESL
      set_bleAppContext_address(address_type, p_address);
  
      // Connect to ESL address  
      UTIL_SEQ_SetTask(1u << CFG_TASK_CONN_DEV_ID, CFG_SEQ_PRIO_0);
    }   
  }  
  else
  {  
    while(i < adv_data_size)
    {
      ad_length = p_adv_data[i];
      ad_type = p_adv_data[i + 1];
      
      switch (ad_type)
      {

      case AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST:
      case AD_TYPE_16_BIT_SERV_UUID:        
        for(int j=i+2; j<i+ad_length; j++)
        {
          if ((p_adv_data[j] == 0x57) && (p_adv_data[j+1] == 0x18))
          {
            printf("+SCAN: %d,%02X%02X%02X%02X%02X%02X\n",
                        address_type,
                        p_address[5],
                        p_address[4],
                        p_address[3],
                        p_address[2],
                        p_address[1],
                        p_address[0]);
            found_status = 1;
            break;
          } 
        }  
        break;
      default:
        break;
      }/* end of switch*/
      
      i += ad_length + 1; /* increment the iterator to go on next element*/
      
      if (found_status != 0)
      {
        break;
      }
    }/* end of while*/
  }  
  return found_status;
}

static void Connect_Request(void)
{
  tBleStatus result;
  
  aci_gap_terminate_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
//  gap_cmd_resp_wait();
  bGeneralConn = false;
  
  result = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, HCI_SCAN_FILTER_BASIC_FILTERED, LE_1M_PHY_BIT, HCI_SCAN_TYPE_PASSIVE, SCAN_INT_MS(500), SCAN_WIN_MS(500));
  if (result != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("aci_gap_set_scan_configuration - fail, result: 0x%02X\n", result);
    return;
  }
  else
  {
    APP_DBG_MSG("==>> aci_gap_set_scan_configuration - Success\n");
  }
  
  APP_DBG_MSG("Create connection to ESL\n");
  result = aci_gap_set_connection_configuration(LE_1M_PHY_BIT,
                                              (CONNECTION_INTERVAL_MS*100)/125, (CONNECTION_INTERVAL_MS*100)/125,
                                              0u,
                                              SUPERVISION_TIMEOUT_MS/10,
                                              2, 2); 
  
  if (result == BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> aci_gap_set_connection_configuration Success, result: 0x%02x\n", result);
  }
  else
  {
    APP_DBG_MSG("==>> aci_gap_set_connection_configuration Failed, result: 0x%02x\n", result);
  }  
  result = aci_gap_create_connection(LE_1M_PHY_BIT,
                                     bleAppContext.deviceServerBdAddrType,
                                     &bleAppContext.a_deviceServerBdAddr[0]);    
  if (result == BLE_STATUS_SUCCESS)
  {
    bleAppContext.Device_Connection_Status = APP_BLE_LP_CONNECTING;
    APP_DBG_MSG("==>> GAP Create connection Success\n");
  }
  else
  {
    APP_DBG_MSG("==>> GAP Create connection Failed, result: 0x%02x\n", result);
  }
  return;
}

uint8_t  Scan_proc(void)
{ 
  uint8_t status;
  uint32_t paramA, paramB, paramC;
 
  paramA = SCAN_INT_MS(500);
  paramB = SCAN_WIN_MS(500);
  paramC = APP_BLE_SCANNING;
  if (bleAppContext.Device_Connection_Status != APP_BLE_CONNECTED_CLIENT)
  {
    aci_gap_terminate_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
    bGeneralConn = false;
    
    status = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, 0x00, LE_1M_PHY_BIT, HCI_SCAN_TYPE_ACTIVE, paramA, paramB);
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("aci_gap_set_scan_configuration - fail, result: 0x%02X\n", status);
      return status;
    }
    else
    {
      APP_DBG_MSG("==>> aci_gap_set_scan_configuration - Success\n");
    }
    
    status = aci_gap_start_procedure (GAP_GENERAL_DISCOVERY_PROC,LE_1M_PHY_BIT,0,0);
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("aci_gap_start_procedure - fail, result: 0x%02X\n", status);
      return status;
    }
    else
    {
      bleAppContext.Device_Connection_Status = (APP_BLE_ConnStatus_t)paramC;
      APP_DBG_MSG("==>> aci_gap_start_procedure - Success\n");
    }
  }
  else
  {
    status = BLE_STATUS_FAILED;   //TBR??? Check if it is correct error return value
  }    
  return status;
}

void set_bleAppContext_address(uint8_t address_type, uint8_t p_address[6])
{
  bleAppContext.deviceServerBdAddrType = address_type;
  memcpy(bleAppContext.a_deviceServerBdAddr, p_address, BD_ADDR_SIZE);
}

uint8_t* get_bleAppContext_address(void)
{
  return(bleAppContext.a_deviceServerBdAddr);
}

/* USER CODE END FD_LOCAL_FUNCTION */

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */

#if (CFG_BUTTON_SUPPORTED == 1)
void APPE_Button1Action(void)
{
  return;
}

void APPE_Button2Action(void)
{  
  return;
}

void APPE_Button3Action(void)
{
  return;
}
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
/* USER CODE END FD_WRAP_FUNCTIONS */

/** \endcond
 */
