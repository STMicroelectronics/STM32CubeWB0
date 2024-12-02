/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/LPUART/LPUART_WakeUpFromDeepStop_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure LPUART peripheral in Asynchronous mode
  *          for being able to wake from Deep Stop mode when a character is received on RX line using
  *          the STM32WB0x LPUART LL API.
  *          Peripheral initialization done using LL initialization function.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/**
  * @brief Variables used for character reception from PC Com port
  */
__IO uint8_t      ubFinalCharReceived = 0;
__IO uint32_t     ubReceivedChar;

/**
  * @brief Text string printed on PC Com port to inform MCU will enter in Deep Stop Mode
  */
uint8_t aTextInfo[] = "\r\nLPUART Example : MCU will now enter in Deep Stop mode.\n\rEnter any character for waking up MCU.\r\n";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void     LED_On(void);
void     LED_Off(void);
void     LED_Blinking(uint32_t Period);
void     LED_Blinking_3s(void);
void     PrepareLPUARTToStopMode(void);
void     EnterDEEPSTOPMode(void);
void     PrintInfo(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB0_GRP1_EnableClock(LL_APB0_GRP1_PERIPH_SYSCFG);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LPUART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* Start main program loop :
     - make LED blink during 3 sec
     - Enter Deep Stop mode (LED turned Off)
     - Wait for any character received on LPUART RX line for waking up MCU
  */
  while (ubFinalCharReceived == 0)
  {
    /* LED blinks during 3 seconds */
    LED_Blinking_3s();

    /* Send Text Information on LPUART TX to PC Com port */
    PrintInfo();

    /* Prepare LPUART for entering Stop Mode */
    PrepareLPUARTToStopMode();

    /* Enter Deep Stop mode */
    EnterDEEPSTOPMode();

    /* At this point, MCU just wakes up from Deep Stop mode */
  }


  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_PWR_SetNoPullB(LL_PWR_GPIO_BIT_12 | LL_PWR_GPIO_BIT_13);
  LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_MEDIUMLOW);

  LL_RCC_LSE_Enable();
   /* Wait till LSE is ready */
  while(LL_RCC_LSE_IsReady() != 1)
  {
  }

   /* Set HSE Capacitor Tuning */
  LL_RCC_HSE_SetCapacitorTuning(CFG_HW_RCC_HSE_CAPACITOR_TUNE);

   /* Set HSE Current Control */
  LL_RCC_HSE_SetCurrentControl(LL_RCC_HSE_CURRENTMAX_3);

   /* Enable the HSE */
  LL_RCC_HSE_Enable();
/* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() == 0U);

  LL_FLASH_SetLatency(LL_FLASH_WAIT_STATES_1);

   /* Enable the RC64MPLL*/
  LL_RCC_RC64MPLL_Enable();
/* Wait till RC64MPLL is ready */
  while(LL_RCC_RC64MPLL_IsReady() == 0);

 /* Configure the RC64MPLL multiplication factor */
  LL_RCC_SetRC64MPLLPrescaler(LL_RCC_RC64MPLL_DIV_1);

 /* Update the SystemCoreClock global variable */
  LL_SetSystemCoreClock(64000000);

 /* Configure the source of time base considering new system clocks settings*/
  LL_Init1msTick(64000000);
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  LL_RCC_SetSMPSPrescaler(LL_RCC_SMPS_DIV_4);
}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  LL_LPUART_InitTypeDef LPUART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUCLKSEL_CLKLSE);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**LPUART1 GPIO Configuration
  PB7   ------> LPUART1_RX
  PB6   ------> LPUART1_TX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7|LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_3;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* LPUART1 interrupt Init */
  NVIC_SetPriority(LPUART1_IRQn, 0);
  NVIC_EnableIRQ(LPUART1_IRQn);

  /* USER CODE BEGIN LPUART1_Init 1 */
  /* Set LPUART1 clock source as HSI */
  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUCLKSEL_CLKLSE);

  /* USER CODE END LPUART1_Init 1 */
  LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
  LPUART_InitStruct.BaudRate = 9600;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
  LL_LPUART_SetTXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);

  /* USER CODE BEGIN WKUPType LPUART1 */

  /* Set the wake-up event type : specify wake-up on RXNE flag */
  LL_LPUART_SetWKUPType(LPUART1, LL_LPUART_WAKEUP_ON_RXNE);

  /* USER CODE END WKUPType LPUART1 */

  LL_LPUART_Enable(LPUART1);

  /* Polling LPUART1 initialisation */
  while((!(LL_LPUART_IsActiveFlag_TEACK(LPUART1))) || (!(LL_LPUART_IsActiveFlag_REACK(LPUART1))))
  {
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);

  /**/
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Function to configure LPUART for being ready to enter "Deep Stop" mode.
  * @param  None
  * @retval None
  */
