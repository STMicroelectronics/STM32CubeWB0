/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OTS_app.c
  * @author  GPAM Application Team
  * @brief   Implementation of Object Transfer Profile for ESL images
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "ots_app.h"
#include "ots.h"
#include "stm32_seq.h"
#include "esl_device.h"

/* Private includes ----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#define MAX_OBJ_IDX                                             (NUM_IMAGES - 1)
    
/* Bitmask for Mode Parameter of OACP write */    
#define OACP_WRITE_MODE_TRUNCATE                                            0x02

/* Bitmask for Object Properties */
#define OBJ_PROP_DELETE                                               0x00000001
#define OBJ_PROP_EXEC                                                 0x00000002
#define OBJ_PROP_READ                                                 0x00000004
#define OBJ_PROP_WRITE                                                0x00000008
#define OBJ_PROP_APPEND                                               0x00000010
#define OBJ_PROP_TRUNC                                                0x00000020
#define OBJ_PROP_PATCH                                                0x00000040
#define OBJ_PROP_MARK                                                 0x00000080

/* Object properties: Write & Patch */
#define OBJ_PROPERTIES            (OBJ_PROP_WRITE|OBJ_PROP_TRUNC|OBJ_PROP_PATCH)

/* Object type: unspecified */
#define OBJ_TYPE                                                          0x2ACA

#define MAX_OBJ_NAME_LENGTH                                                   10

#define PAGE_OFFSET_MASK                           (_MEMORY_BYTES_PER_PAGE_ - 1)

#define OBJ_TRANSFER_TIMEOUT_MS                                            30000

/* Private typedef -----------------------------------------------------------*/

typedef union
{
  uint32_t  b32[_MEMORY_BYTES_PER_PAGE_/4];
  uint8_t   b8[_MEMORY_BYTES_PER_PAGE_];
}buff_t;

typedef struct
{
  uint8_t   curr_obj_idx;
  uint8_t   curr_obj_id[OBJECT_ID_SIZE];
  char      curr_obj_name[MAX_OBJ_NAME_LENGTH];
  uint16_t  conn_handle;
  uint16_t  cid;
  VTIMER_HandleType timer;    /* timer used for timeout */
  bool      write_started;
  bool      flash_buff_valid; /* true if content of flash_buff is valid and still needs to be written */
  uint32_t  write_length;
  uint8_t   write_mode;
  uint32_t  curr_address;     /* current address in Flash where data has to be written  */
  uint32_t  end_address;      /* first free address after last data to be written in Flash. */
  buff_t    flash_buff;       /* This is needed to modify the page */
} OTS_APP_Context_t;

/* External variables --------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

#define BEGIN_OF_PAGE(a)                             ((a) & (~PAGE_OFFSET_MASK))
#define END_OF_PAGE(a)                            (((a) | PAGE_OFFSET_MASK) + 1)

/* Private variables ---------------------------------------------------------*/
static OTS_APP_Context_t OTS_APP_Context;

static const uint16_t obj_type = OBJ_TYPE;

/* Private function prototypes -----------------------------------------------*/

static void loadFlashPage(uint32_t address);
static void writeFlashPage(uint32_t address);
static void ObjTransferTimeout(void *arg);

/* Functions Definition ------------------------------------------------------*/

void OTS_APP_Init(void)
{
  OTS_Init();
  
  OTS_APP_Context.timer.callback = ObjTransferTimeout;

  return;
}

void OTS_GetCurrentObjName(char **name_p)
{
  snprintf(OTS_APP_Context.curr_obj_name, MAX_OBJ_NAME_LENGTH, "Image %d", OTS_APP_Context.curr_obj_idx);
  *name_p = OTS_APP_Context.curr_obj_name;  
}

/*uuid_type: 0 for 16-bit UUIDs, 1 for 128-bit UUIDS */
void OTS_GetCurrentObjType(uint8_t *uuid_type_p, uint8_t **uuid_p)
{
  *uuid_type_p = 0;
  *uuid_p = (uint8_t *)&obj_type;
}

void OTS_GetCurrentObjSize(uint32_t *current_size_p, uint32_t *allocated_size_p)
{
  uint32_t *obj_header_p;
  
  obj_header_p = (uint32_t *)(OTS_FLASH_STORAGE_START_ADDRESS + OTS_APP_Context.curr_obj_idx * (OBJ_ALLOC_SIZE + OBJ_HEADER_SIZE));
  
  if(*obj_header_p == 0xFFFFFFFF)
  {
    /* Special value: empty */
    *current_size_p = 0;
  }
  else
  {
    *current_size_p = *obj_header_p;
  }
  
  *allocated_size_p = OBJ_ALLOC_SIZE;
}

/* TODO: a dedicated sector can be used to avoid too many writes. Hovever, the
   size only changes if truncation is active, which should not happen since images
   should have fixed size in our case. */
