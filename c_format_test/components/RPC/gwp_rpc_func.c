#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#include "gwp_common.h"


#include "gwp_error_common.h"

#include "gwp_rpc.h"
#include "gwp_list.h"
#include "gwp_rpc_indication.h"
#if defined(NRF52840_XXAA)

#include "gwp_ota_process.h"
#include "gwp_ota_task.h"

#include "secure_storage_ota.h"
#include "secure_storage_api.h"
#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )
#include "gwp_dfu_process.h"
#else
#include "gwp_ble_interface.h"
#include "gwp_ble_tunnel.h"

#include "secure_storage_api.h"//h145764
#endif




#include "gwp_rpc_init.h"
const static uint8_t sw_version_desc[VERSION_SIZE] = "version 0.1.2.3";
extern uint32_t ble_product_id, ble_hw_version;

ret_code_t gwp_rpc_func_lc_version(uint8_t *response, uint16_t *out_size)
{
    uint8_t sw_version[4];
    ret_code_t ret_val = GWP_SUCCESS;
    ret_val = gwp_secure_read_product_info(LOGIC_PRODUCT_INFO_CURRENT_VERSION_ADDRESS, sw_version, sizeof(sw_version));
    if(GWP_SUCCESS != ret_val)
    {
        return ret_val;
    }
//ins_rpc
    memcpy(response, sw_version, 4);
    response += 4;
    memcpy(response, (uint8_t *)&ble_hw_version, 4);
    //memcpy(response,sw_version,4);
    *out_size += 8;
    response += 4;
    memcpy(response, sw_version_desc, VERSION_SIZE);
    *out_size += VERSION_SIZE;
    response += VERSION_SIZE;
    return GWP_SUCCESS;
}

//Response:
//PDU 4ytes+Command 1byte+RDI 1byte+last PDU 4bytes+polling cmd 1byte +ble_ind 1byte +sub_ble_ind byte+data
ret_code_t gwp_rpc_func_lc_polling(uint8_t *response, uint16_t *out_size)
{
    gwp_item_header_t *item = NULL;
    uint16_t length, i = 0;
    uint32_t ReadPos;
    uint32_t Pdu;
    item = (gwp_item_header_t *)gwp_list_pop_front(&gwp_reply_queue);
    if(item == NULL)
    {
        *out_size = 0;
        return GWP_SUCCESS;
    }
    ReadPos = item->ReadPos;
    /*Report PDU command payload to host*/
    //pdu
    Pdu = item->pdu_seq;
    for(i = 0; i < SEQ_SIZE; i++)
    {
        *response = Pdu % 0x100;
        Pdu = (Pdu >> 8);
        response ++;
    }
    *out_size += SEQ_SIZE;
    //command
    *response = item->command;
    response ++;
    *out_size += COMMAND_SIZE;
    //payload
    length = item->length;
    for(i = 0; i < length; i++)
    {
        *response = gwp_polling_buff[ReadPos];
        response ++;
        ReadPos = (ReadPos + 1) % GWP_POLLING_BUFF_SIZE;
    }
    *out_size += length;
    gwp_indication_item_free(item);
    return GWP_SUCCESS;
}



ret_code_t gwp_rpc_func_lc_reset_factory(uint8_t *response, uint16_t *out_size)
{
    *response = SUCCESS;
    *out_size += 1;
    return GWP_SUCCESS;
}


ret_code_t gwp_rpc_func_lc_reset(uint8_t *response, uint16_t *out_size)
{
    *response = SUCCESS;
    *out_size += 1;
    return GWP_SUCCESS;
}


