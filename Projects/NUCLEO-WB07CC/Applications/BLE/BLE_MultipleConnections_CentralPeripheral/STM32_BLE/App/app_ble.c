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
#include "mc_centralperipheral.h"
#include "mc_centralperipheral_app.h"
#include "gatt_client_app.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

  /* To track the status for the Central role */
  APP_BLE_ConnStatus_t Device_Connection_Status_Central;
  
  /* To track the status for the Peripheral role */
  APP_BLE_ConnStatus_t Device_Connection_Status_Peripheral;
  
  /* USER CODE BEGIN PTD_1*/
  
  /* Advertising timeout timerID*/
  VTIMER_HandleType Advertising_mgr_timer_Id;
  
  /* Led Timeout timerID */
  VTIMER_HandleType SwitchOffGPIO_timer_Id;

  uint8_t connIntervalFlag;
  
  /* Central Role */
  uint8_t deviceServerFound;
  uint8_t deviceServerBdAddrType;
  uint8_t a_deviceServerBdAddr[BD_ADDR_SIZE];
  
  /* USER CODE END PTD_1 */
}BleApplicationContext_t;

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_ON_TIMEOUT_MS              (5)
#define ADV_TIMEOUT_MS                 (60 * 1000)

#define MAX_NUM_BONDED_DEVICES          2U
/* USER CODE END PD */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */
/* Private variables ---------------------------------------------------------*/

NO_INIT(uint32_t dyn_alloc_a[BLE_DYN_ALLOC_SIZE>>2]);

static BleApplicationContext_t bleAppContext;

MC_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t MC_CENTRALPERIPHERALHandleNotification;

GATT_CLIENT_APP_ConnHandle_Notif_evt_t clientHandleNotification;

static const char a_GapDeviceName[] = {  'M', 'u', 'l', 't', 'i', 'C', '_', 'C', 'P' }; /* Gap Device Name */

/**
 * Advertising Data
 */
uint8_t a_AdvData[] =
{
  2, AD_TYPE_FLAGS, FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE|FLAG_BIT_BR_EDR_NOT_SUPPORTED,
  10, AD_TYPE_COMPLETE_LOCAL_NAME, 'm', 'c', '_', 'c', '_', 'p', '_', 'X', 'X',  /* Complete name */
  15, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x30, 0x00, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */, 0x00 /*  */,
};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

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
static void Adv_Cancel_Req(void *arg);
static void Adv_Cancel(void);
static void Switch_OFF_GPIO(void *arg);
static void fill_advData(uint8_t *p_adv_data, uint8_t tab_size, const uint8_t*p_bd_addr);
static uint8_t analyse_adv_report(uint8_t adv_data_size, uint8_t *p_adv_data, uint8_t address_type, uint8_t *p_address);
static void Configure_Filter_List(void);
static void Connect_Request(void);
static void Start_Auto_Connection(void);
void Print_Bonded_Devices(void);
uint8_t Get_Num_Bonded_Devices(void);
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
    APP_DBG_MSG("Error in BLE_STACK_Init() 0x%02X\r\n", ret);
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
  role |= GAP_CENTRAL_ROLE;
#if CFG_BLE_PRIVACY_ENABLED
  privacy_type = 0x02;
#endif

/* USER CODE BEGIN Role_Mngt*/
  APP_DBG_MSG("\n* * * * * * Application name : BLE_MultipleConnections_CentralPeripheral * * * * * *\n");
  
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
  APP_DBG_MSG("  Static Random Bluetooth Address: %02X:%02X:%02X:%02X:%02X:%02X\n",bd_address[5],bd_address[4],bd_address[3],bd_address[2],bd_address[1],bd_address[0]);
#elif (CFG_BD_ADDRESS_TYPE == HCI_ADDR_PUBLIC)
  APP_DBG_MSG("  Public Bluetooth Address: %02X:%02X:%02X:%02X:%02X:%02X\n",bd_address[5],bd_address[4],bd_address[3],bd_address[2],bd_address[1],bd_address[0]);
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

  /* Initialize Default PHY */
  ret = hci_le_set_default_phy(0x00, HCI_TX_PHYS_LE_2M_PREF, HCI_RX_PHYS_LE_2M_PREF);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : hci_le_set_default_phy command, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: hci_le_set_default_phy command\n");
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
  fill_advData(&a_AdvData[0], sizeof(a_AdvData), bd_address);
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

void BLE_STACK_ProcessRequest(void)
{
  BLEStack_Process_Schedule();
}

