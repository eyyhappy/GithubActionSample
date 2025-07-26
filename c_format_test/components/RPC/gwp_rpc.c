#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//#include "app_error.h"
//#include "app_util.h"
//#include "boards.h"
//#include "app_timer.h"
//#include "nrf_crypto.h"
//#include "sdk_config.h"
#include "gwp_common.h"


#include "gwp_slip.h"

#include "gwp_rpc_crypto.h"

#include "gwp_rpc.h"
#include "gwp_error_common.h"

//#include "rng_host.h"
#include "gwp_list.h"
#include "gwp_rpc_indication.h"
#include "gwp_rpc_func.h"
#include "gwp_rpc_init.h"

//#include "hmac256_host.h"
//#include "gwp_mem_manager.h"

#ifdef FREERTOS
    #include "task.h"
#else

#endif






#define RPC_UART

#ifdef RPC_UART
    // #include "gwp_rpc_uart.h"
#elif  RPC_SPI
    #include "gwp_rpc_spi.h"
#else
    #error "Please define RPC_UART or  RPC_SPI"
#endif

#if defined(NRF52840_XXAA)

    #include "gwp_ota_process.h"

#elif defined(STM32WB55xx)

    #define CODE_PAGE_SIZE (MBR_PAGE_SIZE_IN_WORDS * sizeof(uint32_t))

#elif defined(CONFIG_IDF_TARGET_ESP32S3)


    #define CODE_PAGE_SIZE (1024)
#else

    #error "Architecture not set."

#endif
static slip_t m_slip;
gwp_rpc_t gwp_rpc = GWP_RPC_INSTANCE();
gwp_list_head_t          gwp_rm_req_queue;
gwp_list_head_t          gwp_rm_send_queue;
gwp_list_head_t          gwp_reply_queue;


static const uint8_t m_lc_req[] = LC_REQUESTS;

#define RM_REQ_COUNT  (sizeof(m_rm_req)/sizeof(m_rm_req[0]))
#define LC_REQ_COUNT  (sizeof(m_lc_req)/sizeof(m_lc_req[0]))

#define MAX_RTC_COUNTER_VAL     0x00FFFFFF                                  /**< Maximum value of the RTC counter. */


#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )

#else
static const uint8_t m_rm_req[] = RM_REQUESTS;


#if defined(NRF52840_XXAA)

/**@brief Function for returning the current value of the RTC1 counter.
 *
 * @return     Current value of the RTC1 counter.
 */
static __INLINE uint32_t rtc1_counter_get(void)
{
    return NRF_RTC1->COUNTER;
}


#elif defined(STM32WB55xx)

static __INLINE uint32_t rtc1_counter_get(void)
{
    return 0;
}

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
//static __INLINE uint32_t rtc1_counter_get(void)
//{
//    return 0;
//}
//#include "core_cm4.h"                /* Cortex-M4 processor and core peripherals */

#else



#error "Architecture not set."

#endif



/**@brief Function for computing the difference between two RTC1 counter values.
 *
 * @return     Number of ticks elapsed from ticks_old to ticks_now.
 */
//static __INLINE uint32_t ticks_diff_get(uint32_t ticks_now, uint32_t ticks_old)
//{
//    return ((ticks_now - ticks_old) & MAX_RTC_COUNTER_VAL);
//}
#endif





static ret_code_t gwp_rpc_slip_port_send(uint8_t *buf, uint16_t length)
{
    uint32_t out_length = 0;
    uint8_t send_buf[MAX_RESP_SIZE];
    ret_code_t ret_code;
    gwp_slip_encode(send_buf, buf, length, &out_length);
    ret_code = gwpFunctions.rpcPortSend(send_buf, out_length);
    return ret_code;
}


/**
 * \brief   Helper function for finding an entry in an array
 * \param   array
 *          Search array
 * \param   size
 *          Elements in search array
 * \param   value
 *          The value to search
 */
static bool find(const uint8_t * array,
                 const uint8_t size,
                 const uint8_t value)
{
    for (uint32_t i = 0; i < size; i++)
    {
        if (array[i] == value)
        {
            return true;
        }
    }
    return false;
}


static bool gwp_is_lc_request(uint8_t func)
{
    return find(m_lc_req, LC_REQ_COUNT, func);
}


#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )

#else
static bool gwp_is_rm_request(uint8_t func)
{
    return find(m_rm_req, RM_REQ_COUNT, func);
}
#endif




static ret_code_t calculate_verify_data(uint8_t *in_data, uint32_t in_len, uint8_t *out_data, uint8_t out_len)
{
    ret_code_t ret_code;
    uint8_t hamc_data[32] = {0};
    uint32_t len = 32;
    uint8_t rpc_session_key[RPC_KEY_SIZE] = {    0x36, 0x6d, 0x0f, 0xa2, 0x37, 0xa1, 0xb0, 0x2c, 0x95, 0xea, 0xe8, 0x21, 0xbe, 0x7b, 0x3d, 0xd5,
                                                 0xbc, 0xe7, 0xa6, 0xb9, 0xbd, 0x6d, 0x77, 0x5f, 0x3e, 0xb2, 0x56, 0x1f, 0x5c, 0x15, 0x25, 0xca
                                            };
    get_rpc_key(rpc_session_key);
    ret_code = gwpFunctions.hmac256Calculate(in_data, in_len, rpc_session_key, sizeof(rpc_session_key), hamc_data, &len);
    if(ret_code != GWP_SUCCESS)
    {
        return ret_code;
    }
    memcpy(out_data, hamc_data, out_len);
    return GWP_SUCCESS;
}


