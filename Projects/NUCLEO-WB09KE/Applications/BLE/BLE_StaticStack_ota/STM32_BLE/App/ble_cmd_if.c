/**
  ******************************************************************************
  * @file    bluenrg_lp_cmd_if.c
  * @author  AMS - RF Application team
  * @date    13 April 2023
  * @brief   Autogenerated files, do not edit!!
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT STMicroelectronics</center></h2>
  ******************************************************************************
  */
#include "main.h"    
#include "ble_stack.h"
#include "bleplat.h"

/* Table containing function that does not need to be removed by the linker. */

SECTION(".cmd_call_table")
REQUIRED(void * const cmd_call_table[]) = {
  (void *) BLE_STACK_Init,
  (void *) BLE_STACK_Tick,
  (void *) BLE_STACK_SleepCheck,
  (void *) BLE_STACK_RadioHandler,
  (void *) BLE_STACK_ReadNextRadioActivity,
  (void *) hci_disconnect,
  (void *) hci_read_remote_version_information,
  (void *) hci_set_event_mask,
#if (CONTROLLER_CIS_ENABLED == 1)
  (void *) hci_read_connection_accept_timeout,
  (void *) hci_write_connection_accept_timeout,
#endif
  (void *) hci_read_transmit_power_level,
#if (CONNECTION_ENABLED == 1) &&\
    (CONTROLLER_CHAN_CLASS_ENABLED == 1)
  (void *) hci_read_afh_channel_assessment_mode,
  (void *) hci_write_afh_channel_assessment_mode,
#endif
  (void *) hci_set_event_mask_page_2,
  (void *) hci_read_authenticated_payload_timeout,
  (void *) hci_write_authenticated_payload_timeout,
  (void *) hci_read_local_version_information,
  (void *) hci_read_local_supported_commands,
  (void *) hci_read_local_supported_features,
  (void *) hci_read_bd_addr,
  (void *) hci_read_rssi,
  (void *) hci_le_set_event_mask,
  (void *) hci_le_read_buffer_size,
  (void *) hci_le_read_local_supported_features,
  (void *) hci_le_set_random_address,
  (void *) hci_le_set_advertising_parameters,
  (void *) hci_le_read_advertising_physical_channel_tx_power,
  (void *) hci_le_set_advertising_enable,
#if (CONTROLLER_MASTER_ENABLED == 1)
  (void *) hci_le_set_scan_parameters,
  (void *) hci_le_set_scan_enable,
  (void *) hci_le_create_connection,
  (void *) hci_le_create_connection_cancel,
#endif
  (void *) hci_le_read_filter_accept_list_size,
  (void *) hci_le_clear_filter_accept_list,
  (void *) hci_le_add_device_to_filter_accept_list,
  (void *) hci_le_remove_device_from_filter_accept_list,
#if (CONTROLLER_MASTER_ENABLED == 1)
  (void *) hci_le_connection_update,
#endif
#if (CONTROLLER_MASTER_ENABLED == 1) ||\
    (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1)
  (void *) hci_le_set_host_channel_classification,
#endif
  (void *) hci_le_read_channel_map,
  (void *) hci_le_read_remote_features,
  (void *) hci_le_encrypt,
  (void *) hci_le_rand,
#if (CONTROLLER_MASTER_ENABLED == 1)
  (void *) hci_le_enable_encryption,
#endif
  (void *) hci_le_long_term_key_request_reply,
  (void *) hci_le_long_term_key_request_negative_reply,
  (void *) hci_le_read_supported_states,
  (void *) hci_le_receiver_test,
  (void *) hci_le_transmitter_test,
  (void *) hci_le_test_end,
#if (CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED == 1)
  (void *) hci_le_set_data_length,
  (void *) hci_le_read_suggested_default_data_length,
  (void *) hci_le_write_suggested_default_data_length,
#endif
#if (SECURE_CONNECTIONS_ENABLED == 1)
  (void *) hci_le_read_local_p256_public_key,
  (void *) hci_le_generate_dhkey,
#endif
#if (CONTROLLER_PRIVACY_ENABLED == 1)
  (void *) hci_le_add_device_to_resolving_list,
  (void *) hci_le_remove_device_from_resolving_list,
  (void *) hci_le_clear_resolving_list,
  (void *) hci_le_read_resolving_list_size,
  (void *) hci_le_read_peer_resolvable_address,
  (void *) hci_le_read_local_resolvable_address,
  (void *) hci_le_set_address_resolution_enable,
  (void *) hci_le_set_resolvable_private_address_timeout,
#endif
#if (CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED == 1)
  (void *) hci_le_read_maximum_data_length,
#endif
#if (CONTROLLER_2M_CODED_PHY_ENABLED == 1)
  (void *) hci_le_read_phy,
  (void *) hci_le_set_default_phy,
  (void *) hci_le_set_phy,
  (void *) hci_le_receiver_test_v2,
  (void *) hci_le_transmitter_test_v2,
#endif
#if (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1)
  (void *) hci_le_set_advertising_set_random_address,
  (void *) hci_le_set_extended_advertising_parameters,
  (void *) hci_le_set_extended_advertising_enable,
  (void *) hci_le_read_number_of_supported_advertising_sets,
  (void *) hci_le_remove_advertising_set,
  (void *) hci_le_clear_advertising_sets,
  (void *) hci_le_set_periodic_advertising_parameters,
#endif
#if ((CONTROLLER_PERIODIC_ADV_ENABLED == 1) &&\
    (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1))
  (void *) hci_le_set_periodic_advertising_enable,