/* Functions Definition ------------------------------------------------------*/
void APP_BLE_Init(void)
{
  /* USER CODE BEGIN APP_BLE_Init_1 */
  tBleStatus ret;
  /* USER CODE END APP_BLE_Init_1 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BLE_STACK, UTIL_SEQ_RFU, BLEStack_Process);
  UTIL_SEQ_RegTask(1U << CFG_TASK_VTIMER, UTIL_SEQ_RFU, VTimer_Process);
  UTIL_SEQ_RegTask(1U << CFG_TASK_NVM, UTIL_SEQ_RFU, NVM_Process);
  ModulesInit();

  /* Initialization of HCI & GATT & GAP layer */
  BLE_Init();

  /**
  * Initialization of the BLE App Context
  */
  bleAppContext.Device_Connection_Status_Central = APP_BLE_IDLE;
  bleAppContext.Device_Connection_Status_Peripheral = APP_BLE_IDLE;

  bleAppContext.BleApplicationContext_legacy.connectionHandle = 0xFFFF;

  /* From here, all initialization are BLE application specific */

  /* USER CODE BEGIN APP_BLE_Init_4 */
  
  bleAppContext.connIntervalFlag = 0;
  
  /* For Peripheral */
  UTIL_SEQ_RegTask(1U << CFG_TASK_ADV_CANCEL_ID, UTIL_SEQ_RFU, Adv_Cancel);
  UTIL_SEQ_RegTask(1U << CFG_TASK_CONFIGURE_FILTER_LIST_AND_NEW_ADV, UTIL_SEQ_RFU, Configure_Filter_List);
  
  /* Create timer to handle the Advertising Stop */
  bleAppContext.Advertising_mgr_timer_Id.callback = Adv_Cancel_Req;    
  
  /* Create timer to handle the Led Switch OFF */
  bleAppContext.SwitchOffGPIO_timer_Id.callback = Switch_OFF_GPIO;
  
  /* USER CODE END APP_BLE_Init_4 */

  /**
  * Initialize Services and Characteristics.
  */
  APP_DBG_MSG("\n");
  APP_DBG_MSG("Services and Characteristics creation\n");
  MC_CENTRALPERIPHERAL_APP_Init();
  APP_DBG_MSG("End of Services and Characteristics creation\n");
  APP_DBG_MSG("\n");

  /* USER CODE BEGIN APP_BLE_Init_3 */
  
  ret = aci_hal_set_radio_activity_mask(0x0006);
  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_hal_set_radio_activity_mask command, result: 0x%2X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_hal_set_radio_activity_mask command\n\r");
  }
  
  /* SetTask CFG_TASK_CONFIGURE_FILTER_LIST_AND_NEW_ADV at application startup, if there are bonded devices, put in adv */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_CONFIGURE_FILTER_LIST_AND_NEW_ADV, CFG_SEQ_PRIO_0); 
  
  /* USER CODE END APP_BLE_Init_3 */

  /**
  * Initialize GATT Client Application
  */
  GATT_CLIENT_APP_Init();

  /* USER CODE BEGIN APP_BLE_Init_2 */
  
  /* Create timer to handle the Advertising Stop */
  bleAppContext.Advertising_mgr_timer_Id.callback = Adv_Cancel_Req;
  
  /* Create timer to handle the Led Switch OFF */
  bleAppContext.SwitchOffGPIO_timer_Id.callback = Switch_OFF_GPIO;
  
  /* For Central */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_CONNECTION_REQUEST, UTIL_SEQ_RFU, Connect_Request);
  UTIL_SEQ_RegTask( 1U << CFG_TASK_AUTO_CONNECTION_REQUEST, UTIL_SEQ_RFU, Start_Auto_Connection);
  
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
        APP_DBG_MSG(">>== HCI_DISCONNECTION_COMPLETE_EVT_CODE\n");
        APP_DBG_MSG("     - Connection Handle:   0x%02X\n     - Reason:    0x%02X\n",
                    p_disconnection_complete_event->Connection_Handle,
                    p_disconnection_complete_event->Reason);
      gap_cmd_resp_release();
      /* USER CODE BEGIN EVT_DISCONN_COMPLETE_1 */
      
      /* USER CODE END EVT_DISCONN_COMPLETE_1 */
      MC_CENTRALPERIPHERALHandleNotification.EvtOpcode = MC_CENTRALPERIPHERAL_DISCON_HANDLE_EVT;
      MC_CENTRALPERIPHERALHandleNotification.ConnectionHandle = p_disconnection_complete_event->Connection_Handle;
      MC_CENTRALPERIPHERAL_APP_EvtRx(&MC_CENTRALPERIPHERALHandleNotification);
      /* USER CODE BEGIN EVT_DISCONN_COMPLETE */
      
      /* USER CODE END EVT_DISCONN_COMPLETE */
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
      case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
        {
          hci_le_connection_complete_event_rp0 *p_conn_complete;
          p_conn_complete = (hci_le_connection_complete_event_rp0 *) p_meta_evt->data;

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
          
          /* Central Role */
          uint8_t found_status;
          
          found_status = analyse_adv_report(p_adv_report->Advertising_Report.Data_Length, p_adv_report->Advertising_Report.Data_RSSI,
                                            p_adv_report->Advertising_Report.Address_Type, p_adv_report->Advertising_Report.Address);
          if (found_status != 0)
          {
            APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_SCAN_TERMINATE);
          }
          /* USER CODE END EVT_LE_ADVERTISING_REPORT */
	  UNUSED(p_adv_report);

        }
        break;
      case HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE:
        {
          hci_le_extended_advertising_report_event_rp0 *p_ext_adv_report;
          p_ext_adv_report = (hci_le_extended_advertising_report_event_rp0 *) p_meta_evt->data;
          /* USER CODE BEGIN EVT_LE_EXT_ADVERTISING_REPORT */
          
          /* Central Role */
          uint8_t found_status;
          
          found_status = analyse_adv_report(p_ext_adv_report->Extended_Advertising_Report.Data_Length, p_ext_adv_report->Extended_Advertising_Report.Data,
                                            p_ext_adv_report->Extended_Advertising_Report.Address_Type, p_ext_adv_report->Extended_Advertising_Report.Address);
          if (found_status != 0)
          {
            APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_SCAN_TERMINATE);
          }
          /* USER CODE END EVT_LE_EXT_ADVERTISING_REPORT */
          UNUSED(p_ext_adv_report);

        }
        break;
      /* USER CODE BEGIN EVT_LE_META_EVENT_1 */
        
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

          /* Central Role */
          if ((p_gap_proc_complete->Procedure_Code == GAP_GENERAL_DISCOVERY_PROC) && (p_gap_proc_complete->Status == 0x00))
          {
            APP_DBG_MSG("-- GAP_GENERAL_DISCOVERY_PROC completed  GAP_GENERAL_DISCOVERY_PROC\n");
            UTIL_SEQ_SetTask(1U << CFG_TASK_CONNECTION_REQUEST, CFG_SEQ_PRIO_0);
          }
          if ((p_gap_proc_complete->Procedure_Code == GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC) && (p_gap_proc_complete->Status == 0x00))
          {
            APP_DBG_MSG("-- GAP_GENERAL_DISCOVERY_PROC completed  GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC\n");
            UTIL_SEQ_SetTask(1U <<  CFG_TASK_CONNECTION_REQUEST, CFG_SEQ_PRIO_0);
          }
          /* USER CODE END EVT_GAP_PROCEDURE_COMPLETE */
        }
        break;
      case ACI_HAL_END_OF_RADIO_ACTIVITY_VSEVT_CODE:
        /* USER CODE BEGIN RADIO_ACTIVITY_EVENT*/
        BSP_LED_On(LED_GREEN);
        HAL_RADIO_TIMER_StopVirtualTimer(&bleAppContext.SwitchOffGPIO_timer_Id);
        HAL_RADIO_TIMER_StartVirtualTimer(&bleAppContext.SwitchOffGPIO_timer_Id, LED_ON_TIMEOUT_MS);
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
          
          APP_DBG_MSG(">>== aci_gap_numeric_comparison_value_confirm_yesno connectionHandle=0x%04X\n", bleAppContext.BleApplicationContext_legacy.connectionHandle);
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
          
          /* 0x03 : Encryption failed, LTK missing on local device */
          if (p_pairing_complete->Status == 0x03)
          {    
            /*
            * 0x03 Encryption failed, LTK missing on local device
            * Description of the Pairing Process and Handling Pairing Failures 0x03
            *
            * 1. Peripheral in advertising general discovery:
            *    - The peripheral device starts broadcasting advertising packets to be discovered by central devices.
            *
            * 2. Pairing complete + bonding:
            *    - The pairing process is successfully completed, and the devices store the security keys (bonding).
            *
            * 3. Disconnection (central or peripheral):
            *    - The connection between the central and peripheral device is terminated.
            *
            * 4. Peripheral clears the security DB:
            *    - The peripheral device clears its security database, removing all stored pairing keys.
            *
            * 5. Peripheral in advertising general discovery:
            *    - The peripheral device goes back to advertising mode to be discovered again.
            *
            * 6. The same central that was successfully bonded tries to auto-connect to the peripheral:
            *    - The central device, which still has the stored pairing keys, attempts to automatically reconnect to the peripheral.
            *
            * 7. The central device, which still has the stored pairing keys, attempts to automatically reconnect to the peripheral:
            *    - The connection between the central and peripheral device is re-established.
            *
            * 8. The connection is established:
            *    - The connection between the central and peripheral device is re-established.
            *
            * 9. Pairing KO with Status: 0x03 (Encryption failed, LTK missing on local device) Reason: 0x00:
            *    - The pairing process fails because the peripheral device no longer has the Long Term Key (LTK) required for encryption, while the central device still believes it is bonded.
            *
            * @brief What to do in case of pairing KO:
            *
            * 10. When a pairing failure with status 0x03 (Encryption failed, LTK missing on local device) is detected:
            *     - The central device should clear its security database and retry the pairing process.
            *
            * In this section of the code, we should clear only the obsolete keys using the aci_gap_remove_bonded_device function to ensure that outdated bonding information is removed from the device.
            */
            ret = aci_gap_clear_security_db();
            if (ret != BLE_STATUS_SUCCESS)
            {
              APP_DBG_MSG("==>> aci_gap_clear_security_db - Fail, result: 0x%02X\n", ret);
            }
            else
            {
              APP_DBG_MSG("==>> aci_gap_clear_security_db - Success\n");
            }
            
            MC_CENTRALPERIPHERALHandleNotification.EvtOpcode = MC_CENTRALPERIPHERAL_PAIRING_ERROR_HANDLE_EVT;
            MC_CENTRALPERIPHERALHandleNotification.ConnectionHandle = p_pairing_complete->Connection_Handle;
            MC_CENTRALPERIPHERAL_APP_EvtRx(&MC_CENTRALPERIPHERALHandleNotification);
            
            UTIL_SEQ_SetTask( 1U << CFG_TASK_PERIPHERAL_SECURITY_REQUEST, CFG_SEQ_PRIO_0);
            
          } /* END p_pairing_complete->Status == 0x03 */
          
          
          
          /* At pairing completion, I can start the advertising again in auto connection mode with a filter. 
          * Please note that by deciding to put this restart here, we must use the pairing for our connection. 
          * To ensure that this advertising starts every time, even when the central device fails to pair due 
          * to some error, a timer must be created and linked with the action to restart the advertising. 
          * Pairing KO due to the timeout is included in the covered restart advertising.
          */
          
          APP_DBG_MSG(">>== ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE connectionHandle=0x%04X    --> MC_CENTRALPERIPHERAL_PAIRING_SUCCESS_HANDLE_EVT\n", p_pairing_complete->Connection_Handle);
          MC_CENTRALPERIPHERALHandleNotification.EvtOpcode = MC_CENTRALPERIPHERAL_PAIRING_SUCCESS_HANDLE_EVT;
          MC_CENTRALPERIPHERALHandleNotification.ConnectionHandle = p_pairing_complete->Connection_Handle;
          MC_CENTRALPERIPHERAL_APP_EvtRx(&MC_CENTRALPERIPHERALHandleNotification);
          
          
          /* USER CODE END ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE*/
        }
        break;
      case ACI_GATT_SRV_READ_VSEVT_CODE :
        {
          APP_DBG_MSG(">>== ACI_GATT_SRV_READ_VSEVT_CODE\n");

          aci_gatt_srv_read_event_rp0    *p_read;
          p_read = (aci_gatt_srv_read_event_rp0*)p_blecore_evt->data;
          uint8_t error_code = BLE_ATT_ERR_INSUFF_AUTHORIZATION;

          APP_DBG_MSG("Handle 0x%04X\n",  p_read->Attribute_Handle);

          /* USER CODE BEGIN ACI_GATT_SRV_READ_VSEVT_CODE_1*/
          
          /* USER CODE END ACI_GATT_SRV_READ_VSEVT_CODE_1*/

          aci_gatt_srv_resp(p_read->Connection_Handle,
                            p_read->CID,
                            p_read->Attribute_Handle,
                            error_code,
                            0,
                            NULL);

          /* USER CODE BEGIN ACI_GATT_SRV_READ_VSEVT_CODE_2*/
          
          /* USER CODE END ACI_GATT_SRV_READ_VSEVT_CODE_2*/
          break;
        }
        /* USER CODE BEGIN EVT_VENDOR_1 */
        
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

      if (p_hci_hardware_error_event->Hardware_Code <= 0x03)
      {
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
  if (Role == HCI_ROLE_PERIPHERAL)
  {
    APP_DBG_MSG(">>== hci_le_connection_complete_event - Role: HCI_ROLE_PERIPHERAL\n");
    
    /* The connection is done, there is no need anymore to schedule the LP ADV */
    HAL_RADIO_TIMER_StopVirtualTimer(&bleAppContext.Advertising_mgr_timer_Id);
  }
  else
  {
    APP_DBG_MSG(">>== hci_le_connection_complete_event - Role: HCI_ROLE_CENTRAL\n");
  }
  /* USER CODE END HCI_EVT_LE_CONN_COMPLETE_1 */
  APP_DBG_MSG(">>== hci_le_connection_complete_event - Connection handle: 0x%04X\n", Connection_Handle);
  APP_DBG_MSG("     - Connection established with @:%02X:%02X:%02X:%02X:%02X:%02X\n",
              Peer_Address[5],
              Peer_Address[4],
              Peer_Address[3],
              Peer_Address[2],
              Peer_Address[1],
              Peer_Address[0]);
   APP_DBG_MSG("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout: %d ms\n",
               INT(Conn_Interval*1.25),
               FRACTIONAL_2DIGITS(Conn_Interval*1.25),
               Peripheral_Latency,
               Supervision_Timeout * 10
                 );

  bleAppContext.BleApplicationContext_legacy.connectionHandle = Connection_Handle;

  if (Role == HCI_ROLE_PERIPHERAL)
  {
    MC_CENTRALPERIPHERALHandleNotification.EvtOpcode = MC_CENTRALPERIPHERAL_PERIPH_CONN_HANDLE_EVT;
  }
  else /* HCI_ROLE_CENTRAL */
  {
    MC_CENTRALPERIPHERALHandleNotification.EvtOpcode = MC_CENTRALPERIPHERAL_CENTR_CONN_HANDLE_EVT;
  }

  MC_CENTRALPERIPHERALHandleNotification.ConnectionHandle = Connection_Handle;

  MC_CENTRALPERIPHERAL_APP_EvtRx(&MC_CENTRALPERIPHERALHandleNotification);

  /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */
  if (Role == HCI_ROLE_PERIPHERAL)
  { 
    /* Connection as Peripheral */
    bleAppContext.Device_Connection_Status_Peripheral = APP_BLE_CONNECTED_SERVER;
    
    UTIL_SEQ_SetTask(1U << CFG_TASK_PERIPHERAL_SECURITY_REQUEST, CFG_SEQ_PRIO_0);
  }
  else
  {
    APP_DBG_MSG(">>== hci_le_connection_complete_event - Role: HCI_ROLE_CENTRAL\n");
    if (bleAppContext.Device_Connection_Status_Central == APP_BLE_LP_CONNECTING || bleAppContext.Device_Connection_Status_Central == APP_BLE_LP_CONNECTING_AUTO_CONNECTION)
    {
      /* Connection as Central */
      bleAppContext.Device_Connection_Status_Central = APP_BLE_CONNECTED_CLIENT;
      
      APP_DBG_MSG("HCI_EVT_LE_CONN_COMPLETE \n");
      UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);
      
      /* Ste the event CFG_IDLEEVT_CONNECTION_COMPLETE for the Connect_Request() function. */
      APP_DBG_MSG("Ste the event CFG_IDLEEVT_CONNECTION_COMPLETE for the Connect_Request() function.\n");
      UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_CONNECTION_COMPLETE);
    }
    
  }
  
  /* USER CODE END HCI_EVT_LE_CONN_COMPLETE */
}/* end hci_le_connection_complete_event() */

