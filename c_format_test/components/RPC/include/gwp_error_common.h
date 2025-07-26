#ifndef __GWP_ERROR_COMMON_H__
#define __GWP_ERROR_COMMON_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "gwp_common.h"

#if defined(NRF52840_XXAA)
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
//#include "sdk_common.h"
//#include "sdk_errors.h"
//
//#include "app_error.h"
//#include "nrf_log.h"
//#include "nrf_log_ctrl.h"
//#include "nrf_log_default_backends.h"
//
//#include "nrf_section.h"
//#include "app_fifo.h"
//#include "app_uart.h"
//#include "app_util.h"
//#include "boards.h"
//#include "app_error.h"
//#include "app_util.h"
//#include "boards.h"
//#include "app_timer.h"
//#include "sdk_config.h"
//#include "nrf_crypto.h"
#endif
//#include "mem_map.h"






#define NRF_ERROR_DFU_BASE_NUM  (0x5000)    ///< DFU error base


#define GWP_DFU_ERROR_PACKAGE                                   (NRF_ERROR_DFU_BASE_NUM + 0)
#define GWP_DFU_ERROR_MODULE_TYPE                               (NRF_ERROR_DFU_BASE_NUM + 2)
#define GWP_DFU_ERROR_STATUS                                    (NRF_ERROR_DFU_BASE_NUM + 4)
#define GWP_DFU_ERROR_PACKAGE_FALLBACK_VERSION                  (NRF_ERROR_DFU_BASE_NUM + 5)
#define GWP_DFU_ERROR_PACKAGE_VERSION                           (NRF_ERROR_DFU_BASE_NUM + 6)
#define GWP_DFU_ERROR_SET_PACKAGE_VERSION                       (NRF_ERROR_DFU_BASE_NUM + 3)
#define GWP_DFU_ERROR_UPDATE_PROCESS                            (NRF_ERROR_DFU_BASE_NUM + 7)
#define GWP_DFU_ERROR_LOCATION                                  (NRF_ERROR_DFU_BASE_NUM + 8)
#define GWP_DFU_ERROR_MODULE_VERIFY                             (NRF_ERROR_DFU_BASE_NUM + 11)
#define GWP_DFU_ERROR_HEADER                                    (NRF_ERROR_DFU_BASE_NUM + 12)






#define GWP_RPC_ERROR_BASE_NUM  (0x6000)    ///< RPC error base


#define RPC_ERROR_FRAME_START                               (GWP_RPC_ERROR_BASE_NUM + 0)
#define RPC_ERROR_FRAME_SIZE                                (GWP_RPC_ERROR_BASE_NUM + 1)
#define RPC_ERROR_FRAME_VERIFY                              (GWP_RPC_ERROR_BASE_NUM + 2)
#define RPC_ERROR_FRAME_TYPE                                (GWP_RPC_ERROR_BASE_NUM + 3)
#define RPC_ERROR_FRAME_SEQ                                 (GWP_RPC_ERROR_BASE_NUM + 4)
#define RPC_ERROR_FRAME_PARAM_SIZE                          (GWP_RPC_ERROR_BASE_NUM + 5)
#define RPC_ERROR_FRAME_CRYPTO                              (GWP_RPC_ERROR_BASE_NUM + 6)
#define RPC_ERROR_FRAME_FUNC                                (GWP_RPC_ERROR_BASE_NUM + 7)

#define RPC_ERROR_MEMORY_OUT                                (GWP_RPC_ERROR_BASE_NUM + 8)

#define RPC_ERROR_COMMAND_RM_TYPE                           (GWP_RPC_ERROR_BASE_NUM + 9)
#define RPC_ERROR_QUEUE_SEND                                (GWP_RPC_ERROR_BASE_NUM + 10)
#define RPC_ERROR_QUEUE_RECEIVE                             (GWP_RPC_ERROR_BASE_NUM + 11)

#define RPC_ERROR_INIT_STATUS                               (GWP_RPC_ERROR_BASE_NUM + 12)
#define RPC_ERROR_CERT_SEQ                                  (GWP_RPC_ERROR_BASE_NUM + 13)





