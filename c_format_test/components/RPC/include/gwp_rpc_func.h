#ifndef _GWP_RPC_FUNC_H_
#define _GWP_RPC_FUNC_H_



ret_code_t gwp_rpc_func_lc_version(uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_lc_polling(uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_lc_reset_factory(uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_lc_reset(uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_lc_fw_dl_image(uint8_t *in_data, uint16_t length, uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_lc_fw_install(uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_lc_dfu_request(uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_lc_get_dl_status(uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_lc_get_OTA_status(uint8_t *response, uint16_t *out_size);




#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )
// bootloader without ble related code h145764
#else
ret_code_t gwp_rpc_func_lc_ble_cmd(uint8_t *in_data, uint16_t length, uint8_t *response, uint16_t *out_size);
ret_code_t gwp_rpc_func_rm_version(uint8_t *address);
ret_code_t gwp_rpc_func_rm_relay_ctl(uint8_t *address, uint8_t status);
ret_code_t gwp_rpc_func_rm_switch_lock(uint8_t *address, uint8_t status);
ret_code_t gwp_rpc_func_rm_gatt_ctl(uint8_t *address, uint8_t status);
ret_code_t gwp_rpc_func_rm_read_sm_param(uint8_t *address, uint8_t type);
ret_code_t gwp_rpc_func_rm_set_sm_alarm_cfg(uint8_t *address, uint8_t cfg, uint8_t* value);
ret_code_t gwp_rpc_func_rm_reset_to_factory(uint8_t *address);
ret_code_t gwp_rpc_func_rm_restart(uint8_t *address);
ret_code_t gwp_rpc_func_rm_fw_dl_image(uint8_t fw_pid, uint16_t length, uint8_t *payload);
ret_code_t gwp_rpc_func_rm_type_info(uint8_t *address);
#endif




#endif // !_GWP_RPC_FUNC_H_