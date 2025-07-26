//#include "nrf_crypto.h"
//#include "gwp_common.h"


#if defined(NRF52840_XXAA)


#include "aes_host.h"

#include "secure_storage_api.h"
#include "gwp_rpc_crypto.h"
//#include "nrf_crypto_hkdf.h"


uint8_t rpc_session_key[32];
uint8_t salt_key[4] = {0}; //RANDOM
uint8_t salt_key_all[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}; //
uint8_t rpc_handshake_status = 0; //0- init rpc handshake  1-update seesion key  2-complete handshake


ret_code_t update_rpc_session_key()
{
    ret_code_t err_code = GWP_SUCCESS;
    size_t Session_key_size = 32;
    uint8_t root_key[32] =
    {
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
    };
    uint8_t const add_info_ssion[] = "GWP_RPC_SESSION";
    #ifdef CONFIG_GWP_ECCHIP_SUPPORT
    //Read root key
    atecc608_rw_enc_params_t rw_params;
    ATCA_STATUS status = ATCA_SUCCESS;
    rw_params.slot_number = OTA_KEY;
    rw_params.offset = 0;
    rw_params.buffer = root_key;
    rw_params.length = 32;
    rw_params.enckey = gwp_fm.encrypt_read_key;
    rw_params.enc_slot_id = ENCRYPT_READ_KEY;
    status = atecc608_read_data_enc(&rw_params);
    if( ATCA_SUCCESS != status )
    {
        //break;
    }
    #else
//    _gwp_storage_read(GWP_DEV_INFO_ADDR + SECUREBOOT_KROOT_OFFSET, root_key, 32);
    #endif
    memset(rpc_session_key, 0, sizeof(rpc_session_key));
    #if defined(NRF52840_XXAA)
    err_code = nrf_crypto_hkdf_calculate(NULL,
                                         &g_nrf_crypto_hmac_sha256_info,
                                         rpc_session_key,                          // Output key
                                         &Session_key_size,                    // Output key size
                                         root_key,                                // Input key
                                         sizeof(root_key),                        // Input key size
                                         salt_key_all,                           // Salt
                                         sizeof(salt_key_all),                   // Salt size
                                         &add_info_ssion[0],                     // Additional info
                                         sizeof(add_info_ssion) - 1,                                   // Additional info size
                                         GWP_CRYPTO_HKDF_EXTRACT_AND_EXPAND);   // Mode
    #elif defined(STM32WB55xx)
    err_code = gwp_hkdf_calculate(
                   rpc_session_key,                          // Output key
                   root_key,                                // Input key
                   sizeof(root_key),                        // Input key size
                   salt_key_all,                           // Salt
                   sizeof(salt_key_all),                   // Salt size
                   &add_info_ssion[0],                     // Additional info
                   sizeof(add_info_ssion) - 1,                                   // Additional info size
               );   // Mode
    #else
#error "Architecture not set."
    #endif
    /*for(uint8_t i=0;i<4;i++)
       GWP_LOG_INFO("salt_key = %02x ",salt_key[i]);
      NRF_LOG_FLUSH();

      for(uint8_t i=0;i<15;i++)
      GWP_LOG_INFO("root_key = %02x ",rpc_session_key[i]);
      NRF_LOG_FLUSH();

      for(uint8_t i=16;i<32;i++)
      GWP_LOG_INFO("root_key = %02x ",rpc_session_key[i]);
      NRF_LOG_FLUSH();
    */
    return err_code;
}

ret_code_t get_rpc_key(uint8_t *key)
{
    memcpy(key, rpc_session_key, RPC_KEY_SIZE);
    return GWP_SUCCESS;
}



ret_code_t init_rpc_session_key(void)
{
    ret_code_t err_code = GWP_SUCCESS;
    uint8_t salt_key_init[8] = {0}; //0X55 *32
    size_t Session_key_size = 32;
    uint8_t root_key[32] = {0};
    uint8_t const add_info_ssion[] = "GWP_RPC_INIT";
    #ifdef CONFIG_GWP_ECCHIP_SUPPORT
    //Read root key
    atecc608_rw_enc_params_t rw_params;
    ATCA_STATUS status = ATCA_SUCCESS;
    rw_params.slot_number = OTA_KEY;
    rw_params.offset = 0;
    rw_params.buffer = root_key;
    rw_params.length = 32;
    rw_params.enckey = gwp_fm.encrypt_read_key;
    rw_params.enc_slot_id = ENCRYPT_READ_KEY;
    status = atecc608_read_data_enc(&rw_params);
    if( ATCA_SUCCESS != status )
    {
        //break; gwp_firmware_t
    }
    #else
    _gwp_storage_read(GWP_DEV_INFO_ADDR + SECUREBOOT_KROOT_OFFSET, root_key, 32);
    #endif
    #if defined(NRF52840_XXAA)
    err_code = nrf_crypto_hkdf_calculate(NULL,
                                         &g_nrf_crypto_hmac_sha256_info,
                                         rpc_session_key,                          // Output key
                                         &Session_key_size,                    // Output key size
                                         root_key,                                // Input key
                                         sizeof(root_key),                        // Input key size
                                         salt_key_init,                           // Salt
                                         sizeof(salt_key_init),                   // Salt size
                                         &add_info_ssion[0],                     // Additional info
                                         sizeof(add_info_ssion) - 1,                                   // Additional info size
                                         GWP_CRYPTO_HKDF_EXTRACT_AND_EXPAND);   // Mode
    #elif defined(STM32WB55xx)
    #else
#error "Architecture not set."
    #endif
    return err_code;
}