#define GWP_ERROR_BASE_NUM      (0x0)       ///< Global error base
#define GWP_SUCCESS                           (GWP_ERROR_BASE_NUM + 0)  ///< Successful command
#define GWP_ERROR_SVC_HANDLER_MISSING         (GWP_ERROR_BASE_NUM + 1)  ///< SVC handler is missing
#define GWP_ERROR_SOFTDEVICE_NOT_ENABLED      (GWP_ERROR_BASE_NUM + 2)  ///< SoftDevice has not been enabled
#define GWP_ERROR_INTERNAL                    (GWP_ERROR_BASE_NUM + 3)  ///< Internal Error
#define GWP_ERROR_NO_MEM                      (GWP_ERROR_BASE_NUM + 4)  ///< No Memory for operation
#define GWP_ERROR_NOT_FOUND                   (GWP_ERROR_BASE_NUM + 5)  ///< Not found
#define GWP_ERROR_NOT_SUPPORTED               (GWP_ERROR_BASE_NUM + 6)  ///< Not supported
#define GWP_ERROR_INVALID_PARAM               (GWP_ERROR_BASE_NUM + 7)  ///< Invalid Parameter
#define GWP_ERROR_INVALID_STATE               (GWP_ERROR_BASE_NUM + 8)  ///< Invalid state, operation disallowed in this state
#define GWP_ERROR_INVALID_LENGTH              (GWP_ERROR_BASE_NUM + 9)  ///< Invalid Length
#define GWP_ERROR_INVALID_FLAGS               (GWP_ERROR_BASE_NUM + 10) ///< Invalid Flags
#define GWP_ERROR_INVALID_DATA                (GWP_ERROR_BASE_NUM + 11) ///< Invalid Data
#define GWP_ERROR_DATA_SIZE                   (GWP_ERROR_BASE_NUM + 12) ///< Data size exceeds limit
#define GWP_ERROR_TIMEOUT                     (GWP_ERROR_BASE_NUM + 13) ///< Operation timed out
#define GWP_ERROR_NULL                        (GWP_ERROR_BASE_NUM + 14) ///< Null Pointer
#define GWP_ERROR_FORBIDDEN                   (GWP_ERROR_BASE_NUM + 15) ///< Forbidden Operation
#define GWP_ERROR_INVALID_ADDR                (GWP_ERROR_BASE_NUM + 16) ///< Bad Memory Address
#define GWP_ERROR_BUSY                        (GWP_ERROR_BASE_NUM + 17) ///< Busy

#define GWP_INVALID_PRODUCT_ID                (GWP_ERROR_BASE_NUM + 18) /// invalid product ID 2 -16
#define GWP_INVALID_HWVERSION                 (GWP_ERROR_BASE_NUM + 19) /// invalid HW version 3
#define GWP_INVALID_HEADER_SIGNATURE          (GWP_ERROR_BASE_NUM + 20) ///  corrupted OTA package header signature�ꡧarea:1c0-200��? 4 
#define GWP_INVALID_PACKAGE_SIGNATURE         (GWP_ERROR_BASE_NUM + 21) ///corrupted OTA package signature (the last 64 bytes) 5 
#define GWP_SAME_VERSION                      (GWP_ERROR_BASE_NUM + 22) /// same version 6 
#define GWP_OLDER_VERSION                     (GWP_ERROR_BASE_NUM + 23) /// older version 7  
#define GWP_UNDER_PROVISION                   (GWP_ERROR_BASE_NUM + 24) /// During provision,can't update again. 8  
#define GWP_NOT_FALLBACK_VERSION              (GWP_ERROR_BASE_NUM + 25) /// For snap on io not fallback version. 9