static bool verify_data(uint8_t *in_data, uint32_t in_len, uint8_t *hamc, uint8_t hmac_len)
{
    ret_code_t ret_code;
    uint8_t verify_data[4] = {0};
    ret_code = calculate_verify_data(in_data, in_len, verify_data, sizeof(verify_data));
    if(ret_code != GWP_SUCCESS)
    {
        return false;
    }
    if(memcmp(hamc, verify_data, sizeof(verify_data)) == 0)
    {
        return true;
    }
    return false;
}


/**
 * @brief
 *
 * @param[in] buf
 * @param[in] length
 * @param[in] rpc_frame
 * @return ret_code_t
 */
static ret_code_t gwp_frame_parse(uint8_t*  buf, uint32_t length, rpc_frame_t *rpc_frame)
{
    uint8_t  hmac[4] = {0};
    uint8_t *buf_tail;
    buf_tail = buf;
    if (length == 0)
    {
        return RPC_ERROR_FRAME_SIZE;
    }
    //check start sign
    if(*buf != FRAME_START)
    {
        return RPC_ERROR_FRAME_START;
    }
    buf += START_SIZE;
    //copy primitive
    rpc_frame->primitive.value = *buf;
    buf += PRIMITIVE_SIZE;
    if(rpc_frame->primitive.fields.type != FRAME_TYPE_PING)
    {
        //copy length
        memcpy(&rpc_frame->payload_length, buf, LENGTH_SIZE);
        if(rpc_frame->payload_length != (length - START_SIZE - PRIMITIVE_SIZE - LENGTH_SIZE  - VERIFY_SIZE))
        {
            return RPC_ERROR_FRAME_SIZE;
        }
        buf += LENGTH_SIZE;
        //copy payload
        if(rpc_frame->primitive.fields.crypto_enable == 1)
        {
            rpc_frame->payload_length -= IV_SIZE;
            decrypted_payload(buf + IV_SIZE, rpc_frame->payload_length, rpc_frame->u.payload, buf);
            memcpy(buf + IV_SIZE, rpc_frame->u.payload, rpc_frame->payload_length);
            buf += IV_SIZE;
        }
        else
        {
            memcpy(rpc_frame->u.payload, buf, rpc_frame->payload_length);
        }
        buf += rpc_frame->payload_length;
    }
    //verify data hmac
    memcpy(hmac, buf, sizeof(hmac));
    if(!verify_data(buf_tail, length - VERIFY_SIZE, hmac, sizeof(hmac)))
    {
        return RPC_ERROR_FRAME_VERIFY;
    }
#ifdef DEBUG_RPC_PROTOCOL
    GWP_LOG_INFO("receive decoded data :<<<<<<<<<<<<< ");
    GWP_LOG_HEXDUMP_INFO(rpc_frame->u.payload, rpc_frame->payload_length);
    NRF_LOG_FLUSH();
#endif
    return GWP_SUCCESS;
}

/**
 * @brief  encrypted data and pack data
 *
 * @param[in] rpc_frame
 * @param[in] out_data
 * @param[in] out_len
 * @return ret_code_t
 */

static ret_code_t gwp_frame_package(rpc_frame_t *rpc_frame, uint8_t *out_data, uint16_t *out_len)
{
    uint8_t *encrypted_payload_tail;
    uint8_t *tail;
    uint8_t *iv;
    uint8_t hmac[4];
    //uint8_t crypto_buf[MAX_PARAM_SIZE];
    ret_code_t ret_code;
    uint16_t length;
    tail = out_data;
    //copy start sign
    *out_data = FRAME_START;
    out_data += 1;
    *out_len += 1;
    // copy primitive id
    *out_data = rpc_frame->primitive.value;
    out_data += 1;
    *out_len += 1;
    if(rpc_frame->primitive.fields.crypto_enable)
    {
        //copy length
        length = rpc_frame->payload_length + IV_SIZE;
        memcpy(out_data, &length, sizeof(uint16_t));
        out_data += sizeof(uint16_t);
        *out_len += sizeof(uint16_t);
        // generate iv
#ifdef DEBUG_RPC_PROTOCOL
        const uint8_t fix_iv[16] = {0x51, 0xf2, 0xff, 0x13, 0x23, 0x77, 0x6a, 0x49, 0x6f, 0x36, 0x58, 0xa7, 0xd4, 0xcd, 0x99, 0xf8};
        memcpy(out_data, fix_iv, IV_SIZE);
#else
        gwpFunctions.rngVectorGenerate(out_data, IV_SIZE);
#endif /* DEBUG_RPC_PROTOCOL */
        iv = out_data;
        out_data += IV_SIZE;
        *out_len += IV_SIZE;
        //copy decrypto payload
        if(rpc_frame->payload_length <= MAX_PARAM_SIZE)
        {
            memcpy(out_data, rpc_frame->u.payload, rpc_frame->payload_length);
        }
        encrypted_payload_tail = out_data;
        out_data += rpc_frame->payload_length;
        *out_len += rpc_frame->payload_length;
        //copy hmac
        calculate_verify_data(tail, *out_len, hmac, sizeof(hmac));
#ifdef DEBUG_RPC_PROTOCOL
        print_hex(hmac, sizeof(hmac), "hmac print");
        GWP_LOG_INFO("send origin payload: \t\t\t\t>>>>>>>>>");
        GWP_LOG_HEXDUMP_INFO(encrypted_payload_tail, rpc_frame->payload_length);
        NRF_LOG_FLUSH();
#endif
        //copy encrypted payload
        ret_code = encrypted_payload(encrypted_payload_tail, rpc_frame->payload_length, rpc_frame->u.payload, iv);
        if(ret_code != GWP_SUCCESS)
        {
            return RPC_ERROR_FRAME_CRYPTO;
        }
        memcpy(out_data, hmac, sizeof(hmac));
        *out_len += sizeof(hmac);
        memcpy(encrypted_payload_tail, rpc_frame->u.payload, rpc_frame->payload_length);
    }
    else
    {
        if(rpc_frame->primitive.fields.type != FRAME_TYPE_ACK)
        {
            //copy length
            length = rpc_frame->payload_length;
            memcpy(out_data, &length, sizeof(uint16_t));
            out_data += sizeof(uint16_t);
            *out_len += sizeof(uint16_t);
            // copy payload
            if(rpc_frame->payload_length <= MAX_PARAM_SIZE)
            {
                memcpy(out_data, rpc_frame->u.payload, rpc_frame->payload_length);
            }
            out_data += rpc_frame->payload_length;
            *out_len += rpc_frame->payload_length;
        }
        //copy hmac
        calculate_verify_data(tail, *out_len, hmac, sizeof(hmac));
        memcpy(out_data, hmac, sizeof(hmac));
        *out_len += sizeof(hmac);
    }
    return GWP_SUCCESS;
}