ret_code_t decrypted_payload(uint8_t *buf, uint16_t length, uint8_t* payload, uint8_t *iv)
{
    ret_code_t ret_code = GWP_SUCCESS;
    uint8_t key[RPC_KEY_SIZE];
    get_rpc_key(key);
    size_t out_length = length;
    ret_code = gwpFunctions.aesCrypt(GWP_CRYPTO_AES_MODE_CTR,
                                     GWP_CRYPTO_DECRYPT,
                                     key,
                                     iv,
                                     buf,
                                     length,
                                     payload,
                                     &out_length);
    return ret_code;
}



ret_code_t encrypted_payload(uint8_t *src_address, uint16_t length, uint8_t *dest_address, uint8_t *iv)
{
    ret_code_t ret_code = GWP_SUCCESS;
    uint8_t key[RPC_KEY_SIZE];
    get_rpc_key(key);
    size_t out_length = length;
    ret_code = gwpFunctions.aesCrypt(GWP_CRYPTO_AES_MODE_CTR,
                                     GWP_CRYPTO_ENCRYPT,
                                     key,
                                     iv,
                                     src_address,
                                     length,
                                     dest_address,
                                     &out_length);
    return ret_code;
}


#elif defined(STM32WB55xx) || defined(CONFIG_IDF_TARGET_ESP32S3)


//#include "aes_host.h"
#include "gwp_error_common.h"
//#include "hkdf_host.h"
//#include "secure_storage_api.h"
#include "gwp_rpc_crypto.h"
//#include "nrf_crypto_hkdf.h"


uint8_t rpc_session_key[32] =
{
    0xdf, 0x5f, 0xb7, 0xfe, 0x71, 0x36, 0x5d, 0x4d, 0x95, 0xba, 0x26, 0xe7, 0xb1, 0x73, 0x58, 0x18,
    0x23, 0xa8, 0xd7, 0xc6, 0x8b, 0x0d, 0x9b, 0x56, 0x4d, 0x04, 0x55, 0xaa, 0x45, 0xfd, 0xde, 0x64
};
uint8_t salt_key[4] = {0}; //RANDOM
uint8_t salt_key_all[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}; //
uint8_t rpc_handshake_status = 0; //0- init rpc handshake  1-update seesion key  2-complete handshake


ret_code_t update_rpc_session_key()
{
    ret_code_t err_code = GWP_SUCCESS;
//    size_t Session_key_size = 32;
    uint8_t root_key[32] =
    {
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
    };
    uint8_t const add_info_ssion[] = "GWP_RPC_SESSION";
    #ifdef CONFIG_GWP_ECCHIP_SUPPORT
    //Read root key
    atecc608_rw_enc_params_t rw_params;
    ATCA_STATUS status = ATCA_SUCCESS;
    rw_params.slot_number = OTA_KEY;
    rw_params.offset = 0;
    rw_params.buffer = root_key;
    rw_params.length = 32;
    rw_params.enckey = gwp_fm.encrypt_read_key;
    rw_params.enc_slot_id = ENCRYPT_READ_KEY;
    status = atecc608_read_data_enc(&rw_params);
    if( ATCA_SUCCESS != status )
    {
        //break;
    }
    #else
//    memset(root_key, 0, sizeof(root_key));
    // _gwp_storage_read(GWP_DEV_INFO_ADDR + SECUREBOOT_KROOT_OFFSET, root_key, 32);
    #endif
    memset(rpc_session_key, 0, sizeof(rpc_session_key));
    #if defined(NRF52840_XXAA)
    err_code = nrf_crypto_hkdf_calculate(NULL,
                                         &g_nrf_crypto_hmac_sha256_info,
                                         rpc_session_key,                          // Output key
                                         &Session_key_size,                    // Output key size
                                         root_key,                                // Input key
                                         sizeof(root_key),                        // Input key size
                                         salt_key_all,                           // Salt
                                         sizeof(salt_key_all),                   // Salt size
                                         &add_info_ssion[0],                     // Additional info
                                         sizeof(add_info_ssion) - 1,                                   // Additional info size
                                         GWP_CRYPTO_HKDF_EXTRACT_AND_EXPAND);   // Mode
    #elif defined(STM32WB55xx) || defined(CONFIG_IDF_TARGET_ESP32S3)
    err_code =  gwpFunctions.hkdfCalculate(  rpc_session_key,                          // Output key
                (uint8_t const *)&root_key,                                // Input key
                sizeof(root_key),                        // Input key size
                salt_key_all,                           // Salt
                sizeof(salt_key_all),                   // Salt size
                (uint8_t *)&add_info_ssion[0],                     // Additional info
                sizeof(add_info_ssion) - 1                                 // Additional info size
                                          );
    print_hex(rpc_session_key, sizeof(rpc_session_key), "rpc_session_key");
    print_hex(salt_key_all, sizeof(salt_key_all), "salt_key_all");
    #else
#error "Architecture not set."
    #endif
    /*for(uint8_t i=0;i<4;i++)
       GWP_LOG_INFO("salt_key = %02x ",salt_key[i]);
      NRF_LOG_FLUSH();

      for(uint8_t i=0;i<15;i++)
      GWP_LOG_INFO("root_key = %02x ",rpc_session_key[i]);
      NRF_LOG_FLUSH();

      for(uint8_t i=16;i<32;i++)
      GWP_LOG_INFO("root_key = %02x ",rpc_session_key[i]);
      NRF_LOG_FLUSH();
    */
    return err_code;
}

