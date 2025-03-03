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
  uint8_t deviceServerFound;
  uint8_t deviceServerBdAddrType;
  uint8_t a_deviceServerBdAddr[BD_ADDR_SIZE];
  /* LED2 Timeout timerID. It is used to turn off LED2, 5ms after radio activity is received. */
  VTIMER_HandleType Scanning_Switch_On_Off_Led_timer_Id;
  
  /* LED1 timerID. LED1 Behavior Based on Path Loss Zones.
   * The LED1 blinks at different rates depending on the path loss zone:
   * - High Rate: LED1 blinks at a high rate in the low path loss zone (boards close to each other).
   * - Medium Rate: LED1 blinks at a medium rate in the medium path loss zone.
   * - Low Rate: LED1 blinks at a low rate in the high path loss zone (boards far from each other).
   * - No Evt: LED1 always ON means that no hci_le_transmit_power_reporting_event has been received.
   */
  VTIMER_HandleType Path_Loss_LED_timer_Id;

  uint8_t connIntervalFlag;
  /* USER CODE END PTD_1 */
}BleApplicationContext_t;

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SCAN_SWITCH_ON_OFF_LED2_TIMEOUT_MS                                   100
#define WRITE_CHAR_TIMER_MS                                                  300
#define STATS_INTERVAL_MS                                                  10000

/* Values for power control */
#define RSSI_TARGET_1M              -70  // dBm
#define RSSI_TARGET_2M              -67  // dBm
#define RSSI_TARGET_CODED_S8        -77  // dBm
#define RSSI_TARGET_CODED_S2        -73  // dBm
#define RSSI_HYSTERESIS               8  // dB

/* Values for path loss monitoring */
#define HIGH_THRESHOLD      74  // dB
#define HIGH_HYSTERESIS     6   // dB
#define LOW_THRESHOLD       55  // dB
#define LOW_HYSTERESIS      6   // dB
#define MIN_TIME            4   // Connection events

#define PATHLOSS_LOW_LED_INTERVAL_MS    100
#define PATHLOSS_MID_LED_INTERVAL_MS    300
#define PATHLOSS_HIGH_LED_INTERVAL_MS   600
#define LED1_ALWAYS_ON                  500

#define PERIPHERAL_PUBLIC_BD_ADDRESS                              0x0280E10000E1

/* USER CODE END PD */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */
/* Private variables ---------------------------------------------------------*/

NO_INIT(uint32_t dyn_alloc_a[BLE_DYN_ALLOC_SIZE>>2]);

static BleApplicationContext_t bleAppContext;
GATT_CLIENT_APP_ConnHandle_Notif_evt_t clientHandleNotification;

static const char a_GapDeviceName[] = {  'P', 'o', 'w', 'e', 'r', 'C', '_', 'C', 'e', 'n', 't', 'r', 'a', 'l' }; /* Gap Device Name */

/* USER CODE BEGIN PV */
uint8_t path_loss_zone = 0;
const uint16_t path_loss_interval[] = {PATHLOSS_LOW_LED_INTERVAL_MS, PATHLOSS_MID_LED_INTERVAL_MS, PATHLOSS_HIGH_LED_INTERVAL_MS, 1000};
const char* path_loss_zone_str[] = {"PATHLOSS_LOW", "PATHLOSS_MID", "PATHLOSS_HIGH"};
/* 
 * Possible values for PHY (Physical Layer):
 * - 0x01: HCI_PHY_LE_1M       // LE 1M PHY (1 Mbps)
 * - 0x03: HCI_PHY_LE_CODED    // LE Coded PHY (Long range)
 */
static uint8_t phy = LE_1M_PHY_BIT;
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
void hci_le_transmit_power_reporting_event(uint8_t Status,
                                           uint16_t Connection_Handle,
                                           uint8_t Reason,
                                           uint8_t PHY,
                                           int8_t Transmit_Power_Level,
                                           uint8_t Transmit_Power_Level_Flag,
                                           int8_t Delta);
void hci_le_path_loss_threshold_event(uint16_t Connection_Handle,
                                      uint8_t Current_Path_Loss,
                                      uint8_t Zone_Entered);