/* USER CODE BEGIN EVT_VENDOR_3 */

/* USER CODE END EVT_VENDOR_3 */

APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status(void)
{
  return bleAppContext.Device_Connection_Status_Peripheral;
}

APP_BLE_ConnStatus_t APP_BLE_Get_Client_Connection_Status(uint16_t Connection_Handle)
{
  return bleAppContext.Device_Connection_Status_Central;
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

void APP_BLE_Procedure_Gap_Peripheral(ProcGapPeripheralId_t ProcGapPeripheralId)
{
  tBleStatus status;
  uint32_t paramA = ADV_INTERVAL_MIN;
  uint32_t paramB = ADV_INTERVAL_MAX;
  uint32_t paramC, paramD;

  /* First set parameters before calling ACI APIs, only if needed */
  switch(ProcGapPeripheralId)
  {
    case PROC_GAP_PERIPH_ADVERTISE_START_FAST:
    {
      paramA = ADV_INTERVAL_MIN;
      paramB = ADV_INTERVAL_MAX;
      paramC = APP_BLE_ADV_FAST;

      /* USER CODE BEGIN PROC_GAP_PERIPH_ADVERTISE_START_FAST */
      
      /* USER CODE END PROC_GAP_PERIPH_ADVERTISE_START_FAST */
      break;
    }/* PROC_GAP_PERIPH_ADVERTISE_START_FAST */
    case PROC_GAP_PERIPH_ADVERTISE_START_LP:
    {
      paramA = ADV_LP_INTERVAL_MIN;
      paramB = ADV_LP_INTERVAL_MAX;
      paramC = APP_BLE_ADV_LP;

      /* USER CODE BEGIN PROC_GAP_PERIPH_ADVERTISE_START_LP */
      
      /* USER CODE END PROC_GAP_PERIPH_ADVERTISE_START_LP */
      break;
    }/* PROC_GAP_PERIPH_ADVERTISE_START_LP */
    case PROC_GAP_PERIPH_ADVERTISE_STOP:
    {
      paramC = APP_BLE_IDLE;

      /* USER CODE BEGIN PROC_GAP_PERIPH_ADVERTISE_STOP */
      
      /* USER CODE END PROC_GAP_PERIPH_ADVERTISE_STOP */
      break;
    }/* PROC_GAP_PERIPH_ADVERTISE_STOP */
    case PROC_GAP_PERIPH_CONN_PARAM_UPDATE:
    {
      paramA = CONN_INT_MS(1000);
      paramB = CONN_INT_MS(1000);
      paramC = 0x0000;
      paramD = 0x01F4;

      /* USER CODE BEGIN CONN_PARAM_UPDATE */
      if (bleAppContext.connIntervalFlag != 0)
      {
        bleAppContext.connIntervalFlag = 0;
        paramA = CONN_INT_MS(50);
        paramB = CONN_INT_MS(50);
      }
      else
      {
        bleAppContext.connIntervalFlag = 1;
        paramA = CONN_INT_MS(1000);
        paramB = CONN_INT_MS(1000);
      }
      /* USER CODE END CONN_PARAM_UPDATE */
      break;
    }/* PROC_GAP_PERIPH_CONN_PARAM_UPDATE */
    case PROC_GAP_PERIPH_CONN_TERMINATE:
    {
      status = aci_gap_terminate(bleAppContext.BleApplicationContext_legacy.connectionHandle, 0x13);
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
    }
    /* PROC_GAP_PERIPH_CONN_TERMINATE */
    /* USER CODE BEGIN ProcGapPeripheralId */

    /* USER CODE END ProcGapPeripheralId */
    default:
      break;
  }

  /* Call ACI APIs */
  switch(ProcGapPeripheralId)
  {
    case PROC_GAP_PERIPH_ADVERTISE_START_FAST:
    case PROC_GAP_PERIPH_ADVERTISE_START_LP:
    {

      /* USER CODE BEGIN PROC_GAP_PERIPHERAL_ID */
      a_AdvData[1] = AD_TYPE_FLAGS;
      a_AdvData[2] = FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE|FLAG_BIT_BR_EDR_NOT_SUPPORTED;
      /* USER CODE END PROC_GAP_PERIPHERAL_ID */

      Advertising_Set_Parameters_t Advertising_Set_Parameters = {0};

      /* Start Fast or Low Power Advertising */

      /* Set advertising configuration for legacy advertising */
      status = aci_gap_set_advertising_configuration(0,
                                                     GAP_MODE_GENERAL_DISCOVERABLE,
                                                     ADV_TYPE,
                                                     paramA,
                                                     paramB,
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
        bleAppContext.Device_Connection_Status_Peripheral = (APP_BLE_ConnStatus_t)paramC;
        APP_DBG_MSG("==>> Success: aci_gap_set_advertising_configuration\n");
      }

      status = aci_gap_set_advertising_data(0, ADV_COMPLETE_DATA, sizeof(a_AdvData), (uint8_t*) a_AdvData);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("==>> aci_gap_set_advertising_data Failed, result: 0x%02X\n", status);
      }
      else
      {
        APP_DBG_MSG("==>> Success: aci_gap_set_advertising_data\n");
      }

      /* Enable advertising */
      status = aci_gap_set_advertising_enable(ENABLE, 1, &Advertising_Set_Parameters);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("==>> aci_gap_set_advertising_enable Failed, result: 0x%02X\n", status);
      }
      else
      {
        APP_DBG_MSG("==>> Success: aci_gap_set_advertising_enable\n");
      }
      break;
    }
    case PROC_GAP_PERIPH_ADVERTISE_STOP:
    {
      status = aci_gap_set_advertising_enable(DISABLE, 0, NULL);
      if (status != BLE_STATUS_SUCCESS)
      {
        bleAppContext.Device_Connection_Status_Peripheral = (APP_BLE_ConnStatus_t)paramC;
        APP_DBG_MSG("Disable advertising - fail, result: 0x%02X\n",status);
      }
      else
      {
        APP_DBG_MSG("==>> Disable advertising - Success\n");
      }
      break;
    }/* PROC_GAP_PERIPH_ADVERTISE_STOP */
    case PROC_GAP_PERIPH_CONN_PARAM_UPDATE:
    {
       status = aci_l2cap_connection_parameter_update_req(
                                                       bleAppContext.BleApplicationContext_legacy.connectionHandle,
                                                       paramA,
                                                       paramB,
                                                       paramC,
                                                       paramD);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("aci_l2cap_connection_parameter_update_req - fail, result: 0x%02X\n",status);
      }
      else
      {
        APP_DBG_MSG("==>> aci_l2cap_connection_parameter_update_req - Success\n");
      }

      break;
    }/* PROC_GAP_PERIPH_CONN_PARAM_UPDATE */

    case PROC_GAP_PERIPH_SET_BROADCAST_MODE:
    {

      break;
    }/* PROC_GAP_PERIPH_SET_BROADCAST_MODE */
    /* USER CODE BEGIN ProcGapPeripheralId */

    /* USER CODE END ProcGapPeripheralId */
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

      break;
    }/* PROC_GAP_CENTRAL_SCAN_START */
    case PROC_GAP_CENTRAL_SCAN_TERMINATE:
    {
      paramA = 1;
      paramB = 1;
      paramC = APP_BLE_IDLE;

      break;
    }/* PROC_GAP_CENTRAL_SCAN_TERMINATE */
    /* USER CODE BEGIN ProcGapCentralId */

    /* USER CODE END ProcGapCentralId */
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
        bleAppContext.Device_Connection_Status_Central = (APP_BLE_ConnStatus_t)paramC;
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
        bleAppContext.Device_Connection_Status_Central = (APP_BLE_ConnStatus_t)paramC;
        APP_DBG_MSG("==>> aci_gap_terminate_gap_proc - Success\n");
      }
      break;
    }/* PROC_GAP_CENTRAL_SCAN_TERMINATE */
    /* USER CODE BEGIN ProcGapCentralId */

    /* USER CODE END ProcGapCentralId */
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