/**
 * @brief send event to polling buffer
 *        push request into request list
 *        if request list is full response fail
 *
 * @return ret_code_t
 */
ret_code_t gwp_send_event_to_polling(uint8_t init_cmd, uint8_t sub_cmd, uint8_t *data, uint16_t length)
{
    gwp_item_header_t *item;
    uint16_t total_length;
    uint32_t ReadPos;
    uint32_t i = 0;
    if(length + 2 > (SEQ_SIZE + COMMAND_SIZE + MAX_PARAM_SIZE))
    {
        GWP_LOG_INFO("RPC_ERROR_FRAME_PARAM_SIZE");
        return RPC_ERROR_FRAME_PARAM_SIZE;
    }
    total_length = length + 9; //PDU length 4+ TYPE_LC_POLL_INDICATION 1+ length 2+ init_cmd 1+sub_cmd 1=9
    item = gwp_indication_item_reserve(total_length);
    if(item != NULL)
    {
        ReadPos = item->ReadPos;
        item->pdu_seq = 0x55555555;
        item->command = TYPE_LC_POLL_INDICATION;
        item->length = length + 2;
        /*Add payload to polling buff*/
        gwp_polling_buff[ReadPos] = init_cmd;
        ReadPos = (ReadPos + 1) % GWP_POLLING_BUFF_SIZE;
        gwp_polling_buff[ReadPos] = sub_cmd;
        ReadPos = (ReadPos + 1) % GWP_POLLING_BUFF_SIZE;
        for(i = 0; i < length; i++)
        {
            gwp_polling_buff[ReadPos] = data[i];
            ReadPos = (ReadPos + 1) % GWP_POLLING_BUFF_SIZE;
        }
        gwp_list_push_back(&gwp_reply_queue, (gwp_list_t *)item);
        return GWP_SUCCESS;
    }
    else
    {
        GWP_LOG_INFO("Polling buff full!!");
        return GWP_ERROR_NULL;
    }
}


/**
 * @brief when receive ping frame , response ACK to host device
 *
 * @param[in] rpc_frame
 * @return ret_code_t
 */

static ret_code_t gwp_type_ping_process(rpc_frame_t *rpc_frame)
{
    ret_code_t ret_code;
    uint8_t frame_data[64] = {0};
    uint16_t out_len = 0;
    rpc_frame->primitive.fields.type = FRAME_TYPE_ACK;
    ret_code = gwp_frame_package(rpc_frame, frame_data, &out_len);
    if(ret_code != GWP_SUCCESS)
    {
        return ret_code;
    }
    gwp_rpc_slip_port_send(frame_data, out_len);
    return GWP_SUCCESS;
}


/**
 * @brief when receive initial frame ,get rng pdu sequence number and response to host device
 *
 * @param[in] rpc_frame
 * @return ret_code_t
 */
extern uint8_t rpc_handshake_status;  //init done flag
static ret_code_t gwp_type_init_process(rpc_frame_t *rpc_frame)
{
    ret_code_t ret_code = GWP_SUCCESS;
    uint8_t frame_data[64] = {0};
    uint8_t result[32] = {0};
    uint16_t out_len = 0;
    uint8_t init_cmd = rpc_frame->u.payload_init.init_cmd;
    switch(init_cmd)
    {
        case INIT_CMD_SEQ:
            rpc_handshake_status = 0;
            out_len = SEQ_SIZE;
            ret_code = gwp_init_cmd_get_seq(result, &out_len);
            break;
        case INIT_CMD_CHECK_DEVICE_RANDOM:
            out_len = INIT_RANDOM_SIZE;
            ret_code = gwp_init_cmd_check_device_random(rpc_frame, result, &out_len);
            break;
        case INIT_CMD_CHECK_CERT_HASH:
            out_len = 1;
            ret_code = gwp_init_cmd_check_cert_hash(rpc_frame, result, &out_len);
            break;
        case INIT_CMD_SEND_CERT:
            out_len = 1;
            ret_code = gwp_init_cmd_send_cert(rpc_frame, result, &out_len);
            break;
        case INIT_CMD_GET_RANDOM:
            out_len = INIT_RANDOM_SIZE;
            ret_code = gwp_init_cmd_get_random(rpc_frame, result, &out_len);
            break;
        case INIT_CMD_VERIFY_HOST:
            out_len = 1;
            ret_code = gwp_init_cmd_verify_random_signature(rpc_frame, result, &out_len);
            rpc_handshake_status = 1;
            break;
        case INIT_CMD_PROBE_GWP_CAPACITY:
            out_len = 6;
            ret_code = gwp_init_cmd_probe_gwp_capacity(rpc_frame, result, &out_len);
            //rpc_handshake_status = 1;
            break;
        default:
            break;
    }
    if(ret_code != GWP_SUCCESS)
    {
        return ret_code;
    }
    rpc_frame->payload_length = SEQ_SIZE + INIT_CMD_SIZE;
    memcpy(rpc_frame->u.payload_init.payload, result, out_len);
    rpc_frame->payload_length += out_len;
#ifdef RPC_TOOL_PYTHON_SUPPORT
    out_len = 0;
#endif
    ret_code = gwp_frame_package(rpc_frame, frame_data, &out_len);
    if(ret_code != GWP_SUCCESS)
    {
        return ret_code;
    }
    gwp_rpc_slip_port_send(frame_data, out_len);
    if(rpc_handshake_status == 1)
    {
        rpc_handshake_status = 2;
        update_rpc_session_key();//calculate session key
    }
    return GWP_SUCCESS;
}