ret_code_t gwp_rpc_func_lc_fw_dl_image(uint8_t *in_data, uint16_t length, uint8_t *response, uint16_t *out_size)
{
    ret_code_t ret_code = GWP_SUCCESS;
    //const TickType_t xMaxBlockTime = pdMS_TO_TICKS(300); /*  set max wait time is 300 ms */
    ota_msg_t msg_send = {0};
    *out_size += 1;
    memcpy(&msg_send.count, &in_data[0], sizeof(msg_send.count));
    memcpy(&msg_send.length, &in_data[2], sizeof(msg_send.length));
    memcpy(msg_send.data, &in_data[4], msg_send.length);
    //GWP_LOG_INFO("fw_count = %d ", msg_send.count);
    //NRF_LOG_FLUSH();
    ret_code = gwp_secure_ota_process(msg_send.count, msg_send.data, msg_send.length);
    //ret_code = gwp_queue_send(ota_queue,(void*)&msg_send,(TickType_t)xMaxBlockTime);
    if((ret_code == GWP_SUCCESS) || (ret_code == GWP_OTA_IDLE) || (ret_code == GWP_OTA_IMAGE_SAVING)
       || (ret_code == GWP_OTA_IMAGE_SAVED) || (ret_code == GWP_OTA_READYTOINSTALL))
    {
        *response = SUCCESS;
    }
    else
    {
        if(ret_code >= NRF_INVALID_PRODUCT_ID && ret_code <= NRF_NOT_FALLBACK_VERSION)
        {
            *response = ret_code - NRF_INVALID_PRODUCT_ID + 2;
        }
        else
        {
            *response = FAIL;
        }
        NRF_LOG_INFO("dl image error = 0x%02x", ret_code);
        NRF_LOG_FLUSH();
    }
    return GWP_SUCCESS;
}

int OTA_install_Flag = 0;
ret_code_t gwp_rpc_func_lc_fw_install(uint8_t *response, uint16_t *out_size)
{
    ret_code_t ret_val = GWP_SUCCESS;
    *response = SUCCESS;
    *out_size += 1;
    ret_val = gwp_secure_ota_install();
    if(ret_val == GWP_SUCCESS)
    {
        //NVIC_SystemReset();
        OTA_install_Flag = 1;
        return GWP_SUCCESS;
    }
    else
    {
        *response = FAIL;
        return NRF_ERROR_FORBIDDEN;
    }
}


#ifdef RPC_APPLICATION_CODE
typedef enum DFU_FLAG
{
    DFU_FLAG_N = 0,// enter application
    DFU_FLAG_Y = 1,// bootloader mode
    DFU_FLAG_WATCHDOG_RESET = 2,// bootloader mode watchdog reset
    DFU_FLAG_UNUSE    = 0xFFFFFFFFUL,
} DFU_FLAG_ENUM;




uint8_t check_update_dfu_flag(DFU_FLAG_ENUM flag)
{
    ret_code_t ret_val = GWP_SUCCESS;
    DFU_FLAG_ENUM dfu_info_tmp ;
    dfu_status_t A_status, B_status;
    _secure_ota_read_config_A_B(&A_status, &B_status);
//    if ( A_status == DFU_PROVISIONAL )
    if ( (A_status | B_status) == (DFU_PROVISIONAL | DFU_FALLBACK) )
    {
        ret_val = NRF_ERROR_FORBIDDEN;//reject dfu
        return ret_val;
    }
    else
    {
        gwp_secure_read_product_info(LOGIC_PRODUCT_INFO_DFU_FLAG_ADDRESS, (uint8_t *)&dfu_info_tmp, sizeof(DFU_FLAG_ENUM));
        if ( dfu_info_tmp != flag )
        {
            gwp_secure_write_product_info(LOGIC_PRODUCT_INFO_DFU_FLAG_ADDRESS, (uint8_t *)&flag, sizeof(DFU_FLAG_ENUM));
        }
        //host need reset devices.
        return ret_val;
    }
}

#endif
ret_code_t gwp_rpc_func_lc_dfu_request(uint8_t *response, uint16_t *out_size)
{
    ret_code_t ret_val = GWP_SUCCESS;
    *response = SUCCESS;
    *out_size += 1;
    ret_val =  check_update_dfu_flag(DFU_FLAG_Y);//set dfu flag
    if(ret_val == GWP_SUCCESS)
    {
        return GWP_SUCCESS;
    }
    else
    {
        *response = FAIL;
        return NRF_ERROR_FORBIDDEN;
    }
}


ret_code_t gwp_rpc_func_check_OTA_status(void)

