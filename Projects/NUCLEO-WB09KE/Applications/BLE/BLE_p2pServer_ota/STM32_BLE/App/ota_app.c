/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ota_app.c
  * @author  MCD Application Team
  * @brief   ota_app application definition.
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
#include "ota_app.h"
#include "ota.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "flash_driver.h"
#include "flash_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef enum
{
  User_Conf,
  Fw_App,
} OTA_APP_FileType_t;

/* Round to next multiple of 4. */
#define ROUND4(a) ((a+3) & ~0x03)
/* USER CODE END PTD */

typedef enum
{
  Conf_INDICATION_OFF,
  Conf_INDICATION_ON,
  /* USER CODE BEGIN Service2_APP_SendInformation_t */
  OTA_APP_No_Pending,
  OTA_APP_Pending,
  OTA_APP_Ready_Pending,
  /* USER CODE END Service2_APP_SendInformation_t */
  OTA_APP_SENDINFORMATION_LAST
} OTA_APP_SendInformation_t;

typedef struct
{
  OTA_APP_SendInformation_t     Conf_Indication_Status;
  /* USER CODE BEGIN Service2_APP_Context_t */
  uint32_t base_address;
  uint8_t sectors;
  uint32_t write_value[ROUND4(CFG_BLE_ATT_MTU_MAX-3)/4];
  uint8_t  file_type;
  /* USER CODE END Service2_APP_Context_t */
  uint16_t              ConnectionHandle;
} OTA_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Define list of reboot reason */
#define REBOOT_ON_FW_APP          (0x00)
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* Compute start address where the User Configuration shall be located */
#define USER_CFG_SLOT_START_SECTOR_INDEX      (FLASH_PAGE_NUMBER - CFG_NVM_STATIC_NB_SECTORS - CFG_USER_CFG_NB_SECTORS)

/* Compute size in Page of Download and Active slots */
#define DOWNLOAD_ACTIVE_NB_SECTORS            (FLASH_PAGE_NUMBER - CFG_NVM_STATIC_NB_SECTORS - CFG_USER_CFG_NB_SECTORS - CFG_ACTIVE_SLOT_START_SECTOR_INDEX)

/* Compute size in Page of Download or active slot */
#define DOWNLOAD_SLOT_NB_SECTORS              (DOWNLOAD_ACTIVE_NB_SECTORS / 2)
#define APP_SLOT_NB_SECTORS                   DOWNLOAD_SLOT_NB_SECTORS

/* Compute start address where the NEW application shall be downloaded */
#define DOWNLOAD_SLOT_START_SECTOR_INDEX      (CFG_ACTIVE_SLOT_START_SECTOR_INDEX + APP_SLOT_NB_SECTORS)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static OTA_APP_Context_t OTA_APP_Context;

uint8_t a_OTA_UpdateCharData[247];

/* USER CODE BEGIN PV */
VTIMER_HandleType reset_timer;
static uint32_t size_left;
static uint32_t address_offset;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void OTA_Conf_SendIndication(void);

