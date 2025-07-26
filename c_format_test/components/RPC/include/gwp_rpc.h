#ifndef __GWP_RPC_H__
#define __GWP_RPC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "gwp_common.h"
#include "gwp_list.h"

#define MAX_PARAM_SIZE          2300//256
#define MAX_RESP_SIZE           2300//256  //ble receive 2155+GWP packet++ble polling packet =2295bytes

#define MAX_PAYLOAD_SIZE        (MAX_PARAM_SIZE + SEQ_SIZE + COMMAND_SIZE)

#define FRAME_START                 0xE9

#define START_SIZE                  1
#define PRIMITIVE_SIZE              1
#define LENGTH_SIZE                 2
#define IV_SIZE                     16
#define SEQ_SIZE                    4
#define COMMAND_SIZE                1
#define ADDRESS_SIZE                6
#define VERIFY_SIZE                 4
#define RDI_SIZE                    1

#define VERSION_SIZE                33
#define TRAVEL_TIME_SIZE            4




#define TIME_OUT                    1000000
#define RESEND_TIMES                3

#define PAYLOAD_SEQ_ADDRESS                     0
#define PAYLOAD_SEQ_SIZE                        SEQ_SIZE
#define PAYLOAD_COMMAND_ADDRESS                 (PAYLOAD_SEQ_ADDRESS + PAYLOAD_SEQ_SIZE)
#define PAYLOAD_COMMAND_SIZE                    COMMAND_SIZE
#define PAYLOAD_PARAM_ADDRESS                   (PAYLOAD_COMMAND_ADDRESS + PAYLOAD_COMMAND_SIZE)
#define PAYLOAD_RESP_ADDRESS                    (PAYLOAD_COMMAND_ADDRESS + PAYLOAD_COMMAND_SIZE)

#define INIT_CMD_SIZE                           1
#define INIT_RANDOM_SIZE                        4
#define INIT_HASH_SIZE                          32
#define INIT_SIGNATURE_SIZE                     64


#define FRAME_TYPE_ACK                          0
#define FRAME_TYPE_DATA                         1
#define FRAME_TYPE_INIT                         2
#define FRAME_TYPE_CFG                          3
#define FRAME_TYPE_PING                         4


#define INIT_CMD_SEQ                            1
#define INIT_CMD_CHECK_DEVICE_RANDOM            2
#define INIT_CMD_CHECK_CERT_HASH                3
#define INIT_CMD_SEND_CERT                      4
#define INIT_CMD_GET_RANDOM                     5
#define INIT_CMD_VERIFY_HOST                    6
#define INIT_CMD_PROBE_GWP_CAPACITY             7


typedef enum
{
    INIT_SEQ_NUM  = 1,
    INIT_PROBE_CAP,
    INIT_CHECK_DEVICE,
    INIT_CHECK_CERT_HASH,
    INIT_SEND_CERT,
    INIT_GET_RANDOM,
    INIT_VERIFY_HOST,
} rpc_init_verify_t;



#define TYPE_LC_SW_VERSION                      0
#define TYPE_RM_SW_VERSION                      1
#define TYPE_RM_RELAY_CTL                       2
#define TYPE_RM_SWITCH_LOCK                     3
#define TYPE_RM_GATT_CTL                        4
#define TYPE_RM_READ_SM_PARAM                   5
#define TYPE_RM_SET_SM_ALARM_CFG                6
#define TYPE_LC_RESET_TO_FACTORY_DEFAULTS       7
#define TYPE_RM_RESET_TO_FACTORY_DEFAULTS       8
#define TYPE_LC_RESTART                         9
#define TYPE_RM_RESTART                         10
#define TYPE_LC_POLL_INDICATION                 11

#define TYPE_LC_FW_DL_IMAGE                     14
#define TYPE_RM_FW_DL_IMAGE                     15
#define TYPE_LC_FW_INSTALL                      16
#define TYPE_LC_DFU_REQUEST                     17
#define TYPE_LC_FW_GET_DL_STATUS                18
#define TYPE_RM_TYPE_INFO                       19
#define TYPE_RM_GET_OTA_STATUS                  20
#define TYPE_LC_GET_OTA_STATUS                  21
#define TYPE_LC_BLE_CMD                         22

//sub command for TYPE_LC_POLL_INDICATION
#define TYPE_IND_ALARM                          0
#define TYPE_IND_REPORT                         1
#define TYPE_IND_SYSTEM                         2
#define TYPE_IND_BLE                            3

//sub command for TYPE_LC_BLE_CMD
typedef enum
{
    LE_ADV  = 0,
    LE_CONNECT,
    LE_CONNECTCANCEL,
    LE_DISCONNECT,
    LE_DISCOVERY,
    LE_SCAN,
    LE_FUNCTION,
    LE_WRITEDATA,
    LE_SIGNRESP,
} BLE_Subcommand;