#endif
#if (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1)
  (void *) hci_le_set_extended_scan_parameters,
  (void *) hci_le_set_extended_scan_enable,
  (void *) hci_le_extended_create_connection,
#endif
#if ((CONTROLLER_PERIODIC_ADV_ENABLED == 1) &&\
    (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1))
  (void *) hci_le_periodic_advertising_create_sync,
  (void *) hci_le_periodic_advertising_create_sync_cancel,
  (void *) hci_le_periodic_advertising_terminate_sync,
  (void *) hci_le_add_device_to_periodic_advertiser_list,
  (void *) hci_le_remove_device_from_periodic_advertiser_list,
  (void *) hci_le_clear_periodic_advertiser_list,
  (void *) hci_le_read_periodic_advertiser_list_size,
#endif
  (void *) hci_le_read_transmit_power,
  (void *) hci_le_read_rf_path_compensation,
  (void *) hci_le_write_rf_path_compensation,
#if (CONTROLLER_PRIVACY_ENABLED == 1)
  (void *) hci_le_set_privacy_mode,
#endif
#if (CONTROLLER_CTE_ENABLED == 1)
  (void *) hci_le_receiver_test_v3,
  (void *) hci_le_transmitter_test_v3,
  (void *) hci_le_set_connectionless_cte_transmit_parameters,
  (void *) hci_le_set_connectionless_cte_transmit_enable,
  (void *) hci_le_set_connectionless_iq_sampling_enable,
  (void *) hci_le_set_connection_cte_receive_parameters,
  (void *) hci_le_set_connection_cte_transmit_parameters,
  (void *) hci_le_connection_cte_request_enable,
  (void *) hci_le_connection_cte_response_enable,
  (void *) hci_le_read_antenna_information,
#endif
#if ((CONTROLLER_PERIODIC_ADV_ENABLED == 1) &&\
    (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1))
  (void *) hci_le_set_periodic_advertising_receive_enable,
  (void *) hci_le_periodic_advertising_sync_transfer,
  (void *) hci_le_periodic_advertising_set_info_transfer,
  (void *) hci_le_set_periodic_advertising_sync_transfer_parameters,
  (void *) hci_le_set_default_periodic_advertising_sync_transfer_parameters,
#endif
#if (CONTROLLER_ISO_ENABLED == 1)
  (void *) hci_le_read_buffer_size_v2,
  (void *) hci_le_read_iso_tx_sync,
#endif
#if (CONTROLLER_CIS_ENABLED == 1)
  (void *) hci_le_set_cig_parameters,
  (void *) hci_le_set_cig_parameters_test,
  (void *) hci_le_create_cis,
  (void *) hci_le_remove_cig,
  (void *) hci_le_accept_cis_request,
  (void *) hci_le_reject_cis_request,
#endif
#if (CONTROLLER_BIS_ENABLED == 1)
  (void *) hci_le_create_big,
  (void *) hci_le_create_big_test,
  (void *) hci_le_terminate_big,
  (void *) hci_le_big_create_sync,
  (void *) hci_le_big_terminate_sync,
#endif
#if (CONTROLLER_CIS_ENABLED == 1)
  (void *) hci_le_request_peer_sca,
#endif
#if (CONTROLLER_ISO_ENABLED == 1)
  (void *) hci_le_setup_iso_data_path,
  (void *) hci_le_remove_iso_data_path,
  (void *) hci_le_iso_transmit_test,
  (void *) hci_le_iso_receive_test,
  (void *) hci_le_iso_read_test_counters,
  (void *) hci_le_iso_test_end,
  (void *) hci_le_set_host_feature,
  (void *) hci_le_read_iso_link_quality,
#endif
#if (CONTROLLER_POWER_CONTROL_ENABLED == 1)
  (void *) hci_le_enhanced_read_transmit_power_level,
  (void *) hci_le_read_remote_transmit_power_level,
  (void *) hci_le_set_path_loss_reporting_parameters,
  (void *) hci_le_set_path_loss_reporting_enable,
  (void *) hci_le_set_transmit_power_reporting_enable,
#endif
#if (CONTROLLER_CTE_ENABLED == 1)
  (void *) hci_le_transmitter_test_v4,
#endif
#if (CONTROLLER_PRIVACY_ENABLED == 1)
  (void *) hci_le_set_data_related_address_changes,
#endif
#if (CONNECTION_SUBRATING_ENABLED == 1)
  (void *) hci_le_set_default_subrate,
  (void *) hci_le_subrate_request,
#endif
#if (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1)
  (void *) hci_le_set_extended_advertising_parameters_v2,
#endif
#if (CONTROLLER_PERIODIC_ADV_WR_ENABLED == 1)
  (void *) hci_le_set_periodic_sync_subevent,
  (void *) hci_le_extended_create_connection_v2,
  (void *) hci_le_set_periodic_advertising_parameters_v2,
