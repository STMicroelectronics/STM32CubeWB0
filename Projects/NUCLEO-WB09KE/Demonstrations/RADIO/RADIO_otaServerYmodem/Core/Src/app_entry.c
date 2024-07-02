/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_entry.c
  * @author  GPM WBL Application Team
  * @brief   Entry point of the application
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
#if (CFG_LPM_SUPPORTED == 1)
#include "stm32_lpm.h"
#endif /* CFG_LPM_SUPPORTED */

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
#define MIN(a,b)                        (((a) < (b))? (a) : (b))
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */
extern uint8_t rx_ota_buffer[MAX_PACKET_LENGTH], tx_ota_buffer[MAX_PACKET_LENGTH];

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/


static void RxUART_Init(void)
{
  /* Enable the RX not empty interrupt */
  
  LL_USART_EnableIT_RXNE(USART1);
  
  HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif
}

uint32_t MX_APPE_Init(void *p_param)
{
  UNUSED(p_param);
  
  /* USER CODE BEGIN APPE_Init_1 */
  BSP_LED_Init(LD1);
  
  COM_InitTypeDef COM_Init = {0};

  COM_Init.BaudRate= 115200;
  COM_Init.HwFlowCtl = COM_HWCONTROL_NONE;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.Parity = COM_PARITY_NONE;
  COM_Init.StopBits = COM_STOPBITS_1;
  BSP_COM_Init(COM1, &COM_Init);
  
  RxUART_Init();
    
//  Clock_Init();
  
  /* Build OTA packet */
  tx_ota_buffer[0] = HEADER_CONNECTION;
  tx_ota_buffer[1] = 0;

  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(OTA_ACCESS_ADDRESS);

  /* Configures the transmit power level */
  HAL_RADIO_SetTxPower(0x18);
  /* USER CODE END APPE_Init_1 */
    

#if (CFG_LPM_SUPPORTED == 1)
  /* Low Power Manager Init */
  UTIL_LPM_Init();
#endif /* CFG_LPM_SUPPORTED */
/* USER CODE BEGIN APPE_Init_2 */
  OTA_Init();
  Ymodem_Init();
  
  BSP_LED_On(LD1);
  printf("\r\nOTA manager server (version: %s)\r\n", OTA_MANAGER_VERSION_STRING);
/* USER CODE END APPE_Init_2 */
   return 0;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
#if (CFG_LPM_SUPPORTED == 1)
static PowerSaveLevels App_PowerSaveLevel_Check(void)
{
  PowerSaveLevels output_level = POWER_SAVE_LEVEL_STOP;

  /* USER CODE BEGIN App_PowerSaveLevel_Check_1 */
  uint8_t app_tonken_status = OTA_Get_App_Token_Status();

  if(app_tonken_status == TOKEN_TAKEN_YMODEM)
  {
    output_level = POWER_SAVE_LEVEL_RUNNING;
  }
  
  ota_state_machine_t ota_status = OTA_GetStatus();
  if (ota_status == OTA_DATAREQ)
  {
    output_level = POWER_SAVE_LEVEL_RUNNING;
  }
  /* USER CODE END App_PowerSaveLevel_Check_1 */

  return output_level;
}
#endif

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/
void MX_APPE_Process(void)
{
  /* USER CODE BEGIN MX_APPE_Process_1 */
  
  /* USER CODE END MX_APPE_Process_1 */
  
  /* Perform calibration procedure */
  HAL_RADIO_TIMER_Tick();
  
  /* USER CODE BEGIN MX_APPE_Process_2 */
  OTA_ymodem_tick();
  OTA_Tick();
  /* USER CODE END MX_APPE_Process_2 */
}


void MX_APPE_Idle(void)
{
#if (CFG_LPM_SUPPORTED == 1)
  PowerSaveLevels app_powerSave_level, vtimer_powerSave_level, final_level;

  /* Need to consume some CSTACK, due to bootloader CSTACK usage. */
  volatile uint32_t dummy[15];
  uint8_t i;
  for (i=0; i<10; i++)
  {
    dummy[i] = 0;
    __NOP();
  }
  
  app_powerSave_level = App_PowerSaveLevel_Check();
  
  if(app_powerSave_level != POWER_SAVE_LEVEL_RUNNING) 
  {  
    vtimer_powerSave_level = HAL_RADIO_TIMER_PowerSaveLevelCheck();
    final_level = (PowerSaveLevels)MIN(vtimer_powerSave_level, app_powerSave_level);
     
    switch(final_level)
    {
    case POWER_SAVE_LEVEL_RUNNING:
      /* Not Power Save device is busy */
      return;
      break;
    case POWER_SAVE_LEVEL_CPU_HALT:
      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
      break;
    case POWER_SAVE_LEVEL_STOP_LS_CLOCK_ON:
      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
      break;
    case POWER_SAVE_LEVEL_STOP:
      UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
      UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_ENABLE);
      break;
    }
    
    UTIL_LPM_EnterLowPower();
  }
#endif /* CFG_LPM_SUPPORTED */

}

void HAL_RADIO_CallbackRcvError(RxStats_t *rxPacketStats)
{
  /* USER CODE BEGIN */
  
  /* USER CODE END */
}

void HAL_RADIO_CallbackRcvTimeout(RxStats_t *rxPacketStats)
{
  /* USER CODE BEGIN */
  
  /* USER CODE END */
}

void HAL_RADIO_CallbackTxDone(void)
{
  /* USER CODE BEGIN */
  
  /* USER CODE END */
}

void HAL_RADIO_CallbackRcvOk(RxStats_t *rxPacketStats)
{
  /* USER CODE BEGIN */
  
  /* USER CODE END */
}
