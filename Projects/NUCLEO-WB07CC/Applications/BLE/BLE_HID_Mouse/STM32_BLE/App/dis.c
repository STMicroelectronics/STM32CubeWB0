/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    DIS.c
  * @author  MCD Application Team
  * @brief   DIS definition.
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
#include <app_common.h>
#include "ble.h"
#include "dis.h"
#include "dis_app.h"
#include "ble_evt.h"

/* USER CODE BEGIN Includes */
#include "app_ble.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  DisSvcHdle;				/**< Dis Service Handle */
  uint16_t  MansCharHdle;			/**< MANS Characteristic Handle */
  uint16_t  MonsCharHdle;			/**< MONS Characteristic Handle */
  uint16_t  FrsCharHdle;			/**< FRS Characteristic Handle */
  uint16_t  SrsCharHdle;			/**< SRS Characteristic Handle */
  uint16_t  PniCharHdle;			/**< PNI Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}DIS_Context_t;

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET        2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET             1
#define MANS_SIZE        32	/* Manufacturer Name Characteristic size */
#define MONS_SIZE        32	/* Model Number Characteristic size */
#define FRS_SIZE        32	/* Firmware Revision Characteristic size */
#define SRS_SIZE        32	/* Software Revision Characteristic size */
#define PNI_SIZE        7	/* PnPID Characteristic size */
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static DIS_Context_t DIS_Context;

/* USER CODE BEGIN PV */
extern devContext_Type devContext;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

/*
 * UUIDs for Device Information service
 */
#define DEVICE_INFORMATION_UUID			(0x180A)
#define MANUFACTURER_NAME_UUID			(0x2A29)
#define MODEL_NUMBER_UUID			(0x2A24)
#define FIRMWARE_REVISION_UUID			(0x2A26)
#define SOFTWARE_REVISION_UUID			(0x2A28)
#define PNPID_UUID			(0x2A50)

/* USER CODE BEGIN DESCRIPTORS DECLARATION */

/* USER CODE END DESCRIPTORS DECLARATION */

uint8_t mans_val_buffer[MANS_SIZE];

static ble_gatt_val_buffer_def_t mans_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = MANS_SIZE,
  .buffer_len = sizeof(mans_val_buffer),
  .buffer_p = mans_val_buffer
};

uint8_t mons_val_buffer[MONS_SIZE];

static ble_gatt_val_buffer_def_t mons_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = MONS_SIZE,
  .buffer_len = sizeof(mons_val_buffer),
  .buffer_p = mons_val_buffer
};

uint8_t frs_val_buffer[FRS_SIZE];

static ble_gatt_val_buffer_def_t frs_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = FRS_SIZE,
  .buffer_len = sizeof(frs_val_buffer),
  .buffer_p = frs_val_buffer
};

uint8_t srs_val_buffer[SRS_SIZE];

static ble_gatt_val_buffer_def_t srs_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG | BLE_GATT_SRV_OP_VALUE_VAR_LENGTH_FLAG,
  .val_len = SRS_SIZE,
  .buffer_len = sizeof(srs_val_buffer),
  .buffer_p = srs_val_buffer
};

uint8_t pni_val_buffer[PNI_SIZE];

static ble_gatt_val_buffer_def_t pni_val_buffer_def = {
  .op_flags = BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG,
  .val_len = PNI_SIZE,
  .buffer_len = sizeof(pni_val_buffer),
  .buffer_p = pni_val_buffer
};

/* Device Information service characteristics definition */
static const ble_gatt_chr_def_t dis_chars[] = {
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(MANUFACTURER_NAME_UUID),
        .val_buffer_p = &mans_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(MODEL_NUMBER_UUID),
        .val_buffer_p = &mons_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(FIRMWARE_REVISION_UUID),
        .val_buffer_p = &frs_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(SOFTWARE_REVISION_UUID),
        .val_buffer_p = &srs_val_buffer_def
    },
	{
        .properties = BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ,
        .min_key_size = 0x10,
        .uuid = BLE_UUID_INIT_16(PNPID_UUID),
        .val_buffer_p = &pni_val_buffer_def
    },
};