#define GWP_ECC_ERROR_BASE_NUM  (0x6500)    ///< ecc error base  --this value is assigned temporarily for possible conflict
#define GWP_ERROR_ECC_INIT           (GWP_ECC_ERROR_BASE_NUM + 17) ///< ECC init error
#define GWP_ERROR_ECC_SELFTEST       (GWP_ECC_ERROR_BASE_NUM + 18) ///< ECC selftest error
#define GWP_ERROR_ECC_SECBOOT        (GWP_ECC_ERROR_BASE_NUM + 19) ///< ECC secure boot error
#define GWP_ERROR_ECC_VERIFY         (GWP_ECC_ERROR_BASE_NUM + 20) ///< ECC signature verify error
#define GWP_ERROR_ECC_CHKMAC         (GWP_ECC_ERROR_BASE_NUM + 21) ///< ECC check mac error
#define GWP_ERROR_ECC_SETLATCH       (GWP_ECC_ERROR_BASE_NUM + 22) ///< ECC setlatch error
#define GWP_ERROR_ECC_GETLATCH       (GWP_ECC_ERROR_BASE_NUM + 23) ///< ECC getlatch error
#define GWP_ERROR_ECC_CHKLATCH       (GWP_ECC_ERROR_BASE_NUM + 24) ///< ECC check latch state error
#define GWP_ERROR_ECC_ENREAD         (GWP_ECC_ERROR_BASE_NUM + 25) ///< ECC encrypt read error
#define GWP_ERROR_ECC_NOKEY          (GWP_ECC_ERROR_BASE_NUM + 26) ///< ECC no key error
#define GWP_ERROR_ECC_CMD            (GWP_ECC_ERROR_BASE_NUM + 27) ///< ECC command error
#define GWP_ERROR_BOOT_MODE          (GWP_ECC_ERROR_BASE_NUM + 28) ///< Invlaid boot mode
#define GWP_ERROR_ECC_ELSE           (GWP_ECC_ERROR_BASE_NUM + 29) ///< No Hon ECC
#define GWP_ERROR_ECC_KDF            (GWP_ECC_ERROR_BASE_NUM + 30) ///< ECC KDF error
#define GWP_ERROR_ECC_PROV           (GWP_ECC_ERROR_BASE_NUM + 31) ///< ECC provision error
#define GWP_ERROR_ECC_CSR            (GWP_ECC_ERROR_BASE_NUM + 32) ///< ECC csr error




#define GWP_ERROR_CRYPTO_ERR_BASE           (0x8500)    /**< Base address for crypto related errors. */
#define GWP_ERROR_CRYPTO_NOT_INITIALIZED            (GWP_ERROR_CRYPTO_ERR_BASE + 0x00)      /**< @ref GWP_crypto_init was not called prior to this crypto function. */
#define GWP_ERROR_CRYPTO_CONTEXT_NULL               (GWP_ERROR_CRYPTO_ERR_BASE + 0x01)      /**< A null pointer was provided for the context structure. */
#define GWP_ERROR_CRYPTO_CONTEXT_NOT_INITIALIZED    (GWP_ERROR_CRYPTO_ERR_BASE + 0x02)      /**< The context was not initialized prior to this call or it was corrupted. Call the corresponding init function for the algorithm to initialize it. */
#define GWP_ERROR_CRYPTO_FEATURE_UNAVAILABLE        (GWP_ERROR_CRYPTO_ERR_BASE + 0x03)      /**< The function was called with a feature that is unavailable. */
#define GWP_ERROR_CRYPTO_BUSY                       (GWP_ERROR_CRYPTO_ERR_BASE + 0x04)      /**< The function could not be called because the crypto backend was busy. Rerun the cryptographic routine at a later time. */