#if defined(NRF52840_XXAA)


static ret_code_t gwp_lc_frame_handle(rpc_frame_t *rpc_frame)
{
    uint16_t out_size = 0;
    uint8_t response[MAX_RESP_SIZE];
    frame_payload_command_t *payload_command = &rpc_frame->u.payload_command;
    switch(payload_command->command)
    {
        case TYPE_LC_SW_VERSION:
            gwp_rpc_func_lc_version(response, &out_size);
            break;
        case TYPE_LC_RESET_TO_FACTORY_DEFAULTS:
            gwp_rpc_func_lc_reset_factory(response, &out_size);
            break;
        case TYPE_LC_RESTART:
            gwp_rpc_func_lc_reset(response, &out_size);
            break;
        case TYPE_LC_POLL_INDICATION:
            gwp_rpc_func_lc_polling(response, &out_size);
            break;
        case TYPE_LC_FW_DL_IMAGE:
            gwp_rpc_func_lc_fw_dl_image(payload_command->data.parameter,
                                        rpc_frame->payload_length - PAYLOAD_PARAM_ADDRESS,
                                        response,
                                        &out_size);
            break;
        case TYPE_LC_FW_INSTALL:
            gwp_rpc_func_lc_fw_install(response, &out_size);
            break;
#if (defined NONE_SELF_CONTAIN)&&(defined APPLICATION_CODE )
        case TYPE_LC_DFU_REQUEST:
            gwp_rpc_func_lc_dfu_request(response, &out_size);
            break;
#else //Compile for board with external flash
        case TYPE_LC_FW_GET_DL_STATUS:
            gwp_rpc_func_lc_get_dl_status(response, &out_size);
            break;
        case TYPE_LC_GET_OTA_STATUS:
            gwp_rpc_func_lc_get_OTA_status(response, &out_size);
            break;
#endif
#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )
// bootloader without ble related code h145764
#else
        case TYPE_LC_BLE_CMD:
            gwp_rpc_func_lc_ble_cmd(payload_command->data.parameter, rpc_frame->payload_length, response, &out_size);
            break;
#endif
        default:
            break;
    }
    rpc_frame->payload_length = PAYLOAD_RESP_ADDRESS;
    payload_command->data.resp.rdi_count = gwp_indication_item_size();
    rpc_frame->payload_length += 1;
    if(out_size != 0)
    {
        memcpy(payload_command->data.resp.resp_data, response, out_size);
        rpc_frame->payload_length +=  out_size;
    }
    return GWP_SUCCESS;
}

#elif defined(STM32WB55xx)


static ret_code_t gwp_lc_frame_handle(rpc_frame_t *rpc_frame)
{
    uint16_t out_size = 0;
    uint8_t response[MAX_RESP_SIZE];
    frame_payload_command_t *payload_command = &rpc_frame->u.payload_command;
    switch(payload_command->command)
    {
        default:
            break;
    }
    rpc_frame->payload_length = PAYLOAD_RESP_ADDRESS;
    payload_command->data.resp.rdi_count = gwp_indication_item_size();
    rpc_frame->payload_length += 1;
    if(out_size != 0)
    {
        memcpy(payload_command->data.resp.resp_data, response, out_size);
        rpc_frame->payload_length +=  out_size;
    }
    return GWP_SUCCESS;
}

#elif defined(CONFIG_IDF_TARGET_ESP32S3)


static ret_code_t gwp_lc_frame_handle(rpc_frame_t *rpc_frame)
{
    uint16_t out_size = 0;
    uint8_t response[MAX_RESP_SIZE];
    frame_payload_command_t *payload_command = &rpc_frame->u.payload_command;
    switch(payload_command->command)
    {
        default:
            break;
    }
    rpc_frame->payload_length = PAYLOAD_RESP_ADDRESS;
    payload_command->data.resp.rdi_count = gwp_indication_item_size();
    rpc_frame->payload_length += 1;
    if(out_size != 0)
    {
        memcpy(payload_command->data.resp.resp_data, response, out_size);
        rpc_frame->payload_length +=  out_size;
    }
    return GWP_SUCCESS;
}


#else

#error "Architecture not set."

#endif




#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )

#else

/**
 * @brief handle host request what need to response from remote device
 *        push request into request list
 *        if request list is full response fail
 *
 * @param[in] rpc_frame
 * @return ret_code_t
 */