#endif
  (void *) aci_hal_get_fw_build_number,
  (void *) aci_hal_write_config_data,
  (void *) aci_hal_read_config_data,
  (void *) aci_hal_set_tx_power_level,
  (void *) aci_hal_le_tx_test_packet_number,
  (void *) aci_hal_get_link_status,
  (void *) aci_hal_set_radio_activity_mask,
#if (CONTROLLER_POWER_CONTROL_ENABLED == 1)
  (void *) aci_hal_set_le_power_control,
#endif
  (void *) aci_hal_peripheral_latency_enable,
  (void *) aci_hal_get_evt_fifo_max_level,
  (void *) aci_gap_init,
  (void *) aci_gap_set_io_capability,
  (void *) aci_gap_set_security_requirements,
  (void *) aci_gap_passkey_resp,
  (void *) aci_gap_set_security,
  (void *) aci_gap_get_security_level,
  (void *) aci_gap_set_le_event_mask,
  (void *) aci_gap_terminate,
  (void *) aci_gap_clear_security_db,
  (void *) aci_gap_pairing_resp,
#if (CONTROLLER_MASTER_ENABLED == 1)
  (void *) aci_gap_create_connection,
  (void *) aci_gap_terminate_proc,
  (void *) aci_gap_start_connection_update,
#endif
  (void *) aci_gap_resolve_private_addr,
  (void *) aci_gap_get_bonded_devices,
  (void *) aci_gap_is_device_bonded,
#if (SECURE_CONNECTIONS_ENABLED == 1)
  (void *) aci_gap_numeric_comparison_value_confirm_yesno,
  (void *) aci_gap_passkey_input,
#endif
  (void *) aci_gap_get_oob_data,
  (void *) aci_gap_set_oob_data,
  (void *) aci_gap_remove_bonded_device,
  (void *) aci_gap_set_advertising_configuration,
  (void *) aci_gap_set_advertising_enable,
#if (CONTROLLER_MASTER_ENABLED == 1)
  (void *) aci_gap_set_scan_configuration,
  (void *) aci_gap_set_connection_configuration,
  (void *) aci_gap_start_procedure,
  (void *) aci_gap_discover_name,
#endif
  (void *) aci_gap_add_devices_to_filter_accept_and_resolving_list,
  (void *) aci_gap_configure_filter_accept_and_resolving_list,
#if (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1)
  (void *) aci_gap_remove_advertising_set,
  (void *) aci_gap_clear_advertising_sets,
#endif
#if ((CONTROLLER_PERIODIC_ADV_ENABLED == 1) &&\
    (CONTROLLER_EXT_ADV_SCAN_ENABLED == 1))
  (void *) aci_gap_create_periodic_advertising_connection,
#endif
  (void *) aci_gap_set_advertising_data,
  (void *) aci_gap_set_scan_response_data,
  (void *) aci_gap_encrypt_adv_data,
  (void *) aci_gap_decrypt_adv_data,
  (void *) aci_gatt_set_event_mask,
  (void *) aci_gatt_clt_exchange_config,
  (void *) aci_gatt_clt_prepare_write_req,
  (void *) aci_gatt_clt_execute_write_req,
  (void *) aci_gatt_clt_disc_all_primary_services,
  (void *) aci_gatt_clt_disc_primary_service_by_uuid,
  (void *) aci_gatt_clt_find_included_services,
  (void *) aci_gatt_clt_disc_all_char_of_service,
  (void *) aci_gatt_clt_disc_char_by_uuid,
  (void *) aci_gatt_clt_disc_all_char_desc,
  (void *) aci_gatt_clt_read,
  (void *) aci_gatt_clt_read_using_char_uuid,
  (void *) aci_gatt_clt_read_long,
  (void *) aci_gatt_clt_read_multiple_char_value,
  (void *) aci_gatt_clt_write_without_resp,
  (void *) aci_gatt_clt_signed_write_without_resp,
  (void *) aci_gatt_clt_confirm_indication,
  (void *) aci_gatt_srv_notify,
  (void *) aci_gatt_srv_write_multiple_instance_handle_value,
  (void *) aci_gatt_srv_multi_notify,
  (void *) aci_gatt_clt_read_multiple_var_len_char_value,
  (void *) aci_gatt_srv_add_service,
  (void *) aci_gatt_srv_include_service,
  (void *) aci_gatt_srv_add_char,
  (void *) aci_gatt_srv_add_char_desc,
  (void *) aci_gatt_srv_rm_service,
  (void *) aci_gatt_srv_rm_include_service,
  (void *) aci_gatt_srv_rm_char,
  (void *) aci_gatt_srv_get_service_handle,
  (void *) aci_gatt_srv_get_include_service_handle,
  (void *) aci_gatt_srv_get_char_decl_handle,
  (void *) aci_gatt_srv_get_descriptor_handle,
  (void *) aci_gatt_srv_read_handle_value,
  (void *) aci_gatt_srv_resp,
  (void *) aci_gatt_clt_write,
  (void *) aci_gatt_clt_write_long,
  (void *) aci_gatt_clt_write_char_reliable,
  (void *) aci_gatt_srv_read_multiple_instance_handle_value,
  (void *) aci_gatt_clt_add_subscription_security_level,
  (void *) aci_l2cap_connection_parameter_update_req,
