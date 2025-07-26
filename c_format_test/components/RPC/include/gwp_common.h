#ifndef _GWP_COMMON_H_
#define _GWP_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// #define ESP32S3
#ifndef RET_CODE_T
    #define RET_CODE_T
    typedef uint32_t ret_code_t;
#endif





//
#if defined(NRF52840_XXAA)

#include "sdk_common.h"
#include "nrf_section.h"

#include "app_error.h"

#include "app_error.h"


#ifndef BOOTLOADER_CODE
    #include "app_fifo.h"

    #if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )
        #include "gwp_dfu_process.h"
    #else

        #if (defined RPC_APPLICATION_CODE)
            #include "gwp_ble_interface.h"
            #include "gwp_ble_tunnel.h"

        #endif

        #define GWP_LOG_FLUSH NRF_LOG_FLUSH
    #endif
    #include "app_util.h"
    #include "boards.h"

    //#include "gwp_common.h"
    //#include "gwp_error_common.h"
    #include "sdk_errors.h"
    #include "app_error.h"
    #include "app_util.h"
    #include "boards.h"
    #include "app_timer.h"
    #include "nrf_crypto.h"
    #include "sdk_config.h"
    #include "nrf_log.h"
    #include "nrf_crypto_hkdf.h"
    //#include "sdk_errors.h"
    //    #include "secure_storage_api.h"//h145764
#endif

#elif defined(STM32WB55xx)
//#define     SUCCESS                                0
#define     FAIL                                1
#include "main.h"
#include "gwp_swversion.h"
#define GWP_LOG_FLUSH(...)
#include "SEGGER_RTT.h"

#define printf(...) SEGGER_RTT_printf(0, __VA_ARGS__)


#elif defined(CONFIG_IDF_TARGET_ESP32S3)
extern void print_hex(const void* data, uint32_t length, const char* debug_info);
typedef enum
{
    SUCCESS = 0,
    ERROR = !SUCCESS
} ErrorStatus;

#if   defined ( __CC_ARM )
    #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
    #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */

#elif defined ( __ICCARM__ )
    #define __ASM           __asm                                       /*!< asm keyword for IAR Compiler          */
    #define __INLINE        inline                                      /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */

#elif defined ( __GNUC__ )
    #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
    #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */

#elif defined ( __TASKING__ )
    #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
    #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */

#endif



typedef enum
{
    NRF_CRYPTO_AES_MODE_CBC,                // supported by: MBEDTLS & CC310
    NRF_CRYPTO_AES_MODE_CBC_PAD_PCKS7,      // supported by: MBEDTLS & CC310
    NRF_CRYPTO_AES_MODE_CFB,                // supported by: MBEDTLS
    NRF_CRYPTO_AES_MODE_CTR,                // supported by: MBEDTLS & CC310
    NRF_CRYPTO_AES_MODE_ECB,                // supported by: MBEDTLS & CC310
    NRF_CRYPTO_AES_MODE_ECB_PAD_PCKS7,      // supported by: MBEDTLS & CC310

    // Authentication modes
    NRF_CRYPTO_AES_MODE_CBC_MAC,            // supported by: MBEDTLS & CC310
    NRF_CRYPTO_AES_MODE_CBC_MAC_PAD_PCKS7,  // supported by: MBEDTLS & CC310
    NRF_CRYPTO_AES_MODE_CMAC,               // supported by: MBEDTLS & CC310
} nrf_crypto_aes_mode_t;


/**@brief Type specifying whether decrypt or encrypt operation shall be performed.
 */
typedef enum
{
    NRF_CRYPTO_DECRYPT       = 0,
    NRF_CRYPTO_ENCRYPT       = 1,
    NRF_CRYPTO_MAC_CALCULATE = 2
} nrf_crypto_operation_t;





// Define function pointer types
//typedef ret_code_t (*GwpAesCryptFunc)(nrf_crypto_aes_mode_t mode, nrf_crypto_operation_t operation,
//                                      const uint8_t *key, const uint8_t *iv, const uint8_t *input,//                                      size_t input_length, uint8_t *output, size_t *output_length);
//
//typedef ret_code_t (*GwpHkdfCalculateFunc)(uint8_t *output_key, const uint8_t *input_key,
//        size_t input_key_size, const uint8_t *salt,
//        size_t salt_size, const uint8_t *additional_info,
//        size_t additional_info_size);
//
//typedef ret_code_t (*GwpHmac256CalculateFunc)(uint8_t const * p_data,
//        size_t data_size,
//        uint8_t const * p_key,
//        size_t key_size,
//        uint8_t* p_digest,
//        size_t* const p_digest_size);