{
    ret_code_t ret_val = GWP_SUCCESS;
    dfu_loc_t location;
    //Get OTA status
    dfu_status_t dfu_status[DFU_LOCATION_COUNT] = {DFU_STATUS_NONE};
    ret_val = _secure_ota_read_config_A_B(&dfu_status[DFU_LOCATION_A], &dfu_status[DFU_LOCATION_B]);
    APP_ERROR_CHECK(ret_val);
    if((dfu_status[DFU_LOCATION_A] | dfu_status[DFU_LOCATION_B]) == (DFU_DOWNLOADING | DFU_VALID))
    {
//        NRF_LOG_INFO("Before  A region is 0x%02x, B region is 0x%02x", dfu_status[DFU_LOCATION_A], dfu_status[DFU_LOCATION_B]);
        if(dfu_status[DFU_LOCATION_A] == DFU_DOWNLOADING)
        {
            location = DFU_LOCATION_A;
        }
        else
        {
            location = DFU_LOCATION_B;
        }
        ret_val = _secure_ota_update_region_status(DFU_INVALID, DFU_VALID, location);
//        _secure_ota_read_config_A_B(&dfu_status[DFU_LOCATION_A], &dfu_status[DFU_LOCATION_B]);
//        NRF_LOG_INFO("After update   A region is 0x%02x, B region is 0x%02x", dfu_status[DFU_LOCATION_A], dfu_status[DFU_LOCATION_B]);
        if (GWP_SUCCESS != ret_val)
            return ret_val;
    }
    return ret_val;
}




ret_code_t gwp_rpc_func_lc_get_dl_status(uint8_t *response, uint16_t *out_size)
{
    *response = SUCCESS;
    *out_size += 1;
    return GWP_SUCCESS;
}




ret_code_t gwp_rpc_func_lc_get_OTA_status(uint8_t *response, uint16_t *out_size)

{
    ret_code_t ret_val = GWP_SUCCESS;
    //Get OTA status
    dfu_status_t dfu_status[DFU_LOCATION_COUNT] = {DFU_STATUS_NONE};
    ret_val = _secure_ota_read_config_A_B(&dfu_status[DFU_LOCATION_A], &dfu_status[DFU_LOCATION_B]);
    APP_ERROR_CHECK(ret_val);
    *response = GWP_OTA_SUCESS;
    switch (dfu_status[DFU_LOCATION_A] |
            dfu_status[DFU_LOCATION_B])
    {
        case DFU_INVALID        |   DFU_VALID:
            *response = GWP_OTA_FAIL;
            break;
        case DFU_PRIOR          |   DFU_VALID:    //Primarily finish and OTA success state
            *response = GWP_OTA_SUCESS;
            break;
        //New FW downloand END
        case DFU_DOWNLOADDONE   |   DFU_VALID:
            *response = GWP_OTA_DONWLOAD_DONE;
            break;
        //During New FW download
        case DFU_DOWNLOADING    |   DFU_VALID:
            //   *response = 5;
            //   break;
            *response = GWP_OTA_UPDATING;
            break;
        case DFU_PROVISIONAL    |   DFU_FALLBACK:
            *response = GWP_OTA_PROVISIONAL;
            break;
        //for snap on io start
        case DFU_INVALID    |   DFU_FALLBACK:
            *response = GWP_OTA_FAIL;
            break;
        //for snap on io end
        default:
            *response = GWP_OTA_STATUS_NONE;
            break;
    }
    *out_size += 1;
    return ret_val;
}




#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )

#else

ret_code_t gwp_rpc_func_rm_version(uint8_t *address)
{
    return NRF_ERROR_SVC_HANDLER_MISSING;//GWP_SUCCESS;
}


ret_code_t gwp_rpc_func_rm_relay_ctl(uint8_t *address, uint8_t status)
{
    return GWP_SUCCESS;
}

ret_code_t gwp_rpc_func_rm_switch_lock(uint8_t *address, uint8_t status)
{
    return GWP_SUCCESS;
}

ret_code_t gwp_rpc_func_rm_gatt_ctl(uint8_t *address, uint8_t status)
{
    return GWP_SUCCESS;
}

ret_code_t gwp_rpc_func_rm_read_sm_param(uint8_t *address, uint8_t type)
{
    return GWP_SUCCESS;
}


ret_code_t gwp_rpc_func_rm_set_sm_alarm_cfg(uint8_t *address, uint8_t cfg, uint8_t* value)
{
    return GWP_SUCCESS;
}


ret_code_t gwp_rpc_func_rm_reset_to_factory(uint8_t *address)
{
    return GWP_SUCCESS;
}

ret_code_t gwp_rpc_func_rm_restart(uint8_t *address)
{
    return GWP_SUCCESS;
}



ret_code_t gwp_rpc_func_rm_fw_dl_image(uint8_t fw_pid, uint16_t length, uint8_t *payload)
{
    return GWP_SUCCESS;
}

ret_code_t gwp_rpc_func_rm_type_info(uint8_t *address)
{
    return GWP_SUCCESS;
}


//LE  RPC Part
/********************************************************
 Note:
 1)length=rpc_frame->payload_length=PDU4+command1+parameter>5
 2)in_data=parameter include 1)s ubcommand; 2)payload of subcommand
 ********************************************************/
