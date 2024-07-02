/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    types.h
  * @author  GPM WBL Application Team
  * @brief   Define the various basic types needed by the Library
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRL_TYPES_H__
#define __CRL_TYPES_H__


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup UserAPI User Level API
  * @{
  */

/** @addtogroup TYPES Data Types Definitions
  * @{
  */

#if !defined(_STDINT_H) && !defined(__stdint_h) && !defined(_STDINT)

#ifndef __uint8_t_defined

typedef unsigned int uint32_t;       /*!< stdint.h definition of uint32_t */
typedef unsigned char uint8_t;       /*!< stdint.h definition of uint8_t */
typedef unsigned short int uint16_t; /*!< stdint.h definition of uint16_t */
typedef unsigned long long uint64_t; /*!< stdint.h definition of uint64_t */

#define __uint8_t_defined
#define __uint16_t_defined
#define __uint32_t_defined
#define __uint64_t_defined
#endif /*!<  __uint8_t_defined */

#ifndef __int8_t_defined

typedef signed long long int64_t;  /*!< stdint.h definition of int64_t */
typedef signed int int32_t;        /*!< stdint.h definition of int32_t */
typedef signed short int int16_t;  /*!< stdint.h definition of int16_t */
typedef signed char int8_t;        /*!< stdint.h definition of int8_t */
#define __int8_t_defined
#define __int16_t_defined
#define __int32_t_defined
#define __int64_t_defined
#endif /* __int8_t_defined */

#endif /* !defined(_STDINT_H) && !defined(__stdint_h) && !defined(_STDINT) */

/** @brief Type definitation for a pre-allocated memory buffer that is required by some functions */
typedef struct 
{
  uint8_t *pmBuf;  /*!< Pointer to the pre-allocated memory buffer, this must be set by the user*/
  uint16_t  mSize; /*!< Total size of the pre-allocated memory buffer */
  uint16_t  mUsed; /*!< Currently used portion of the buffer, should be inititalized by user to zero */
}
membuf_stt;

/**
  * @}
  */

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif


#endif /* __CRL_TYPES_H__ */