typedef enum
{
    GWP_CRYPTO_AES_MODE_CBC,                // supported by: MBEDTLS & CC310
    GWP_CRYPTO_AES_MODE_CBC_PAD_PCKS7,      // supported by: MBEDTLS & CC310
    GWP_CRYPTO_AES_MODE_CFB,                // supported by: MBEDTLS
    GWP_CRYPTO_AES_MODE_CTR,                // supported by: MBEDTLS & CC310
    GWP_CRYPTO_AES_MODE_ECB,                // supported by: MBEDTLS & CC310
    GWP_CRYPTO_AES_MODE_ECB_PAD_PCKS7,      // supported by: MBEDTLS & CC310

    // Authentication modes
    GWP_CRYPTO_AES_MODE_CBC_MAC,            // supported by: MBEDTLS & CC310
    GWP_CRYPTO_AES_MODE_CBC_MAC_PAD_PCKS7,  // supported by: MBEDTLS & CC310
    GWP_CRYPTO_AES_MODE_CMAC,               // supported by: MBEDTLS & CC310
} gwp_crypto_aes_mode_t;



/**@brief Type specifying whether decrypt or encrypt operation shall be performed.
 */
typedef enum
{
    GWP_CRYPTO_DECRYPT       = 0,
    GWP_CRYPTO_ENCRYPT       = 1,
    GWP_CRYPTO_MAC_CALCULATE = 2
} gwp_crypto_operation_t;

#define ESCS_ECDH_KEY_SIZE              (32)
#define ESCS_HKDF_SIZE              (32)

typedef ret_code_t (*GwpAesCryptFunc)(gwp_crypto_aes_mode_t mode,
                                      gwp_crypto_operation_t operation,
                                      uint8_t* p_key,
                                      uint8_t* p_iv,
                                      uint8_t* p_data_in,
                                      size_t data_size,
                                      uint8_t* p_data_out,
                                      size_t* p_data_out_size);

typedef ret_code_t (*GwpAesCcmCryptFunc)(gwp_crypto_operation_t operation,
        uint8_t* p_key,
        uint8_t* p_nonce,
        uint8_t nonce_size,
        uint8_t* p_adata,
        size_t adata_size,
        uint8_t* p_data_in,
        size_t data_size,
        uint8_t* p_data_out,
        uint8_t* p_mac,
        uint8_t mac_size);

typedef uint32_t (*GwpChecksumCrcFunc)(const uint8_t* p_data, uint32_t size);

typedef bool (*GwpChecksumCrcOkFunc)(const uint8_t* p_data, uint32_t size, uint32_t crc_chk);

typedef ret_code_t (*GwpEcdhComputeFunc)(const uint8_t* private_key,
        const uint8_t* public_key,
        uint8_t* share_key);

typedef ret_code_t (*GwpSecp256r1VerifyFunc)(const uint8_t* raw_public_key,
        const uint8_t* digest,
        const uint8_t* signature);

typedef ret_code_t (*GwpSecp256r1SignatureFunc)(const uint8_t* raw_private_key,
        const uint8_t* p_hash,
        uint8_t* p_signature);

typedef ret_code_t (*GwpSecp256r1GeneratePairFunc)(uint8_t* r_public_key,
        uint8_t* r_private_key);

typedef ret_code_t (*GwpHkdfCalculateFunc)(uint8_t* output,
        const uint8_t* input_key,
        uint8_t input_size,
        uint8_t* salt,
        uint8_t salt_size,
        uint8_t* info,
        uint8_t info_size);

typedef ret_code_t (*GwpHmac256CalculateFunc)(const uint8_t* p_data,
        size_t data_size,
        const uint8_t* p_key,
        size_t key_size,
        uint8_t* p_digest,
        size_t* p_digest_size);

typedef ret_code_t (*GwpRngVectorGenerateFunc)(uint8_t* const p_target, size_t size);
typedef ret_code_t (*GwpHash256InitFunc)(void* const p_context);
typedef ret_code_t (*GwpHash256UpdateFunc)(void* const p_context, const uint8_t* p_data, size_t data_size);
typedef ret_code_t (*GwpHash256FinalizeFunc)(void* const p_context, uint8_t* p_digest, size_t* p_digest_size);
typedef ret_code_t (*GwpHash256CalculateFunc)(const uint8_t* p_data,
        size_t data_size,
        uint8_t* p_digest,
        size_t* p_digest_size);
typedef ret_code_t (*GwpX509CrtVerifyFunc)(const uint8_t* crt, size_t crt_len, const uint8_t* ca, size_t ca_len,
        uint8_t* crl, size_t crl_len);