void Path_Loss_LED_TimeoutCB(void *param);
static void Connect_Request_Fixed_Addr(void);
static void Switch_On_Off_GPIO(void *arg);
void hci_le_phy_update_complete_event(uint8_t Status, uint16_t Connection_Handle, uint8_t TX_PHY, uint8_t RX_PHY);
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
  role |= GAP_CENTRAL_ROLE;
#if CFG_BLE_PRIVACY_ENABLED
  privacy_type = 0x02;
#endif

/* USER CODE BEGIN Role_Mngt*/
  APP_DBG_MSG("\n\n * * * * * Application name : BLE_PowerControl_Central * * * * * \n\n");
  
  /* Enable LE Power Control for LE 1M PHY with specified parameters */
  ret = aci_hal_set_le_power_control(ENABLE, HCI_PHY_LE_1M, RSSI_TARGET_1M, RSSI_HYSTERESIS, 6, 3);

  /* Enable LE Power Control for LE 2M PHY with specified parameters */
  ret |= aci_hal_set_le_power_control(ENABLE, HCI_PHY_LE_2M, RSSI_TARGET_2M, RSSI_HYSTERESIS, 6, 3);

  /* Enable LE Power Control for LE Coded PHY S8 with specified parameters */
  ret |= aci_hal_set_le_power_control(ENABLE, HCI_PHY_LE_CODED_S8, RSSI_TARGET_CODED_S8, RSSI_HYSTERESIS, 6, 3);

  /* Enable LE Power Control for LE Coded PHY S2 with specified parameters */
  ret |= aci_hal_set_le_power_control(ENABLE, HCI_PHY_LE_CODED_S2, RSSI_TARGET_CODED_S2, RSSI_HYSTERESIS, 6, 3);
  
  if(ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_hal_set_le_power_control command, result: 0x%02X\n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_hal_set_le_power_control command\n");    
  }
  
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

  /* Definition of the LE event mask */
  uint8_t LE_Event_Mask[8] = {
      0x1F, // Enable "LE Connection Complete" event
      0x02, // Enable "LE Enhanced Connection Complete" event
      0x20, // Enable "LE Connection IQ Report" event
      0x80, // Enable "LE Path Loss Threshold" event
      0x01, // Enable "LE Transmit Power Reporting" event
      0x00, // No Evt for byte 5
      0x00, // No Evt for byte 6
      0x00  // No Evt for byte 7
  };

  /* Call the function to set the LE event mask */
  ret = aci_gap_set_le_event_mask(LE_Event_Mask);
  if (ret != BLE_STATUS_SUCCESS)
  {
      APP_DBG_MSG("  Fail   : aci_gap_set_le_event_mask command, result: 0x%02X\n", ret);
  }
  else
  {
      APP_DBG_MSG("  Success: aci_gap_set_le_event_mask command\n");
  }
    
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

  /* Create timer to handle the Led Switch OFF */
  bleAppContext.Scanning_Switch_On_Off_Led_timer_Id.callback = Switch_On_Off_GPIO;
  
  /* Create a timer to handle the toggling of LED1 according to the Path Loss strategy. */
  bleAppContext.Path_Loss_LED_timer_Id.callback = Path_Loss_LED_TimeoutCB;
  
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
  
  /* Register a task to handle connection requests with a fixed address */
  UTIL_SEQ_RegTask(1U << CFG_TASK_CONNECT_REQUEST_FIXED_ADDRESS, UTIL_SEQ_RFU, Connect_Request_Fixed_Addr);

  /* Debug message to indicate that the task for creating a connection with a fixed address is set */
  APP_DBG_MSG("==>> SetTask TASK_BUTTON_1 to create connection with fixed address.\n");

  /* Set the task for button 1 to initiate the connection with the fixed address, with priority 0 */
  UTIL_SEQ_SetTask(1U << TASK_BUTTON_1, CFG_SEQ_PRIO_0);
  
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

      if (p_disconnection_complete_event->Connection_Handle == bleAppContext.BleApplicationContext_legacy.connectionHandle)
      {
        bleAppContext.BleApplicationContext_legacy.connectionHandle = 0xFFFF;
        bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
        APP_DBG_MSG(">>== HCI_DISCONNECTION_COMPLETE_EVT_CODE\n");
        APP_DBG_MSG("     - Connection Handle:   0x%02X\n     - Reason:    0x%02X\n",
                    p_disconnection_complete_event->Connection_Handle,
                    p_disconnection_complete_event->Reason);

        /* USER CODE BEGIN EVT_DISCONN_COMPLETE_2 */
        /* Stop the virtual timer associated with path loss LED */
        HAL_RADIO_TIMER_StopVirtualTimer(&bleAppContext.Path_Loss_LED_timer_Id);
        /* Turn off all LEDs */
        BSP_LED_Off(LD1);
        BSP_LED_Off(LD2);
        BSP_LED_Off(LD3);
        /* Set the task for button 1 to initiate a new connection with a fixed address */
        /* At any disconnection, central and peripheral will reconnect automatically */
        APP_DBG_MSG("==>> SetTask TASK_BUTTON_1 to create connection with fixed address.\n");
        UTIL_SEQ_SetTask( 1U << TASK_BUTTON_1, CFG_SEQ_PRIO_0);
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
          
          hci_le_phy_update_complete_event(p_le_phy_update_complete->Status, p_le_phy_update_complete->Connection_Handle, p_le_phy_update_complete->TX_PHY, p_le_phy_update_complete->RX_PHY);
          
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

          /* USER CODE END EVT_LE_ADVERTISING_REPORT */
	  UNUSED(p_adv_report);

        }
        break;
      case HCI_LE_EXTENDED_ADVERTISING_REPORT_SUBEVT_CODE:
        {
          hci_le_extended_advertising_report_event_rp0 *p_ext_adv_report;
          p_ext_adv_report = (hci_le_extended_advertising_report_event_rp0 *) p_meta_evt->data;
          /* USER CODE BEGIN EVT_LE_EXT_ADVERTISING_REPORT */

          /* USER CODE END EVT_LE_EXT_ADVERTISING_REPORT */
          UNUSED(p_ext_adv_report);

        }
        break;
      /* USER CODE BEGIN EVT_LE_META_EVENT_1 */
     case HCI_LE_TRANSMIT_POWER_REPORTING_SUBEVT_CODE:
        {
            hci_le_transmit_power_reporting_event_rp0 *p_tx_power_report;
            p_tx_power_report = (hci_le_transmit_power_reporting_event_rp0 *) p_meta_evt->data;

            hci_le_transmit_power_reporting_event(p_tx_power_report->Status,
                                                  p_tx_power_report->Connection_Handle,
                                                  p_tx_power_report->Reason,
                                                  p_tx_power_report->PHY,
                                                  p_tx_power_report->Transmit_Power_Level,
                                                  p_tx_power_report->Transmit_Power_Level_Flag,
                                                  p_tx_power_report->Delta);
        }
        break;
      case HCI_LE_PATH_LOSS_THRESHOLD_SUBEVT_CODE:
      {
          hci_le_path_loss_threshold_event_rp0 *p_path_loss_threshold;
          p_path_loss_threshold = (hci_le_path_loss_threshold_event_rp0 *) p_meta_evt->data;

          hci_le_path_loss_threshold_event(p_path_loss_threshold->Connection_Handle,
                                           p_path_loss_threshold->Current_Path_Loss,
                                           p_path_loss_threshold->Zone_Entered);
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
          UNUSED(p_gap_proc_complete);
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
  if(Status != 0)
  {
    APP_DBG_MSG("==>> connection_complete_event Fail, Status: 0x%02X\n", Status);
    bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
    return;
  }
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
   APP_DBG_MSG("     - Connection Interval:   %d.%02d ms\n     - Connection latency:    %d\n     - Supervision Timeout: %d ms\n",
               INT(Conn_Interval*1.25),
               FRACTIONAL_2DIGITS(Conn_Interval*1.25),
               Peripheral_Latency,
               Supervision_Timeout * 10
                 );

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

   GATT_CLIENT_APP_Set_Conn_Handle(0, Connection_Handle);

   /* USER CODE BEGIN HCI_EVT_LE_CONN_COMPLETE */
   
   if (bleAppContext.Device_Connection_Status == APP_BLE_CONNECTED_CLIENT)
   {
     /* Discovers all services, characteristics, and descriptors, and enables all notifications for a given GATT client. */
     UTIL_SEQ_SetTask( 1U << CFG_TASK_DISCOVER_SERVICES_ID, CFG_SEQ_PRIO_0);
   }
   
   if (Role == HCI_ROLE_CENTRAL)
   {
     UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_CONNECTION_COMPLETE);
   }
   
  /* Once connected the LED2 is always on, on both devices Central and Peripheral. */
  BSP_LED_On(LD2);
  
  /* Enable TX power and path loss reporting */

  tBleStatus status = hci_le_set_transmit_power_reporting_enable(Connection_Handle, ENABLE, ENABLE);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("hci_le_set_transmit_power_reporting_enable failure: reason=0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> hci_le_set_transmit_power_reporting_enable : Success\n");
  }
      
  status = hci_le_set_path_loss_reporting_parameters(Connection_Handle, HIGH_THRESHOLD, HIGH_HYSTERESIS, LOW_THRESHOLD, LOW_HYSTERESIS, MIN_TIME);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("hci_le_set_path_loss_reporting_parameters failure: reason=0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> hci_le_set_path_loss_reporting_parameters : Success\n");
  }
  
  status = hci_le_set_path_loss_reporting_enable(Connection_Handle, ENABLE);
  if (status != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("hci_le_set_path_loss_reporting_enable failure: reason=0x%02X\n", status);
  }
  else
  {
    APP_DBG_MSG("==>> hci_le_set_path_loss_reporting_enable : Success\n");
  }
  
  /* LED1 always ON means that no hci_le_transmit_power_reporting_event has been received (LED1_ALWAYS_ON).  */
  path_loss_zone = 3U;
  HAL_RADIO_TIMER_StartVirtualTimer(&bleAppContext.Path_Loss_LED_timer_Id, path_loss_interval[path_loss_zone]);
  
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
      phy = LE_1M_PHY_BIT;
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
    /* USER CODE BEGIN GAP_CENTRAL_1 */
    case PROC_GAP_CENTRAL_SCAN_START_CODED_PHY:
    {
      paramA = SCAN_INT_MS(500);
      paramB = SCAN_WIN_MS(500);
      paramC = APP_BLE_SCANNING;
      break;
    }/* PROC_GAP_CENTRAL_SCAN_START_CODED_PHY */
    case PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_1M_PHY:
    {
      phy = LE_1M_PHY_BIT;
      break;
    }/* PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_1M_PHY */
    case PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_CODED_PHY:
    {
      phy = LE_CODED_PHY_BIT;
      break;
    }/* PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_CODED_PHY */
    case PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS:
    {
      paramA = SCAN_INT_MS(500);
      paramB = SCAN_WIN_MS(500);
      paramC = APP_BLE_LP_CONNECTING;
      break;
    }/* PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS */
    /* USER CODE END GAP_CENTRAL_1 */
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
    /* USER CODE BEGIN GAP_CENTRAL_2 */

    
    
    case PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_1M_PHY: 
      {
        phy = LE_1M_PHY_BIT;
        UTIL_SEQ_SetTask( 1U << CFG_TASK_CONNECT_REQUEST_FIXED_ADDRESS, CFG_SEQ_PRIO_0);
        break;
      } /* PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_1M_PHY */
    case PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_CODED_PHY: 
      {
        phy = LE_CODED_PHY_BIT;
        UTIL_SEQ_SetTask( 1U << CFG_TASK_CONNECT_REQUEST_FIXED_ADDRESS, CFG_SEQ_PRIO_0);
        break;
      } /* PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_CODED_PHY */
    case PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS: 
      {
        /* Start the timer to manage the blinking of LED2 during the scanning process. */
        HAL_RADIO_TIMER_StartVirtualTimer(&bleAppContext.Scanning_Switch_On_Off_Led_timer_Id, SCAN_SWITCH_ON_OFF_LED2_TIMEOUT_MS);
        
        paramA = SCAN_INT_MS(500);
        paramB = SCAN_WIN_MS(500);
        paramC = APP_BLE_SCANNING;
        
        if(phy == LE_1M_PHY_BIT)
        {
          APP_DBG_MSG("==>> PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS for  LE_1M_PHY_BIT\n");
        }
        else
        {
          APP_DBG_MSG("==>> PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS for  LE_CODED_PHY_BIT\n");
        }
        
        status = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, 
                                                0x00, 
                                                phy, 
                                                HCI_SCAN_TYPE_PASSIVE, //HCI_SCAN_TYPE_ACTIVE, 
                                                paramA, 
                                                paramB);
        if(status != BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("aci_gap_set_scan_configuration - fail, result: 0x%02X\n", status);
        }
        else
        {
          APP_DBG_MSG("==>> aci_gap_set_scan_configuration - Success\n");
        }
                
        status = aci_gap_set_connection_configuration(phy,
                                                      CONN_INT_MS(50u), 
                                                      CONN_INT_MS(100u),
                                                      0u,
                                                      CONN_SUP_TIMEOUT_MS(5000u),
                                                      CONN_CE_LENGTH_MS(10u),
                                                      CONN_CE_LENGTH_MS(10u));
        if(status == BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("==>> aci_gap_set_connection_configuration Success , status: 0x%02X\n", status);
        }
        else
        {
          APP_DBG_MSG("==>> aci_gap_set_connection_configuration Failed , status: 0x%02X\n", status);
        }  

        uint8_t peripheral_bd_address[6] = {0};

        peripheral_bd_address[0] = (uint8_t)((PERIPHERAL_PUBLIC_BD_ADDRESS & 0x0000000000FF));
        peripheral_bd_address[1] = (uint8_t)((PERIPHERAL_PUBLIC_BD_ADDRESS & 0x00000000FF00) >> 8);
        peripheral_bd_address[2] = (uint8_t)((PERIPHERAL_PUBLIC_BD_ADDRESS & 0x000000FF0000) >> 16);
        peripheral_bd_address[3] = (uint8_t)((PERIPHERAL_PUBLIC_BD_ADDRESS & 0x0000FF000000) >> 24);
        peripheral_bd_address[4] = (uint8_t)((PERIPHERAL_PUBLIC_BD_ADDRESS & 0x00FF00000000) >> 32);
        peripheral_bd_address[5] = (uint8_t)((PERIPHERAL_PUBLIC_BD_ADDRESS & 0xFF0000000000) >> 40);
          
        /*
         * A direct connection with a peer device can be built by the aci_gap_create_connection(). This API specifies
         * the PHY only to be used for the connection, the peer device type (Peer_Address_Type parameter) and the
         * peer address (Peer_Address parameter).
         */
        APP_DBG_MSG("==>> (FIXED ADDRESS) aci_gap_create_connection to ");
        APP_DBG_MSG("Peripheral Public Bluetooth Address: %02X:%02X:%02X:%02X:%02X:%02X\n",peripheral_bd_address[5],peripheral_bd_address[4],peripheral_bd_address[3],peripheral_bd_address[2],peripheral_bd_address[1],peripheral_bd_address[0]);
        status = aci_gap_create_connection(phy, HCI_ADDR_PUBLIC, peripheral_bd_address);
        if (status == BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("==>> (FIXED ADDRESS) aci_gap_create_connection GAP Create connection Success\n");
          bleAppContext.Device_Connection_Status = APP_BLE_LP_CONNECTING;
          APP_DBG_MSG("     (FIXED ADDRESS)  wait for event HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE\n");
          UTIL_SEQ_WaitEvt(1u << CFG_IDLEEVT_CONNECTION_COMPLETE);
          APP_DBG_MSG("     (FIXED ADDRESS)  received event HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE\n");
        }
        else
        {
          APP_DBG_MSG("==>> (FIXED ADDRESS) aci_gap_create_connection GAP Create connection Failed , status: 0x%02X\n", status);
          bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
          
          APP_DBG_MSG("==>> GAP Create connection will re-start due to failed connection.\n");
          UTIL_SEQ_SetTask( 1U << CFG_TASK_CONNECT_REQUEST_FIXED_ADDRESS, CFG_SEQ_PRIO_0);
        }

        break;
      } /* PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS */ 
    
    case PROC_GAP_CENTRAL_SCAN_START_CODED_PHY:
    {
      phy = LE_CODED_PHY_BIT;
     
      status = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, 
                                              0x00, 
                                              LE_1M_PHY_BIT, 
                                              HCI_SCAN_TYPE_PASSIVE, 
                                              paramA, 
                                              paramB);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("(1M PHY) aci_gap_set_scan_configuration - fail, result: 0x%02X\n", status);
      }
      else
      {
        APP_DBG_MSG("==>> (1M PHY) aci_gap_set_scan_configuration - Success\n");
      }
      status = aci_gap_set_scan_configuration(DUPLICATE_FILTER_ENABLED, 
                                              0x00, 
                                              LE_CODED_PHY_BIT, 
                                              HCI_SCAN_TYPE_PASSIVE, 
                                              paramA, 
                                              paramB);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("(CODED PHY) aci_gap_set_scan_configuration - fail, result: 0x%02X\n", status);
      }
      else
      {
        APP_DBG_MSG("==>> (CODED PHY) aci_gap_set_scan_configuration - Success\n");
      }
      
      status = aci_gap_start_procedure (GAP_GENERAL_DISCOVERY_PROC,
                                        LE_CODED_PHY_BIT,
                                        0,
                                        0);
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("(CODED PHY) aci_gap_start_procedure - fail, result: 0x%02X\n", status);
      }
      else
      {
        bleAppContext.Device_Connection_Status = (APP_BLE_ConnStatus_t)paramC;
        APP_DBG_MSG("==>> (CODED PHY) aci_gap_start_procedure - Success\n");
      }
      break;
    }/* PROC_GAP_CENTRAL_SCAN_START_CODED_PHY */
    
    /* USER CODE END GAP_CENTRAL_2 */
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
 * @brief Initiates a connection request to a fixed public address.
 *
 * This function demonstrates how to perform a connection request to a device
 * with a fixed public address. It is intended for demo purposes and assumes
 * that the target device's address is known and fixed.
 *
 * @note Ensure that the target device with the fixed public address is
 *       available and in advertising mode before calling this function.
 *
 * @return None
 */