ret_code_t gwp_rpc_func_lc_ble_cmd(uint8_t *in_data, uint16_t length, uint8_t *response, uint16_t *out_size)
{
    frame_le_command_t *le_subcommand = (frame_le_command_t *)in_data;
    if(length > 5)   //rpc_frame->payload_length=PDU4+command1+parameter>5
    {
        switch(le_subcommand->sub_command)
        {
            case LE_ADV:   //1  8bytes name+16bytes uuid
                if(le_subcommand->data[0] == LE_ADV_Disable)
                {
                    UpdateAdvData(LE_ADV_Disable, NULL, NULL, NULL, NULL); //ADV_BLE_Switch(switch,name,uuid)
                }
                else if (le_subcommand->data[0] == LE_ADV_Enable)
                {
                    uint8_t ADV_name[8] = {0};
                    uint8_t adv_name_length = 0; //Max=8
                    uint8_t adv_manuf_length = 0;
                    uint8_t adv_manuf[27] = {0}; //Max=27
                    //
                    adv_name_length = le_subcommand->data[1];
                    if(adv_name_length < 9)
                    {
                        memcpy(ADV_name, &le_subcommand->data[2], adv_name_length);
                        adv_manuf_length = le_subcommand->data[2 + adv_name_length];
                        if(adv_manuf_length < 28)
                            memcpy(adv_manuf, &le_subcommand->data[3 + adv_name_length], adv_manuf_length);
                        else
                        {
                            adv_manuf_length = 27;
                            memcpy(adv_manuf, &le_subcommand->data[3 + adv_name_length], 27);
                        }
                    }
                    else
                    {
                        memcpy(ADV_name, &le_subcommand->data[2], 8);
                        adv_manuf_length = le_subcommand->data[2 + adv_name_length];
                        if(adv_manuf_length < 28)
                        {
                            memcpy(adv_manuf, &le_subcommand->data[3 + adv_name_length], adv_manuf_length);
                            adv_name_length = 8;
                        }
                        else
                        {
                            adv_manuf_length = 27;
                            memcpy(adv_manuf, &le_subcommand->data[3 + adv_name_length], 27);
                        }
                    }
                    UpdateAdvData(LE_ADV_Enable, adv_name_length, ADV_name, adv_manuf_length, adv_manuf);
                }
                break;
            case LE_CONNECT:
                break;
            case LE_CONNECTCANCEL:
                break;
            case LE_DISCONNECT:   //2
            {
                uint16_t service_handle;
                service_handle = le_subcommand->data[1];
                service_handle = (uint16_t)((uint16_t)service_handle << 8) + le_subcommand->data[0];
                ble_disconnect(service_handle);
            }
            break;
            case LE_DISCOVERY:
                break;
            case LE_SCAN:
                break;
            case LE_FUNCTION:
                break;
            case LE_WRITEDATA:   //3
            {
                uint16_t character_read_handle;
                uint16_t length;
                character_read_handle = le_subcommand->data[1];
                character_read_handle = (uint16_t)((uint16_t)character_read_handle << 8) + le_subcommand->data[0];
                length = le_subcommand->data[3];
                length = (uint16_t)((uint16_t)length << 8) + le_subcommand->data[2];
                rpc_data_send(&le_subcommand->data[4], length, character_read_handle);
            }
            break;
            case LE_SIGNRESP:
            {
                uint8_t ret_val;
                ret_val = set_reuqest_data_signature_from_host(&le_subcommand->data[0], 64);
                if(ret_val != GWP_SUCCESS)
                {
                    GWP_LOG_INFO("get sign respense------------------------fail ");
                    *response = FAIL;
                }
                else
                {
                    *response = SUCCESS;
                    GWP_LOG_INFO("get sign respense------------------------success ");
                }
                break;
            }
            default:
                break;
        }
        //ack contain sub command
        *response = le_subcommand->sub_command;
        response += 1;
        *out_size += 1;
        if(FAIL != *response)
        {
            *response = SUCCESS;
        }
        *out_size += 1;
    }
    else
    {
        *response = le_subcommand->sub_command;
        response += 1;
        *out_size += 1;
        *response = FAIL;
        *out_size += 1;
    }
    return GWP_SUCCESS;
}
#endif
#elif defined(STM32WB55xx) || defined(CONFIG_IDF_TARGET_ESP32S3)



#else

#error "Architecture not set."

#endif










