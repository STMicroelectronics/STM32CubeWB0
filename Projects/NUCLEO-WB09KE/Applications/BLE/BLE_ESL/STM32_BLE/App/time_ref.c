/**
  ******************************************************************************
  * @file         time_ref.c
  * @brief        This library implements a 32 bit virtual clock with tick
  *               duration of 1 millisecond, without the need to have a real
  *               tick (it relies on system time).
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

#include "stm32wb0x.h"
#include "stm32wb0x_hal_radio_timer.h"
    
#include "time_ref.h"     

#define MAX_ABS_TIME                        0xFFFFFFFF

/* This is the time after which a snapshot is taken of current system time and current absolute time. */
#define TIME_REF_UPATE_INTERVAL_MS   (MAX_ABS_TIME - 60000)

typedef struct
{
  uint32_t abs_time; /* Absolute time in milliseconds */
  uint64_t sys_time; /* System time that corresponds to the absolute time */
  
  VTIMER_HandleType sys_time_update_timer; /* Timer used to force an update of the reference system time. */
  
}TIMEREF_Context_t;

static TIMEREF_Context_t context;

static void SysTimeUpdateCallback(void * arg);

void TIMEREF_SetAbsoluteTime(uint32_t absolute_time)
{
  context.abs_time = absolute_time;
  context.sys_time = HAL_RADIO_TIMER_GetCurrentSysTime();
  
  context.sys_time_update_timer.callback = SysTimeUpdateCallback;
  HAL_RADIO_TIMER_StopVirtualTimer(&context.sys_time_update_timer);
  HAL_RADIO_TIMER_StartVirtualTimer(&context.sys_time_update_timer, TIME_REF_UPATE_INTERVAL_MS);  
}

static void update_ref_time(void)
{
  uint64_t current_sys_time;
  int64_t time_diff_ms;
  uint32_t current_abs_time;
  
  current_sys_time = HAL_RADIO_TIMER_GetCurrentSysTime();
  
  /* Calculate time in milliseconds from reference time. */
  time_diff_ms = HAL_RADIO_TIMER_DiffSysTimeMs(current_sys_time, context.sys_time);
  
  /* Add the absolute time in milliseconds at the reference time */  
  current_abs_time = context.abs_time + (uint32_t)time_diff_ms;  
  
  context.abs_time = current_abs_time;
  context.sys_time = current_sys_time;
}

uint32_t TIMEREF_GetCurrentAbsTime(void)
{
  uint64_t current_sys_time;
  int64_t time_diff_ms;
  uint32_t current_abs_time;
  
  current_sys_time = HAL_RADIO_TIMER_GetCurrentSysTime();
  
  /* Calculate time in milliseconds from reference time. */
  time_diff_ms = HAL_RADIO_TIMER_DiffSysTimeMs(current_sys_time, context.sys_time);
  
  /* Add the absolute time in milliseconds taken at the reference time */  
  current_abs_time = context.abs_time + (uint32_t)time_diff_ms;
  
  return current_abs_time;
}

static void SysTimeUpdateCallback(void * arg)
{
  update_ref_time();
  
  HAL_RADIO_TIMER_StartVirtualTimer(&context.sys_time_update_timer, TIME_REF_UPATE_INTERVAL_MS);
}