static void updateObjectSize(uint32_t size)
{
  uint32_t obj_address;
  uint32_t current_size;
  uint32_t allocated_size;
  uint32_t page_start;
  uint32_t page_offset;
  
  OTS_GetCurrentObjSize(&current_size, &allocated_size);
  
  if((size < current_size) && !(OTS_APP_Context.write_mode & OACP_WRITE_MODE_TRUNCATE))
  {
    /* Do not change size if it is less than current and truncation is not enabled.  */
    return;
  }
  
  if(current_size != size)
  {
    obj_address = OTS_FLASH_STORAGE_START_ADDRESS + OTS_APP_Context.curr_obj_idx * (OBJ_ALLOC_SIZE + OBJ_HEADER_SIZE);
    page_start = BEGIN_OF_PAGE(obj_address);
    page_offset = obj_address - page_start;  
    loadFlashPage(obj_address);
    memcpy(&OTS_APP_Context.flash_buff.b8[page_offset], &size, 4);  
    writeFlashPage(obj_address);
  }
}

void OTS_GetCurrentObjID(uint8_t **id_p)
{
  /* Add the obj_index to the base value 0x000000000100 to obtain the object ID. */  
  OTS_APP_Context.curr_obj_id[1] = 0x01;
  OTS_APP_Context.curr_obj_id[0] = OTS_APP_Context.curr_obj_idx;
  *id_p = OTS_APP_Context.curr_obj_id;
}

void OTS_GetCurrentObjProp(uint32_t *prop_p)
{
  *prop_p = OBJ_PROPERTIES;
}

uint8_t OTS_OACPWrite(uint32_t offset, uint32_t length, uint8_t mode)
{
  uint32_t obj_address;
  uint32_t current_size;
  uint32_t allocated_size;
  
  APP_DBG_MSG("OTS_OACPWrite\n");
  
  if(OTS_APP_Context.cid == 0)
  {
    return OACP_RESULT_CHANNEL_UNAVAILABLE;
  }
  
  OTS_GetCurrentObjSize(&current_size, &allocated_size);
  
  if(offset > current_size)
  {
    return OACP_RESULT_INVALID_PARAM;
  }
  if(offset + length > OBJ_ALLOC_SIZE)
  {
    return OACP_RESULT_INVALID_PARAM;
  }
  
  OTS_APP_Context.write_length = length;
  OTS_APP_Context.write_mode = mode;
  
  obj_address = OTS_FLASH_STORAGE_START_ADDRESS + OTS_APP_Context.curr_obj_idx * (OBJ_ALLOC_SIZE + OBJ_HEADER_SIZE);
  
  OTS_APP_Context.curr_address = obj_address + OBJ_HEADER_SIZE + offset;
  OTS_APP_Context.end_address = OTS_APP_Context.curr_address + length;
  
  /* This is a check in case MAX_OBJ_IDX is not set accordingly to the reserved space  */
  if(OTS_APP_Context.end_address > OTS_FLASH_STORAGE_END_ADDRESS)
  {
    return OACP_RESULT_OPERATION_FAILED;
  }
  
  if(mode & OACP_WRITE_MODE_TRUNCATE)
  {
    updateObjectSize(offset);
  }
  
  loadFlashPage(OTS_APP_Context.curr_address);
  
  OTS_APP_Context.write_started = true;
  
  HAL_RADIO_TIMER_StartVirtualTimer(&OTS_APP_Context.timer, OBJ_TRANSFER_TIMEOUT_MS);
  
  return OACP_RESULT_SUCCESS;  
}

static void loadFlashPage(uint32_t address)
{
  uint32_t *flash_p;
  
  flash_p = (uint32_t *)BEGIN_OF_PAGE(address);
  
  /* Load Flash content in RAM. */
  /* TBR: use memcpy instead? */
  for(uint32_t i = 0; i < _MEMORY_BYTES_PER_PAGE_/4; i ++)
  {
    OTS_APP_Context.flash_buff.b32[i] = flash_p[i];
  }    
}

//TODO: Use flash manager/flash driver instead, to avoid collisions with radio activity?
static void writeFlashPage(uint32_t address)
{
  uint32_t page_address = BEGIN_OF_PAGE(address);
  uint32_t page_num = (page_address - _MEMORY_FLASH_BEGIN_) / _MEMORY_BYTES_PER_PAGE_;
  uint32_t PageError;
  
  FLASH_EraseInitTypeDef EraseInit = {
    .TypeErase = FLASH_TYPEERASE_PAGES,
    .Page = page_num,
    .NbPages = 1,
  };
  
  APP_DBG_MSG("OTP: Writing Flash Page\n");
  
  HAL_FLASHEx_Erase(&EraseInit, &PageError);
  
  for(int i = 0; i < _MEMORY_BYTES_PER_PAGE_; i += 16)
  {
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_BURST, page_address + i, (uint32_t)&OTS_APP_Context.flash_buff.b8[i]);
  }  
}

