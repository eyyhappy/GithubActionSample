#ifndef _GWP_RPC_INDICATION_H_
#define _GWP_RPC_INDICATION_H_

#include "gwp_rpc.h"


typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
}
param_rm_version_t;


typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
    uint8_t status;
}
param_rm_relay_ctl_t;

typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
    uint8_t status;
}
param_rm_switch_lock_t;


typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
    uint8_t status;
}
param_rm_gatt_ctl_t;


typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
    uint8_t type;
}
param_rm_read_sm_param_t;

typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
    uint8_t cfg;
    uint8_t value[4];
}
param_rm_set_sm_alarm_t;

typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
}
param_rm_reset_to_factory_t;


typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
}
param_rm_restart_t;


typedef struct __attribute__ ((__packed__))
{
    uint8_t fw_pid;
    uint16_t length;
    uint8_t payload[MAX_PARAM_SIZE - 3];
}
param_rm_fw_dl_image_t;


typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
}
param_rm_type_info_t;


typedef struct __attribute__ ((__packed__))
{
    uint8_t ind_ble;
    uint8_t ind_ble_sub;
    uint8_t payload[MAX_PARAM_SIZE - 2];
}
param_polling_ind_ble_t;

typedef union
{
    uint8_t param[MAX_PARAM_SIZE];
    param_rm_version_t rm_version;
    param_rm_relay_ctl_t rm_relay_ctl;
    param_rm_switch_lock_t rm_switch_lock;
    param_rm_gatt_ctl_t rm_gatt_ctl;
    param_rm_read_sm_param_t rm_read_sm_param;
    param_rm_set_sm_alarm_t rm_set_sm_alarm;
    param_rm_reset_to_factory_t rm_reset_to_fac;
    param_rm_restart_t rm_restart;
    param_rm_fw_dl_image_t rm_fw_dl_image;
    param_rm_type_info_t rm_type_info;
    param_polling_ind_ble_t polling_ind_ble;
} gwp_param_t;



typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
    uint32_t travel_time;
    uint8_t version[33];
}
resp_rm_version_t;

typedef struct __attribute__ ((__packed__))
{
    uint8_t address[ADDRESS_SIZE];
    uint32_t travel_time;
    uint8_t status;
}
resp_rm_relay_ctl_t;




typedef union
{
    uint8_t resp[MAX_RESP_SIZE];
    resp_rm_version_t rm_version;
    resp_rm_relay_ctl_t rm_relay_ctl;
} gwp_resp_t;


/*
typedef struct {
    //gwp_list_t           list;
    //uint32_t start_time;
    //uint32_t pdu_seq;
    //uint8_t count;
    //uint32_t last_time;
    //uint8_t command;
    //uint16_t length;
    union{
        gwp_param_t parameter;
        gwp_resp_t respond;
    }u;
}gwp_item_t;*/
//gwp_item_t Header
#define GWP_ITEM_PAYLOAD                     0

/*******Payload Param*****/
//param_rm_version_t
#define PARAM_RM_VERISON_ADDRESS_6              0

//param_rm_switch_lock_t
#define PARAM_RM_SWITCH_LOCK_ADDRESS_6        0
#define PARAM_RM_SWITCH_LOCK_STATUS_1         6

//param_rm_gatt_ctl_t;
#define PARAM_RM_GATT_CTL_ADDRESS_6        0
#define PARAM_RM_GATT_CTL_STATUS_1         6

//param_rm_read_sm_param_t;
#define PARAM_RM_READ_SM_ADDRESS_6        0
#define PARAM_RM_READ_SM_TYPE_1           6

//param_rm_set_sm_alarm_t;
#define PARAM_RM_SET_SM_ALARM_ADDRESS_6        0
#define PARAM_RM_SET_SM_ALARM_CFG_1            6
#define PARAM_RM_SET_SM_ALARM_VALUE_4          7

//param_rm_reset_to_factory_t
#define PARAM_RM_RESET_TO_FACTORY_ADDRESS_6    0

//param_rm_restart_t
#define PARAM_RM_RESTART_ADDRESS_6             0

//param_rm_type_info_t
#define PARAM_RM_TYPE_INFO_ADDRESS_6           0

//param_rm_fw_dl_image_t
#define PARAM_RM_FW_DL_IMAGE_FW_PID_1          0
#define PARAM_RM_FW_DL_IMAGE_LENGTH_2          1
#define PARAM_RM_FW_DL_IMAGE_PAYLOAD_MAX       3


/*******Payoad Respond*****/
//resp_rm_version_t
#define RESPOND_RM_VERISON_ADDRESS_6           0
#define RESPOND_RM_VERISON_TRAVEL_TIME_4       6
#define RESPOND_RM_VERISON_VERSION_33          10

//resp_rm_relay_ctl_t
#define RESPOND_RM_RELAY_CTL_ADDRESS_6         0
#define RESPOND_RM_RELAY_CTL_TRAVEL_TIME_4     6
#define RESPOND_RM_RELAY_CTL_STATUS_1          10






typedef struct
{
    gwp_list_t           list;
    uint32_t             ReadPos;
    uint32_t             Reserve_length;    //reserver buffer
    uint32_t             length;            //actual payload length
    uint32_t pdu_seq;
    uint8_t command;
    uint32_t start_time;
    uint8_t count;
    uint32_t last_time;
} gwp_item_header_t;


#define GWP_POLLING_ITEMS_MAX            50
#define GWP_POLLING_BUFF_SIZE           3000
#define GWP_RM_MAX_PAYLOAD              40

extern gwp_item_header_t   gwp_item_bank[GWP_POLLING_ITEMS_MAX];
extern uint8_t gwp_polling_buff[GWP_POLLING_BUFF_SIZE];

void gwp_indication_item_init(void);
gwp_item_header_t *gwp_indication_item_reserve(uint16_t length);
void gwp_indication_item_free(gwp_item_header_t *item);
uint8_t gwp_indication_item_size();

#endif // !_GWP_RPC_INDICATION_H_