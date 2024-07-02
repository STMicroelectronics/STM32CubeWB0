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
#define MAX_NUM_PACKET         (100)            /* Number of packets used for the test */
#define TX_WAKEUP_TIME         (10000+700)      /* 10 ms + 700 us of guard time */
#define RX_TIMEOUT_ACK         (1000)           /*  1 ms */
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MIN(a,b)                        (((a) < (b))? (a) : (b))
#define PRINT_INT(x)    ((int)(x))
#define PRINT_FLOAT(x)  (x>0)? ((int) (((x) - PRINT_INT(x)) * 1000)) : (-1*(((int) (((x) - PRINT_INT(x)) * 1000))))
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t sendData[MAX_PACKET_LENGTH];
volatile uint8_t sendNewPacket;
uint8_t receivedData[MAX_PACKET_LENGTH];
uint8_t print_res = 0;

uint16_t packet_counter = 0;
uint16_t packet_counter_res = 0;
uint16_t crc_error_counter_res = 0;
uint16_t timeout_error_counter_res = 0;

uint16_t crc_error_counter;
uint16_t timeout_error_counter;
/* USER CODE END PV */

/* Global variables ----------------------------------------------------------*/

/* USER CODE BEGIN GV */

/* USER CODE END GV */

/* Private functions prototypes-----------------------------------------------*/

/* USER CODE BEGIN PFP */

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
  
  /* Build packet */
  sendData[0] = 0x02;
  sendData[1] = 5;   /* Length position is fixed */
  sendData[2] = 0x01;
  sendData[3] = 0x02;
  sendData[4] = 0x03;
  sendData[5] = 0x04;
  sendData[6] = 0;
  
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(0x88DF88DF);
  
  /* Configures the transmit power level */
  HAL_RADIO_SetTxPower(0x18);
  
  sendNewPacket = TRUE;
  /* USER CODE END APPE_Init_1 */


#if (CFG_LPM_SUPPORTED == 1)
  /* Low Power Manager Init */
  UTIL_LPM_Init();
#endif /* CFG_LPM_SUPPORTED */
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
  if(sendNewPacket == TRUE)
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
  uint8_t ret = SUCCESS_0;
  
  if(sendNewPacket){
    sendNewPacket = FALSE;
    ret =  HAL_RADIO_SendPacketWithAck(24, TX_WAKEUP_TIME, sendData, receivedData, RX_TIMEOUT_ACK, MAX_LL_PACKET_LENGTH, HAL_RADIO_Callback);
    if(ret != SUCCESS_0) {
      printf("ERROR %d (%d)\n",ret, packet_counter);
    }
    
    sendData[6] = (uint8_t)(++packet_counter);
    
    if(packet_counter == MAX_NUM_PACKET) {
      print_res = 1;
      packet_counter_res = packet_counter;
      crc_error_counter_res = crc_error_counter;
      timeout_error_counter_res = timeout_error_counter;
      packet_counter = 0;
      timeout_error_counter = 0;
      crc_error_counter = 0;
      sendData[6] = 0;
    }
  }
  if(print_res) {
    printf("Channel = %d, Timeout errors = %d, CRC errors = %d, PER = %d.%02d %%\n", 24, timeout_error_counter_res, crc_error_counter_res, PRINT_INT((timeout_error_counter_res + crc_error_counter_res)*100/packet_counter_res),PRINT_FLOAT((timeout_error_counter_res + crc_error_counter_res)*100/packet_counter_res));
    print_res = 0;
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
  BSP_LED_Toggle(LD3);
  crc_error_counter++;
  sendNewPacket = TRUE;
}

void HAL_RADIO_CallbackRcvTimeout(RxStats_t *rxPacketStats)
{
  BSP_LED_Toggle(LD2);
  timeout_error_counter++;
  sendNewPacket = TRUE;
}


void HAL_RADIO_CallbackRcvOk(RxStats_t *rxPacketStats)
{
  BSP_LED_Toggle(LD1);
  sendNewPacket = TRUE;
}