/**
* @brief Request to cancel advertising linked to the timer.
*
* This function sets a task to cancel the current advertising and restart it 
* in non-discoverable mode with an active filter.
*
* @param arg Pointer to the argument (not used in this function).
*
*/
static void Adv_Cancel_Req(void *arg)
{
  APP_DBG_MSG("Adv_Cancel_Req\n");
  Adv_Cancel();
  /* Restart advertising in non-discoverable mode with filter */
  UTIL_SEQ_SetTask( 1U << CFG_TASK_CONFIGURE_FILTER_LIST_AND_NEW_ADV, CFG_SEQ_PRIO_0); 
  return;
}

/**
* @brief Switch off the GPIO (LED).
*
* This function turns off the green LED.
*
* @param arg Pointer to the argument (not used in this function).
*/
static void Switch_OFF_GPIO(void *arg)
{
  BSP_LED_Off(LED_GREEN);
  return;
}

/**
* @brief Cancel the advertising process.
*
* This function stops the advertising process and updates the device connection
* status to idle. It also turns off the green LED to indicate that advertising
* has been stopped.
*
* @note This function is specific to CFG_TASK_ADV_CANCEL_ID.
*/
static void Adv_Cancel(void)
{
  BSP_LED_Off(LED_GREEN);
  APP_BLE_Procedure_Gap_Peripheral(PROC_GAP_PERIPH_ADVERTISE_STOP);
  bleAppContext.Device_Connection_Status_Peripheral = APP_BLE_IDLE; 
  return;
}