//Sub_TYPE_IND_BLE
typedef enum
{
    LE_CONNECTION_UP  = 0,
    LE_CONNECTION_DOWN,
    LE_DATA_RECEIVE,
    LE_SCAN_EVENTS,
    LE_DISCOVERY_EVENTS,
    LE_DATA_SEND_STATUS,
    LE_CONNECTION_FAIL,
    LE_SCAN_TIMEOUT,
    LE_REQUEST_HOST_SIGN,
} BLE_SubInd;

typedef enum
{
    LE_ADV_Disable = 0, //false,
    LE_ADV_Enable  = 1, //true,
} BLE_ADV_Switch;

//TYPE_IND_SYSTEM
#define IND_OTA_STATUS                          1

#define LC_REQUESTS                             \
    {                                               \
        TYPE_LC_SW_VERSION,                         \
        TYPE_LC_RESET_TO_FACTORY_DEFAULTS,          \
        TYPE_LC_RESTART,                            \
        TYPE_LC_POLL_INDICATION,                    \
        TYPE_LC_FW_DL_IMAGE,                        \
        TYPE_LC_FW_INSTALL,                         \
        TYPE_LC_DFU_REQUEST,                        \
        TYPE_LC_FW_GET_DL_STATUS,                   \
        TYPE_LC_GET_OTA_STATUS,                     \
        TYPE_LC_BLE_CMD,                            \
    }

#define RM_REQUESTS                             \
    {                                               \
        TYPE_RM_SW_VERSION,                         \
        TYPE_RM_RELAY_CTL,                          \
        TYPE_RM_SWITCH_LOCK,                        \
        TYPE_RM_GATT_CTL,                           \
        TYPE_RM_READ_SM_PARAM,                      \
        TYPE_RM_SET_SM_ALARM_CFG,                   \
        TYPE_RM_RESET_TO_FACTORY_DEFAULTS,          \
        TYPE_RM_RESTART,                            \
        TYPE_RM_FW_DL_IMAGE,                        \
        TYPE_RM_TYPE_INFO,                          \
        TYPE_RM_GET_OTA_STATUS,                     \
    }



#if defined(NRF52840_XXAA)

    #define     SUCCESS                             0

#elif defined(STM32WB55xx)

#elif defined(CONFIG_IDF_TARGET_ESP32S3)


#else

    #error "Architecture not set."

#endif


#define     FAIL                                1


typedef union
{
    struct
    {
        uint8_t seq                 : 2;
        uint8_t type                : 3;
        uint8_t crypto_enable       : 1;
        uint8_t direction_flag      : 1;
        uint8_t reserve             : 1;
    } fields;
    uint8_t value;
} frame_primitive_t;





typedef struct
{
    uint32_t pdu_seq;
    uint8_t command;
    union
    {
        struct
        {
            uint8_t rdi_count;
            uint8_t resp_data[MAX_PAYLOAD_SIZE];
        } resp;
        uint8_t parameter[MAX_PAYLOAD_SIZE];
    } data;
} frame_payload_command_t;


typedef struct
{
    uint8_t sub_command;
    uint8_t data[MAX_PAYLOAD_SIZE];
} frame_le_command_t;


typedef struct
{
    uint32_t pdu_seq;
    uint8_t init_cmd;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} frame_payload_init_t;

typedef struct
{
    frame_primitive_t primitive;

    uint16_t payload_length;

    union
    {
        frame_payload_init_t payload_init;

        frame_payload_command_t payload_command;

        uint8_t payload[MAX_PAYLOAD_SIZE];
    } u;
} rpc_frame_t;


typedef enum
{
    RPC_STATE_INIT = 1,
    RPC_STATE_CONFIG,
    RPC_STATE_START
} rpc_state_t;


typedef struct
{
    rpc_init_verify_t   rpc_init_status;
    rpc_state_t  rpc_state;
    bool    crypto_enable;
    uint32_t     pdu_seq;
} gwp_rpc_t;


#define GWP_RPC_INSTANCE() \
    {                                 \
        .rpc_state  =   RPC_STATE_INIT,               \
                        .rpc_init_status    =       INIT_SEQ_NUM,   \
                                .crypto_enable = false,            \
                                        .pdu_seq    =   0,           \
    }

extern gwp_list_head_t          gwp_rm_req_queue;
extern gwp_list_head_t          gwp_reply_queue;

ret_code_t gwp_rpc_init();

ret_code_t gwp_rpc_run();

ret_code_t gwp_send_event_to_polling(uint8_t init_cmd, uint8_t sub_cmd, uint8_t *data, uint16_t length);

#endif // !__GWP_RPC_H__