static ret_code_t gwp_rm_frame_handle(rpc_frame_t *rpc_frame)
{
    gwp_item_header_t *item;
    uint32_t ReadPos;
    uint16_t i = 0;
    frame_payload_command_t *payload_command = &rpc_frame->u.payload_command;
    //reserve  the max length for RM package
    item = gwp_indication_item_reserve(rpc_frame->payload_length + GWP_RM_MAX_PAYLOAD);
    if(item != NULL)
    {
        ReadPos = item->ReadPos;
        item->start_time = 0;
        item->pdu_seq = payload_command->pdu_seq;
        item->command = payload_command->command;
        if(rpc_frame->payload_length > (SEQ_SIZE + COMMAND_SIZE + MAX_PARAM_SIZE))
        {
            return RPC_ERROR_FRAME_PARAM_SIZE;
        }
        item->length = rpc_frame->payload_length - SEQ_SIZE - COMMAND_SIZE;
        ReadPos = (ReadPos + GWP_ITEM_PAYLOAD) % GWP_POLLING_BUFF_SIZE;
        for(i = 0; i < item->length; i++)
        {
            gwp_polling_buff[ReadPos] = payload_command->data.parameter[i];
            ReadPos = (ReadPos + 1) % GWP_POLLING_BUFF_SIZE;
        }
        gwp_list_push_back(&gwp_rm_req_queue, (gwp_list_t *)item);
        payload_command->data.resp.resp_data[0] = SUCCESS;
    }
    else
    {
        payload_command->data.resp.resp_data[0]   = FAIL;
    }
    payload_command->data.resp.rdi_count = gwp_indication_item_size();
    rpc_frame->payload_length = SEQ_SIZE + COMMAND_SIZE + RDI_SIZE + 1;
    return GWP_SUCCESS;
}

#endif



/**
 * @brief data type process
 *          1. check pdu_seq
 *          2.handle remote or local data
 *          3.pack data to frame
 *          4.send host ack
 *
 * @param[in] rpc_frame
 * @return ret_code_t
 */
extern int OTA_install_Flag;
static ret_code_t gwp_type_data_process(rpc_frame_t *rpc_frame)
{
    uint8_t frame_data[MAX_PARAM_SIZE] = {0};
    uint8_t command = 0;
    ret_code_t ret_code;
    uint16_t out_len = 0;
    frame_payload_command_t *payload_command = &rpc_frame->u.payload_command;
    if(gwp_rpc.rpc_state != RPC_STATE_START)
    {
        GWP_LOG_INFO("RPC_ERROR_FRAME_TYPE");
        return RPC_ERROR_FRAME_TYPE;
    }
    // check seq
    if(gwp_rpc.pdu_seq != payload_command->pdu_seq)
    {
        GWP_LOG_INFO("RPC_ERROR_FRAME_SEQ");
        return RPC_ERROR_FRAME_SEQ;
    }
    gwp_rpc.pdu_seq ++;
    command = payload_command->command;
#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )
    if(gwp_is_lc_request(command))
#else
    if(gwp_is_rm_request(command))
    {
        gwp_rm_frame_handle(rpc_frame);
    }
    else if(gwp_is_lc_request(command))
#endif
    {
        gwp_lc_frame_handle(rpc_frame);
    }
    else
    {
        return RPC_ERROR_FRAME_FUNC;
    }
    ret_code = gwp_frame_package(rpc_frame, frame_data, &out_len);
    if(ret_code != GWP_SUCCESS)
    {
        return ret_code;
    }
    gwp_rpc_slip_port_send(frame_data, out_len);
#if defined(NRF52840_XXAA)
    if(OTA_install_Flag == 1)
    {
#ifdef RPC_APPLICATION_CODE
        vTaskDelay(pdMS_TO_TICKS(100));
#else
//todo: h145764 add delay
#endif
        OTA_install_Flag = 0;
        NVIC_SystemReset();
    }
#elif defined(STM32WB55xx)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#else
#error "Architecture not set."
#endif
    return GWP_SUCCESS;
}

/**
 * @brief check frame type ,execute init or data process
 *
 * @param[in] rpc_frame
 * @return ret_code_t
 */

static ret_code_t gwp_rpc_frame_handle(rpc_frame_t *rpc_frame)
{
    ret_code_t ret = GWP_SUCCESS;
    switch(rpc_frame->primitive.fields.type)
    {
        case FRAME_TYPE_ACK:
            //gwp_type_ack_process(rpc_frame);
            break;
        case FRAME_TYPE_DATA:
            gwp_type_data_process(rpc_frame);
            break;
        case FRAME_TYPE_INIT:
            gwp_type_init_process(rpc_frame);
            break;
        case FRAME_TYPE_CFG:
            break;
        case FRAME_TYPE_PING:
            gwp_type_ping_process(rpc_frame);
            break;
        default:
            ret = RPC_ERROR_FRAME_TYPE;
            break;
    }
    return ret;
}
/**
 * @brief Set the crypto enable object
 *       encryption layer is used, can be set only/ no reset supported
 * @param[in] primitive
 */
static void set_crypto_enable(frame_primitive_t *primitive)
{
    if(gwp_rpc.crypto_enable)
    {
        primitive->fields.crypto_enable = 1;
    }
    return;
}

/**
 * @brief receive data ,parse frame and handle frame
 *
 * @param[in] data
 * @return ret_code_t
 */
static ret_code_t gwp_rpc_data_handle(uint8_t *data, uint16_t length)
{
    ret_code_t ret_code = GWP_SUCCESS;
    rpc_frame_t rpc_frame;
    uint16_t i;
//    GWP_LOG_INFO("5555555555");
//    NRF_LOG_FLUSH();
    for(i = 0; i < length; i++)
    {
        ret_code = gwp_slip_decode_add_byte(&m_slip, data[i]);
        if (ret_code != GWP_SUCCESS)
        {
            continue;
        }
        memset(&rpc_frame, 0, sizeof(rpc_frame));
        ret_code = gwp_frame_parse((uint8_t *)(m_slip.p_buffer), m_slip.current_index, &rpc_frame);
        gwp_slip_reset(&m_slip);
        if (ret_code != GWP_SUCCESS)
        {
            continue;
        }
        set_crypto_enable(&rpc_frame.primitive);
        ret_code = gwp_rpc_frame_handle(&rpc_frame);
        if (ret_code != GWP_SUCCESS)
        {
            continue;
        }
    }
    return GWP_SUCCESS;
}





bool check_is_pduseq(gwp_item_header_t * item, uint32_t seq)
{
#if 0
    if(item->pdu_seq == seq)
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    return true;
#endif
}