/**
* @brief Configures the filter list and sets up advertising with the filter list enabled.
* 
* Specific implementation for CFG_TASK_CONFIGURE_FILTER_LIST_AND_NEW_ADV.
* This function handles the configuration of the filter list and the advertising parameters.
* It performs the following steps:
* - Adds the bonded device identity address and type to the controller's filter list and the list of address translations used to resolve resolvable private addresses.
* - Sets the device privacy mode for each bonded device.
* - Configures the advertising parameters to use the filter list for scan and connection requests.
* - Sets the advertising data and scan response data.
* - Enables advertising with the configured parameters.
* 
* @retval None
*/
static void Configure_Filter_List(void)
{
  uint8_t status = 0;
  Advertising_Set_Parameters_t Advertising_Set_Parameters = {0};
  
  if(bleAppContext.Device_Connection_Status_Peripheral == APP_BLE_ADV_FAST_NON_DISC_WITH_FILTER)
  {
    APP_DBG_MSG(" (FILTER) Advertising is already running...\n");
    return;
  }    
  
  if(bleAppContext.Device_Connection_Status_Peripheral == APP_BLE_ADV_FAST || bleAppContext.Device_Connection_Status_Peripheral == APP_BLE_ADV_LP)
  {
    APP_DBG_MSG(" Advertising is already running...\n");
    return;
  }    
  
  if(APP_Context_canStartAdvertising() == -1)
  {
    APP_DBG_MSG("  (FILTER) Maximum number of central connections reached. Advertising (FILTER) cannot be started.\n");
    return;
  }
  
  /**
  * Add the bonded device identity address and type to the Bluetooth Low Energy device controller filter list and 
  * to the list of address translations used to resolve resolvable private addresses in the controller
  */
  status = aci_gap_configure_filter_accept_and_resolving_list(0x01 | 0x02);
  
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gap_configure_filter_accept_and_resolving_list(), error:0x%02X\n", status);
    return;
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gap_configure_filter_accept_and_resolving_list\n");
    
    /* Check if there are any bonded devices */
    if (Get_Num_Bonded_Devices() == 0)
    {
      APP_DBG_MSG("No bonded device found - No Advertising with filter list started.\n");
      return;
    }
    
    Print_Bonded_Devices();
    
    
    /* Configure advertising for filtering */
    status = aci_gap_set_advertising_configuration(0,
                                                   GAP_MODE_NON_DISCOVERABLE,
                                                   ADV_TYPE,
                                                   ADV_INTERVAL_MIN,
                                                   ADV_INTERVAL_MAX,
                                                   HCI_ADV_CH_ALL,
                                                   0,
                                                   NULL, /* No peer address */
                                                   HCI_ADV_FILTER_ACCEPT_LIST_SCAN_CONNECT,
                                                   0, /* 0 dBm */
                                                   HCI_PHY_LE_1M, /* Primary advertising PHY */
                                                   0, /* 0 skips */
                                                   HCI_PHY_LE_1M, /* Secondary advertising PHY. Not used with legacy advertising. */
                                                   0, /* SID */
                                                   0 /* No scan request notifications */);
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("==>> aci_gap_set_advertising_configuration (GAP_MODE_NON_DISCOVERABLE) - fail, result: 0x%02X\n", status);
      return;
    }
    else
    {
      bleAppContext.Device_Connection_Status_Peripheral = APP_BLE_ADV_FAST_NON_DISC_WITH_FILTER;
      APP_DBG_MSG("==>> Success: aci_gap_set_advertising_configuration (GAP_MODE_NON_DISCOVERABLE)\n");
    }
    
    /* Set the advertising data */
    a_AdvData[2] = FLAG_BIT_BR_EDR_NOT_SUPPORTED;
    status = aci_gap_set_advertising_data(0, ADV_COMPLETE_DATA, 0x3, (uint8_t*) a_AdvData);
    APP_DBG_MSG("a_AdvData[2] = FLAG_BIT_BR_EDR_NOT_SUPPORTED;\n");
    
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("==>> aci_gap_set_advertising_data Failed (FILTER), result: 0x%02X\n", status);
      return;
    }
    else
    {
      APP_DBG_MSG("==>> Success: aci_gap_set_advertising_data (FILTER)\n");
    }
    
    status = aci_gap_set_advertising_enable(ENABLE, 1, &Advertising_Set_Parameters);
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("==>> aci_gap_set_advertising_enable Failed (FILTER), result: 0x%02X\n", status);
    }
    else
    {
      APP_DBG_MSG("==>> Success: aci_gap_set_advertising_enable (FILTER)\n");
    }
  }
  
  return;
}