static void Connect_Request_Fixed_Addr(void)
{
  APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS);
  return;
}


/**
* @brief Switch off the GPIO (LED).
*
* This function turns off the green LED.
*
* @param arg Pointer to the argument (not used in this function).
*/
static void Switch_On_Off_GPIO(void *arg)
{
  if(bleAppContext.Device_Connection_Status == APP_BLE_CONNECTED_CLIENT)
  {
    /* Stop the Scanning_Switch_On_Off_Led_timer_Id timer. No more needed. */
    BSP_LED_On(LD2);
  }
  else
  {
    BSP_LED_Toggle(LD2);
    HAL_RADIO_TIMER_StartVirtualTimer(&bleAppContext.Scanning_Switch_On_Off_Led_timer_Id, SCAN_SWITCH_ON_OFF_LED2_TIMEOUT_MS);
  }
  return;
}


/* This function is called when there is a L2CAP_CONN_UPDATE_RESP_Event vendor specific event. */ 
void aci_l2cap_connection_update_resp_event(uint16_t Connection_Handle,
                                            uint16_t Result)
{
  if(Result) {
    APP_DBG_MSG("> Connection parameters rejected.\n");
  } else  {
    APP_DBG_MSG("> Connection parameters accepted.\n");
  }
}


void hci_le_phy_update_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t TX_PHY,
                                      uint8_t RX_PHY)
{
  if(TX_PHY == HCI_TX_PHY_LE_CODED && RX_PHY == HCI_RX_PHY_LE_CODED)
  {
    /* Coded Phy Led3 On*/
    BSP_LED_On(LD3);
    APP_DBG_MSG("  PHY changed, tx:%d rx:%d Coded Phy Led3 On\n", TX_PHY, RX_PHY);
  }
  else 
  {
    if(TX_PHY == HCI_TX_PHY_LE_1M && RX_PHY == HCI_RX_PHY_LE_1M)
    {
      /* 1M Led3 Off */
      BSP_LED_Off(LD3);
      APP_DBG_MSG("  PHY changed, tx:%d rx:%d 1M Led3 Off\n", TX_PHY, RX_PHY);
    }
    else 
    {
      APP_DBG_MSG("  PHY changed Unexpected, tx:%d rx:%d 1M Led3 Off\n", TX_PHY, RX_PHY);
      for(int i=0; i<10; i++)
      {
        BSP_LED_Toggle(LD3);
        HAL_Delay(500);
      }
    }
  }
  
}


