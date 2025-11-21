/**
  ******************************************************************************
  * @file           : transparent_mode_config.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UPDATER_CONFIG_H
#define UPDATER_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "system_stm32wb0x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#if defined(STM32WB05) || defined(STM32WB09)
#define BLUE_FLAG_FLASH_BASE_ADDRESS    (0x10041014)
#define DTM_APP_ADDR                    (0x10041000)


/* SPI */

#define TM_SPI                  (SPI3)
#define TM_SPI_CLK()            LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3)
#define TM_SPI_DMA_REQ_TX      (DMA_REQUEST_SPI3_TX)
#define TM_SPI_DMA_REQ_RX      (DMA_REQUEST_SPI3_RX)


#define TM_GPIO_CLK()  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); \
                                   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

#define TM_SPI_MISO_GPIO_PORT  (GPIOA)
#define TM_SPI_MISO_PIN        (LL_GPIO_PIN_8)
#define TM_SET_AF_MISO_PIN()    LL_GPIO_SetAFPin_8_15(TM_SPI_MISO_GPIO_PORT, TM_SPI_MISO_PIN, LL_GPIO_AF_3)
#define TM_AF_MISO_PIN        (LL_GPIO_AF_3)


#define TM_SPI_MOSI_GPIO_PORT  (GPIOA)
#define TM_SPI_MOSI_PIN        (LL_GPIO_PIN_11)
#define TM_SET_AF_MOSI_PIN()    LL_GPIO_SetAFPin_8_15(TM_SPI_MOSI_GPIO_PORT, TM_SPI_MOSI_PIN, LL_GPIO_AF_3)
#define TM_AF_MOSI_PIN        (LL_GPIO_AF_3)
#define TM_SPI_MOSI_PWR_PORT   (PWR_GPIO_A)
#define TM_SPI_MOSI_PWR_PIN    (PWR_GPIO_BIT_11)


#define TM_SPI_SCK_GPIO_PORT   (GPIOB)
#define TM_SPI_SCK_PIN         (LL_GPIO_PIN_3)
#define TM_SET_AF_SCK_PIN()     LL_GPIO_SetAFPin_0_7(TM_SPI_SCK_GPIO_PORT, TM_SPI_SCK_PIN, LL_GPIO_AF_4)
#define TM_AF_SCK_PIN        (LL_GPIO_AF_4)

#define TM_SPI_CS_GPIO_PORT    (GPIOA)
#define TM_SPI_CS_PIN          (LL_GPIO_PIN_9)
#define TM_SET_AF_CS_PIN()      LL_GPIO_SetAFPin_8_15(TM_SPI_CS_GPIO_PORT, TM_SPI_CS_PIN, LL_GPIO_AF_3)
#define TM_AF_CS_PIN        (LL_GPIO_AF_3)

#define TM_SPI_IRQ_GPIO_PORT   (GPIOA)
#define TM_SPI_IRQ_PIN         (LL_GPIO_PIN_10)
#define TM_SPI_IRQ_PWR_GPIO_PIN         (PWR_GPIO_A)
#define TM_SPI_IRQ_PWR_GPIO_PORT        (PWR_GPIO_BIT_10)


/* UART */
#define TM_UART_GPIO_CLK()  __HAL_RCC_GPIOA_CLK_ENABLE();   \
                               __HAL_RCC_GPIOB_CLK_ENABLE();     

#define TM_UART_RX_GPIO_PORT   (GPIOB)
#define TM_UART_RX_PIN         (GPIO_PIN_0)
#define TM_SET_AF_UART_RX_PIN()   LL_GPIO_SetAFPin_0_7(TM_UART_RX_GPIO_PORT, TM_UART_RX_PIN, LL_GPIO_AF_0);
#define TM_AF_UART_RX_PIN       (GPIO_AF0_USART1)
                       
  
#define TM_UART_TX_GPIO_PORT   (GPIOA)
#define TM_UART_TX_PIN         (GPIO_PIN_1)
#define TM_SET_AF_UART_TX_PIN()   LL_GPIO_SetAFPin_0_7(TM_UART_TX_GPIO_PORT, TM_UART_TX_PIN, LL_GPIO_AF_2)
#define TM_AF_UART_TX_PIN       (GPIO_AF2_USART1)