/**
* @brief Fill the advertising data with specific information.
*
* This function fills the advertising data with specific information such as the
* manufacturer-specific data and the complete local name. It also converts parts
* of the Bluetooth device address into ASCII characters and inserts them into the
* advertising data.
*
* @param p_adv_data Pointer to the advertising data array.
* @param tab_size Size of the advertising data array.
* @param p_bd_addr Pointer to the Bluetooth device address.
*/
static void fill_advData(uint8_t *p_adv_data, uint8_t tab_size, const uint8_t* p_bd_addr)
{
  uint16_t i =0;
  uint8_t bd_addr_1, bd_addr_0;
  uint8_t ad_length, ad_type;  
  
  while(i < tab_size)
  {
    ad_length = p_adv_data[i];
    ad_type = p_adv_data[i + 1];
    
    switch (ad_type)
    {
    case AD_TYPE_FLAGS:
      break;
    case AD_TYPE_TX_POWER_LEVEL:
      break;
    case AD_TYPE_COMPLETE_LOCAL_NAME:
      {
        if((p_adv_data[i + ad_length] == 'X') && (p_adv_data[i + ad_length - 1] == 'X'))
        {
          bd_addr_1 = ((p_bd_addr[0] & 0xF0)>>4);
          bd_addr_0 = (p_bd_addr[0] & 0xF);
          
          /* Convert hex value into ascii */
          if(bd_addr_1 > 0x09)
          {
            p_adv_data[i + ad_length - 1] = bd_addr_1 + '7';
          }
          else
          {
            p_adv_data[i + ad_length - 1] = bd_addr_1 + '0';
          }
          
          if(bd_addr_0 > 0x09)
          {
            p_adv_data[i + ad_length] = bd_addr_0 + '7';
          }
          else
          {
            p_adv_data[i + ad_length] = bd_addr_0 + '0';
          }
        }
        break;
      }
    case AD_TYPE_MANUFACTURER_SPECIFIC_DATA:
      {
        p_adv_data[i+2] = ST_MANUF_ID_LSB;
        p_adv_data[i+3] = ST_MANUF_ID_MSB;
        p_adv_data[i+4] = BLUESTSDK_V2; /* blueST SDK version */
        p_adv_data[i+5] = BOARD_ID_NUCLEO_WB0; /* Board ID */
        p_adv_data[i+6] = FW_ID_MC_PERIPHERAL; /* FW ID */
        p_adv_data[i+7] = FW_ID_MC_PERIPHERAL_D1; /* FW data 1 */
        p_adv_data[i+8] = FW_ID_MC_PERIPHERAL_D2; /* FW data 2 */
        p_adv_data[i+9] = FW_ID_MC_PERIPHERAL_D3; /* FW data 3 */
        p_adv_data[i+10] = p_bd_addr[5]; /* MSB BD address */
        p_adv_data[i+11] = p_bd_addr[4];
        p_adv_data[i+12] = p_bd_addr[3];
        p_adv_data[i+13] = p_bd_addr[2];
        p_adv_data[i+14] = p_bd_addr[1];
        p_adv_data[i+15] = p_bd_addr[0]; /* LSB BD address */
        break;
      }
    default:
      break;
    }
    i += ad_length + 1; /* increment the iterator to go on next element*/
  }
}

/**
* @brief Analyse the advertising report to find a specific peripheral.
*
* This function parses the advertising data to identify a specific peripheral
* based on manufacturer-specific data. If the peripheral is found, its address
* and address type are stored in the application context.
*
* @param adv_data_size Size of the advertising data.
* @param p_adv_data Pointer to the advertising data.
* @param address_type Type of the address (public or random).
* @param p_address Pointer to the address of the advertising device.
* @return uint8_t Returns 1 if the specific peripheral is found, otherwise 0.
*/
static uint8_t analyse_adv_report(uint8_t adv_data_size, uint8_t *p_adv_data,
                                  uint8_t address_type, uint8_t *p_address)
{
  uint8_t found_status = 0;
  uint16_t i = 0;
  uint8_t ad_length, ad_type;
  
  while(i < adv_data_size)
  {
    ad_length = p_adv_data[i];
    ad_type = p_adv_data[i + 1];
    
    switch (ad_type)
    {
    case AD_TYPE_FLAGS:
      break;
    case AD_TYPE_TX_POWER_LEVEL:
      break;
    case AD_TYPE_MANUFACTURER_SPECIFIC_DATA:
      {
        if ((ad_length >= 7)                              && 
            (p_adv_data[i + 2] == ST_MANUF_ID_LSB)        &&
              (p_adv_data[i + 3] == ST_MANUF_ID_MSB)        &&
                (p_adv_data[i + 4] == BLUESTSDK_V2)           &&
                  (p_adv_data[i + 6] == FW_ID_MC_PERIPHERAL)    &&
                    (p_adv_data[i + 7] == FW_ID_MC_PERIPHERAL_D1) &&
                      (p_adv_data[i + 8] == FW_ID_MC_PERIPHERAL_D2) &&
                        (p_adv_data[i + 9] == FW_ID_MC_PERIPHERAL_D3))
        {
          /* Peripheral BlueST v2 detected */
          bleAppContext.deviceServerBdAddrType = address_type;
          memcpy(bleAppContext.a_deviceServerBdAddr, p_address, BD_ADDR_SIZE);
          
          bleAppContext.deviceServerFound = 0x01;
          
          APP_DBG_MSG("  Peripheral, db addr 0x%02X:%02X:%02X:%02X:%02X:%02X\n",
                      bleAppContext.a_deviceServerBdAddr[5],
                      bleAppContext.a_deviceServerBdAddr[4],
                      bleAppContext.a_deviceServerBdAddr[3],
                      bleAppContext.a_deviceServerBdAddr[2],
                      bleAppContext.a_deviceServerBdAddr[1],
                      bleAppContext.a_deviceServerBdAddr[0]);
          found_status = 1;
        }
        break;
      }
    default:
      break;
    }/* end of switch*/
    
    i += ad_length + 1; /* increment the iterator to go on next element*/
    
    if (found_status != 0)
    {
      break;
    }
  }/* end of while*/
  
  return found_status;
}