#if (CONTROLLER_MASTER_ENABLED == 1)
  (void *) aci_l2cap_connection_parameter_update_resp,
#endif
#if (L2CAP_COS_ENABLED == 1)
  (void *) aci_l2cap_cos_connection_req,
  (void *) aci_l2cap_cos_connection_resp,
  (void *) aci_l2cap_cos_flow_control_credits_ind,
  (void *) aci_l2cap_cos_disconnect_req,
  (void *) aci_l2cap_cos_sdu_data_transmit,
  (void *) aci_l2cap_cos_reconfigure_req,
  (void *) aci_l2cap_cos_reconfigure_resp,
  (void *) aci_l2cap_cos_sdu_data_extract,
#endif
  (void *) ll_set_legacy_advertising_data_ptr,
  (void *) ll_set_legacy_scan_reponse_data_ptr,
  (void *) ll_set_advertising_data_ptr,
  (void *) ll_set_scan_reponse_data_ptr,
  (void *) ll_get_advertising_info,
  (void *) ll_set_periodic_advertising_data_ptr,
  (void *) aci_hal_get_anchor_point,
  (void *) ll_set_periodic_advertising_subevent_data_ptr,
  (void *) ll_set_periodic_advertising_response_data_ptr
};

void ** const app_call_table = (void **)EV_CALL_TABLE_ADDR;

#define BLE_STACK_EVENT_FUNC_IDX                                     0
#define BLE_STACK_PROCESSREQUEST_FUNC_IDX                            1
#define BLEPLAT_MEMCPY_FUNC_IDX                                      2
#define BLEPLAT_MEMSET_FUNC_IDX                                      3
#define BLEPLAT_MEMCMP_FUNC_IDX                                      4
#define BLEPLAT_GETPARTINFO_FUNC_IDX                                 5
#define BLEPLAT_NVMADD_FUNC_IDX                                      6
#define BLEPLAT_NVMGET_FUNC_IDX                                      7
#define BLEPLAT_NVMCOMPARE_FUNC_IDX                                  8
#define BLEPLAT_NVMDISCARD_FUNC_IDX                                  9
#define BLEPLAT_PKASTARTP256KEY_FUNC_IDX                             10
#define BLEPLAT_PKASTARTDHKEY_FUNC_IDX                               11
#define BLEPLAT_AESECBENCRYPT_FUNC_IDX                               12
#define BLEPLAT_AESCMACENCRYPTINIT_FUNC_IDX                          13
#define BLEPLAT_AESCMACENCRYPTAPPEND_FUNC_IDX                        14
#define BLEPLAT_AESCMACENCRYPTFINISH_FUNC_IDX                        15
#define BLEPLAT_RNGGETRANDOM16_FUNC_IDX                              16
#define BLEPLAT_RNGGETRANDOM32_FUNC_IDX                              17
#define BLEPLAT_DBMTOPALEVEL_FUNC_IDX                                18
#define BLEPLAT_DBMTOPALEVELGE_FUNC_IDX                              19
#define BLEPLAT_PALEVELTODBM_FUNC_IDX                                20
#define BLEPLAT_READTRANSMITPOWER_FUNC_IDX                           21
#define BLEPLAT_SETHIGHPOWER_FUNC_IDX                                22
#define BLEPLAT_CALCULATERSSI_FUNC_IDX                               23
#define BLEPLAT_UPDATEAVGRSSI_FUNC_IDX                               24
#define BLEPLAT_GETDEMODCI_FUNC_IDX                                  25
#define BLEPLAT_GETMAXPALEVEL_FUNC_IDX                               26
#define BLEPLAT_GETDEFAULTPALEVEL_FUNC_IDX                           27
#define BLEPLAT_INITCTE_FUNC_IDX                                     28
#define BLEPLAT_DEINITCTE_FUNC_IDX                                   29
#define BLEPLAT_CALIBRATECTE_FUNC_IDX                                30
#define BLEPLAT_ANTIDXREMAP_FUNC_IDX                                 31
#define BLEPLAT_GETCURRENTSYSTIME_FUNC_IDX                           32
#define BLEPLAT_GETFUTURESYSTIME64_FUNC_IDX                          33
#define BLEPLAT_STARTTIMER_FUNC_IDX                                  34
#define BLEPLAT_STOPTIMER_FUNC_IDX                                   35
#define BLEPLAT_SETRADIOTIMERVALUE_FUNC_IDX                          36
#define BLEPLAT_CLEARRADIOTIMERVALUE_FUNC_IDX                        37
#define BLEPLAT_GETANCHORPOINT_FUNC_IDX                              38
#define BLEPLAT_SETRADIOCLOSETIMEOUT_FUNC_IDX                        39
#define BLEPLAT_SETRADIOTIMERRELATIVEUSVALUE_FUNC_IDX                40