#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )

#else



#ifndef CONFIG_IDF_TARGET_ESP32S3



/**
 * @brief  execute remote request (such as send data using ble)
 *
 * @param[in] item
 * @return ret_code_t
 */

static ret_code_t send_rm_request( gwp_item_header_t *item)
{
    ret_code_t ret_code = GWP_SUCCESS;
    uint32_t ReadPos;
    uint8_t i, address[ADDRESS_SIZE];
    uint8_t status;
    ReadPos = item->ReadPos;
    for(i = 0; i < ADDRESS_SIZE; i++)
    {
        address[i] = gwp_polling_buff[(ReadPos + PARAM_RM_VERISON_ADDRESS_6 + i) % GWP_POLLING_BUFF_SIZE];
    }
    switch(item->command)
    {
        case TYPE_RM_SW_VERSION:
            ret_code = gwp_rpc_func_rm_version(address);
            break;
        case TYPE_RM_RELAY_CTL:
            status = gwp_polling_buff[(ReadPos + RESPOND_RM_RELAY_CTL_STATUS_1) % GWP_POLLING_BUFF_SIZE];
            gwp_rpc_func_rm_relay_ctl(address, status);
            break;
        case TYPE_RM_SWITCH_LOCK:
            status = gwp_polling_buff[(ReadPos + PARAM_RM_SWITCH_LOCK_STATUS_1) % GWP_POLLING_BUFF_SIZE];
            ret_code = gwp_rpc_func_rm_switch_lock(address, status);
            break;
        case TYPE_RM_GATT_CTL:
            status = gwp_polling_buff[(ReadPos + PARAM_RM_GATT_CTL_STATUS_1) % GWP_POLLING_BUFF_SIZE];
            ret_code = gwp_rpc_func_rm_gatt_ctl(address, status);
            break;
        case TYPE_RM_READ_SM_PARAM:
        {
            uint8_t type = gwp_polling_buff[(ReadPos + PARAM_RM_READ_SM_TYPE_1) % GWP_POLLING_BUFF_SIZE];
            ret_code = gwp_rpc_func_rm_read_sm_param(address, type);
        }
        break;
        case TYPE_RM_SET_SM_ALARM_CFG:
        {
            uint8_t i, value[4];
            uint8_t cfg = gwp_polling_buff[(ReadPos + PARAM_RM_SET_SM_ALARM_CFG_1) % GWP_POLLING_BUFF_SIZE];
            for(i = 0; i < 4; i++)
            {
                value[i] = gwp_polling_buff[(ReadPos + PARAM_RM_SET_SM_ALARM_VALUE_4 + i) % GWP_POLLING_BUFF_SIZE];
            }
            ret_code = gwp_rpc_func_rm_set_sm_alarm_cfg(address, cfg, value);
        }
        break;
        case TYPE_RM_RESET_TO_FACTORY_DEFAULTS:
            ret_code = gwp_rpc_func_rm_reset_to_factory(address);
            break;
        case TYPE_RM_RESTART:
            ret_code = gwp_rpc_func_rm_restart(address);
            break;
        case TYPE_RM_FW_DL_IMAGE:
        {
            uint8_t fw_pid = gwp_polling_buff[(ReadPos + PARAM_RM_SET_SM_ALARM_CFG_1) % GWP_POLLING_BUFF_SIZE];
            uint16_t length = (uint16_t)gwp_polling_buff[(ReadPos + PARAM_RM_FW_DL_IMAGE_LENGTH_2) % GWP_POLLING_BUFF_SIZE] +
                              (uint16_t)((uint16_t)gwp_polling_buff[(ReadPos + PARAM_RM_FW_DL_IMAGE_LENGTH_2) % GWP_POLLING_BUFF_SIZE] << 8);
            uint8_t i, payload[256];
            for(i = 0; i < length; i++)
            {
                payload[i] = gwp_polling_buff[(ReadPos + PARAM_RM_FW_DL_IMAGE_PAYLOAD_MAX + i) % GWP_POLLING_BUFF_SIZE];
            }
            ret_code = gwp_rpc_func_rm_fw_dl_image(fw_pid, length, payload);
        }
        break;
        case TYPE_RM_TYPE_INFO:
            ret_code = gwp_rpc_func_rm_type_info(address);
            break;
        default:
            ret_code = RPC_ERROR_COMMAND_RM_TYPE;
            break;
    }
    return ret_code;
}
/**
 * @brief  request finish , pack data on the basis of design document,push in reply list
 *
 * @param[in] item
 * @param[in] status :SUCCESS(1) FAIL(0)
 * @param[in] context :data
 */