/**
* @brief Initiate a connection request to a discovered BLE peripheral.
*
* This function attempts to create a connection to a discovered BLE peripheral device.
* It first sets the connection configuration and then attempts to create the connection.
* If the connection is successfully created, the connection handle is set.
*
* @note This function is specific to CFG_TASK_CONNECTION_REQUEST.
*/
static void Connect_Request(void)
{
  tBleStatus result;
  APP_DBG_MSG("Connect_Request() begin\n");
  
  if (bleAppContext.deviceServerFound != 0)
  {
    
    bleAppContext.deviceServerFound = 0x00;
    
    APP_DBG_MSG("Create connection to Peripheral\n");
    
    result = aci_gap_set_connection_configuration(LE_1M_PHY_BIT,
                                                  CONN_INT_MS(50u), CONN_INT_MS(100u),
                                                  0u,
                                                  CONN_SUP_TIMEOUT_MS(5000u),
                                                  CONN_CE_LENGTH_MS(10u), CONN_CE_LENGTH_MS(10u));
    
    if (result == BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("==>> aci_gap_set_connection_configuration Success , result: 0x%02X\n", result);
    }
    else
    {
      APP_DBG_MSG("==>> aci_gap_set_connection_configuration Failed , result: 0x%02X\n", result);
    }  
    result = aci_gap_create_connection(LE_1M_PHY_BIT,
                                       bleAppContext.deviceServerBdAddrType,
                                       &bleAppContext.a_deviceServerBdAddr[0]);    
    if (result == BLE_STATUS_SUCCESS)
    {
      bleAppContext.Device_Connection_Status_Central = APP_BLE_LP_CONNECTING;
      APP_DBG_MSG("  wait for event HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE\n");
      UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_CONNECTION_COMPLETE);
      APP_DBG_MSG("  received event HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE\n");
      bleAppContext.Device_Connection_Status_Central = APP_BLE_IDLE;
    }
    else
    {
      APP_DBG_MSG("==>> GAP Create connection Failed , result: 0x%02X\n", result);
      bleAppContext.Device_Connection_Status_Central = APP_BLE_IDLE;
    }
  }
  else
  {
    APP_DBG_MSG("--Connect_Request() Device Server Not Found\n");
    bleAppContext.Device_Connection_Status_Central = APP_BLE_IDLE;
  }
  
  APP_DBG_MSG("--Connect_Request() end \n");
  return;
}


/**
* @brief  Initiates the auto connection establishment procedure if bonded devices are found.
* 
* This function checks immediately if there are any bonded devices (in this scenario, peripheral bonded devices could be found).
* If the check is true, it means that a connection with a peripheral was previously established, and the auto connection establishment
* procedure can be started correctly.
* Handles the action for CFG_TASK_AUTO_CONNECTION_REQUEST event.
* 
* @retval None
*/
static void Start_Auto_Connection(void)
{  
  uint32_t status, paramA = SCAN_INT_MS(500), paramB = SCAN_WIN_MS(500);

  if (bleAppContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode)
  {
    status = aci_gap_configure_filter_accept_and_resolving_list(0x01);
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("  Fail   : (FILTER) aci_gap_configure_filter_accept_and_resolving_list command, result: 0x%02X\n", status);
    }
    else
    {
      APP_DBG_MSG("  Success: (FILTER) aci_gap_configure_filter_accept_and_resolving_list command\n");
    }
  }
  
  status = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, 0x00, LE_1M_PHY_BIT, HCI_SCAN_TYPE_ACTIVE, paramA, paramB);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("(FILTER) aci_gap_set_scan_configuration - fail, result: 0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> (FILTER) aci_gap_set_scan_configuration - Success\n");
  }
  
  status = aci_gap_set_connection_configuration(LE_1M_PHY_BIT,
                                                CONN_INT_MS(50u), 
                                                CONN_INT_MS(100u),
                                                0u,
                                                CONN_SUP_TIMEOUT_MS(5000u),
                                                CONN_CE_LENGTH_MS(10u), 
                                                CONN_CE_LENGTH_MS(10u));
  
  if (status == BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("==>> (FILTER) aci_gap_set_connection_configuration Success , result: 0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> (FILTER) aci_gap_set_connection_configuration Failed , result: 0x%02X\n", status);
  }  
  
  /* Start the auto connection establishment procedure */
  status = aci_gap_start_procedure(GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC, LE_1M_PHY_BIT, 0, 0);
  
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("(FILTER) aci_gap_start_procedure GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC - fail, result: 0x%02X\n", status);
  }
  else
  {
    bleAppContext.Device_Connection_Status_Central = APP_BLE_LP_CONNECTING_AUTO_CONNECTION;
    APP_DBG_MSG("==>> (FILTER) aci_gap_start_procedure GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC - Success\n");
  }
  
  return;
}


/**
* @brief  Prints the list of bonded devices.
* 
* This function retrieves and prints the list of bonded devices. It iterates through all bonded devices
* and prints their address type and address. If no bonded devices are found, it prints a message indicating
* that there are no bonded devices.
* 
* @retval None
*/
void Print_Bonded_Devices(void)
{
  Bonded_Device_Entry_t bonded_device;
  uint8_t num_returned_devices;
  uint8_t num_bonded_devices = 0;
  
  APP_DBG_MSG("Bonded devices:\n");
  
  for(int i = 0;; i++) {  
    aci_gap_get_bonded_devices(i, 1, &num_returned_devices, &bonded_device);
    
    if(num_returned_devices)
    {
      APP_DBG_MSG("Type %d, @:%02X:%02X:%02X:%02X:%02X:%02X\n", bonded_device.Address_Type, bonded_device.Address[5], bonded_device.Address[4], bonded_device.Address[3], bonded_device.Address[2], bonded_device.Address[1], bonded_device.Address[0]);
    }
    else {
      break;
    }    
    num_bonded_devices += num_returned_devices;    
  }
  
  if(num_bonded_devices == 0){
    
    APP_DBG_MSG("No bonded devices.\n");
  }
}

/**
* @brief  Retrieves the number of bonded devices.
* 
* This function iterates through all bonded devices and counts the total number of bonded devices.
* 
* @retval uint8_t  The total number of bonded devices.
*/
uint8_t Get_Num_Bonded_Devices(void)
{
  Bonded_Device_Entry_t bonded_device;
  uint8_t num_returned_devices;
  uint8_t num_bonded_devices = 0;
  
  for(int i = 0;; i++) {  
    aci_gap_get_bonded_devices(i, 1, &num_returned_devices, &bonded_device);
    
    if(num_returned_devices == 0){
      break;
    }
    num_bonded_devices += num_returned_devices;    
  }
  
  return num_bonded_devices;
}