typedef void (* BLE_STACK_Event_func_type)(hci_pckt *hci_pckt, uint16_t length);
typedef void (* BLE_STACK_ProcessRequest_func_type)(void);
typedef void (* BLEPLAT_MemCpy_func_type)(void *Dest, const void *Src, unsigned int Size);
typedef void (* BLEPLAT_MemSet_func_type)(void *Ptr, int Value, unsigned int Size);
typedef int (* BLEPLAT_MemCmp_func_type)(void *S1, void *S2, unsigned int Size);
typedef void (* BLEPLAT_GetPartInfo_func_type)(uint8_t *pDeviceId, uint8_t *pMajorCut, uint8_t *pMinorCut);
typedef BLEPLAT_NvmStatusTypeDef (* BLEPLAT_NvmAdd_func_type)(BLEPLAT_NvmRecordTypeDef Type,
                                        const uint8_t* pData,
                                        uint16_t Size,
                                        const uint8_t* pExtraData,
                                        uint16_t ExtraSize);
typedef BLEPLAT_NvmStatusTypeDef (* BLEPLAT_NvmGet_func_type)(BLEPLAT_NvmSeekModeTypeDef Mode,
                                        BLEPLAT_NvmRecordTypeDef Type,
                                        uint16_t Offset,
                                        uint8_t* pData,
                                        uint16_t Size);
typedef int (* BLEPLAT_NvmCompare_func_type)(uint16_t Offset, const uint8_t* pData, uint16_t Size);
typedef void (* BLEPLAT_NvmDiscard_func_type)(BLEPLAT_NvmSeekModeTypeDef Mode);
typedef BLEPLAT_PkaStatusTypeDef (* BLEPLAT_PkaStartP256Key_func_type)(const uint32_t *PrivateKey, BLEPLAT_PkaFuncCb FuncCb);
typedef BLEPLAT_PkaStatusTypeDef (* BLEPLAT_PkaStartDHkey_func_type)(uint32_t* PrivateKey,
                                         uint32_t* PublicKey,
                                         BLEPLAT_PkaFuncCb FuncCb);
typedef void (* BLEPLAT_AesEcbEncrypt_func_type)(const uint32_t *plainTextData,
                           const uint32_t *key,
                           uint32_t *encryptedData);
typedef int32_t (* BLEPLAT_AesCMACEncryptInit_func_type)(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx);
typedef int32_t (* BLEPLAT_AesCMACEncryptAppend_func_type)(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx,
                                     const uint8_t  *pInputBuffer, 
                                     int32_t InputSize);
typedef int32_t (* BLEPLAT_AesCMACEncryptFinish_func_type)(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx,
                                     uint8_t *pOutputBuffer,
                                     int32_t *pOutputSize);
typedef void (* BLEPLAT_RngGetRandom16_func_type)(uint16_t* Num);
typedef void (* BLEPLAT_RngGetRandom32_func_type)(uint32_t* Num);
typedef uint8_t (* BLEPLAT_DBmToPALevel_func_type)(int8_t TxDBm);
typedef uint8_t (* BLEPLAT_DBmToPALevelGe_func_type)(int8_t TxDBm);
typedef int8_t (* BLEPLAT_PALevelToDBm_func_type)(uint8_t PaLevel);
typedef void (* BLEPLAT_ReadTransmitPower_func_type)(int8_t *MinTxPower, int8_t *MaxTxPower);
typedef void (* BLEPLAT_SetHighPower_func_type)(uint8_t Enable);
typedef int8_t (* BLEPLAT_CalculateRSSI_func_type)(void);
typedef int8_t (* BLEPLAT_UpdateAvgRSSI_func_type)(int8_t AvgRssi, int8_t Rssi, uint8_t RssiFilterCoeff);
typedef uint8_t (* BLEPLAT_GetDemodCI_func_type)(void);
typedef uint8_t (* BLEPLAT_GetMaxPALevel_func_type)(void);
typedef uint8_t (* BLEPLAT_GetDefaultPALevel_func_type)(void);
typedef void (* BLEPLAT_InitCTE_func_type)(uint8_t smNo);
typedef void (* BLEPLAT_DeinitCTE_func_type)(void);
typedef void (* BLEPLAT_CalibrateCTE_func_type)(uint8_t smNo);
typedef void (* BLEPLAT_AntIdxRemap_func_type)(uint8_t antPattLen, uint8_t *pAntRamTable, const uint8_t* pAntPatt);
typedef uint64_t (* BLEPLAT_GetCurrentSysTime_func_type)(void);
typedef uint64_t (* BLEPLAT_GetFutureSysTime64_func_type)(uint32_t SysTime);
typedef int (* BLEPLAT_StartTimer_func_type)(BLEPLAT_TimerHandleTypeDef *TimerHandle, uint64_t Time);
typedef void (* BLEPLAT_StopTimer_func_type)(BLEPLAT_TimerHandleTypeDef *TimerHandle);
typedef uint8_t (* BLEPLAT_SetRadioTimerValue_func_type)(uint32_t Time, uint8_t EventType, uint8_t CalReq);
typedef uint8_t (* BLEPLAT_ClearRadioTimerValue_func_type)(void);
typedef uint64_t (* BLEPLAT_GetAnchorPoint_func_type)(uint64_t *current_system_time);
typedef void (* BLEPLAT_SetRadioCloseTimeout_func_type)(void);
typedef uint8_t (* BLEPLAT_SetRadioTimerRelativeUsValue_func_type)(uint32_t RelTimeoutUs, uint8_t Tx, uint8_t PLLCal);