static void rm_request_finish(gwp_item_header_t *item, uint8_t status, uint8_t *context)
{
    uint16_t i;
    uint32_t start_time, ReadPos;
    ReadPos = item->ReadPos;
    switch(item->command)
    {
        case TYPE_RM_SW_VERSION:
            start_time = (uint32_t)(ticks_diff_get(rtc1_counter_get(), item->start_time) * 30.517);
            for(i = 0; i < 4; i++)
            {
                gwp_polling_buff[(ReadPos + RESPOND_RM_VERISON_TRAVEL_TIME_4 + i) % GWP_POLLING_BUFF_SIZE] = start_time % 0x100;
                start_time = (start_time >> 8);
            }
            item->length = ADDRESS_SIZE + TRAVEL_TIME_SIZE;
            if(status == SUCCESS && context != NULL)
            {
                for(i = 0; i < 33; i++)
                {
                    gwp_polling_buff[(ReadPos + RESPOND_RM_VERISON_VERSION_33 + i) % GWP_POLLING_BUFF_SIZE] = context[i];
                }
                item->length += 33;
            }
            break;
        case TYPE_RM_RELAY_CTL:
        case TYPE_RM_SWITCH_LOCK:
        case TYPE_RM_GATT_CTL:
        case TYPE_RM_SET_SM_ALARM_CFG:
        case TYPE_RM_RESET_TO_FACTORY_DEFAULTS:
        case TYPE_RM_RESTART:
        case TYPE_RM_FW_DL_IMAGE:
            gwp_polling_buff[(ReadPos + RESPOND_RM_RELAY_CTL_STATUS_1) % GWP_POLLING_BUFF_SIZE] = status;
            start_time = (uint32_t)(ticks_diff_get(rtc1_counter_get(), item->start_time) * 30.517);
            for(i = 0; i < 4; i++)
            {
                gwp_polling_buff[(ReadPos + RESPOND_RM_RELAY_CTL_TRAVEL_TIME_4 + i) % GWP_POLLING_BUFF_SIZE] = start_time % 0x100;
                start_time = (start_time >> 8);
            }
            item->length = ADDRESS_SIZE + TRAVEL_TIME_SIZE + 1;
            break;
        case TYPE_RM_READ_SM_PARAM:
            gwp_polling_buff[(ReadPos + RESPOND_RM_RELAY_CTL_STATUS_1) % GWP_POLLING_BUFF_SIZE] = status;
            start_time = (uint32_t)(ticks_diff_get(rtc1_counter_get(), item->start_time) * 30.517);
            for(i = 0; i < 4; i++)
            {
                gwp_polling_buff[(ReadPos + RESPOND_RM_RELAY_CTL_TRAVEL_TIME_4 + i) % GWP_POLLING_BUFF_SIZE] = start_time % 0x100;
                start_time = (start_time >> 8);
            }
            item->length = ADDRESS_SIZE + TRAVEL_TIME_SIZE + 1;
            if(status == SUCCESS && context != NULL)
            {
                for(i = 0; i < 4; i++)
                {
                    gwp_polling_buff[(ReadPos + RESPOND_RM_VERISON_VERSION_33 + i) % GWP_POLLING_BUFF_SIZE] = context[i];
                }
                item->length += 4;
            }
            break;
        case TYPE_RM_TYPE_INFO:
            gwp_polling_buff[(ReadPos + RESPOND_RM_RELAY_CTL_STATUS_1) % GWP_POLLING_BUFF_SIZE] = status;
            start_time = (uint32_t)(ticks_diff_get(rtc1_counter_get(), item->start_time) * 30.517);
            for(i = 0; i < 4; i++)
            {
                gwp_polling_buff[(ReadPos + RESPOND_RM_RELAY_CTL_TRAVEL_TIME_4 + i) % GWP_POLLING_BUFF_SIZE] = start_time % 0x100;
                start_time = (start_time >> 8);
            }
            item->length = ADDRESS_SIZE + TRAVEL_TIME_SIZE + 1;
            if(status == SUCCESS && context != NULL)
            {
                for(i = 0; i < 7; i++)
                {
                    gwp_polling_buff[(ReadPos + RESPOND_RM_VERISON_VERSION_33 + i) % GWP_POLLING_BUFF_SIZE] = context[i];
                }
                item->length += 7;
            }
            break;
        default:
            break;
    }
    gwp_list_push_back(&gwp_reply_queue, (gwp_list_t *)item);
}

/**
 * @brief   1.pop front from request list ,
 *          2.send request to remote device,
 *          3.when timeout(TIME_OUT 1s),resend data
 *          4.when resend count is more then RESEND_TIMES (3),reply  fail
 *
 */
static void gwp_rm_request_process()
{
    ret_code_t ret_code  = GWP_SUCCESS;
    gwp_item_header_t *send_item = NULL;
    gwp_list_t *list = (gwp_list_t *)&gwp_rm_send_queue;
    //re-send  timeout rm request frame
    if(gwp_list_next(list) != NULL)
    {
        list = gwp_list_next(list);
        send_item = (gwp_item_header_t *)list;
        while(list != NULL)
        {
            send_item = (gwp_item_header_t *)list;
            if(ticks_diff_get(rtc1_counter_get(), send_item->last_time) * 30.517 > TIME_OUT)
            {
                send_item->last_time =  rtc1_counter_get();
                send_item->count++;
            }
            else
            {
                list = gwp_list_next((gwp_list_t *)send_item);
                continue;
            }
            if(send_item->count >= RESEND_TIMES)
            {
                list = gwp_list_next((gwp_list_t *)send_item);
                gwp_list_remove(&gwp_rm_send_queue, (gwp_list_t *)send_item);
                rm_request_finish(send_item, FAIL, NULL);
            }
            else
            {
                ret_code = send_rm_request(send_item);
                if(ret_code != GWP_SUCCESS)
                {
                    //todo
                }
                list = gwp_list_next((gwp_list_t *)send_item);
                continue;
            }
        }
    }
    //send rm request frame first
    while(gwp_list_size(&gwp_rm_req_queue) > 0)
    {
        send_item = (gwp_item_header_t *)gwp_list_pop_front(&gwp_rm_req_queue);
        if(send_item == NULL)
        {
            break;
        }
        send_item->count = 0;
        send_item->start_time = rtc1_counter_get();
        send_item->last_time =  rtc1_counter_get();
        ret_code = send_rm_request(send_item);
        if(ret_code != GWP_SUCCESS)
        {
            //todo
        }
        gwp_list_push_back(&gwp_rm_send_queue, (gwp_list_t *)send_item);
    }
}



/**
 * @brief test function
 *
 */
