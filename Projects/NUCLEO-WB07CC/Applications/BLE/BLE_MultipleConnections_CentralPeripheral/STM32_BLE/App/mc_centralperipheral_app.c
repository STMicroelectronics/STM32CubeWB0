/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file    MC_CentralPeripheral_app.c
* @author  MCD Application Team
* @brief   MC_CentralPeripheral_app application definition.
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
#include "mc_centralperipheral_app.h"
#include "mc_centralperipheral.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gatt_client_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef struct{
  uint8_t             Device_Led_Selection;
  uint8_t             Led1;
}P2P_LedCharValue_t;

typedef struct{
  uint8_t             Device_Button_Selection;
  uint8_t             ButtonStatus;
}P2P_ButtonCharValue_t;

typedef struct{
  uint16_t             connectionHandle;
  /*
  0x00   init
  0x01   paring request to be done
  0x02   paring request done
  0x03   paring request fail
  0x04   paring request success
  0x05   paring request 0x03 error
  */
  uint8_t             pairing_status;
}ContextData_t;


/* USER CODE END PTD */

typedef enum
{
  Switch_c_NOTIFICATION_OFF,
  Switch_c_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */
  
  /* USER CODE END Service1_APP_SendInformation_t */
  MC_CENTRALPERIPHERAL_APP_SENDINFORMATION_LAST
} MC_CENTRALPERIPHERAL_APP_SendInformation_t;

typedef struct
{
  MC_CENTRALPERIPHERAL_APP_SendInformation_t     Switch_c_Notification_Status;
  uint16_t MC_CENTRALPERIPHERAL_periph_connHdl;
  /* USER CODE BEGIN Service1_APP_Context_t */

  ContextData_t central_array[CFG_BLE_NUM_CLT_CONTEXT_MAX];
  ContextData_t peripheral_array[CFG_BLE_PERIPHERAL_HANDLES_MAX];
  
  P2P_LedCharValue_t              LedControl;
  P2P_ButtonCharValue_t           ButtonControl;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} MC_CENTRALPERIPHERAL_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static MC_CENTRALPERIPHERAL_APP_Context_t MC_CENTRALPERIPHERAL_APP_Context;

uint8_t a_MC_CENTRALPERIPHERAL_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MC_CENTRALPERIPHERAL_Switch_c_SendNotification(void);