#define GWP_ERROR_CRYPTO_INPUT_NULL                 (GWP_ERROR_CRYPTO_ERR_BASE + 0x10)      /**< One or more of the input arguments for this function were NULL. */
#define GWP_ERROR_CRYPTO_INPUT_LENGTH               (GWP_ERROR_CRYPTO_ERR_BASE + 0x11)      /**< The length of one or more of the input arguments was invalid. */
#define GWP_ERROR_CRYPTO_INPUT_LOCATION             (GWP_ERROR_CRYPTO_ERR_BASE + 0x12)      /**< Input data not in RAM. */
#define GWP_ERROR_CRYPTO_OUTPUT_NULL                (GWP_ERROR_CRYPTO_ERR_BASE + 0x13)      /**< One or more of the output arguments for this function were NULL. */
#define GWP_ERROR_CRYPTO_OUTPUT_LENGTH              (GWP_ERROR_CRYPTO_ERR_BASE + 0x14)      /**< The length of one or more output arguments was too small. */
#define GWP_ERROR_CRYPTO_ALLOC_FAILED               (GWP_ERROR_CRYPTO_ERR_BASE + 0x15)      /**< A required memory allocation failed. */
#define GWP_ERROR_CRYPTO_INTERNAL                   (GWP_ERROR_CRYPTO_ERR_BASE + 0x16)      /**< An internal error occurred when calling this function. */
#define GWP_ERROR_CRYPTO_INVALID_PARAM              (GWP_ERROR_CRYPTO_ERR_BASE + 0x17)      /**< Invalid combination of input parameters. */
#define GWP_ERROR_CRYPTO_KEY_SIZE                   (GWP_ERROR_CRYPTO_ERR_BASE + 0x18)      /**< Size of the key is not supported by choosen backend. */
#define GWP_ERROR_CRYPTO_STACK_OVERFLOW             (GWP_ERROR_CRYPTO_ERR_BASE + 0x19)      /**< Stack overflow detected. */

#define GWP_ERROR_CRYPTO_ECC_ERR_BASE               (GWP_ERROR_CRYPTO_ERR_BASE + 0x40)      /**< Base error code for ECC. */
#define GWP_ERROR_CRYPTO_ECC_KEY_NOT_INITIALIZED    (GWP_ERROR_CRYPTO_ECC_ERR_BASE + 0x00)  /**< The key was not initialized. */
#define GWP_ERROR_CRYPTO_ECDH_CURVE_MISMATCH        (GWP_ERROR_CRYPTO_ECC_ERR_BASE + 0x01)  /**< Public and private key provided to ECDH have different types of curves. */
#define GWP_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE    (GWP_ERROR_CRYPTO_ECC_ERR_BASE + 0x02)  /**< Signature verification check reported invalid signature. */
#define GWP_ERROR_CRYPTO_ECC_INVALID_KEY            (GWP_ERROR_CRYPTO_ECC_ERR_BASE + 0x03)  /**< Provided key is invalid. */

#define GWP_ERROR_CRYPTO_AES_ERR_BASE               (GWP_ERROR_CRYPTO_ERR_BASE + 0x50)      /**< Base error code for all AES modes. */
#define GWP_ERROR_CRYPTO_AES_INVALID_PADDING        (GWP_ERROR_CRYPTO_AES_ERR_BASE + 0x00)  /**< Message padding is corrupted. */

#define GWP_ERROR_CRYPTO_AEAD_ERR_BASE              (GWP_ERROR_CRYPTO_ERR_BASE + 0x60)      /**< Base error code for all AEAD modes. */
#define GWP_ERROR_CRYPTO_AEAD_INVALID_MAC           (GWP_ERROR_CRYPTO_AEAD_ERR_BASE + 0x00) /**< MAC not matching encrypted text. */
#define GWP_ERROR_CRYPTO_AEAD_NONCE_SIZE            (GWP_ERROR_CRYPTO_AEAD_ERR_BASE + 0x01) /**< Size of the nonce is not supported in this AEAD mode. */
#define GWP_ERROR_CRYPTO_AEAD_MAC_SIZE              (GWP_ERROR_CRYPTO_AEAD_ERR_BASE + 0x02) /**< Size of the MAC (tag) is not supported in this AEAD mode. */

#define GWP_ERROR_CRYPTO_RNG_ERR_BASE               (GWP_ERROR_CRYPTO_ERR_BASE + 0x70)      /**< Base error code for all RNG modes. */
#define GWP_ERROR_CRYPTO_RNG_INIT_FAILED            (GWP_ERROR_CRYPTO_RNG_ERR_BASE + 0x00)  /**< Initialization or startup of RNG failed. */
#define GWP_ERROR_CRYPTO_RNG_RESEED_REQUIRED        (GWP_ERROR_CRYPTO_RNG_ERR_BASE + 0x01)  /**< Reseed required (reseed counter overflowed). */



