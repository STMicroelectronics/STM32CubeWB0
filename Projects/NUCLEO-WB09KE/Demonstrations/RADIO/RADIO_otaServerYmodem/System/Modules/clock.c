/**
******************************************************************************
* @file    clock.c 
* @author  GPM WBL Application Team
* @brief   clock APIs based on SySTick
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
/* Includes ------------------------------------------------------------------*/

#include "clock.h"
#include "stm32wb0x_ll_rcc.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile tClockTime sys_tick_count;
const tClockTime CLOCK_SECOND = 1000;

/*---------------------------------------------------------------------------*/
void SysCount_Handler(void) 
{
  sys_tick_count++;
  //printf("%d\r\n", sys_tick_count); //TBR
}

/*---------------------------------------------------------------------------*/

void Clock_Init(void)
{
  /** Configure SysTick to generate Interrupt with 1ms period */
  
  LL_RCC_ClocksTypeDef RCC_Clocks;
  
  LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
  
  SysTick_Config(((RCC_Clocks.SYSCLK_Frequency)/1000) -1UL); 
}

/*---------------------------------------------------------------------------*/

tClockTime Clock_Time(void)
{
  return sys_tick_count;
}

/*---------------------------------------------------------------------------*/
/**
 * Wait for a multiple of 1 ms.
 *
 */
void Clock_Wait(uint32_t i)
{
  tClockTime start;

  start = Clock_Time();
  while(Clock_Time() - start < (tClockTime)i);
}
/*---------------------------------------------------------------------------*/