void hci_le_transmit_power_reporting_event(uint8_t Status,
                                           uint16_t Connection_Handle,
                                           uint8_t Reason,
                                           uint8_t PHY,
                                           int8_t Transmit_Power_Level,
                                           uint8_t Transmit_Power_Level_Flag,
                                           int8_t Delta)
{
  if(Reason==LOCAL_TX_POWER_CHANGE)
  {
    APP_DBG_MSG("  hci_le_transmit_power_reporting_event - Local: %d dBm (%d dB)\n", Transmit_Power_Level, Delta);
  }
  else
  {
    APP_DBG_MSG("  hci_le_transmit_power_reporting_event - Remote: %d dBm (%d dB)\n", Transmit_Power_Level, Delta);
  }
}


void hci_le_path_loss_threshold_event(uint16_t Connection_Handle,
                                      uint8_t Current_Path_Loss,
                                      uint8_t Zone_Entered)
{
  APP_DBG_MSG("  hci_le_path_loss_threshold_event - PATH LOSS: %d dB (zone %d)\n", Current_Path_Loss, Zone_Entered);
  path_loss_zone = Zone_Entered;
}


void Path_Loss_LED_TimeoutCB(void *param)
{
  if(path_loss_zone == 3U)
  {
    /* Led1 ON: the hci_le_transmit_power_reporting_event has not been received. */
    BSP_LED_On(LD1);
  }
  else
  {
#if CFG_LPM_SUPPORTED && defined(PWR_IOxCFG_IOCFG6)
    uint32_t tmp = LL_PWR_GetPA6OutputinDEEPSTOP();
    if(tmp==LL_PWR_IOCFG_HIGH)
      LL_PWR_SetPA6OutputinDEEPSTOP(LL_PWR_IOCFG_LOW);
    else
      LL_PWR_SetPA6OutputinDEEPSTOP(LL_PWR_IOCFG_HIGH);
#else
    BSP_LED_Toggle(LD1);
#endif
  }
  HAL_RADIO_TIMER_StartVirtualTimer(&bleAppContext.Path_Loss_LED_timer_Id, path_loss_interval[path_loss_zone]);
}