static void gwp_receive_ble_data_test(uint32_t pdu_seq)
{
    uint8_t buf[128] = {0};
    uint16_t out_size;
    gwp_item_header_t *send_item = NULL;
    while(gwp_list_size(&gwp_rm_send_queue) > 0)
    {
        send_item = (gwp_item_header_t *)gwp_list_pop_front(&gwp_rm_send_queue);
        if(send_item == NULL)
        {
            break;
        }
        if(!check_is_pduseq(send_item, pdu_seq))
        {
            continue;
        }
        if(send_item != NULL)
        {
            switch(send_item->command)
            {
                case TYPE_RM_SW_VERSION:
                    gwp_rpc_func_lc_version(buf, &out_size);
                    rm_request_finish(send_item, SUCCESS, buf);
                    break;
                case TYPE_RM_RELAY_CTL:
                case TYPE_RM_SWITCH_LOCK:
                case TYPE_RM_GATT_CTL:
                case TYPE_RM_SET_SM_ALARM_CFG:
                case TYPE_RM_RESET_TO_FACTORY_DEFAULTS:
                case TYPE_RM_RESTART:
                case TYPE_RM_FW_DL_IMAGE:
                case TYPE_RM_GET_OTA_STATUS:
                    rm_request_finish(send_item, SUCCESS, NULL);
                    break;
                case TYPE_RM_READ_SM_PARAM:
                    rm_request_finish(send_item, SUCCESS, buf);
                    break;
                case TYPE_RM_TYPE_INFO:
                    buf[0] = 2;
                    rm_request_finish(send_item, SUCCESS, buf);
                    break;
                default:
                    break;
            }
        }
    }
}


#endif
#endif

/**
 * @brief gwp remote protocol communication initial
 *        include : 1.request memory space for rpc
 *                  2.slip initial
 *                  3.request list initial (used for save host request what need get response from remote device)
 *                  4.reply list initial (when receive remote device response ,save in it,
 *                   and get data from it when host device poll indication )
 *                  5.crypto initial
 *
 * @return ret_code_t
 */


#if defined(NRF52840_XXAA)


extern ota_status_t ota_status;
ret_code_t gwp_rpc_init()
{
    ret_code_t ret_code = GWP_SUCCESS;
    gwp_indication_item_init();
    gwp_slip_init(&m_slip);
    gwp_list_init(&gwp_rm_req_queue);
    gwp_list_init(&gwp_reply_queue);
    gwp_list_init(&gwp_rm_send_queue);
    ota_storage_init(&ota_status.ota_process);
    ret_code = gwp_rpc_port_init();
#ifdef CONFIG_GWP_CRYPTO_SUPPORT
    if(!nrf_crypto_is_initialized())
    {
        ret_code = nrf_crypto_init();
        if(ret_code != GWP_SUCCESS)
        {
            return ret_code;
        }
    }
#endif
    return ret_code;
}


#elif defined(STM32WB55xx) || defined(CONFIG_IDF_TARGET_ESP32S3)


//extern ota_status_t ota_status;
ret_code_t gwp_rpc_init()
{
    ret_code_t ret_code = GWP_SUCCESS;
    gwp_indication_item_init();
    gwp_slip_init(&m_slip);
    gwp_list_init(&gwp_rm_req_queue);
    gwp_list_init(&gwp_reply_queue);
    gwp_list_init(&gwp_rm_send_queue);
//    ota_storage_init(&ota_status.ota_process); rpcPortInit .rpcPortReceive
    ret_code = gwpFunctions.rpcPortInit();
#ifdef CONFIG_GWP_CRYPTO_SUPPORT
    if(!nrf_crypto_is_initialized())
    {
        ret_code = nrf_crypto_init();
        if(ret_code != GWP_SUCCESS)
        {
            return ret_code;
        }
    }
#endif
    return ret_code;
}


#else

#error "Architecture not set."

#endif


#if (((defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE ))||(defined STM32WB55xx)) || defined(CONFIG_IDF_TARGET_ESP32S3)
uint8_t rpc_data_buffer[MAX_PAYLOAD_SIZE];
volatile uint8_t flag_data_receive = 0;
extern uint32_t timeout_rpc;

/**
 * @brief gwp rpc start include 1.receive data from uart
 *                              2.handle data
 *                              3.pop front request list and send request to remote device
 *
 * @return ret_code_t
 */

ret_code_t gwp_rpc_run()
{
    //uint8_t data[MAX_PAYLOAD_SIZE];
    uint16_t length = 0;
    while(flag_data_receive == 0)
    {
        if(timeout_rpc == 0)
        {
            break;
        }
    }
    if (flag_data_receive == 1)
    {
        //gwpFunctions.rngVectorGenerate
        gwpFunctions.rpcPortReceive(rpc_data_buffer, &length);
        if(length != 0)
        {
            gwp_rpc_data_handle(rpc_data_buffer, length);
            //GWP_LOG_INFO("111222");
            //NRF_LOG_FLUSH();
        }
    }
    flag_data_receive = 0;
    return GWP_SUCCESS;
}

#else

/**
 * @brief gwp rpc start include 1.receive data from uart
 *                              2.handle data
 *                              3.pop front request list and send request to remote device
 *
 * @return ret_code_t
 */
extern void stack_heap_error(void);
ret_code_t gwp_rpc_run()
{
    //uint8_t data[MAX_PAYLOAD_SIZE];
    uint16_t length = 0;
    uint8_t *data = NULL;
    data = gwp_malloc(MAX_PAYLOAD_SIZE);
    if (data == NULL)
    {
        GWP_LOG_ERROR("GWP malloc error3");
        stack_heap_error();
    }
    //gwpFunctions.rngVectorGenerate
    gwpFunctions.rpcPortReceive(data, &length);
    if(length != 0)
    {
        gwp_rpc_data_handle(data, length);
    }
    gwp_rm_request_process();
    gwp_receive_ble_data_test(1);
    gwp_free(data);
    return GWP_SUCCESS;
}

#endif