/* USER CODE BEGIN PFP */
static void MC_CENTRALPERIPHERAL_APP_LED_BUTTON_context_Init(void);
void APP_Context_init(void);
int APP_Context_getActiveConnectionCount(ContextData_t* contextData_Array, int maxHandles);
int APP_Context_findFirstFreeIndex(ContextData_t* connHdlArray, int maxHandles);
int APP_Context_addConnection(MC_CENTRALPERIPHERAL_APP_Context_t* context, uint16_t connHdl, int isRolePeripheral);
int APP_Context_removeConnection(MC_CENTRALPERIPHERAL_APP_Context_t* context, uint16_t connHdl, int isRolePeripheral);
int APP_Context_updatePairingStatus(uint16_t connHdl, uint8_t newStatus);
int APP_Context_isConnectionHandle(MC_CENTRALPERIPHERAL_APP_Context_t* context, uint16_t connHdl, int isRolePeripheral);
int APP_Context_isCentralConnectionHandle(uint16_t connHdl);
int APP_Context_isPeripheralConnectionHandle(uint16_t connHdl);
void APP_Context_printContextArrays(void);
static void Peripheral_Security_Request(void);
static void Terminate_Connections(MC_CENTRALPERIPHERAL_APP_Context_t* context, int isRolePeripheral);
static void Terminate_Central_Connections(void);
static void Terminate_Peripheral_Connections(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void MC_CENTRALPERIPHERAL_Notification(MC_CENTRALPERIPHERAL_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */
  
  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */
    
    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case MC_CENTRALPERIPHERAL_LED_C_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */
    
      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case MC_CENTRALPERIPHERAL_LED_C_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */
    if(p_Notification->DataTransfered.p_Payload[1] == 0x01)
    {
      BSP_LED_On(LED_BLUE);
      APP_DBG_MSG("-- APPLICATION SERVER : LED1 ON\n"); 
      MC_CENTRALPERIPHERAL_APP_Context.LedControl.Led1 = 0x01; /* LED1 ON */
    }
    if(p_Notification->DataTransfered.p_Payload[1] == 0x00)
    {
      BSP_LED_Off(LED_BLUE);
      APP_DBG_MSG("-- APPLICATION SERVER : LED1 OFF\n"); 
      MC_CENTRALPERIPHERAL_APP_Context.LedControl.Led1 = 0x00; /* LED1 OFF */
    }
      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case MC_CENTRALPERIPHERAL_SWITCH_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
    MC_CENTRALPERIPHERAL_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_ON;
    APP_DBG_MSG("-- APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
    APP_DBG_MSG(" \n");
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case MC_CENTRALPERIPHERAL_SWITCH_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
    MC_CENTRALPERIPHERAL_APP_Context.Switch_c_Notification_Status = Switch_c_NOTIFICATION_OFF;
    APP_DBG_MSG("-- APPLICATION SERVER : NOTIFICATION DISABLED\n"); 
    APP_DBG_MSG(" \n");
      /* USER CODE END Service1Char2_NOTIFY_DISABLED_EVT */
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

void MC_CENTRALPERIPHERAL_APP_EvtRx(MC_CENTRALPERIPHERAL_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */
  
  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */
  case MC_CENTRALPERIPHERAL_PAIRING_ERROR_HANDLE_EVT:
    APP_Context_updatePairingStatus(p_Notification->ConnectionHandle, 0x05);
    break;
  case MC_CENTRALPERIPHERAL_PAIRING_SUCCESS_HANDLE_EVT:
    if(APP_Context_isCentralConnectionHandle(p_Notification->ConnectionHandle))
    {
      /* On pairing complete, if acting as a role central, I need to ... */
      ;
    }
    else
    {
      if(APP_Context_isPeripheralConnectionHandle(p_Notification->ConnectionHandle))
      {
        /* On pairing complete, if acting as a role peripheral, I need to restart advertising in non-discoverable mode with filter */
        UTIL_SEQ_SetTask( 1U << CFG_TASK_CONFIGURE_FILTER_LIST_AND_NEW_ADV, CFG_SEQ_PRIO_0); 
      }
    }
    break;
    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case MC_CENTRALPERIPHERAL_CENTR_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CENTR_CONN_HANDLE_EVT */
    if( APP_Context_addConnection(&MC_CENTRALPERIPHERAL_APP_Context, p_Notification->ConnectionHandle, HCI_ROLE_CENTRAL) == -1)
    {
      APP_DBG_MSG("  Fail: APP_Context_addConnection    ConnectionHandle = 0x%04X\n", p_Notification->ConnectionHandle);
    }
    else
    {
      APP_DBG_MSG("  APP_Context_addConnection ConnectionHandle = 0x%04X\n", p_Notification->ConnectionHandle);
    }
      /* USER CODE END Service1_APP_CENTR_CONN_HANDLE_EVT */
      break;

    case MC_CENTRALPERIPHERAL_PERIPH_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_PERIPH_CONN_HANDLE_EVT */
    if( APP_Context_addConnection(&MC_CENTRALPERIPHERAL_APP_Context, p_Notification->ConnectionHandle, HCI_ROLE_PERIPHERAL) == -1)
    {
      APP_DBG_MSG("  Fail: APP_Context_addConnection     ConnectionHandle = 0x%04X \n", p_Notification->ConnectionHandle);
    }
    else
    {
      APP_DBG_MSG("  Success : APP_Context_addConnection     ConnectionHandle = 0x%04X\n", p_Notification->ConnectionHandle);
    }
    
      /* USER CODE END Service1_APP_PERIPH_CONN_HANDLE_EVT */
      break;

    case MC_CENTRALPERIPHERAL_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */

    if( APP_Context_removeConnection(&MC_CENTRALPERIPHERAL_APP_Context, p_Notification->ConnectionHandle, HCI_ROLE_PERIPHERAL) == -1)
    {
      if( APP_Context_removeConnection(&MC_CENTRALPERIPHERAL_APP_Context, p_Notification->ConnectionHandle, HCI_ROLE_CENTRAL) == -1)
      {
        APP_DBG_MSG("  Fail : APP_Context_removeConnection - Connection handle not found. ConnectionHandle = 0x%04X.\n", p_Notification->ConnectionHandle);
      }
      else
      {
        /* 
         * The disconnection event HCI_DISCONNECTION_COMPLETE_EVT_CODE has been generated from a ConnectionHandle of the role Central. 
         * This means that after the disconnection, it is required to start the scanning in auto connection mode (with filter).
         */
        UTIL_SEQ_SetTask(1U << CFG_TASK_AUTO_CONNECTION_REQUEST, CFG_SEQ_PRIO_0);
      }
    }
    else
    {
      /* 
       * The disconnection event HCI_DISCONNECTION_COMPLETE_EVT_CODE has been generated from a ConnectionHandle of the role Peripheral. 
       * This means that after the disconnection, it is required to start advertising in auto connection mode (with filter). 
       */         
      UTIL_SEQ_SetTask(1U << CFG_TASK_CONFIGURE_FILTER_LIST_AND_NEW_ADV, CFG_SEQ_PRIO_0);
    }

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

void MC_CENTRALPERIPHERAL_APP_Init(void)
{
  MC_CENTRALPERIPHERAL_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  APP_Context_init();
  
  UTIL_SEQ_RegTask(1U << CFG_TASK_PERIPHERAL_SECURITY_REQUEST, UTIL_SEQ_RFU, Peripheral_Security_Request);
  UTIL_SEQ_RegTask( 1U << CFG_TASK_SEND_NOTIF_ID, UTIL_SEQ_RFU, MC_CENTRALPERIPHERAL_Switch_c_SendNotification);
  UTIL_SEQ_RegTask(1U << CFG_TASK_TERMINATE_ALL_CENTRAL_CONNECTIONS, UTIL_SEQ_RFU, Terminate_Central_Connections);
  UTIL_SEQ_RegTask(1U << CFG_TASK_TERMINATE_ALL_PERIPHERAL_CONNECTIONS, UTIL_SEQ_RFU, Terminate_Peripheral_Connections);
  
  
  /**
  * Initialize LedButton Service
  */
  MC_CENTRALPERIPHERAL_APP_Context.Switch_c_Notification_Status= Switch_c_NOTIFICATION_OFF;
  MC_CENTRALPERIPHERAL_APP_LED_BUTTON_context_Init();
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void MC_CENTRALPERIPHERAL_APP_LED_BUTTON_context_Init(void)
{  
  BSP_LED_Off(LED_BLUE);
  MC_CENTRALPERIPHERAL_APP_Context.LedControl.Device_Led_Selection=0x01; /* Device1 */
  MC_CENTRALPERIPHERAL_APP_Context.LedControl.Led1=0x00; /* led OFF */
  MC_CENTRALPERIPHERAL_APP_Context.ButtonControl.Device_Button_Selection=0x01;/* Device1 */
  MC_CENTRALPERIPHERAL_APP_Context.ButtonControl.ButtonStatus=0x00;
  
  return;
}

/**
* @brief Initialize the context structure.
* 
* @param context Pointer to the context structure to initialize.
*/
void APP_Context_init(void)
{
  for (int i = 0; i < CFG_BLE_NUM_CLT_CONTEXT_MAX; i++) 
  {
    MC_CENTRALPERIPHERAL_APP_Context.central_array[i].connectionHandle = 0xFFFF;
    MC_CENTRALPERIPHERAL_APP_Context.central_array[i].pairing_status = 0x0;
  }
  for (int i = 0; i < CFG_BLE_PERIPHERAL_HANDLES_MAX; i++) 
  {
    MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle = 0xFFFF;
    MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].pairing_status = 0x0;  
  }
}

/**
* @brief Get the number of active connections in the context array.
* 
* @param contextData_Array Array to check for active connections.
* @param maxHandles Maximum number of handles.
* @return int Number of active connections.
*/
int APP_Context_getActiveConnectionCount(ContextData_t* contextData_Array, int maxHandles) 
{
  int count = 0;
  for (int i = 0; i < maxHandles; i++) 
  {
    if (contextData_Array[i].connectionHandle != 0xFFFF) 
    {
      count++;
    }
  }
  return count;
}

int APP_Context_getActiveConnectionCountCentral(void)
{
  return APP_Context_getActiveConnectionCount(MC_CENTRALPERIPHERAL_APP_Context.central_array, CFG_BLE_NUM_CLT_CONTEXT_MAX);
}

int APP_Context_getActiveConnectionCountPeripheral(void)
{
  return APP_Context_getActiveConnectionCount(MC_CENTRALPERIPHERAL_APP_Context.peripheral_array, CFG_BLE_PERIPHERAL_HANDLES_MAX);
}

/**
* @brief Find the first free index in the connection handle array.
* 
* @param connHdlArray Array to check for free slots.
* @param maxHandles Maximum number of handles.
* @return int Index of the first free element, or -1 if no free element is found.
*/
int APP_Context_findFirstFreeIndex(ContextData_t* contextData_Array, int maxHandles) 
{
  for (int i = 0; i < maxHandles; i++) 
  {
    if (contextData_Array[i].connectionHandle == 0xFFFF) 
    {
      return i;
    }
  }
  return -1; // No free element found
}
  
/**
* @brief Check if advertising can be started.
* 
* This function uses the findFirstFreeIndex function to determine if there is still space 
* for new handles in the peripheral array. If there are free indices, it means that 
* advertising can be started; otherwise, all slots are occupied.
* 
* @return int -1 if advertising cannot be started, other values if advertising can be started.
*/
int APP_Context_canStartAdvertising(void) 
{
  return APP_Context_findFirstFreeIndex(MC_CENTRALPERIPHERAL_APP_Context.peripheral_array, CFG_BLE_PERIPHERAL_HANDLES_MAX); 
}

/**
* @brief Check if scanning can be started.
* 
* This function uses the findFirstFreeIndex function to determine if there is still space 
* for new handles in the central array. If there are free indices, it means that 
* scanning can be started; otherwise, all slots are occupied.
* 
* @return int -1 if scanning cannot be started, other values if scanning can be started.
*/
int APP_Context_canStartScanning(void) 
{
  return APP_Context_findFirstFreeIndex(MC_CENTRALPERIPHERAL_APP_Context.central_array, CFG_BLE_NUM_CLT_CONTEXT_MAX);
}

/**
* @brief Add a new connection handle to the context.
* 
* This function adds a new connection handle to the appropriate context array 
* (either peripheral or central) and updates the GATT client context if the 
* connection is for a central device.
*
* @param context Pointer to the context structure.
* @param connHdl Connection handle to add.
* @param isRolePeripheral Flag indicating if the connection is for a peripheral device.
* @return int 0 if the connection handle was added successfully, -1 if no free space is available.
*/
int APP_Context_addConnection(MC_CENTRALPERIPHERAL_APP_Context_t* context, uint16_t connHdl, int isRolePeripheral)
{
  ContextData_t* contextDataArray = isRolePeripheral ? context->peripheral_array : context->central_array;
  int maxHandles = isRolePeripheral ? CFG_BLE_PERIPHERAL_HANDLES_MAX : CFG_BLE_NUM_CLT_CONTEXT_MAX;
  
  int index = APP_Context_findFirstFreeIndex(contextDataArray, maxHandles);
  if (index == -1) 
  {
    return -1; // No free space available
  }
  contextDataArray[index].connectionHandle = connHdl;
  
  contextDataArray[index].pairing_status = 0x1;
  if(!isRolePeripheral)
  {
    /* Role: Central */
    /* When the central role completes a connection with a peripheral, it is mandatory to update the GATT client context with the new connection handle */
    GATT_CLIENT_APP_Set_Conn_Handle(index, connHdl);
  }
  APP_Context_printContextArrays();
  return 0;
}

/**
* @brief Remove a connection handle from the context.
* 
* @param context Pointer to the context structure.
* @param connHdl Connection handle to remove.
* @param isRolePeripheral Flag indicating if the connection is for a peripheral device.
* @return int 0 if the connection handle was removed successfully, -1 if the connection handle was not found.
*/
int APP_Context_removeConnection(MC_CENTRALPERIPHERAL_APP_Context_t* context, uint16_t connHdl, int isRolePeripheral)
{
  ContextData_t* contextDataArray = isRolePeripheral ? context->peripheral_array : context->central_array;
  int maxHandles = isRolePeripheral ? CFG_BLE_PERIPHERAL_HANDLES_MAX : CFG_BLE_NUM_CLT_CONTEXT_MAX;
  
  for (int index = 0; index < maxHandles; index++) 
  {
    if (contextDataArray[index].connectionHandle == connHdl) 
    {
      contextDataArray[index].connectionHandle = 0xFFFF;
      contextDataArray[index].pairing_status = 0x0;
      if(!isRolePeripheral)
      {
        /* Role: Central */
        /* When the central role completes a disconnection with a peripheral, it is mandatory to update the GATT client context removing the old connection handle */
        GATT_CLIENT_APP_Clear_Conn_Handle(index, connHdl);
      }
      APP_Context_printContextArrays();
      return 0;
    }
  }
  return -1; // Connection handle not found
}

/**
 * @brief Update the pairing status for a given connection handle.
 * 
 * This function updates the pairing status to the specified new status for the given connection handle
 * in either the central or peripheral array within the context.
 * 
 * @param connHdl Connection handle for which the pairing status needs to be updated.
 * @param newStatus The new pairing status to be set.
 * @return int 0 if the pairing status was updated successfully, -1 if the connection handle was not found.
 */
int APP_Context_updatePairingStatus(uint16_t connHdl, uint8_t newStatus)
{
  for (int i = 0; i < CFG_BLE_NUM_CLT_CONTEXT_MAX; i++)
  {
    if(MC_CENTRALPERIPHERAL_APP_Context.central_array[i].connectionHandle == connHdl)
    {
      MC_CENTRALPERIPHERAL_APP_Context.central_array[i].pairing_status = newStatus;
      APP_Context_printContextArrays();
      return 0;
    }
  }
  
  for (int i = 0; i < CFG_BLE_PERIPHERAL_HANDLES_MAX; i++)
  {
    if(MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle == connHdl)
    {
      MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].pairing_status = newStatus;
      APP_Context_printContextArrays();
      return 0;
    }
  }
  return -1; // Connection handle not found
}

/**
 * @brief Check if a connection handle exists in the context.
 * 
 * This function checks if the specified connection handle exists in either the central or peripheral array within the context.
 * 
 * @param context Pointer to the context structure.
 * @param connHdl Connection handle to check.
 * @param isRolePeripheral Flag indicating if the connection is for a peripheral device.
 * @return int 1 if the connection handle exists, 0 if the connection handle was not found.
 */
int APP_Context_isConnectionHandle(MC_CENTRALPERIPHERAL_APP_Context_t* context, uint16_t connHdl, int isRolePeripheral)
{
  ContextData_t* contextDataArray = isRolePeripheral ? context->peripheral_array : context->central_array;
  int maxHandles = isRolePeripheral ? CFG_BLE_PERIPHERAL_HANDLES_MAX : CFG_BLE_NUM_CLT_CONTEXT_MAX;
  
  for (int i = 0; i < maxHandles; i++) 
  {
    if (contextDataArray[i].connectionHandle == connHdl) 
    {
      APP_Context_printContextArrays();
      return 1;
    }
  }
  return 0; // Connection handle not found
}

/**
 * @brief Check if a connection handle exists in the central array.
 * 
 * This function checks if the specified connection handle exists in the central array within the context.
 * 
 * @param connHdl Connection handle to check.
 * @return int 0 if the connection handle exists, -1 if the connection handle was not found.
 */
int APP_Context_isCentralConnectionHandle(uint16_t connHdl)
{
  return APP_Context_isConnectionHandle(&MC_CENTRALPERIPHERAL_APP_Context, connHdl, HCI_ROLE_CENTRAL);
}

/**
 * @brief Check if a connection handle exists in the peripheral array.
 * 
 * This function checks if the specified connection handle exists in the peripheral array within the context.
 * 
 * @param connHdl Connection handle to check.
 * @return int 0 if the connection handle exists, -1 if the connection handle was not found.
 */
int APP_Context_isPeripheralConnectionHandle(uint16_t connHdl)
{
  return APP_Context_isConnectionHandle(&MC_CENTRALPERIPHERAL_APP_Context, connHdl, HCI_ROLE_PERIPHERAL); 
}


/**
 * @brief Prints the values of central_array and peripheral_array in the given context.
 * 
 * @param context The context containing the arrays to be printed.
 */
void APP_Context_printContextArrays(void) {
  printf("\tCentral Array:\n");
  for (int i = 0; i < CFG_BLE_NUM_CLT_CONTEXT_MAX; i++)
  {
    printf("\tCentral[%d]: ConnectionHandle = 0x%04X, PairingStatus = 0x%02X\n", 
           i, MC_CENTRALPERIPHERAL_APP_Context.central_array[i].connectionHandle, MC_CENTRALPERIPHERAL_APP_Context.central_array[i].pairing_status);
  }

  printf("\tPeripheral Array:\n");
  for (int i = 0; i < CFG_BLE_PERIPHERAL_HANDLES_MAX; i++)
  {
    printf("\tPeripheral[%d]: ConnectionHandle = 0x%04X, PairingStatus = 0x%02X\n", 
           i, MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle, MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].pairing_status);
  }
}

/**
* @brief Terminates connections.
* 
* This function terminates the current BLE connections with the error message BLE_ERROR_TERMINATED_LOCAL_HOST.
* It iterates through the connection handles in the provided context and terminates each active connection.
* 
* @param context Pointer to the application context containing the connection handles.
* @param isRolePeripheral Flag indicating whether the role is peripheral (HCI_ROLE_PERIPHERAL) or central (HCI_ROLE_CENTRAL). Numeric values for the role are peripheral (1) or central (0).
* 
* @retval void
*/
static void Terminate_Connections(MC_CENTRALPERIPHERAL_APP_Context_t* context, int isRolePeripheral)
{
  ContextData_t* contextDataArray = isRolePeripheral ? context->peripheral_array : context->central_array;
  int maxHandles = isRolePeripheral ? CFG_BLE_PERIPHERAL_HANDLES_MAX : CFG_BLE_NUM_CLT_CONTEXT_MAX;
  
  for (int i = 0; i < maxHandles; i++) 
  {
    if (contextDataArray[i].connectionHandle != 0xFFFF) 
    {
      APP_DBG_MSG("aci_gap_terminate init: connectionHandle=0x%04X\n", contextDataArray[i].connectionHandle);
      uint32_t status = aci_gap_terminate(contextDataArray[i].connectionHandle, BLE_ERROR_TERMINATED_REMOTE_USER);
      
      /* Check if the termination command was successful */
      if (status != BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("aci_gap_terminate failure: reason=0x%02X\n", status);
      }
      else
      {
        APP_DBG_MSG("==>> aci_gap_terminate : Success\n");
        
        /* Wait for the HCI_DISCONNECTION_COMPLETE_EVT_CODE event */
        UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_PROC_GAP_COMPLETE);
      }
    }
  }
  return;
}

/**
* @brief Terminates all peripheral connections.
* 
* This function is the specific implementation for CFG_TASK_TERMINATE_ALL_PERIPHERAL_CONNECTIONS.
* It terminates all current BLE connections for peripheral devices with the error message BLE_ERROR_TERMINATED_LOCAL_HOST.
* 
* @retval void
*/
static void Terminate_Peripheral_Connections(void)
{
  Terminate_Connections(&MC_CENTRALPERIPHERAL_APP_Context, HCI_ROLE_PERIPHERAL);
}

/**
* @brief Terminates all central connections.
* 
* This function is the specific implementation for CFG_TASK_TERMINATE_ALL_CENTRAL_CONNECTIONS.
* It terminates all current BLE connections for central devices with the error message BLE_ERROR_TERMINATED_LOCAL_HOST.
* 
* @retval void
*/
static void Terminate_Central_Connections(void)
{
  Terminate_Connections(&MC_CENTRALPERIPHERAL_APP_Context, HCI_ROLE_CENTRAL);
}

/**
* @brief Handle the security request task.
*/
static void Peripheral_Security_Request(void)
{
  /* Specific implementation for CFG_TASK_PERIPHERAL_SECURITY_REQUEST */
  uint8_t status = 0;
  
  /**  
  * @note BLE Security Levels and Dependencies:
  *       - GAP_SECURITY_LEVEL_1: No security (no authentication or encryption).
  *          - No MITM protection required.
  *          - No dependency on IO_CAPABILITY.
  *       - GAP_SECURITY_LEVEL_2: Unauthenticated pairing with encryption.
  *          - No MITM protection required.
  *          - No dependency on IO_CAPABILITY.
  *       - GAP_SECURITY_LEVEL_3: Authenticated pairing with encryption.
  *          - MITM protection required.
  *          - Depends on IO_CAPABILITY for pairing method (e.g., Passkey Entry, Numeric Comparison).
  *       - GAP_SECURITY_LEVEL_4: Authenticated LE Secure Connections pairing with encryption using a 128-bit strength encryption key.
  *          - MITM protection required.
  *          - Depends on IO_CAPABILITY for pairing method (e.g., Numeric Comparison, Passkey Entry).
  */ 
  
  for( uint8_t i=0; i<CFG_BLE_PERIPHERAL_HANDLES_MAX; i++)
  {
    if(MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].pairing_status == 0x01 || MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].pairing_status == 0x05)
    {
      MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].pairing_status = 0x02;
      APP_DBG_MSG("   Init   : [ConnectionHandle: 0x%04X] Pairing request \n", MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle);
      status = aci_gap_set_security(MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle,
                                    GAP_SECURITY_LEVEL_2,
                                    0x01);
      if (status != BLE_STATUS_SUCCESS)
      {
        MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].pairing_status = 0x03;
        APP_DBG_MSG("  Fail   : [ConnectionHandle: 0x%04X] Pairing request 0x%02X\n", MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle, status);
      }
      else
      {
        MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].pairing_status = 0x04;
        APP_DBG_MSG("  Success: [ConnectionHandle: 0x%04X] Pairing request \n", MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle);
        BLEStack_Process_Schedule();
      }
    }
  }
  return;
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void MC_CENTRALPERIPHERAL_Switch_c_SendNotification(void) /* Property Notification */
{
  MC_CENTRALPERIPHERAL_APP_SendInformation_t notification_on_off = Switch_c_NOTIFICATION_OFF;
  MC_CENTRALPERIPHERAL_Data_t mc_centralperipheral_notification_data;

  mc_centralperipheral_notification_data.p_Payload = (uint8_t*)a_MC_CENTRALPERIPHERAL_UpdateCharData;
  mc_centralperipheral_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1*/
  
  if(MC_CENTRALPERIPHERAL_APP_Context.ButtonControl.ButtonStatus == 0x00)
  {
    MC_CENTRALPERIPHERAL_APP_Context.ButtonControl.ButtonStatus = 0x01;
  } 
  else
  {
    MC_CENTRALPERIPHERAL_APP_Context.ButtonControl.ButtonStatus = 0x00;
  }
  a_MC_CENTRALPERIPHERAL_UpdateCharData[0] = 0x01; /* Device Led selection */
  a_MC_CENTRALPERIPHERAL_UpdateCharData[1] = MC_CENTRALPERIPHERAL_APP_Context.ButtonControl.ButtonStatus;
  /* Update notification data length */
  mc_centralperipheral_notification_data.Length = (mc_centralperipheral_notification_data.Length) + 2; 
  
  if(MC_CENTRALPERIPHERAL_APP_Context.Switch_c_Notification_Status == Switch_c_NOTIFICATION_ON)
  { 
    APP_DBG_MSG("-- APPLICATION SERVER : INFORM CLIENT BUTTON 1 PUSHED\n");
    notification_on_off = Switch_c_NOTIFICATION_ON;
  } 
  else
  {
    APP_DBG_MSG("-- APPLICATION SERVER : CAN'T INFORM CLIENT - NOTIFICATION DISABLED\n"); 
  } 

  if (notification_on_off != Switch_c_NOTIFICATION_OFF)
  {
    for(uint16_t i=0; i<CFG_BLE_PERIPHERAL_HANDLES_MAX; i++)
    {
      if(MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle != 0xFFFF)
      {
        APP_DBG_MSG("   SendNotification periph_connHdl:0x%04X\n", MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle);
        MC_CENTRALPERIPHERAL_NotifyValue(MC_CENTRALPERIPHERAL_SWITCH_C, &mc_centralperipheral_notification_data, MC_CENTRALPERIPHERAL_APP_Context.peripheral_array[i].connectionHandle);
      }
    }
    return;
  }
  /* USER CODE END Service1Char2_NS_1*/

  if (notification_on_off != Switch_c_NOTIFICATION_OFF && MC_CENTRALPERIPHERAL_APP_Context.MC_CENTRALPERIPHERAL_periph_connHdl != 0xFFFF)
  {
    MC_CENTRALPERIPHERAL_NotifyValue(MC_CENTRALPERIPHERAL_SWITCH_C, &mc_centralperipheral_notification_data, MC_CENTRALPERIPHERAL_APP_Context.MC_CENTRALPERIPHERAL_periph_connHdl);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last*/
  
  /* USER CODE END Service1Char2_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
