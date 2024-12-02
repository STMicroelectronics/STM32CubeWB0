/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : main file
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

#include "updater.h"
#include "transparent_mode_config.h"
#include "system_stm32wb0x.h"
#include "stm32wb0x_ll_gpio.h"

#define ENTERED_REASON_ACI_CMD          2
#define ENTERED_REASON_BAD_BLUEFLAG     3
#define ENTERED_REASON_IRQ_PIN          4



typedef void (*EntryPoint)(void);

int main(void)
{
  uint32_t boot_pin;
  __disable_irq();
  if ((RCC->CSR == 0) && (PWR->SR1 != 0)) {
    EntryPoint entryPoint = (EntryPoint)(*(volatile uint32_t *)(DTM_APP_ADDR + 4));
    __set_MSP(*(volatile uint32_t*) DTM_APP_ADDR);
    entryPoint();
    
    while(1);
  }
  
  // The boot pin is the SPI_MOSI. It is configured in Input Pull-Up @ reset state.
  // We need to re-configure the boot pin with the Pull-Down resistor.
  HAL_PWREx_EnableGPIOPullDown(TM_SPI_MOSI_PWR_PORT, TM_SPI_MOSI_PWR_PIN);
  //If the SPI_MOSI is floating we need to read the line multiple times to have the correct value
  for (int i=0; i<4; i++) {
    boot_pin = LL_GPIO_IsInputPinSet(TM_SPI_MOSI_GPIO_PORT, TM_SPI_MOSI_PIN);
  }
  // Reconfigure the reset pull value
  HAL_PWREx_EnableGPIOPullUp(TM_SPI_MOSI_PWR_PORT, TM_SPI_MOSI_PWR_PIN);
  
  /* if BLUE_FLAG_RESET => a previous programming operation went bad */
  if(RAM_VR.BlueFlag == BLUE_FLAG_RAM_RESET) {
    RAM_VR.BlueFlag = BLUE_FLAG_SET;
    updater_init();
    updater(ENTERED_REASON_ACI_CMD);
  }
  else if(*(uint32_t *)BLUE_FLAG_FLASH_BASE_ADDRESS != BLUE_FLAG_SET) {
    updater_init();
    updater(ENTERED_REASON_BAD_BLUEFLAG);
  }
  else if(boot_pin != 0 && !(RAM_VR.ResetReason & (RCC_CSR_PORRSTF|RCC_CSR_SFTRSTF|RCC_CSR_WDGRSTF|RCC_CSR_LOCKUPRSTF))) {
    updater_init();
    updater(ENTERED_REASON_IRQ_PIN);
  }
  else {
    EntryPoint entryPoint = (EntryPoint)(*(volatile uint32_t *)(DTM_APP_ADDR + 4));
    __set_MSP(*(volatile uint32_t*) DTM_APP_ADDR);
    entryPoint();
    
    while(1);
  }
  return 0;
}
