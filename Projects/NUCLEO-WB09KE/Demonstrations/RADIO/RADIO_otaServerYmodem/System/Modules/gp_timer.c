/**
******************************************************************************
* @file    gp_timer.c 
* @author  GPM WBL Application Team
* @brief   General purpose timer library.
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
#include "gp_timer.h"

/*---------------------------------------------------------------------------*/

void Timer_Set(struct timer *t, tClockTime interval)
{
  t->interval = interval;
  t->start = Clock_Time();
}

void Timer_Reset(struct timer *t)
{
  t->start += t->interval;
}

void Timer_Restart(struct timer *t)
{
  t->start = Clock_Time();
}

uint8_t Timer_Expired(struct timer *t)
{
  tClockTime diff = (Clock_Time() - t->start) + 1;
  return (t->interval < diff);

}

tClockTime Timer_Remaining(struct timer *t)
{
  return (t->start + t->interval - Clock_Time());
}



