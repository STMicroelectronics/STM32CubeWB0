/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    aes_cmac.h
  * @author  GPM WBL Application Team
  * @brief   AES in CMAC Mode
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
#ifndef __CRL_AES_CMAC_H__
#define __CRL_AES_CMAC_H__

#ifdef __cplusplus
extern "C"
{
#endif

  /** @ingroup AESCMAC
    * @{
    */

  /* Exported types ------------------------------------------------------------*/
  typedef struct
  {
    uint32_t   mContextId;   /*!< Unique ID of this context. \b Not \b used in current implementation. */
    SKflags_et mFlags; /*!< 32 bit mFlags, used to perform keyschedule and future use */
    const uint8_t *pmKey; /*!< Pointer to original Key buffer */
    const uint8_t *pmIv; /*!< Pointer to original Initialization Vector buffer */
    int32_t   mIvSize; /*!< Size of the Initialization Vector in bytes */
    uint32_t   amIv[4]; /*!< Temporary result/IV */
    int32_t   mKeySize;   /*!< Key length in bytes */
    uint32_t   amExpKey[CRL_AES_MAX_EXPKEY_SIZE];   /*!< Expanded AES key */
    const uint8_t *pmTag;   /*!< Pointer to Authentication TAG. This value must be set in decryption, and this TAG will be verified */
    int32_t mTagSize; /*!< Size of the Tag to return. This must be set by the caller prior to calling Init */
  }
  AESCMACctx_stt; /*<! AES context structure for CMAC mode */

  /* Exported functions --------------------------------------------------------*/
/**
  * @brief Initialization for AES-CMAC for Authentication TAG Generation
  * @param[in,out]    *P_pAESCMACctx  AES CMAC context
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_CONTEXT   Context was not initialized with valid values, see the note below.
  * @note \c P_pAESCMACctx.pmKey (see \ref AESCMACctx_stt) must be set with a pointer to the AES key
  *          before calling this function.
  * @note \c P_pAESCMACctx.mKeySize must be set with the size of the key prior to calling this function.
  *           Otherwise the following predefined values can be used:
  *         - \ref CRL_AES128_KEY
  *         - \ref CRL_AES192_KEY
  *         - \ref CRL_AES256_KEY
  * @note \c P_pAESCMACctx.mFlags must be set prior to calling this function. Default value is E_SK_DEFAULT.
  *          See \ref SKflags_et for details.
  * @note \c P_pAESCMACctx.mTagSize must be set with the size of authentication TAG that will be generated by the \ref AES_CMAC_Encrypt_Finish
  */
  int32_t AES_CMAC_Encrypt_Init  (AESCMACctx_stt *P_pAESCMACctx);

/**
  * @brief AES Encryption in CMAC Mode
  * @param[in,out] *P_pAESCMACctx     AES CMAC, already initialized, context
  * @param[in]    *P_pInputBuffer   Input buffer
  * @param[in]     P_inputSize      Size of input data in uint8_t (octets)
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER   At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_INPUT_SIZE  P_inputSize < 0 | (P_inputSize % 16 != 0 &&
  *                                    P_pAESCMACctx->mFlags & E_SK_FINAL_APPEND) != E_SK_FINAL_APPEND)
  * @retval    AES_ERR_BAD_OPERATION   Append not allowed
  * @note This function can be called multiple times with P_inputSize multiple of 16 bytes.
  *       The last call allows any positive value for P_inputSize but in the last call to this function the flag E_SK_FINAL_APPEND
  *       \b must \b be \b set inside P_pAESCMACctx->mFlags (i.e. with a simple P_pAESCMACctx->mFlags |= E_SK_FINAL_APPEND )
  */

  int32_t AES_CMAC_Encrypt_Append(AESCMACctx_stt *P_pAESCMACctx, \
                                  const uint8_t  *P_pInputBuffer, \
                                  int32_t         P_inputSize);

/**
  * @brief AES Finalization of CMAC Mode
  * @param[in,out] *P_pAESCMACctx     AES CMAC, already initialized, context
  * @param[out]   *P_pOutputBuffer  Output buffer
  * @param[out]   *P_pOutputSize     Size of written output data in uint8_t
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_CONTEXT   Context was not initialized with valid values, see the note below.
  * @note This function \b requires P_pAESCMACctx->mTagSize to contain a valid value between 1 and 16.
  */
  int32_t AES_CMAC_Encrypt_Finish(AESCMACctx_stt *P_pAESCMACctx,  \
                                  uint8_t        *P_pOutputBuffer, \
                                  int32_t        *P_pOutputSize);

/**
  * @brief Initialization for AES-CMAC for Authentication TAG Verification
  * @param[in,out]    *P_pAESCMACctx  AES CMAC context
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_CONTEXT   Context was not initialized with valid values, see the note below.
  * @note \c P_pAESCMACctx.pmKey (see \ref AESCMACctx_stt) must be set with a pointer to the AES key
  *          before calling this function.
  * @note \c P_pAESCMACctx.mKeySize (see \ref AESCMACctx_stt) must be set with the size of the key prior to calling this function.
  *           Otherwise the following predefined values can be used:
  *         - \ref CRL_AES128_KEY
  *         - \ref CRL_AES192_KEY
  *         - \ref CRL_AES256_KEY
  * @note \c P_pAESCMACctx.mFlags must be set prior to calling this function. Default value is E_SK_DEFAULT.
  *          See \ref SKflags_et for details.
  * @note \c P_pAESCMACctx.pmTag must be set with a pointer to the authentication TAG that will be checked during \ref AES_CMAC_Decrypt_Finish
  * @note \c P_pAESCMACctx.mTagSize must be set with the size of authentication TAG that will be checked by the \ref AES_CMAC_Decrypt_Finish
  * @remark This function is just a wrapper for \ref AES_CMAC_Encrypt_Init
 */
  int32_t AES_CMAC_Decrypt_Init  (AESCMACctx_stt *P_pAESCMACctx);

/**
  * @brief AES-CMAC Data Processing
  * @param[in,out] *P_pAESCMACctx     AES CMAC, already initialized, context
  * @param[in]    *P_pInputBuffer   Input buffer
  * @param[in]     P_inputSize      Size of input data, expressed in bytes
  * @retval    AES_SUCCESS Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER   At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_INPUT_SIZE  P_inputSize <= 0 | (P_inputSize % 16 != 0 &&
  *                                    P_pAESCMACctx->mFlags & E_SK_FINAL_APPEND) != E_SK_FINAL_APPEND)
  * @retval    AES_ERR_BAD_OPERATION   Append not allowed
  * @note This function can be called multiple times with P_inputSize multiple of 16 bytes.
  *       The last call allows any positive value for P_inputSize but in the last call to this function the flag E_SK_FINAL_APPEND
  *       \b must \b be \b set inside P_pAESCMACctx->mFlags (i.e. with a simple P_pAESCMACctx->mFlags |= E_SK_FINAL_APPEND )
  */
  int32_t AES_CMAC_Decrypt_Append(AESCMACctx_stt *P_pAESCMACctx, \
                                  const uint8_t  *P_pInputBuffer, \
                                  int32_t         P_inputSize);

/**
  * @brief AES Finalization of CMAC Mode
  * @param[in,out] *P_pAESCMACctx     AES CMAC, already initialized, context
  * @param[out]   *P_pOutputBuffer   Output buffer
  * @param[out]   *P_pOutputSize     Size of written output data in uint8_t
  * @returns    Result of TAG verification or Error Code
  * @retval    AES_ERR_BAD_PARAMETER At least one of the parameters is a NULL pointer
  * @retval    AES_ERR_BAD_CONTEXT   Context was not initialized with valid values, see the note below.
  * @retval    AUTHENTICATION_SUCCESSFUL if the TAG is verified
  * @retval    AUTHENTICATION_FAILED if the TAG is \b not verified
  * @note This function \b requires:
  *        - \c P_pAESGCMctx->pmTag to be set to a valid pointer to the tag to be checked
  *        - \c P_pAESGCMctx->mTagSize to contain a valid value between 1 and 16.
  */
  int32_t AES_CMAC_Decrypt_Finish(AESCMACctx_stt *P_pAESCMACctx,  \
                                  uint8_t        *P_pOutputBuffer, \
                                  int32_t        *P_pOutputSize);


  /**
   * @} 
   */

#ifdef __cplusplus
}
#endif

#endif /* __CRL_AES_CMAC_H__ */


