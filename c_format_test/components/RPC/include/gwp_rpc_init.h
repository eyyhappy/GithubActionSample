#ifndef _GWP_RPC_INIT_H_
#define _GWP_RPC_INIT_H_

#include "gwp_rpc.h"

#define MAX_CERT_SIZE                           2048


#define PAYLOAD_CERT_COUNT_OFFSET               0
#define PAYLOAD_CERT_COUNT_SIZE                 1
#define PAYLOAD_CERT_SEQ_OFFSET                 (PAYLOAD_CERT_COUNT_OFFSET + PAYLOAD_CERT_COUNT_SIZE)
#define PAYLOAD_CERT_SEQ_SIZE                   1
#define PAYLOAD_CERT_DATA_OFFSET                (PAYLOAD_CERT_SEQ_OFFSET + PAYLOAD_CERT_SEQ_SIZE)


typedef struct
{
    uint8_t cert_payload[MAX_CERT_SIZE];
    uint16_t cert_size;
    uint8_t cert_count;
    uint8_t cert_seq;
} cert_info_t;



typedef struct
{
    uint8_t gwp_auth_cap;
    uint8_t gwp_wireless_cap;
    uint8_t gwp_rpc_version_major;
    uint8_t gwp_rpc_version_minor;
    uint8_t run_stage;//0: bootloader  1:application
    uint8_t self_contain;// 0:no external flash 1:has external flash
} rpc_info_t;

typedef struct
{
//    rpc_info_t rpc_info;
    uint8_t firmware_ver[4];
    uint8_t hardware_ver[2];
    //uint8_t version_str[33];
    const uint8_t sw_version_desc[VERSION_SIZE];
} rpc_instance_t;
extern const rpc_instance_t ins_rpc;
extern gwp_rpc_t gwp_rpc;

ret_code_t gwp_init_cmd_get_seq(uint8_t * result, uint16_t *out_len);

ret_code_t gwp_init_cmd_check_device_random(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len);

ret_code_t gwp_init_cmd_check_cert_hash(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len);

ret_code_t gwp_init_cmd_send_cert(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len);

ret_code_t gwp_init_cmd_get_random(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len);

ret_code_t gwp_init_cmd_verify_random_signature(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len);

ret_code_t gwp_init_cmd_probe_gwp_capacity(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len);
#endif