void BLE_STACK_Event(hci_pckt *hci_pckt, uint16_t length)
{
  BLE_STACK_Event_func_type ptr = (BLE_STACK_Event_func_type)app_call_table[BLE_STACK_EVENT_FUNC_IDX];
  ptr(hci_pckt, length);
}
void BLE_STACK_ProcessRequest(void)
{
  BLE_STACK_ProcessRequest_func_type ptr = (BLE_STACK_ProcessRequest_func_type)app_call_table[BLE_STACK_PROCESSREQUEST_FUNC_IDX];
  ptr();
}
void BLEPLAT_MemCpy(void *Dest, const void *Src, unsigned int Size)
{
  BLEPLAT_MemCpy_func_type ptr = (BLEPLAT_MemCpy_func_type)app_call_table[BLEPLAT_MEMCPY_FUNC_IDX];
  ptr(Dest, Src, Size);
}
void BLEPLAT_MemSet(void *Ptr, int Value, unsigned int Size)
{
  BLEPLAT_MemSet_func_type ptr = (BLEPLAT_MemSet_func_type)app_call_table[BLEPLAT_MEMSET_FUNC_IDX];
  ptr(Ptr, Value, Size);
}
int BLEPLAT_MemCmp(void *S1, void *S2, unsigned int Size)
{
  BLEPLAT_MemCmp_func_type ptr = (BLEPLAT_MemCmp_func_type)app_call_table[BLEPLAT_MEMCMP_FUNC_IDX];
  return  ptr(S1, S2, Size);
}
void BLEPLAT_GetPartInfo(uint8_t *pDeviceId, uint8_t *pMajorCut, uint8_t *pMinorCut)
{
  BLEPLAT_GetPartInfo_func_type ptr = (BLEPLAT_GetPartInfo_func_type)app_call_table[BLEPLAT_GETPARTINFO_FUNC_IDX];
  ptr(pDeviceId, pMajorCut, pMinorCut);
}
BLEPLAT_NvmStatusTypeDef BLEPLAT_NvmAdd(BLEPLAT_NvmRecordTypeDef Type,
                                        const uint8_t* pData,
                                        uint16_t Size,
                                        const uint8_t* pExtraData,
                                        uint16_t ExtraSize)
{
  BLEPLAT_NvmAdd_func_type ptr = (BLEPLAT_NvmAdd_func_type)app_call_table[BLEPLAT_NVMADD_FUNC_IDX];
  return  ptr(Type, pData, Size, pExtraData, ExtraSize);
}
BLEPLAT_NvmStatusTypeDef BLEPLAT_NvmGet(BLEPLAT_NvmSeekModeTypeDef Mode,
                                        BLEPLAT_NvmRecordTypeDef Type,
                                        uint16_t Offset,
                                        uint8_t* pData,
                                        uint16_t Size)
{
  BLEPLAT_NvmGet_func_type ptr = (BLEPLAT_NvmGet_func_type)app_call_table[BLEPLAT_NVMGET_FUNC_IDX];
  return  ptr(Mode, Type, Offset, pData, Size);
}
int BLEPLAT_NvmCompare(uint16_t Offset, const uint8_t* pData, uint16_t Size)
{
  BLEPLAT_NvmCompare_func_type ptr = (BLEPLAT_NvmCompare_func_type)app_call_table[BLEPLAT_NVMCOMPARE_FUNC_IDX];
  return  ptr(Offset, pData, Size);
}
void BLEPLAT_NvmDiscard(BLEPLAT_NvmSeekModeTypeDef Mode)
{
  BLEPLAT_NvmDiscard_func_type ptr = (BLEPLAT_NvmDiscard_func_type)app_call_table[BLEPLAT_NVMDISCARD_FUNC_IDX];
  ptr(Mode);
}
BLEPLAT_PkaStatusTypeDef BLEPLAT_PkaStartP256Key(const uint32_t *PrivateKey, BLEPLAT_PkaFuncCb FuncCb)
{
  BLEPLAT_PkaStartP256Key_func_type ptr = (BLEPLAT_PkaStartP256Key_func_type)app_call_table[BLEPLAT_PKASTARTP256KEY_FUNC_IDX];
  return  ptr(PrivateKey, FuncCb);
}
BLEPLAT_PkaStatusTypeDef BLEPLAT_PkaStartDHkey(uint32_t* PrivateKey,
                                         uint32_t* PublicKey,
                                         BLEPLAT_PkaFuncCb FuncCb)
{
  BLEPLAT_PkaStartDHkey_func_type ptr = (BLEPLAT_PkaStartDHkey_func_type)app_call_table[BLEPLAT_PKASTARTDHKEY_FUNC_IDX];
  return  ptr(PrivateKey, PublicKey, FuncCb);
}
void BLEPLAT_AesEcbEncrypt(const uint32_t *plainTextData,
                           const uint32_t *key,
                           uint32_t *encryptedData)
{
  BLEPLAT_AesEcbEncrypt_func_type ptr = (BLEPLAT_AesEcbEncrypt_func_type)app_call_table[BLEPLAT_AESECBENCRYPT_FUNC_IDX];
  ptr(plainTextData, key, encryptedData);
}
int32_t BLEPLAT_AesCMACEncryptInit(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx)
{
  BLEPLAT_AesCMACEncryptInit_func_type ptr = (BLEPLAT_AesCMACEncryptInit_func_type)app_call_table[BLEPLAT_AESCMACENCRYPTINIT_FUNC_IDX];
  return  ptr(pAESCMACctx);
}
int32_t BLEPLAT_AesCMACEncryptAppend(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx,
                                     const uint8_t  *pInputBuffer, 
                                     int32_t InputSize)
{
  BLEPLAT_AesCMACEncryptAppend_func_type ptr = (BLEPLAT_AesCMACEncryptAppend_func_type)app_call_table[BLEPLAT_AESCMACENCRYPTAPPEND_FUNC_IDX];
  return  ptr(pAESCMACctx, pInputBuffer, InputSize);
}
int32_t BLEPLAT_AesCMACEncryptFinish(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx,
                                     uint8_t *pOutputBuffer,
                                     int32_t *pOutputSize)
{
  BLEPLAT_AesCMACEncryptFinish_func_type ptr = (BLEPLAT_AesCMACEncryptFinish_func_type)app_call_table[BLEPLAT_AESCMACENCRYPTFINISH_FUNC_IDX];
  return  ptr(pAESCMACctx, pOutputBuffer, pOutputSize);
}
void BLEPLAT_RngGetRandom16(uint16_t* Num)
{
  BLEPLAT_RngGetRandom16_func_type ptr = (BLEPLAT_RngGetRandom16_func_type)app_call_table[BLEPLAT_RNGGETRANDOM16_FUNC_IDX];
  ptr(Num);
}
void BLEPLAT_RngGetRandom32(uint32_t* Num)
{
  BLEPLAT_RngGetRandom32_func_type ptr = (BLEPLAT_RngGetRandom32_func_type)app_call_table[BLEPLAT_RNGGETRANDOM32_FUNC_IDX];
  ptr(Num);
}
uint8_t BLEPLAT_DBmToPALevel(int8_t TxDBm)
{
  BLEPLAT_DBmToPALevel_func_type ptr = (BLEPLAT_DBmToPALevel_func_type)app_call_table[BLEPLAT_DBMTOPALEVEL_FUNC_IDX];
  return  ptr(TxDBm);
}
uint8_t BLEPLAT_DBmToPALevelGe(int8_t TxDBm)
{
  BLEPLAT_DBmToPALevelGe_func_type ptr = (BLEPLAT_DBmToPALevelGe_func_type)app_call_table[BLEPLAT_DBMTOPALEVELGE_FUNC_IDX];
  return  ptr(TxDBm);
}
int8_t BLEPLAT_PALevelToDBm(uint8_t PaLevel)
{
  BLEPLAT_PALevelToDBm_func_type ptr = (BLEPLAT_PALevelToDBm_func_type)app_call_table[BLEPLAT_PALEVELTODBM_FUNC_IDX];
  return  ptr(PaLevel);
}
void BLEPLAT_ReadTransmitPower(int8_t *MinTxPower, int8_t *MaxTxPower)
{
  BLEPLAT_ReadTransmitPower_func_type ptr = (BLEPLAT_ReadTransmitPower_func_type)app_call_table[BLEPLAT_READTRANSMITPOWER_FUNC_IDX];
  ptr(MinTxPower, MaxTxPower);
}
void BLEPLAT_SetHighPower(uint8_t Enable)
{
  BLEPLAT_SetHighPower_func_type ptr = (BLEPLAT_SetHighPower_func_type)app_call_table[BLEPLAT_SETHIGHPOWER_FUNC_IDX];
  ptr(Enable);
}
int8_t BLEPLAT_CalculateRSSI(void)
{
  BLEPLAT_CalculateRSSI_func_type ptr = (BLEPLAT_CalculateRSSI_func_type)app_call_table[BLEPLAT_CALCULATERSSI_FUNC_IDX];
  return  ptr();
}
int8_t BLEPLAT_UpdateAvgRSSI(int8_t AvgRssi, int8_t Rssi, uint8_t RssiFilterCoeff)
{
  BLEPLAT_UpdateAvgRSSI_func_type ptr = (BLEPLAT_UpdateAvgRSSI_func_type)app_call_table[BLEPLAT_UPDATEAVGRSSI_FUNC_IDX];
  return  ptr(AvgRssi, Rssi, RssiFilterCoeff);
}
uint8_t BLEPLAT_GetDemodCI(void)
{
  BLEPLAT_GetDemodCI_func_type ptr = (BLEPLAT_GetDemodCI_func_type)app_call_table[BLEPLAT_GETDEMODCI_FUNC_IDX];
  return  ptr();
}
uint8_t BLEPLAT_GetMaxPALevel(void)
{
  BLEPLAT_GetMaxPALevel_func_type ptr = (BLEPLAT_GetMaxPALevel_func_type)app_call_table[BLEPLAT_GETMAXPALEVEL_FUNC_IDX];
  return  ptr();
}
uint8_t BLEPLAT_GetDefaultPALevel(void)
{
  BLEPLAT_GetDefaultPALevel_func_type ptr = (BLEPLAT_GetDefaultPALevel_func_type)app_call_table[BLEPLAT_GETDEFAULTPALEVEL_FUNC_IDX];
  return  ptr();
}
void BLEPLAT_InitCTE(uint8_t smNo)
{
  BLEPLAT_InitCTE_func_type ptr = (BLEPLAT_InitCTE_func_type)app_call_table[BLEPLAT_INITCTE_FUNC_IDX];
  ptr(smNo);
}
void BLEPLAT_DeinitCTE(void)
{
  BLEPLAT_DeinitCTE_func_type ptr = (BLEPLAT_DeinitCTE_func_type)app_call_table[BLEPLAT_DEINITCTE_FUNC_IDX];
  ptr();
}
void BLEPLAT_CalibrateCTE(uint8_t smNo)
{
  BLEPLAT_CalibrateCTE_func_type ptr = (BLEPLAT_CalibrateCTE_func_type)app_call_table[BLEPLAT_CALIBRATECTE_FUNC_IDX];
  ptr(smNo);
}
void BLEPLAT_AntIdxRemap(uint8_t antPattLen, uint8_t *pAntRamTable, const uint8_t* pAntPatt)
{
  BLEPLAT_AntIdxRemap_func_type ptr = (BLEPLAT_AntIdxRemap_func_type)app_call_table[BLEPLAT_ANTIDXREMAP_FUNC_IDX];
  ptr(antPattLen, pAntRamTable, pAntPatt);
}
uint64_t BLEPLAT_GetCurrentSysTime(void)
{
  BLEPLAT_GetCurrentSysTime_func_type ptr = (BLEPLAT_GetCurrentSysTime_func_type)app_call_table[BLEPLAT_GETCURRENTSYSTIME_FUNC_IDX];
  return  ptr();
}
uint64_t BLEPLAT_GetFutureSysTime64(uint32_t SysTime)
{
  BLEPLAT_GetFutureSysTime64_func_type ptr = (BLEPLAT_GetFutureSysTime64_func_type)app_call_table[BLEPLAT_GETFUTURESYSTIME64_FUNC_IDX];
  return  ptr(SysTime);
}
int BLEPLAT_StartTimer(BLEPLAT_TimerHandleTypeDef *TimerHandle, uint64_t Time)
{
  BLEPLAT_StartTimer_func_type ptr = (BLEPLAT_StartTimer_func_type)app_call_table[BLEPLAT_STARTTIMER_FUNC_IDX];
  return  ptr(TimerHandle, Time);
}
void BLEPLAT_StopTimer(BLEPLAT_TimerHandleTypeDef *TimerHandle)
{
  BLEPLAT_StopTimer_func_type ptr = (BLEPLAT_StopTimer_func_type)app_call_table[BLEPLAT_STOPTIMER_FUNC_IDX];
  ptr(TimerHandle);
}
uint8_t BLEPLAT_SetRadioTimerValue(uint32_t Time, uint8_t EventType, uint8_t CalReq)
{
  BLEPLAT_SetRadioTimerValue_func_type ptr = (BLEPLAT_SetRadioTimerValue_func_type)app_call_table[BLEPLAT_SETRADIOTIMERVALUE_FUNC_IDX];
  return  ptr(Time, EventType, CalReq);
}
uint8_t BLEPLAT_ClearRadioTimerValue(void)
{
  BLEPLAT_ClearRadioTimerValue_func_type ptr = (BLEPLAT_ClearRadioTimerValue_func_type)app_call_table[BLEPLAT_CLEARRADIOTIMERVALUE_FUNC_IDX];
  return  ptr();
}
uint64_t BLEPLAT_GetAnchorPoint(uint64_t *current_system_time)
{
  BLEPLAT_GetAnchorPoint_func_type ptr = (BLEPLAT_GetAnchorPoint_func_type)app_call_table[BLEPLAT_GETANCHORPOINT_FUNC_IDX];
  return  ptr(current_system_time);
}
void BLEPLAT_SetRadioCloseTimeout(void)
{
  BLEPLAT_SetRadioCloseTimeout_func_type ptr = (BLEPLAT_SetRadioCloseTimeout_func_type)app_call_table[BLEPLAT_SETRADIOCLOSETIMEOUT_FUNC_IDX];
  ptr();
}
uint8_t BLEPLAT_SetRadioTimerRelativeUsValue(uint32_t RelTimeoutUs, uint8_t Tx, uint8_t PLLCal)
{
  BLEPLAT_SetRadioTimerRelativeUsValue_func_type ptr = (BLEPLAT_SetRadioTimerRelativeUsValue_func_type)app_call_table[BLEPLAT_SETRADIOTIMERRELATIVEUSVALUE_FUNC_IDX];
  return  ptr(RelTimeoutUs, Tx, PLLCal);
}