uint8_t OTS_OLCP(uint8_t op_code)
{
  uint8_t ret = OLCP_RESULT_SUCCESS;
  
  if(OTS_APP_Context.write_started)
  {
    return OLCP_RESULT_OPERATION_FAILED;
  }
    
  switch(op_code)
  {
  case OLCP_OPCODE_FIRST:
    
    OTS_APP_Context.curr_obj_idx = 0;    
    break;
    
  case OLCP_OPCODE_LAST:
    
    OTS_APP_Context.curr_obj_idx = MAX_OBJ_IDX;    
    break;
    
  case OLCP_OPCODE_PREVIOUS:
    
    if(OTS_APP_Context.curr_obj_idx > 0)
    {
      OTS_APP_Context.curr_obj_idx--;      
    }
    else
    {
      ret = OLCP_RESULT_OUT_OF_BOUNDS;
    }
    break;
    
  case OLCP_OPCODE_NEXT:
    
    if(OTS_APP_Context.curr_obj_idx < MAX_OBJ_IDX)
    {
      OTS_APP_Context.curr_obj_idx++;      
    }
    else
    {
      ret = OLCP_RESULT_OUT_OF_BOUNDS;
    }
    break;
    
  default:
    /* Other Op Codes are not supported */
    ret = OLCP_RESULT_NOT_SUPPORTED;
  }
  
  return ret;
}

void OTS_APP_L2CAPChannelOpened(uint16_t conn_handle, uint16_t cid)
{
  APP_DBG_MSG("OTS_APP_L2CAPChannelOpened cid: %d\n", cid);
  
  OTS_APP_Context.conn_handle = conn_handle;
  OTS_APP_Context.cid = cid;
}

//TODO: update object size with data written so far.
void OTS_APP_L2CAPChannelClosed(void)
{
  APP_DBG_MSG("OTS_APP_L2CAPChannelClosed\n");
  
  OTS_APP_Context.cid = 0;
  OTS_APP_Context.write_started = false;
  
  if(OTS_APP_Context.flash_buff_valid == true)
  {
    /* Buffer still has some data to be written */
    writeFlashPage(OTS_APP_Context.curr_address);
    
    OTS_APP_Context.flash_buff_valid = false;
  }
}

void OTS_APP_L2CAPDataReceived(uint16_t sdu_length, uint8_t *sdu_data)
{
  uint32_t page_start = BEGIN_OF_PAGE(OTS_APP_Context.curr_address);
  uint32_t page_end = page_start + _MEMORY_BYTES_PER_PAGE_;
  uint32_t page_offset = OTS_APP_Context.curr_address - page_start;
  uint32_t page_size = sizeof(OTS_APP_Context.flash_buff);
  
  if(OTS_APP_Context.write_started == false)
  {
    return;
  }
  
  HAL_RADIO_TIMER_StopVirtualTimer(&OTS_APP_Context.timer);
  HAL_RADIO_TIMER_StartVirtualTimer(&OTS_APP_Context.timer, OBJ_TRANSFER_TIMEOUT_MS);
  
  APP_DBG_MSG("OTP: Received SDU (length: %d bytes)\n", sdu_length);
  
  if(OTS_APP_Context.curr_address + sdu_length > OTS_APP_Context.end_address)
  {
    /* Attempt to write outside requested space. It should not happen. 
      Limit to requested size. */
    sdu_length = OTS_APP_Context.end_address - OTS_APP_Context.curr_address;
  }
  
  if(page_offset + sdu_length <= page_size)
  {
    OTS_APP_Context.flash_buff_valid = true;
    memcpy(&OTS_APP_Context.flash_buff.b8[page_offset], sdu_data, sdu_length);
    
    OTS_APP_Context.curr_address += sdu_length;
    
    if(OTS_APP_Context.curr_address == page_end &&
       OTS_APP_Context.curr_address != OTS_APP_Context.end_address) /* If OTS_APP_Context.curr_address == OTS_APP_Context.end_address, page is written later. */
    {
      /* End of flash page reached */
      writeFlashPage(page_start);
      OTS_APP_Context.flash_buff_valid = false;
    }    
  }
  else
  {
    /* Data is across two pages. */
    uint32_t first_part_data_length = page_size-page_offset;
    
    OTS_APP_Context.flash_buff_valid = true;
    
    memcpy(&OTS_APP_Context.flash_buff.b8[page_offset], sdu_data, first_part_data_length);
    
    writeFlashPage(page_start);
    
    loadFlashPage(page_end);
    
    memcpy(&OTS_APP_Context.flash_buff.b8[0], sdu_data + first_part_data_length, sdu_length - first_part_data_length);
    
    OTS_APP_Context.curr_address += sdu_length;
  }
  
  if(OTS_APP_Context.curr_address == OTS_APP_Context.end_address)
  {
    APP_DBG_MSG("OTP: Last data received\n");
    HAL_RADIO_TIMER_StopVirtualTimer(&OTS_APP_Context.timer);
    OTS_APP_Context.write_started = false;
    writeFlashPage(page_start);
    OTS_APP_Context.flash_buff_valid = false;
    
    updateObjectSize(OTS_APP_Context.write_length);
  }
  else if((OTS_APP_Context.curr_address & PAGE_OFFSET_MASK) == 0)
  {
    /* Address is now the beginning of a new page. */
    loadFlashPage(OTS_APP_Context.curr_address);
  }
}

static void ObjTransferTimeout(void *arg)
{
  aci_l2cap_cos_disconnect_req(OTS_APP_Context.conn_handle, OTS_APP_Context.cid);
}