/* USER CODE BEGIN PFP */
static void Reboot(void);
static void DeleteSlot( uint8_t page_idx );
static void FM_WriteCallback (FM_FlashOp_Status_t Status);
/* Flag for write status  */
static FM_FlashOp_Status_t FM_WriteStatus;
/* Write test callback */
static FM_CallbackNode_t FM_WriteStatusCallback = 
{
  /* Header for chained list */
  .NodeList = 
  {
    .next = NULL,
    .prev = NULL
  },
  /* Callback for request status */
  .Callback = FM_WriteCallback
};
static void FM_EraseCallback (FM_FlashOp_Status_t Status);
/* Flag for write status  */
static FM_FlashOp_Status_t FM_EraseStatus;
/* Write test callback */
static FM_CallbackNode_t FM_EraseStatusCallback = 
{
  /* Header for chained list */
  .NodeList = 
  {
    .next = NULL,
    .prev = NULL
  },
  /* Callback for request status */
  .Callback = FM_EraseCallback
};
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void OTA_Notification(OTA_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_Notification_1 */
  OTA_Data_t msg_conf;
  /* USER CODE END Service2_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_Notification_Service2_EvtOpcode */
    case OTA_CONF_EVT:
      {
        /**
         * The Remote notifies it has send all the data to be written in Flash
         */
        /**
         * Decide now what to do after all the data has been written in Flash
         */
        switch(OTA_APP_Context.file_type)
        {
          case Fw_App:
            {
              APP_DBG_MSG("OTA_CONF_EVT: Reboot on new application\n");              
              
              UTIL_SEQ_SetTask(1<<CFG_TASK_OTA_REBOOT_REQ_ID, CFG_SEQ_PRIO_0);
            }
            break;
            
          default:
            break;
        }
      }
      break;

    case OTA_READY_EVT:
      break;
    /* USER CODE END Service2_Notification_Service2_EvtOpcode */

    case OTA_BASE_ADR_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service2Char1_WRITE_NO_RESP_EVT */
      {
        switch( ((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Command )
        {
          case OTA_STOP_ALL_UPLOAD:
            break;

          case OTA_USER_CONF_UPLOAD:
            {
              FM_Cmd_Status_t error = FM_ERROR;
              
              APP_DBG_MSG("OTA_USER_CONF_UPLOAD\n");

              OTA_APP_Context.file_type = User_Conf;
              OTA_APP_Context.base_address = USER_CFG_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE + FLASH_START_ADDR;
              address_offset = 0;              
              OTA_APP_Context.sectors = CFG_USER_CFG_NB_SECTORS;              
              OTA_APP_Context.Conf_Indication_Status = OTA_APP_Ready_Pending;
              
              /* Erase the sector */
              /* Clear events before start testing */
              UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
              
              while(error != FM_OK)
              {  
                /* Flash manager erase */
                if(OTA_APP_Context.sectors == 0)
                  OTA_APP_Context.sectors = CFG_USER_CFG_NB_SECTORS;
                error = FM_Erase((uint32_t)USER_CFG_SLOT_START_SECTOR_INDEX, 
                                 (uint32_t)(OTA_APP_Context.sectors),
                                 &FM_EraseStatusCallback);
                
                /* Check write op. */
                if (error == FM_OK)
                {
                  /* Wait for write callback to be invoked */
                  UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                  
                  /* Clear events before start testing */
                  UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                  
                  /* Check status of write op. */
                  if (FM_EraseStatus != FM_OPERATION_COMPLETE)
                  {
                    error = FM_ERROR;
                    APP_DBG_MSG("FM_WriteStatus != FM_OPERATION_COMPLETE => FM_ERROR\n");
                  }
                }
                else if(error == FM_BUSY)
                {
                  /* Wait for write callback to be invoked */
                  UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                  
                  /* Clear events before start testing */
                  UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                }
                else
                {
                  APP_DBG_MSG("FM_ERROR\n");
                }
              } /* while(error != FM_OK) */
              
              a_OTA_UpdateCharData[0] = OTA_READY_TO_RECEIVE_FILE;
              msg_conf.p_Payload = a_OTA_UpdateCharData;
              msg_conf.Length = 1;
              OTA_NotifyValue(OTA_CONF, &msg_conf, OTA_APP_Context.ConnectionHandle);
            }
            break;

          case OTA_APPLICATION_UPLOAD:
            {
              FM_Cmd_Status_t error = FM_ERROR;
              
              APP_DBG_MSG("OTA_APPLICATION_UPLOAD\n");
              
              OTA_APP_Context.file_type = Fw_App;
              OTA_APP_Context.base_address = (DOWNLOAD_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE) + FLASH_START_ADDR;              
              address_offset = 0;              
              OTA_APP_Context.sectors = DOWNLOAD_SLOT_NB_SECTORS;              
              OTA_APP_Context.Conf_Indication_Status = OTA_APP_Ready_Pending;
              
              /* Erase the sectors */
              /* Clear events before start testing */
              UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
              
              while(error != FM_OK)
              {  
                /* Flash manager write */
                if(OTA_APP_Context.sectors == 0)
                  OTA_APP_Context.sectors = DOWNLOAD_SLOT_NB_SECTORS;
                error = FM_Erase((uint32_t)((OTA_APP_Context.base_address - FLASH_START_ADDR) / FLASH_PAGE_SIZE), 
                                 (uint32_t)(OTA_APP_Context.sectors),
                                 &FM_EraseStatusCallback);
                
                /* Check write op. */
                if (error == FM_OK)
                {
                  /* Wait for write callback to be invoked */
                  UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                  
                  /* Clear events before start testing */
                  UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                  
                  /* Check status of write op. */
                  if (FM_EraseStatus != FM_OPERATION_COMPLETE)
                  {
                    error = FM_ERROR;
                    APP_DBG_MSG("FM_WriteStatus != FM_OPERATION_COMPLETE => FM_ERROR\n");
                  }
                }
                else if(error == FM_BUSY)
                {
                  /* Wait for write callback to be invoked */
                  UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                  
                  /* Clear events before start testing */
                  UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                }
                else
                {
                  APP_DBG_MSG("FM_ERROR\n");
                }
              } /* while(error != FM_OK) */
              
              msg_conf.Length = 1;
              a_OTA_UpdateCharData[0] = OTA_READY_TO_RECEIVE_FILE;
              msg_conf.p_Payload = a_OTA_UpdateCharData;
              OTA_NotifyValue(OTA_CONF, &msg_conf, OTA_APP_Context.ConnectionHandle);
            }
            break;

          case OTA_UPLOAD_FINISHED:
            {
              APP_DBG_MSG("OTA_UPLOAD_FINISHED\n");
              if(OTA_APP_Context.file_type == Fw_App)
              { /* Reboot only after new application download */
                OTA_APP_Context.Conf_Indication_Status = OTA_APP_Pending;
                msg_conf.Length = 1;
                a_OTA_UpdateCharData[0] = OTA_REBOOT_CONFIRMED;
                msg_conf.p_Payload = a_OTA_UpdateCharData;
                OTA_NotifyValue(OTA_CONF, &msg_conf, OTA_APP_Context.ConnectionHandle);
              }
            }
            break;

          case OTA_CANCEL_UPLOAD:
            {
              APP_DBG_MSG("OTA_CANCEL_UPLOAD\n");
            }
            break;

          default:
            break;
        }
      }
      /* USER CODE END Service2Char1_WRITE_NO_RESP_EVT */
      break;

    case OTA_CONF_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service2Char2_INDICATE_ENABLED_EVT */
      APP_DBG_MSG("OTA_CONF_INDICATE_ENABLED_EVT\n");
      /* USER CODE END Service2Char2_INDICATE_ENABLED_EVT */
      break;

    case OTA_CONF_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service2Char2_INDICATE_DISABLED_EVT */
      APP_DBG_MSG("OTA_CONF_INDICATE_DISABLED_EVT\n");
      /* USER CODE END Service2Char2_INDICATE_DISABLED_EVT */
      break;

    case OTA_RAW_DATA_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service2Char3_WRITE_NO_RESP_EVT */
      {
        FM_Cmd_Status_t error = FM_ERROR;
        
        /**
         * Write in Flash the data received in the BLE packet
         */
        size_left = p_Notification->DataTransfered.Length;
        
        if(OTA_APP_Context.file_type == Fw_App)
        {
          // Check address
          if(OTA_APP_Context.base_address + address_offset + size_left > FLASH_START_ADDR + ((DOWNLOAD_SLOT_START_SECTOR_INDEX + DOWNLOAD_SLOT_NB_SECTORS) * FLASH_PAGE_SIZE))
          {
            APP_DBG_MSG("Write address outside of allowed space\n");
            return;
          }
        }
        else if(OTA_APP_Context.file_type == User_Conf)
        {
          if(OTA_APP_Context.base_address + address_offset + size_left > FLASH_START_ADDR + ((USER_CFG_SLOT_START_SECTOR_INDEX + CFG_USER_CFG_NB_SECTORS) * FLASH_PAGE_SIZE))
          {
            APP_DBG_MSG("Write address outside of allowed space.\n");
            return;
          }          
        }
        
        /**
         * For the flash manager the address of the data to be stored in FLASH shall be 32bits aligned
         * and the address where the data shall be written shall be 128bits aligned
         */
        memcpy( (uint8_t*)&OTA_APP_Context.write_value,
                ((OTA_Raw_Data_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Raw_Data,
                size_left );

        /* Clear events before start testing */
        UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);
        
        FD_SetStatus(FD_FLASHACCESS_RFTS_BYPASS, FD_FLASH_ENABLE);

        while(error != FM_OK)
        { 
          //APP_DBG_MSG("FM_Write 0x%08X %d\n", (uint32_t *)((OTA_APP_Context.base_address) + address_offset), size_left);
          /* Flash manager write */
          error = FM_Write ((uint32_t *)(&OTA_APP_Context.write_value[0]),
                            (uint32_t *)((OTA_APP_Context.base_address) + address_offset),
                            size_left >> 2,
                            &FM_WriteStatusCallback);

          /* Check write op. */
          if (error == FM_OK)
          {
            /* Wait for write callback to be invoked */
            UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);

            /* Clear events before start testing */
            UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);

            /* Check status of write op. */
            if (FM_WriteStatus != FM_OPERATION_COMPLETE)
            {
              error = FM_ERROR;
              APP_DBG_MSG("OTA_RAW_DATA_ID: FM_WriteStatus != FM_OPERATION_COMPLETE => FM_ERROR\n");
            }
          }
          else if(error == FM_BUSY)
          {
            /* Wait for write callback to be invoked */
            UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);
            
            /* Clear events before start testing */
            UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);
          }
          else
          {
            APP_DBG_MSG("OTA_RAW_DATA_ID: FM_ERROR\n");
            break;
          }
        } /* while(error != FM_OK) */
        
        FD_SetStatus(FD_FLASHACCESS_RFTS_BYPASS, FD_FLASH_DISABLE);

        /* Update write offset address for the next FLASH write */
        address_offset += size_left;
      }
      /* USER CODE END Service2Char3_WRITE_NO_RESP_EVT */
      break;

    default:
      /* USER CODE BEGIN Service2_Notification_default */

      /* USER CODE END Service2_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service2_Notification_2 */

  /* USER CODE END Service2_Notification_2 */
  return;
}

