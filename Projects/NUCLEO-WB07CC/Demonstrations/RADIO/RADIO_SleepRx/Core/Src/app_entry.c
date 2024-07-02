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
#define RX_WAKEUP_TIME         (10000)     /* 10 ms */
#define RX_TIMEOUT_OK           (5000)     /*  5 ms */
#define RX_TIMEOUT_NOTOK       (30000)     /* 30 ms */
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
#define MIN(a,b)                        (((a) < (b))? (a) : (b))
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t schedule_rx;
extern uint8_t print_pckt;
uint32_t rx_timeout = RX_TIMEOUT_NOTOK;
uint8_t receivedData[MAX_PACKET_LENGTH];
uint8_t sendAckData[2] = {0xAE, 0}; /* 0xAE ACK value, length = 0 */
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/

/* USER CODE BEGIN PFP */
void MX_Radio_ReceivePacketWithAck(void);
/* USER CODE END PFP */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Functions Definition ------------------------------------------------------*/

uint32_t MX_APPE_Init(void *p_param)
{
  UNUSED(p_param);
  
  /* USER CODE BEGIN APPE_Init_1 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);
  
  COM_InitTypeDef COM_Init = {0};
  
  COM_Init.BaudRate= 115200;
  COM_Init.HwFlowCtl = COM_HWCONTROL_NONE;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.Parity = COM_PARITY_NONE;
  COM_Init.StopBits = COM_STOPBITS_1;
  BSP_COM_Init(COM1, &COM_Init);
  
  BSP_LED_On(LD1);
  
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(0x88DF88DF);
  
  /* Configures the transmit power level */
  HAL_RADIO_SetTxPower(0x18);
  /* USER CODE END APPE_Init_1 */
  
#if (CFG_LPM_SUPPORTED == 1)  
  /* Low Power Manager Init */
  UTIL_LPM_Init();  
#endif
  
  /* USER CODE BEGIN APPE_Init_2 */
  
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
  if(schedule_rx == TRUE)
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
  if(schedule_rx == TRUE) {
    schedule_rx = FALSE;
    MX_Radio_ReceivePacketWithAck();
    
    if(print_pckt == TRUE) {
      printf("Packet Received: ");
      for(volatile uint16_t i = 0; i < (receivedData[1] + HEADER_LENGTH); i++) {
        printf("%02X ", receivedData[i]);
      }
      printf("\n");
      print_pckt = FALSE;
    }
  }
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
  BSP_LED_Toggle(LD2);
  rx_timeout = RX_TIMEOUT_NOTOK;
  schedule_rx = TRUE;
}

void HAL_RADIO_CallbackRcvTimeout(RxStats_t *rxPacketStats)
{
  BSP_LED_Toggle(LD2);
  rx_timeout = RX_TIMEOUT_NOTOK;
  schedule_rx = TRUE;
}

void HAL_RADIO_CallbackTxDone(void)
{
  schedule_rx = TRUE;
}

void HAL_RADIO_CallbackRcvOk(RxStats_t *rxPacketStats)
{
  BSP_LED_Toggle(LD1);
  print_pckt = TRUE;
  rx_timeout = RX_TIMEOUT_OK;
}


void MX_Radio_ReceivePacketWithAck(void)
{
  uint8_t ret = SUCCESS_0;
  
  ret = HAL_RADIO_ReceivePacketWithAck(24, RX_WAKEUP_TIME, receivedData, sendAckData, rx_timeout, MAX_LL_PACKET_LENGTH, HAL_RADIO_Callback);
  if(ret != SUCCESS_0) {
    printf("ERROR %d\n",ret);
  }
}
