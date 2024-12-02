/**
  ******************************************************************************
  * @file    esl_app.h
  * @author  GPM WBL Application Team
  * @brief   Header file for ESL APP profile.
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
#ifndef ESL_APP_H
#define ESL_APP_H

#define LED_ON_TIMEOUT_SECONDS          60
#define LED_BLINK_ON_TIME_MS            10
#define LED_BLINK_OFF_TIME_MS           1000

void ESL_APP_Process(void);

void ESL_APP_ProcessRequest(void);

#endif /* ESL_APP_H */