void OTA_APP_EvtRx(OTA_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_APP_EvtRx_1 */

  /* USER CODE END Service2_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_APP_EvtRx_Service2_EvtOpcode */

    /* USER CODE END Service2_APP_EvtRx_Service2_EvtOpcode */
    case OTA_CONN_HANDLE_EVT :
      OTA_APP_Context.ConnectionHandle = p_Notification->ConnectionHandle;
      /* USER CODE BEGIN Service2_APP_CENTR_CONN_HANDLE_EVT */

      /* USER CODE END Service2_APP_CENTR_CONN_HANDLE_EVT */
      break;
    case OTA_DISCON_HANDLE_EVT :
      OTA_APP_Context.ConnectionHandle = 0xFFFF;
      /* USER CODE BEGIN Service2_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service2_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service2_APP_EvtRx_default */

      /* USER CODE END Service2_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service2_APP_EvtRx_2 */

  /* USER CODE END Service2_APP_EvtRx_2 */

  return;
}

void OTA_APP_Init(void)
{
  OTA_APP_Context.ConnectionHandle = 0xFFFF;
  OTA_Init();

  /* USER CODE BEGIN Service2_APP_Init */
  reset_timer.callback = reset;  
  
  UTIL_SEQ_RegTask( 1U << CFG_TASK_OTA_REBOOT_REQ_ID, UTIL_SEQ_RFU, Reboot);  
  DeleteSlot( DOWNLOAD_SLOT_START_SECTOR_INDEX ); /* Erase download slot */
  FM_WriteStatus = FM_OPERATION_AVAILABLE;
  size_left = 0;
  address_offset = 0;
  /* USER CODE END Service2_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
static void Reboot(void)
{  
  APP_BLE_Procedure_Gap_General(PROC_GAP_GEN_CONN_TERMINATE);
  
  /**
  * Reboot on FW Application
  */
  CFG_OTA_REBOOT_VAL_MSG_ADDR = REBOOT_ON_FW_APP;
  
  /**
  * Give the download sector
  */
  CFG_OTA_START_SECTOR_IDX_VAL_MSG_ADDR = (OTA_APP_Context.base_address - FLASH_START_ADDR) / FLASH_PAGE_SIZE;
  
  /* Do not reset immediately to give time to write potential bonding info.  */
  HAL_RADIO_TIMER_StartVirtualTimer(&reset_timer, 500);  
}
  