/* USER CODE END FD_LOCAL_FUNCTION */

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */
#if (CFG_BUTTON_SUPPORTED == 1)
void APPE_Button1Action(void)
{
  APP_DBG_MSG("bleAppContext.Device_Connection_Status   0x%02X\n", bleAppContext.Device_Connection_Status);
  
  if (bleAppContext.Device_Connection_Status == APP_BLE_CONNECTED_CLIENT)
  {
    APP_DBG_MSG("Device connected. The connection is terminated.\n");
    APP_BLE_Procedure_Gap_General(PROC_GAP_GEN_CONN_TERMINATE);
    bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
  }
  
  if(bleAppContext.Device_Connection_Status == APP_BLE_IDLE)
  {
    APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_1M_PHY);
  }
  return;
}

void APPE_Button2Action(void)
{
  return;
}

void APPE_Button3Action(void)
{
  APP_DBG_MSG("  (CODED PHY) bleAppContext.Device_Connection_Status   0x%02X\n", bleAppContext.Device_Connection_Status);

  if (bleAppContext.Device_Connection_Status == APP_BLE_CONNECTED_CLIENT )
  {
    APP_DBG_MSG("  (CODED PHY)Device connected. The connection is terminated.\n");
    APP_BLE_Procedure_Gap_General(PROC_GAP_GEN_CONN_TERMINATE);
    bleAppContext.Device_Connection_Status = APP_BLE_IDLE;
  }
    
  if(bleAppContext.Device_Connection_Status == APP_BLE_IDLE)
  {
    APP_BLE_Procedure_Gap_Central(PROC_GAP_CENTRAL_CONNECT_REQUEST_FIXED_ADDRESS_CODED_PHY);
  }
  return;
}
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
/* USER CODE END FD_WRAP_FUNCTIONS */

/** \endcond
 */