#endif /* defined(STM32WB05) || defined(STM32WB09) */


#if defined(STM32WB06) || defined(STM32WB07)
#define BLUE_FLAG_FLASH_BASE_ADDRESS    (0x10041014)
#define DTM_APP_ADDR                    (0x10041000)

#define TM_SPI                  (SPI1)
#define TM_SPI_CLK()            LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI1)
#define TM_SPI_DMA_REQ_TX      (DMA_REQUEST_SPI1_TX)
#define TM_SPI_DMA_REQ_RX      (DMA_REQUEST_SPI1_RX)


#define TM_GPIO_CLK()  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); \
                                   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

#define TM_SPI_MISO_GPIO_PORT  (GPIOA)
#define TM_SPI_MISO_PIN        (LL_GPIO_PIN_14)
#define TM_SET_AF_MISO_PIN()    LL_GPIO_SetAFPin_8_15(TM_SPI_MISO_GPIO_PORT, TM_SPI_MISO_PIN, LL_GPIO_AF_2)
#define TM_AF_MISO_PIN        (LL_GPIO_AF_2)
                                   
#define TM_SPI_MOSI_GPIO_PORT  (GPIOA)
#define TM_SPI_MOSI_PIN        (LL_GPIO_PIN_15)
#define TM_SET_AF_MOSI_PIN()    LL_GPIO_SetAFPin_8_15(TM_SPI_MOSI_GPIO_PORT, TM_SPI_MOSI_PIN, LL_GPIO_AF_2)
#define TM_AF_MOSI_PIN        (LL_GPIO_AF_2)
#define TM_SPI_MOSI_PWR_PORT   (PWR_GPIO_A)
#define TM_SPI_MOSI_PWR_PIN    (PWR_GPIO_BIT_15)

#define TM_SPI_SCK_GPIO_PORT   (GPIOA)
#define TM_SPI_SCK_PIN         (LL_GPIO_PIN_13)
#define TM_SET_AF_SCK_PIN()     LL_GPIO_SetAFPin_8_15(TM_SPI_SCK_GPIO_PORT, TM_SPI_SCK_PIN, LL_GPIO_AF_2)
#define TM_AF_SCK_PIN        (LL_GPIO_AF_2)

#define TM_SPI_CS_GPIO_PORT    (GPIOA)
#define TM_SPI_CS_PIN          (LL_GPIO_PIN_11)
#define TM_SET_AF_CS_PIN()      LL_GPIO_SetAFPin_8_15(TM_SPI_CS_GPIO_PORT, TM_SPI_CS_PIN, LL_GPIO_AF_1)
#define TM_AF_CS_PIN        (LL_GPIO_AF_1)

#define TM_SPI_IRQ_GPIO_PORT   (GPIOA)
#define TM_SPI_IRQ_PIN         (LL_GPIO_PIN_10)
#define TM_SPI_IRQ_PWR_GPIO_PIN         (PWR_GPIO_A)
#define TM_SPI_IRQ_PWR_GPIO_PORT        (PWR_GPIO_BIT_10)


#define TM_UART_GPIO_CLK()  __HAL_RCC_GPIOA_CLK_ENABLE();                  

#define TM_UART_RX_GPIO_PORT   (GPIOA)
#define TM_UART_RX_PIN         (GPIO_PIN_8)
#define TM_SET_AF_UART_RX_PIN()   LL_GPIO_SetAFPin_8_15(TM_UART_RX_GPIO_PORT, TM_UART_RX_PIN, LL_GPIO_AF_0);
#define TM_AF_UART_RX_PIN       (GPIO_AF0_USART1)

#define TM_UART_TX_GPIO_PORT   (GPIOA)
#define TM_UART_TX_PIN         (GPIO_PIN_9)
#define TM_SET_AF_UART_TX_PIN()   LL_GPIO_SetAFPin_8_15(TM_UART_TX_GPIO_PORT, TM_UART_TX_PIN, LL_GPIO_AF_0)
#define TM_AF_UART_TX_PIN       (GPIO_AF0_USART1)

#endif /* defined(STM32WB06) || defined(STM32WB07) */



/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* UPDATER_CONFIG_H */