void reset(void *tnode)
{
  NVIC_SystemReset();  
}

/**
 * Get Confiramation status
 */
uint8_t OTA_APP_GetConfStatus(void)
{
  return(OTA_APP_Context.Conf_Indication_Status);
}

/**
 * Erase active or download slot
 */
static void DeleteSlot( uint8_t page_idx )
{

  /**
   * The number of sectors to erase is read from SRAM1.
   * It shall be checked whether the number of sectors to erase does not overlap on the secured Flash
   * The limit can be read from the SFSA option byte which provides the first secured sector address.
   */

  uint32_t last_page_idx = page_idx + APP_SLOT_NB_SECTORS - 1;
  FLASH_EraseInitTypeDef p_erase_init;
  uint32_t page_error;
  uint32_t NbrOfPageToBeErased = (uint32_t)APP_SLOT_NB_SECTORS;

  if(page_idx < CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
  {
    /**
     * Something has been wrong as there is no case we should delete the BLE_BootMngr application
     * Reboot on the active firmware application
     */
    CFG_OTA_REBOOT_VAL_MSG_ADDR = REBOOT_ON_FW_APP;
    NVIC_SystemReset(); /* it waits until reset */
  }

  if ((page_idx + NbrOfPageToBeErased - 1) > last_page_idx)
  {
    NbrOfPageToBeErased = last_page_idx - page_idx + 1;
  }

  p_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
  p_erase_init.NbPages = NbrOfPageToBeErased;
  p_erase_init.Page = (uint32_t)page_idx;

  if(HAL_FLASHEx_Erase(&p_erase_init, &page_error) != HAL_OK)
  {
    APP_DBG_MSG("Erase error!\n");
  }
  
  return;
}

static void FM_WriteCallback (FM_FlashOp_Status_t Status)
{
  /* Update status */ 
  FM_WriteStatus = Status;

  /* Set event on Process request call */
  UTIL_SEQ_SetEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);
}

static void FM_EraseCallback (FM_FlashOp_Status_t Status)
{
  /* Update status */ 
  FM_EraseStatus = Status;

  /* Set event on Process request call */
  UTIL_SEQ_SetEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void OTA_Conf_SendIndication(void) /* Property Indication */
{
  OTA_APP_SendInformation_t indication_on_off = Conf_INDICATION_OFF;
  OTA_Data_t ota_indication_data;

  ota_indication_data.p_Payload = (uint8_t*)a_OTA_UpdateCharData;
  ota_indication_data.Length = 0;

  /* USER CODE BEGIN Service2Char2_IS_1*/

  /* USER CODE END Service2Char2_IS_1*/

  if (indication_on_off != Conf_INDICATION_OFF && OTA_APP_Context.ConnectionHandle != 0xFFFF)
  {
    OTA_NotifyValue(OTA_CONF, &ota_indication_data, OTA_APP_Context.ConnectionHandle);
  }

  /* USER CODE BEGIN Service2Char2_IS_Last*/

  /* USER CODE END Service2Char2_IS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
