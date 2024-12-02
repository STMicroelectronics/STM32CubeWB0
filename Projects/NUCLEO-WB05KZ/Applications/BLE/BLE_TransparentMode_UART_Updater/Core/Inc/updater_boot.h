/**
  ******************************************************************************
  * @file           : Updater_boot.h
  * @brief          : Updater_boot file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UPDATER_BOOT_H
#define UPDATER_BOOT_H

/* Includes ------------------------------------------------------------------*/
void DTM_SystemInit(void);
extern void SystemCoreClockUpdate(void);
extern uint32_t SystemCoreClock;            /*!< System Clock Frequency */
extern const intvec_elem __vector_table[];
#endif /* UPDATER_BOOT_H */