/* Device Information service definition */
static const ble_gatt_srv_def_t dis_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_16(DEVICE_INFORMATION_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)dis_chars,
       .chr_count = 5U,
   },
};

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static BLEEVT_EvtAckStatus_t DIS_EventHandler(aci_blecore_event *p_evt)
{
  BLEEVT_EvtAckStatus_t return_value = BLEEVT_NoAck;
  aci_gatt_srv_read_event_rp0    *p_read;
  /* USER CODE BEGIN Service2_EventHandler_1 */

  /* USER CODE END Service2_EventHandler_1 */

  switch(p_evt->ecode)
  {
    case ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

      /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

      /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
      break;/* ACI_GATT_SRV_ATTRIBUTE_MODIFIED_VSEVT_CODE */
    }
    case ACI_GATT_SRV_READ_VSEVT_CODE :
    {
      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_BEGIN */

      /* USER CODE END EVT_BLUE_GATT_SRV_READ_BEGIN */
      p_read = (aci_gatt_srv_read_event_rp0*)p_evt->data;
	  if(p_read->Attribute_Handle == (DIS_Context.MansCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service2_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service2_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service2_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service2_Char_1_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (DIS_Context.MansCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
	  else if(p_read->Attribute_Handle == (DIS_Context.MonsCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service2_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service2_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service2_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service2_Char_2_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (DIS_Context.MonsCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
	  else if(p_read->Attribute_Handle == (DIS_Context.FrsCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service2_Char_3_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service2_Char_3_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service2_Char_3_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service2_Char_3_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (DIS_Context.FrsCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
	  else if(p_read->Attribute_Handle == (DIS_Context.SrsCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service2_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service2_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service2_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service2_Char_4_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (DIS_Context.SrsCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
	  else if(p_read->Attribute_Handle == (DIS_Context.PniCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
	  {
		return_value = BLEEVT_Ack;
		/*USER CODE BEGIN Service2_Char_5_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE END Service2_Char_5_ACI_GATT_SRV_READ_VSEVT_CODE_1 */

		/*USER CODE BEGIN Service2_Char_5_ACI_GATT_SRV_READ_VSEVT_CODE_2 */

		  /*USER CODE END Service2_Char_5_ACI_GATT_SRV_READ_VSEVT_CODE_2 */
	  } /* if(p_read->Attribute_Handle == (DIS_Context.PniCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_READ_END */

      /* USER CODE END EVT_EVT_BLUE_GATT_SRV_READ_END */
      break;/* ACI_GATT_SRV_READ_VSEVT_CODE */
    }
    case ACI_GATT_SRV_WRITE_VSEVT_CODE:
    {
      /* USER CODE BEGIN EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE END EVT_BLUE_SRV_GATT_BEGIN */

      /* USER CODE BEGIN EVT_BLUE_GATT_SRV_WRITE_END */

      /* USER CODE END EVT_BLUE_GATT_SRV_WRITE_END */
      break;/* ACI_GATT_SRV_WRITE_VSEVT_CODE */
    }
    case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
    {
      aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
      p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_evt->data;
      UNUSED(p_tx_pool_available_event);

      /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */

      /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
      break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
    }
    case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
    {
      aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
      p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_evt->data;
      UNUSED(p_exchange_mtu);

      /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

      /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
      break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
    }
    /* USER CODE BEGIN BLECORE_EVT */

    /* USER CODE END BLECORE_EVT */
  default:
    /* USER CODE BEGIN EVT_DEFAULT */

    /* USER CODE END EVT_DEFAULT */
    break;
  }

  /* USER CODE BEGIN Service2_EventHandler_2 */

  /* USER CODE END Service2_EventHandler_2 */

  return(return_value);
}/* end DIS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void DIS_Init(void)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  UNUSED(DIS_Context);

  /* USER CODE BEGIN InitService2Svc_1 */

  /* USER CODE END InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  BLEEVT_RegisterGattEvtHandler(DIS_EventHandler);

  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&dis_service);

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail   : aci_gatt_srv_add_service command: DIS, error code: 0x%x \n", ret);
  }
  else
  {
    APP_DBG_MSG("  Success: aci_gatt_srv_add_service command: DIS \n");
  }

  DIS_Context.DisSvcHdle = aci_gatt_srv_get_service_handle((ble_gatt_srv_def_t *) &dis_service);
  DIS_Context.MansCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&dis_chars[0]);
  DIS_Context.MonsCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&dis_chars[1]);
  DIS_Context.FrsCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&dis_chars[2]);
  DIS_Context.SrsCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&dis_chars[3]);
  DIS_Context.PniCharHdle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&dis_chars[4]);

  /* USER CODE BEGIN InitService1Svc_2 */
  devContext.devInfServHandle = DIS_Context.DisSvcHdle;
  /* USER CODE END InitService1Svc_2 */

  if (ret != BLE_STATUS_SUCCESS)
  {
    APP_DBG_MSG("  Fail registering DIS handlers\n");
  }

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the new data to be written in the characteristic
 *
 */
tBleStatus DIS_UpdateValue(DIS_CharOpcode_t CharOpcode, DIS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;

  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case DIS_MANS:
      memcpy(mans_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(mans_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_1*/

      /* USER CODE END Service2_Char_Value_1*/
      break;

    case DIS_MONS:
      memcpy(mons_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(mons_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_2*/

      /* USER CODE END Service2_Char_Value_2*/
      break;

    case DIS_FRS:
      memcpy(frs_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(frs_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_3*/

      /* USER CODE END Service2_Char_Value_3*/
      break;

    case DIS_SRS:
      memcpy(srs_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(srs_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_4*/

      /* USER CODE END Service2_Char_Value_4*/
      break;

    case DIS_PNI:
      memcpy(pni_val_buffer, pData->p_Payload, MIN(pData->Length, sizeof(pni_val_buffer)));
      /* USER CODE BEGIN Service2_Char_Value_5*/

      /* USER CODE END Service2_Char_Value_5*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Update_Char_2 */

  /* USER CODE END Service2_App_Update_Char_2 */

  return ret;
}

/**
 * @brief  Characteristic notification
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: pointer to the data to be notified to the client
 * @param  ConnectionHandle: connection handle identifying the client to be notified.
 *
 */
tBleStatus DIS_NotifyValue(DIS_CharOpcode_t CharOpcode, DIS_Data_t *pData, uint16_t ConnectionHandle)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Notify_Char_1 */

  /* USER CODE END Service2_App_Notify_Char_1 */

  switch(CharOpcode)
  {

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Notify_Char_2 */

  /* USER CODE END Service2_App_Notify_Char_2 */

  return ret;
}