/* USER CODE END FD_LOCAL_FUNCTION */

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */



#if (CFG_BUTTON_SUPPORTED == 1)

/**
* @brief  Handles the action for TASK_BUTTON_1 press event.
* 
* This function is called when Button 1 (B1) is pressed. It attempts to start advertising. 
* The function performs the following steps:
* - Checks if there are available slots to accept new central connections.
* - If the device is currently advertising in auto connection establishment mode, it stops this advertising.
* - If the device is idle, it starts advertising in general mode without a filter list to accept new central devices.
* - If advertising is already running, it prints a message indicating that advertising is already active.
* - A timer is added to stop advertising after a specified timeout period.
* 
* @retval None
*/
void APPE_Button1Action(void)
{
  /* Check if there are available slots to start advertising. If the maximum number of central connections is reached, print a message and exit. */
  if(APP_Context_canStartAdvertising() == -1)
  {
    APP_DBG_MSG("  Maximum number of Central connections reached. Advertising cannot be started.\n");
    return;
  }
  
  /* If the device is currently advertising in auto connection establishment mode, stop this advertising to allow starting advertising in general mode. */
  if (bleAppContext.Device_Connection_Status_Peripheral == APP_BLE_ADV_FAST_NON_DISC_WITH_FILTER)
  {
    Adv_Cancel();
  }
  
  /* If advertising is already running, print a message indicating that advertising is already active. */
  if (bleAppContext.Device_Connection_Status_Peripheral == APP_BLE_ADV_FAST || bleAppContext.Device_Connection_Status_Peripheral == APP_BLE_ADV_LP)
  {
    APP_DBG_MSG("  Advertising is already running...\n");
    return;
  }
  
  /* If the device is idle, on connected as a server with at least one radio task free, start advertising in general mode to accept new central connections. Start a timer to stop advertising after a specified timeout period. */
  if (bleAppContext.Device_Connection_Status_Peripheral == APP_BLE_IDLE || bleAppContext.Device_Connection_Status_Peripheral == APP_BLE_CONNECTED_SERVER)
  {
    APP_BLE_Procedure_Gap_Peripheral(PROC_GAP_PERIPH_ADVERTISE_START_FAST);
    HAL_RADIO_TIMER_StartVirtualTimer(&bleAppContext.Advertising_mgr_timer_Id, ADV_TIMEOUT_MS);
    return;
  }
  
  return;
}

/**
* @brief  Handles the action for TASK_BUTTON_2 press event.
* 
* This function is called when Button 2 (B2) is pressed. It performs the following steps:
* - Checks if there are any active central connections by counting the total peripheral entities stored in the application context.
* - If the count is greater than zero, it sends notifications to all connected devices.
* - If no active connections are found, it clears the security database.
* 
* @retval None
*/
void APPE_Button2Action(void)
{
  /**
  * To understand if there are central devices connected, count the total peripheral entities stored in the application context.
  * Remember that in this case of notification, the application is working as a peripheral, so correctly check if there are peripheral entities stored in the app context of the peripheral.
  * If the count is greater than zero, notifications can be sent to all connected devices.
  */
  if(APP_Context_getActiveConnectionCountPeripheral() > 0)
  {
    APP_DBG_MSG("  Start Notification\n");
    UTIL_SEQ_SetTask(1U << CFG_TASK_SEND_NOTIF_ID, CFG_SEQ_PRIO_0);
  }
  else /** Otherwise, clear the security database */
  {
    tBleStatus ret = aci_gap_clear_security_db();
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("==>> aci_gap_clear_security_db - Fail, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("==>> aci_gap_clear_security_db - Success\n");
    }
  }
  return;
}

/**
* @brief  Handles the action for TASK_BUTTON_3 press event.
* 
* This function is called when Button 3 (B3) is pressed. If the device is connected as a server,
* it terminates all current BLE connections for peripheral entities.
* 
* @retval None
*/
void APPE_Button3Action(void)
{
  UTIL_SEQ_SetTask( 1U << CFG_TASK_TERMINATE_ALL_PERIPHERAL_CONNECTIONS, CFG_SEQ_PRIO_0);
  return;
}

/**
* @brief  Handles the action for TASK_BUTTON_1_LONG_PRESS press event.
* 
* This function is called when B1 is long pressed. It starts a GAP central scan procedure
* if the device is not connected as a client. If the device is connected as a client,
* it sets a task to write a characteristic.
* 
* @retval None
*/
void APPE_Button1LongPressAction(void)
{
  /* Check if there are available slots to start scanning. If the maximum number of connections (as Central) is reached, print a message and exit. */
  if(APP_Context_canStartScanning() == -1)
  {
    APP_DBG_MSG("  Maximum number of connections (with the role of Central) reached. Scanning cannot be started.\n");
    return;
  }
  if(bleAppContext.Device_Connection_Status_Central == APP_BLE_LP_CONNECTING_AUTO_CONNECTION)
  {
    APP_DBG_MSG("APP_BLE_LP_CONNECTING_AUTO_CONNECTION is running\n");
    
    uint8_t status = aci_gap_terminate_proc(GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC);
    if (status != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("aci_gap_terminate_gap_proc - GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC - fail, result: 0x%02X\n",status);
    }
    else
    {
      bleAppContext.Device_Connection_Status_Central = APP_BLE_IDLE;
      APP_DBG_MSG("==>> aci_gap_terminate_gap_proc - GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC - Success\n");
    }
  }
  if (bleAppContext.Device_Connection_Status_Central == APP_BLE_IDLE)
  {
    /* Call the function to start the scan procedure */
    APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_SCAN_START);
  }
  return;
}

void APPE_Button2LongPressAction(void)
{
  if(APP_Context_getActiveConnectionCountCentral() > 0)
  {
    UTIL_SEQ_SetTask( 1U << CFG_TASK_WRITE_ALL_CHAR_ID, CFG_SEQ_PRIO_0);
  }
  else
  {
    tBleStatus ret = aci_gap_clear_security_db();
    if (ret != BLE_STATUS_SUCCESS)
    {
      APP_DBG_MSG("==>> aci_gap_clear_security_db - Fail, result: 0x%02X\n", ret);
    }
    else
    {
      APP_DBG_MSG("==>> aci_gap_clear_security_db - Success\n");
    }
  }
  return;
}

/**
* @brief  Handles the action for TASK_BUTTON_3_LONG_PRESS press event.
* 
* This function is called when Button 3 (B3) is long pressed. It terminates all current BLE connections
* for all entity connected as a central.
* 
* @retval None
*/
void APPE_Button3LongPressAction(void)
{
  UTIL_SEQ_SetTask( 1U << CFG_TASK_TERMINATE_ALL_CENTRAL_CONNECTIONS, CFG_SEQ_PRIO_0);
  return;
}
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
/* USER CODE END FD_WRAP_FUNCTIONS */

/** \endcond
 */