typedef ret_code_t (*GwpUsingCrtVerifySignatureFunc)(const uint8_t* crt,
        size_t crt_len,
        const uint8_t* hash,
        size_t hlen,
        const uint8_t* sig,
        size_t slen);

typedef ret_code_t (*GwpGetPkFromCrtFunc)(const uint8_t* crt,
        size_t crt_len,
        uint8_t* pk,
        size_t pk_len);

typedef ret_code_t (*GwpReadCertFunc)(uint8_t* buff,
                                      uint32_t inlength,
                                      uint32_t* outlen);

typedef ret_code_t (*GwpWriteCertFunc)(uint8_t* in_buff,
                                       uint32_t in_length);


typedef ret_code_t (*GwpRpcPortSendFunc)(uint8_t *buf, uint16_t length);
typedef ret_code_t (*GwpRpcPortReceiveFunc)(uint8_t *data, uint16_t *length);

typedef ret_code_t (*GwpRpcPortInitFunc)();



//// Initialize function pointer with gwp_hmac256_calculate function
//GwpHmac256CalculateFunc gwpHmac256Calculate = gwp_hmac256_calculate;




// Define structure containing function pointers
typedef struct
{
//    GwpAesCryptFunc aesCrypt;
//    GwpHkdfCalculateFunc hkdfCalculate;
//    GwpHmac256CalculateFunc  hmac256_calculate;
    GwpAesCryptFunc aesCrypt;
    GwpAesCcmCryptFunc aesCcmCrypt;
    GwpChecksumCrcFunc checksumCrc;
    GwpChecksumCrcOkFunc checksumCrcOk;
    GwpEcdhComputeFunc ecdhCompute;
    GwpSecp256r1VerifyFunc secp256r1Verify;
    GwpSecp256r1SignatureFunc secp256r1Signature;
    GwpSecp256r1GeneratePairFunc secp256r1GeneratePair;
    GwpHkdfCalculateFunc hkdfCalculate;
    GwpHmac256CalculateFunc hmac256Calculate;
    GwpRngVectorGenerateFunc rngVectorGenerate;
    GwpHash256InitFunc hash256Init;
    GwpHash256UpdateFunc hash256Update;
    GwpHash256FinalizeFunc hash256Finalize;
    GwpHash256CalculateFunc hash256Calculate;
    GwpX509CrtVerifyFunc x509CrtVerify;
    GwpUsingCrtVerifySignatureFunc usingCrtVerifySignature;
    GwpGetPkFromCrtFunc getPkFromCrt;
    GwpReadCertFunc readCert;
    GwpWriteCertFunc writeCert;
    //uart related func
    GwpRpcPortInitFunc rpcPortInit;
    GwpRpcPortSendFunc rpcPortSend;
    GwpRpcPortReceiveFunc rpcPortReceive;

} GwpFunctionPointers;
extern GwpFunctionPointers gwpFunctions;
#define GWP_LOG_FLUSH(...)



#else

#error "Architecture not set."

#endif
#define GWP_CRYPTO_ECC_SECP256R1_RAW_PRIVATE_KEY_SIZE  (256 / 8)      /**< @brief Raw private key size for secp256r1 (NIST 256-bit).         @ingroup nrf_crypto_ecc_secp256r1 */
#define GWP_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE   (2 * 256 / 8)  /**< @brief Raw public key size for curve secp256r1 (NIST 256-bit).    @ingroup nrf_crypto_ecc_secp256r1 */
#define GWP_CRYPTO_ECDSA_SECP256R1_SIGNATURE_SIZE  (2 * 256 / 8)   /**< @brief Size of a signature for secp256r1 (NIST 256-bit) curve.     @ingroup nrf_crypto_ecdsa_secp256r1 */
#define GWP_CRYPTO_HASH_SIZE_MD5        (20)
#define GWP_CRYPTO_HASH_SIZE_SHA1       (20)
#define GWP_CRYPTO_HASH_SIZE_SHA224     (28)
#define GWP_CRYPTO_HASH_SIZE_SHA256     (32)
#define GWP_CRYPTO_HASH_SIZE_SHA384     (48)
#define GWP_CRYPTO_HASH_SIZE_SHA512     (64)

#ifdef NONE_SELF_CONTAIN
    #ifndef GWP_QSPI_ENABLED
        #define GWP_QSPI_ENABLED 0
    #endif
#else
    #ifndef GWP_QSPI_ENABLED
        #define GWP_QSPI_ENABLED 1
    #endif
#endif


#if defined(STM32WB55xx)
    #define DFU_MODULE_ENABLED
#endif

#endif