typedef enum GPW_ERROR_CODE_ALL
{
    auth_none,

//gwp_ble_tunnel file error

    auth_m1_error1 = 2,
    err_secp256r1_verify,
    err_atecc608_sign,
    err_ack_status_hmac,//RECEIVE_STATUS_HMAC_ERROR,
    err_lost_sequence_number,
    err_ecc608_verify,
    err_eccchip_not_config,
    err_m2_malloc,


//gwp_ble_fragment file error
    err_defragment_ack_duplicate,
    err_ack_lostdata,//RECEIVE_STATUS_LOSTDATA
    err_duplicate_pkg,
    err_ble_security_send,//gwp_ble_security_service_send


//  gwp_app_tasks file error
    err_ble_nodify_fail,
    err_ble_defragment_init,
    err_acc_packet_duplicate,//RECEIVE_STATUS_PACKET_DUPLICATE
    err_defragment_ack_msg_lost,//DEFRAGMENT_ACK_MESSAGE_LOST
    err_ble_notify_fail,
    err_ble_ack_lostdata,
    err_gwp_notify_fail,


//gwp_ble_interface file error
    err_app_secure_init,//APP_SECURE_INIT_ERROR
    err_app_secure_datalen,//APP_SECURE_DATALEN_ERROR
    err_app_secure_rpc_send_no_ack,//APP_SECURE_RPC_SEND_NO_ACK
    err_app_secuer_gwp_send_no_ack,//APP_SECURE_GWP_SEND_NO_ACK
    err_app_secure_encryption,//APP_SECURE_ENCRYPTION_ERROR
    err_app_secure_decryp,//APP_SECURE_ENCRYPTION_ERROR
    err_app_secure_lost_seq_number,//APP_SECURE_LOST_SEQUENCE_NUMBER
    err_app_secure_channel_establish,//APP_SECURE_CHANNEL_ESTABLISH_ERROR
    err_callback_parameter,

//gwp_ble_ota file error
    err_dl_image,
    err_ota_install,




//gwp_ble_security file error
    err_Invalid_evt_handle,
    err_notify_fail,
    err_notify_fail_reason,
    err_public_key_add,







} GPW_ERROR_CODE_ALL_ENUM;



#if defined(NRF52840_XXAA)


#ifdef DEBUG
#define DFU_ERROR_HANDLER(ERR_CODE)                                    \
    do                                                                 \
    {                                                                  \
        GWP_LOG_ERROR("app_error_handler err_code:%d %d:%s", (ERR_CODE), __LINE__,  (uint8_t*) __FILE__);  \
    } while (0)
#else
#define DFU_ERROR_HANDLER(ERR_CODE)                                    \
    do                                                                 \
    {                                                                  \
        NRF_LOG_ERROR("Received an error: 0x%08x!", (ERR_CODE));        \
    } while (0)
#endif





#define DFU_ERROR_CHECK(ERR_CODE)                           \
    do                                                      \
    {                                                       \
        const uint32_t LOCAL_ERR_CODE = (ERR_CODE);         \
        if (LOCAL_ERR_CODE != NRF_SUCCESS)                  \
        {                                                   \
            DFU_ERROR_HANDLER(LOCAL_ERR_CODE);              \
            return  LOCAL_ERR_CODE;                         \
        }                                                   \
    } while (0)





//#define __DEBUG

#ifdef  __DEBUG
#define GWP_LOG_ERROR(...)              \
    {                               \
        NRF_LOG_ERROR(__VA_ARGS__);   \
        NRF_LOG_FLUSH();              \
    }                               \


#define GWP_LOG_WARNING(...)            \
    {                               \
        NRF_LOG_WARNING(__VA_ARGS__); \
        NRF_LOG_FLUSH();              \
    }                               \


#define GWP_LOG_INFO(...)               \
    {                               \
        NRF_LOG_INFO(__VA_ARGS__);    \
        NRF_LOG_FLUSH();              \
    }                               \

#define GWP_LOG_HEXDUMP_INFO(...)               \
    {                                       \
        NRF_LOG_HEXDUMP_INFO(__VA_ARGS__);    \
    }

#define GWP_LOG_DEBUG(...)              \
    {                               \
        NRF_LOG_DEBUG(__VA_ARGS__);   \
        NRF_LOG_FLUSH();              \
    }                               \

