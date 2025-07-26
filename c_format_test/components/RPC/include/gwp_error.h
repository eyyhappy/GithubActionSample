#ifndef __DFU_ERROR_H__
#define __DFU_ERROR_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
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


#ifdef DEBUG
#define GWP_ERROR_HANDLER(ERR_CODE)                                    \
    do                                                                 \
    {                                                                   \
        NRF_LOG_ERROR("app_error_handler err_code:%d %s:%d", (ERR_CODE), __LINE__,  (uint8_t*) __FILE__);  \
    } while (0)
#else
#define GWP_ERROR_HANDLER(ERR_CODE)                                    \
    do                                                                 \
    {                                                                  \
        NRF_LOG_ERROR("Received an error: 0x%08x!", (ERR_CODE));        \
    } while (0)
#endif





#define GWP_ERROR_CHECK(ERR_CODE)                           \
    do                                                      \
    {                                                       \
        const uint32_t LOCAL_ERR_CODE = (ERR_CODE);         \
        if (LOCAL_ERR_CODE != NRF_SUCCESS)                  \
        {                                                   \
            GWP_ERROR_HANDLER(LOCAL_ERR_CODE);              \
            return  LOCAL_ERR_CODE;                         \
        }                                                   \
    } while (0)





#ifdef  __DEBUG

#define GWP_LOG_ERROR(...)              \
    {                               \
        NRF_LOG_ERROR(__VA_ARGS__);   \
        NRF_LOG_FLUSH();              \
    }                               \


#define GWP_LOG_WARNING(...)            \
    {                               \
        NRF_LOG_WARNING(__VA_ARGS__); \
    }                               \


#define GWP_LOG_INFO(...)               \
    {                               \
        NRF_LOG_INFO(__VA_ARGS__);    \
    }                               \

#define GWP_LOG_HEXDUMP_INFO(...)               \
    {                                       \
        NRF_LOG_HEXDUMP_INFO(__VA_ARGS__);    \
    }

#define GWP_LOG_DEBUG(...)              \
    {                               \
        NRF_LOG_DEBUG(__VA_ARGS__);   \
    }                               \

#define LOG_GWP_ERR(err_code,fmt,args...) do{NRF_LOG_ERROR("ERR code:[%d] >"fmt,err_code,##args);NRF_LOG_FLUSH();}while(0);


//#define print_warning(fmt, ...) GWP_LOG_INFO("warning@%s %d: "fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__)
//#define print_info(fmt, ...)    GWP_LOG_INFO("info@%s %d: "fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__)
//#define print_err(fmt, ...)     GWP_LOG_INFO("err@%s %d: "fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__)


#else //release














#define GWP_LOG_ERROR(...)              \
    {                               \
        NRF_LOG_ERROR(__VA_ARGS__);   \
        NRF_LOG_FLUSH();              \
    }                               \


#define GWP_LOG_WARNING(...)
#define GWP_LOG_INFO(...)
#define GWP_LOG_HEXDUMP_INFO(...)
#define GWP_LOG_DEBUG(...)


//#define LOG_GWP_ERR(err_code,fmt,args...) do{GWP_LOG_INFO("ERR_code:[%d]",err_code);}while(0);


#define LOG_GWP_ERR(err_code,fmt,args...) do{GWP_LOG_ERROR("ERR_code:[%d]",err_code);}while(0);






#endif



extern void test_print_all( void );





#ifdef __cplusplus
}
#endif
#endif // !__DFU_ERROR_H__



