#ifndef _GWP_RPC_CRYPTO_H_
#define _GWP_RPC_CRYPTO_H_

//#include "mem_map.h"

#define RPC_KEY_SIZE                    32

//extern gwp_firmware_t gwp_fm;;

ret_code_t decrypted_payload(uint8_t *buf, uint16_t length, uint8_t* payload, uint8_t *iv);

ret_code_t encrypted_payload(uint8_t *src_address, uint16_t length, uint8_t *dest_address, uint8_t *iv);

ret_code_t get_rpc_key(uint8_t *key);
ret_code_t update_rpc_session_key();
ret_code_t init_rpc_session_key();

#endif // !_GWP_RPC_CRYPTO_H_