#define LOG_GWP_ERR(err_code,fmt,args...) do{NRF_LOG_ERROR("ERR code:[%d] >"fmt,err_code,##args);NRF_LOG_FLUSH();}while(0);
#else
#define GWP_LOG_ERROR(...)              \
    {                               \
        NRF_LOG_ERROR(__VA_ARGS__);   \
        NRF_LOG_FLUSH();              \
    }                               \


#define GWP_LOG_WARNING(...)

#define GWP_LOG_INFO(...)               \
    {                               \
        NRF_LOG_INFO(__VA_ARGS__);    \
        NRF_LOG_FLUSH();              \
    }

#define GWP_LOG_HEXDUMP_INFO(...)
#define GWP_LOG_DEBUG(...)

//#define GWP_LOG_FLUSH(...)
#define LOG_GWP_ERR(err_code,fmt,args...) do{GWP_LOG_ERROR("ERR_code:[%d]",err_code);}while(0);
#endif


#endif



#if defined(STM32WB55xx) //Need to verify
#define APP_ERROR_HANDLER(ERR_CODE)                                    \
    do                                                                 \
    {                                                                  \
        printf((ERR_CODE), __LINE__, (uint8_t*) __FILE__);  \
    } while (0)


#define APP_ERROR_CHECK_BOOL(BOOLEAN_VALUE)                   \
    do                                                        \
    {                                                         \
        const uint32_t LOCAL_BOOLEAN_VALUE = (BOOLEAN_VALUE); \
        if (!LOCAL_BOOLEAN_VALUE)                             \
        {                                                     \
            APP_ERROR_HANDLER(0);                             \
        }                                                     \
    } while (0)




#define APP_ERROR_CHECK(ERR_CODE)                           \
    do                                                      \
    {                                                       \
        const uint32_t LOCAL_ERR_CODE = (ERR_CODE);         \
        if (LOCAL_ERR_CODE != GWP_SUCCESS)                  \
        {                                                   \
            printf("app error check:%x",LOCAL_ERR_CODE);             \
        }                                                   \
    } while (0)



#define VERIFY_TRUE(statement, err_code)    \
    do                                          \
    {                                           \
        if (!(statement))                       \
        {                                       \
            return err_code;                    \
        }                                       \
    } while (0)



#include "SEGGER_RTT.h"
#define GWP_LOG_INFO(...) SEGGER_RTT_printf(0, __VA_ARGS__)

#define GWP_LOG_ERROR(...)
#define GWP_LOG_HEXDUMP_INFO(...)

#define DFU_ERROR_CHECK(ERR_CODE)                           \
    do                                                      \
    {                                                       \
        const uint32_t LOCAL_ERR_CODE = (ERR_CODE);         \
        if (LOCAL_ERR_CODE != GWP_SUCCESS)                  \
        {                                                   \
            printf("dfu error check: %x\r\n",LOCAL_ERR_CODE);              \
            return  LOCAL_ERR_CODE;                         \
        }                                                   \
    } while (0)



#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define APP_ERROR_CHECK(ERR_CODE)                           \
    do                                                      \
    {                                                       \
        const uint32_t LOCAL_ERR_CODE = (ERR_CODE);         \
        if (LOCAL_ERR_CODE != GWP_SUCCESS)                  \
        {                                                   \
            printf("app error check:%x",LOCAL_ERR_CODE);             \
        }                                                   \
    } while (0)



#define VERIFY_TRUE(statement, err_code)    \
    do                                          \
    {                                           \
        if (!(statement))                       \
        {                                       \
            return err_code;                    \
        }                                       \
    } while (0)
//#define GWP_LOG_INFO(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#define GWP_LOG_INFO(...)

#define GWP_LOG_ERROR(...)

#define LOG_GWP_ERR(...)
#define GWP_LOG_HEXDUMP_INFO(...)
#define GWP_LOG_FLUSH(...)

#define DFU_ERROR_CHECK(ERR_CODE)

#else

//#error "Architecture not set."

#endif





#ifdef __cplusplus
}
#endif
#endif // !__GWP_ERROR_COMMON_H__



