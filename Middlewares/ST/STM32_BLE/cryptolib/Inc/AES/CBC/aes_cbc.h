/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    aes_cbc.h
  * @author  GPM WBL Application Team
  * @brief   AES in CBC Mode
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
#ifndef __CRL_AES_CBC_H__
#define __CRL_AES_CBC_H__

#ifdef __cplusplus
extern "C"
{
#endif

  /** @ingroup AESCBC
    * @{
    */

  /* Exported functions --------------------------------------------------------*/
#ifdef INCLUDE_ENCRYPTION
/**
  * @brief Initialization for AES Encryption in CBC Mode
  * @param[in,out]    *P_pAESCBCctx  AES CBC context
  * @param[in]        *P_pKey        Buffer with the Key
  * @param[in]        *P_pIv         Buffer with the IV
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_CONTEXT   Context was not initialized with valid values, see the note below.
  * @note \c P_pAESCBCctx.mKeySize (see \ref AESCBCctx_stt) must be set with the size of the key prior to calling this function.
  *           Instead of the size of the key, you can also use the following predefined values:
  *         - \ref CRL_AES128_KEY
  *         - \ref CRL_AES192_KEY
  *         - \ref CRL_AES256_KEY
  * @note \c P_pAESCBCctx.mFlags must be set prior to calling this function. Default value is E_SK_DEFAULT.
  *          See \ref SKflags_et for details.
  * @note \c P_pAESCBCctx.mIvSize must be set with the size of the IV (default \ref CRL_AES_BLOCK) prior to calling this function.
  */
  int32_t AES_CBC_Encrypt_Init   (AESCBCctx_stt *P_pAESCBCctx,   \
                                  const uint8_t *P_pKey,         \
                                  const uint8_t *P_pIv);

/**
  * @brief AES Encryption in CBC Mode
  * @param[in,out] *P_pAESCBCctx     AES CBC, already initialized, context
  * @param[in]    *P_pInputBuffer   Input buffer
  * @param[in]     P_inputSize      Size of input data, expressed in bytes
  * @param[out]   *P_pOutputBuffer  Output buffer
  * @param[out]   *P_pOutputSize    Pointer to integer that will contain the size of written output data, expressed in bytes
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_INPUT_SIZE  P_inputSize < 16
  * @retval    AES_ERR_BAD_OPERATION   Append not allowed
  * @note In case of a call where \c P_inputSize is greater than 16 and not multiple of 16, Ciphertext Stealing will be activated.
  *        See CBC-CS2 of <a href="SP 800-38 A - Addendum"> NIST SP 800-38A Addendum </a>
  * @remark This function can be called multiple times, provided that \c P_inputSize is a multiple of 16.
  */
  int32_t AES_CBC_Encrypt_Append (AESCBCctx_stt *P_pAESCBCctx,   \
                                  const uint8_t *P_pInputBuffer, \
                                  int32_t P_inputSize,           \
                                  uint8_t *P_pOutputBuffer,      \
                                  int32_t *P_pOutputSize);
  
/**
  * @brief AES Encryption Finalization of CBC Mode
  * @param[in,out]    *P_pAESCBCctx     AES CBC, already initialized, context
  * @param[out]   *P_pOutputBuffer  Output buffer
  * @param[out]   *P_pOutputSize    Pointer to integer that will contain the size of written output data, expressed in bytes
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @remark    This function won't write output data, thus it can be skipped. \n It is kept for API compatibility.
  */
  int32_t AES_CBC_Encrypt_Finish (AESCBCctx_stt *P_pAESCBCctx,   \
                                  uint8_t       *P_pOutputBuffer, \
                                  int32_t       *P_pOutputSize);
#endif

#ifdef INCLUDE_DECRYPTION
/**
  * @brief Initialization for AES Decryption in CBC Mode
  * @param[in,out]    *P_pAESCBCctx  AES CBC context
  * @param[in]        *P_pKey        Buffer with the Key
  * @param[in]        *P_pIv         Buffer with the IV
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_CONTEXT   Context was not initialized with valid values, see the note below.
  * @note \c P_pAESCBCctx.mKeySize (see \ref AESCBCctx_stt) must be set with the size of the key prior to calling this function.
  *           Instead of the size of the key, you can also use the following predefined values:
  *         - \ref CRL_AES128_KEY
  *         - \ref CRL_AES192_KEY
  *         - \ref CRL_AES256_KEY
  * @note \c P_pAESCBCctx.mFlags must be set prior to calling this function. Default value is E_SK_DEFAULT.
  *          See \ref SKflags_et for details.
  * @note \c P_pAESCBCctx.mIvSize must be set with the size of the IV (default \ref CRL_AES_BLOCK) prior to calling this function.
  */
  int32_t AES_CBC_Decrypt_Init   (AESCBCctx_stt *P_pAESCBCctx,   \
                                  const uint8_t *P_pKey,         \
                                  const uint8_t *P_pIv);

/**
  * @brief AES Decryption in CBC Mode
  * @param[in,out] *P_pAESCBCctx     AES CBC, already initialized, context
  * @param[in]    *P_pInputBuffer   Input buffer
  * @param[in]     P_inputSize      Size of input data, expressed in bytes
  * @param[out]   *P_pOutputBuffer  Output buffer
  * @param[out]   *P_pOutputSize    Pointer to integer that will contain the size of written output data, expressed in bytes
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_INPUT_SIZE  P_inputSize < 16
  * @retval    AES_ERR_BAD_OPERATION   Append not allowed
  * @note In case of a call where \c P_inputSize is greater than 16 and not multiple of 16, Ciphertext Stealing will be activated.
  *        See CBC-CS2 of <a href="SP 800-38 A - Addendum"> NIST SP 800-38A Addendum </a>
  * @remark This function can be called multiple times, provided that \c P_inputSize is a multiple of 16.
  */
  int32_t AES_CBC_Decrypt_Append (AESCBCctx_stt *P_pAESCBCctx,   \
                                  const uint8_t *P_pInputBuffer, \
                                  int32_t        P_inputSize,    \
                                  uint8_t       *P_pOutputBuffer, \
                                  int32_t       *P_pOutputSize);

/**
  * @brief AES Decryption Finalization of CBC Mode
  * @param[in,out] *P_pAESCBCctx     AES CBC, already initialized, context
  * @param[out]   *P_pOutputBuffer  Output buffer
  * @param[out]   *P_pOutputSize    Pointer to integer that will contain the size of written output data, expressed in bytes
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @remark    This function won't write output data, thus it can be skipped. \n It is kept for API compatibility.
  */
  int32_t AES_CBC_Decrypt_Finish (AESCBCctx_stt *P_pAESCBCctx,   \
                                  uint8_t       *P_pOutputBuffer, \
                                  int32_t       *P_pOutputSize);
#endif
  /**
    * @}
    */

#ifdef __cplusplus
}
#endif

#endif /* __CRL_AES_CBC_H__*/