ret_code_t get_rpc_key(uint8_t *key)
{
    memcpy(key, rpc_session_key, RPC_KEY_SIZE);
    return GWP_SUCCESS;
}



ret_code_t init_rpc_session_key(void)
{
    ret_code_t err_code = GWP_SUCCESS;
    uint8_t salt_key_init[8] = {0}; //0X55 *32
//    size_t Session_key_size = 32;
    uint8_t root_key[32] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
                           };
    uint8_t const add_info_ssion[] = "GWP_RPC_INIT";
    #ifdef CONFIG_GWP_ECCHIP_SUPPORT
    //Read root key
    atecc608_rw_enc_params_t rw_params;
    ATCA_STATUS status = ATCA_SUCCESS;
    rw_params.slot_number = OTA_KEY;
    rw_params.offset = 0;
    rw_params.buffer = root_key;
    rw_params.length = 32;
    rw_params.enckey = gwp_fm.encrypt_read_key;
    rw_params.enc_slot_id = ENCRYPT_READ_KEY;
    status = atecc608_read_data_enc(&rw_params);
    if( ATCA_SUCCESS != status )
    {
        //break; gwp_firmware_t
    }
    #else
    // _gwp_storage_read(GWP_DEV_INFO_ADDR + SECUREBOOT_KROOT_OFFSET, root_key, 32);
    //todo read rootkey
    #endif
    #if defined(NRF52840_XXAA)
    err_code = nrf_crypto_hkdf_calculate(NULL,
                                         &g_nrf_crypto_hmac_sha256_info,
                                         rpc_session_key,                          // Output key
                                         &Session_key_size,                    // Output key size
                                         root_key,                                // Input key
                                         sizeof(root_key),                        // Input key size
                                         salt_key_init,                           // Salt
                                         sizeof(salt_key_init),                   // Salt size
                                         &add_info_ssion[0],                     // Additional info
                                         sizeof(add_info_ssion) - 1,                                   // Additional info size
                                         GWP_CRYPTO_HKDF_EXTRACT_AND_EXPAND);   // Mode
    #elif defined(STM32WB55xx) || defined(CONFIG_IDF_TARGET_ESP32S3)
//    gwpFunctions.rngVectorGenerate
    err_code = gwpFunctions.hkdfCalculate(  rpc_session_key,                         // Output key
                                            (const uint8_t *)root_key,                                // Input key
                                            sizeof(root_key),                        // Input key size
                                            salt_key_init,                           // Salt
                                            sizeof(salt_key_init),                   // Salt size
                                            (uint8_t *)&add_info_ssion[0],                     // Additional info
                                            sizeof(add_info_ssion) - 1                                 // Additional info size
                                         );
    #else
#error "Architecture not set."
    #endif
    return err_code;
}



ret_code_t decrypted_payload(uint8_t *buf, uint16_t length, uint8_t* payload, uint8_t *iv)
{
    ret_code_t ret_code = GWP_SUCCESS;
    uint8_t key[RPC_KEY_SIZE];
    get_rpc_key(key);
    size_t out_length = length;
    ret_code = gwpFunctions.aesCrypt(GWP_CRYPTO_AES_MODE_CTR,
                                     GWP_CRYPTO_DECRYPT,
                                     key,
                                     iv,
                                     buf,
                                     length,
                                     payload,
                                     &out_length);
    return ret_code;
}



ret_code_t encrypted_payload(uint8_t *src_address, uint16_t length, uint8_t *dest_address, uint8_t *iv)
{
    ret_code_t ret_code = GWP_SUCCESS;
    uint8_t key[RPC_KEY_SIZE];
    get_rpc_key(key);
    size_t out_length = length;
    ret_code = gwpFunctions.aesCrypt(GWP_CRYPTO_AES_MODE_CTR,
                                     GWP_CRYPTO_ENCRYPT,
                                     key,
                                     iv,
                                     src_address,
                                     length,
                                     dest_address,
                                     &out_length);
    return ret_code;
}


#else

#error "Architecture not set."

#endif