void PrepareLPUARTToStopMode(void)
{

  /* Empty RX Fifo before entering Stop mode (Otherwise, characters already present in FIFO
     will lead to immediate wake up */
  while (LL_LPUART_IsActiveFlag_RXNE(LPUART1))
  {
    /* Read Received character. RXNE flag is cleared by reading of RDR register */
    ubReceivedChar = LL_LPUART_ReceiveData8(LPUART1);
  }

  /* Clear OVERRUN flag */
  LL_LPUART_ClearFlag_ORE(LPUART1);

  /* Make sure that no LPUART transfer is on-going */
  while (LL_LPUART_IsActiveFlag_BUSY(LPUART1) == 1)
  {
  }
  /* Make sure that LPUART is ready to receive */
  while (LL_LPUART_IsActiveFlag_REACK(LPUART1) == 0)
  {
  }

  /* About to enter stop mode: switch off LED */
  LED_Off();

  /* Configure LPUART1 transfer interrupts : */
  /* Clear WUF flag and enable the UART Wake Up from stop mode Interrupt */
  LL_LPUART_ClearFlag_WKUP(LPUART1);
  LL_LPUART_EnableIT_WKUP(LPUART1);

  /* Enable Wake Up From Stop */
  LL_LPUART_EnableInStopMode(LPUART1);
  
  LL_PWR_ClearWakeupSource(LL_PWR_SR1_WUF6);

}

/**
  * @brief  Function to enter in "Deep Stop" mode.
  * @param  None
  * @retval None
  */
void EnterDEEPSTOPMode(void)
{
  /** Request to enter "Deep Stop" mode
    * Following procedure describe in STM32WB0x Reference Manual
    * See PWR part, section Low-power modes, "Deep Stop" mode
    */
  /* Set Deep Stop mode when CPU enters deepsleep */
  LL_PWR_SetPowerMode(LL_PWR_MODE_DEEPSTOP);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  LL_LPM_EnableDeepSleep();

  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @brief  Send Txt information message on LPUART Tx line (to PC Com port).
  * @param  None
  * @retval None
  */
void PrintInfo(void)
{
  uint32_t index = 0;

  /* Send characters one per one, until last char to be sent */
  for (index = 0; index < sizeof(aTextInfo); index++)
  {
    /* Wait for TXE flag to be raised */
    while (!LL_LPUART_IsActiveFlag_TXE(LPUART1))
    {
    }

    /* Write character in Transmit Data register.
       TXE flag is cleared by writing data in TDR register */
    LL_LPUART_TransmitData8(LPUART1, aTextInfo[index]);
  }

  /* Wait for TC flag to be raised for last char */
  while (!LL_LPUART_IsActiveFlag_TC(LPUART1))
  {
  }
}

/**
  * @brief  Turn-on LD1.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LD1 on */
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  Turn-off LD1.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LD1 off */
  LL_GPIO_SetOutputPin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  Set LD1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    LL_mDelay(Period);
  }
}

/**
  * @brief  Set LD1 to Blinking mode during 3s.
  * @param  None
  * @retval None
  */
void LED_Blinking_3s(void)
{
  uint32_t index = 0;

  /* Toggle IO in during 3s (15*200ms) */
  for (index = 0; index < 15; index++)
  {
    LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    LL_mDelay(200);
  }
}

/******************************************************************************/
/*   IRQ HANDLER TREATMENT Functions                                          */
/******************************************************************************/

/**
  * @brief  Function called from LPUART IRQ Handler when RXNE flag is set
  *         Function is in charge of reading character received on LPUART RX line.
  * @param  None
  * @retval None
  */
void LPUART_CharReception_Callback(void)
{
  /* Read Received character. RXNE flag is cleared by reading of RDR register */
  ubReceivedChar = LL_LPUART_ReceiveData8(LPUART1);

  /* Check if received value is corresponding to specific one : S or s */
  if ((ubReceivedChar == 'S') || (ubReceivedChar == 's'))
  {
    /* Turn LD1 On : Expected character has been received */
    LED_On();

    /* End of program : set boolean for main loop exit */
    ubFinalCharReceived = 1;
  }

  /* Echo received character on TX */
  LL_LPUART_TransmitData8(LPUART1, ubReceivedChar);
}

/**
  * @brief  Function called in case of error detected in LPUART IT Handler
  * @param  None
  * @retval None
  */
void Error_Callback(void)
{
  /* Disable USART3_LPUART1_IRQn */
  NVIC_DisableIRQ(LPUART1_IRQn);

  /* Unexpected event : Set LD1 to Blinking mode to indicate error occurs */
  LED_Blinking(LED_BLINK_ERROR);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
