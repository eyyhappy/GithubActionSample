#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>




#if defined(NRF52840_XXAA)

#include "app_error.h"
#include "app_util.h"
#elif defined(STM32WB55xx) || defined(CONFIG_IDF_TARGET_ESP32S3)

#define CODE_PAGE_SIZE (MBR_PAGE_SIZE_IN_WORDS * sizeof(uint32_t))

#else

#error "Architecture not set."

#endif




//#include "rng_host.h"
//#include "sha256_host.h"
//#include "ecdsa_host.h"
//#include "x509_crt_host.h"

#include "gwp_error_common.h"
#include "gwp_rpc.h"
#include "gwp_rpc_init.h"

/*Note:To add product ca in product_ca_data,you need notice:
1)You may need to add ca chain,like QA CA in cluding :SharedQACA.pem and HoneywellQAProductPKI.pem
2)The root CA is in the hihg position of product_ca_data, second CA is in the low position.
3)\r\n is need
*/
#ifdef GWP_PRODUCTION
//ASCProductsCA is used to verify Smart sensor device certificate
//ACSProductsCA.pem + HoneywellProductPKI.pem
const uint8_t *product_ca_data = (const uint8_t *)"\
-----BEGIN CERTIFICATE-----\r\n\
MIICoTCCAkagAwIBAgIIZMlu1ffGmO0wCgYIKoZIzj0EAwIwVDELMAkGA1UEBhMC\r\n\
VVMxJTAjBgNVBAoMHEhvbmV5d2VsbCBJbnRlcm5hdGlvbmFsIEluYy4xHjAcBgNV\r\n\
BAMMFUhvbmV5d2VsbCBQcm9kdWN0IFBLSTAgFw0xNjA5MTMxOTQyNTFaGA85OTk5\r\n\
MDEwMTAwMDAwMFowXDELMAkGA1UEBhMCVVMxJTAjBgNVBAoMHEhvbmV5d2VsbCBJ\r\n\
bnRlcm5hdGlvbmFsIEluYy4xDDAKBgNVBAsMA0FDUzEYMBYGA1UEAwwPQUNTIFBy\r\n\
b2R1Y3RzIENBMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAESnGfWX5rvcpsNPTj\r\n\
N+nBrgc8Buz9I61TDMS0VKSouDV2EliiWR+jncTrSH7JFkvuYFkHu3Rf0YTYy0h4\r\n\
bq5mF6OB9zCB9DAdBgNVHQ4EFgQUTZ6/ykGbNRVXF07fQUnic2PjxQowDwYDVR0T\r\n\
AQH/BAUwAwEB/zAfBgNVHSMEGDAWgBTlk/cRtqSDIbNdH2/Qo5daVmn3KDBIBgNV\r\n\
HSAEQTA/MD0GDCsGAQQBgaQGAgEBATAtMCsGCCsGAQUFBwIBFh9odHRwczovL2hw\r\n\
cGtpLmhvbmV5d2VsbC5jb20vY3BzMEcGA1UdHwRAMD4wPKA6oDiGNmh0dHA6Ly9o\r\n\
cHBraS5ob25leXdlbGwuY29tL2NybC9Ib25leXdlbGxQcm9kdWN0UEtJLmNybDAO\r\n\
BgNVHQ8BAf8EBAMCAQYwCgYIKoZIzj0EAwIDSQAwRgIhAOR7OKLgAFARoYczNe3u\r\n\
d4Arx4/6vJP4pk861KjT89UWAiEA5mvTw3qI22bjN0jE7ossr2Y/WyB4jiPrYIRg\r\n\
1d2zNpI=\r\n\
-----END CERTIFICATE-----\r\n\
-----BEGIN CERTIFICATE-----\r\n\
MIICUDCCAfWgAwIBAgIIAyGeIbWpweYwCgYIKoZIzj0EAwIwVDELMAkGA1UEBhMC\r\n\
VVMxJTAjBgNVBAoMHEhvbmV5d2VsbCBJbnRlcm5hdGlvbmFsIEluYy4xHjAcBgNV\r\n\
BAMMFUhvbmV5d2VsbCBQcm9kdWN0IFBLSTAgFw0xNjA5MDkyMTE1NTlaGA85OTk5\r\n\
MTIzMDIzNTg1OVowVDELMAkGA1UEBhMCVVMxJTAjBgNVBAoMHEhvbmV5d2VsbCBJ\r\n\
bnRlcm5hdGlvbmFsIEluYy4xHjAcBgNVBAMMFUhvbmV5d2VsbCBQcm9kdWN0IFBL\r\n\
STBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABBlN5rrG5cVXd5A43G0ido0Hoyk3\r\n\
DrJX66xzE9q31VpJcDQN5/598RspwretMqtyI612jwndATi6qHwPgdZoc86jga4w\r\n\
gaswHQYDVR0OBBYEFOWT9xG2pIMhs10fb9Cjl1pWafcoMA8GA1UdEwEB/wQFMAMB\r\n\
Af8wHwYDVR0jBBgwFoAU5ZP3EbakgyGzXR9v0KOXWlZp9ygwSAYDVR0gBEEwPzA9\r\n\
BgwrBgEEAYGkBgIBAQEwLTArBggrBgEFBQcCARYfaHR0cHM6Ly9ocHBraS5ob25l\r\n\
eXdlbGwuY29tL2NwczAOBgNVHQ8BAf8EBAMCAQYwCgYIKoZIzj0EAwIDSQAwRgIh\r\n\
AIW7uONSxGXBWlJ6/MK/H1P8vaipQisgifSQ/PS4L5KEAiEAn98Jhnmsbu5EIFcg\r\n\
k3IIvQro9n+DSVSfxCxABBcZ2UE=\r\n\
-----END CERTIFICATE-----";
#else //GWP_PRODUCTION

const uint8_t *ca_data = (const uint8_t *)"\
-----BEGIN CERTIFICATE-----\r\n\
MIIClzCCAj2gAwIBAgIJAJ/xtR0vxK7BMAoGCCqGSM49BAMCMIGnMQswCQYDVQQG\r\n\
EwJVUzERMA8GA1UECAwIc2hhbmdoYWkxETAPBgNVBAcMCHNoYW5naGFpMSUwIwYD\r\n\
VQQKDBxIb25leXdlbGwgSW50ZXJuYXRpb25hbCBJbmMuMQwwCgYDVQQLDANIQlQx\r\n\
EzARBgNVBAMMCnl1YW54aW5mZWkxKDAmBgkqhkiG9w0BCQEWGXhpbmZlaS55dWFu\r\n\
QGhvbmV5d2VsbC5jb20wHhcNMTkxMTIyMDI0OTMyWhcNMjkxMTE5MDI0OTMyWjCB\r\n\
pzELMAkGA1UEBhMCVVMxETAPBgNVBAgMCHNoYW5naGFpMREwDwYDVQQHDAhzaGFu\r\n\
Z2hhaTElMCMGA1UECgwcSG9uZXl3ZWxsIEludGVybmF0aW9uYWwgSW5jLjEMMAoG\r\n\
A1UECwwDSEJUMRMwEQYDVQQDDAp5dWFueGluZmVpMSgwJgYJKoZIhvcNAQkBFhl4\r\n\
aW5mZWkueXVhbkBob25leXdlbGwuY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcD\r\n\
QgAEvGD58hvcZhluwZt6Mzb7vomBB/c0YsJU3s1PhdKarZyYEovAzV+s7H9mp+2g\r\n\
IW6HUa2sufH0jZyU9ePL9Qg1U6NQME4wHQYDVR0OBBYEFCIjMLx8ec9tlpB1sjkc\r\n\
W0qu/E2oMB8GA1UdIwQYMBaAFCIjMLx8ec9tlpB1sjkcW0qu/E2oMAwGA1UdEwQF\r\n\
MAMBAf8wCgYIKoZIzj0EAwIDSAAwRQIhAPSb288Ujkeiijm6yB0MISwCFmCZrMof\r\n\
vn0pon0RsC6MAiBSoE+n6VB4iSY7/0Dhd1Kt5b73v91z19dhNOzKbbXsWw==\r\n\
-----END CERTIFICATE-----";
/*******************HonCA*************************/
/*
//SharedQACA.pem+HoneywellQAProductPKI.pem
const uint8_t *hon_ca_data = "\
-----BEGIN CERTIFICATE-----\r\n\
MIICnjCCAkSgAwIBAgIIT+dkb4TGPkIwCgYIKoZIzj0EAwIwVzELMAkGA1UEBhMC\r\n\
VVMxJTAjBgNVBAoMHEhvbmV5d2VsbCBJbnRlcm5hdGlvbmFsIEluYy4xITAfBgNV\r\n\
BAMMGEhvbmV5d2VsbCBRQSBQcm9kdWN0IFBLSTAgFw0xNjA5MDkwMDM3MTRaGA85\r\n\
OTk5MDIwMTAwMDAwMFowWTELMAkGA1UEBhMCVVMxJTAjBgNVBAoMHEhvbmV5d2Vs\r\n\
bCBJbnRlcm5hdGlvbmFsIEluYy4xDDAKBgNVBAsMA0FDUzEVMBMGA1UEAwwMU2hh\r\n\
cmVkIFFBIENBMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE4EemqtK2p/FLHpCm\r\n\
TSMSY4tmYoD2PjYgZ17CMGvJZHWrsJZIo8aFcOD/4Utwg27awo0EwpeR6+gINPy8\r\n\
nUvjSqOB9TCB8jAdBgNVHQ4EFgQUigo7HXBqSQk3b40Axd8hqEVvrg0wDwYDVR0T\r\n\
AQH/BAUwAwEB/zAfBgNVHSMEGDAWgBS/5qXc0/Re2HshHrKuQamF0KrP3DBJBgNV\r\n\
HSAEQjBAMD4GDCsGAQQBgaQGAgEBATAuMCwGCCsGAQUFBwIBFiBodHRwczovL3Fo\r\n\
cHBraS5ob25leXdlbGwuY29tL2NwczBEBgNVHR8EPTA7MDmgN6A1hjNodHRwOi8v\r\n\
cWhwcGtpLmhvbmV5d2VsbC5jb20vY3JsL0hvbmV5d2VsbFFBUm9vdC5jcmwwDgYD\r\n\
VR0PAQH/BAQDAgEGMAoGCCqGSM49BAMCA0gAMEUCIQCae2Wwyba1jzDeMC0Y4/Pk\r\n\
R4N1cBxOHyE7TzynrhjOjgIgZG+maW6qWDDFcjKeW6l+LTTMDHPhFRYJOX8SFnK6\r\n\
JX8=\r\n\
-----END CERTIFICATE-----\r\n\
-----BEGIN CERTIFICATE-----\r\n\
MIICVzCCAfygAwIBAgIIFfL/t5X46RQwCgYIKoZIzj0EAwIwVzELMAkGA1UEBhMC\r\n\
VVMxJTAjBgNVBAoMHEhvbmV5d2VsbCBJbnRlcm5hdGlvbmFsIEluYy4xITAfBgNV\r\n\
BAMMGEhvbmV5d2VsbCBRQSBQcm9kdWN0IFBLSTAgFw0xNjA5MDkwMDE5MzFaGA85\r\n\
OTk5MTIzMDIzNTg1OVowVzELMAkGA1UEBhMCVVMxJTAjBgNVBAoMHEhvbmV5d2Vs\r\n\
bCBJbnRlcm5hdGlvbmFsIEluYy4xITAfBgNVBAMMGEhvbmV5d2VsbCBRQSBQcm9k\r\n\
dWN0IFBLSTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABOtoX1aZ7aHPz3cyqhyb\r\n\
neMbo9OWy05jNtjaHdB5zzQ5KMrnSMLbQN14XvSG/4aNc84y77xAwOqYZy28md+3\r\n\
Mr2jga8wgawwHQYDVR0OBBYEFL/mpdzT9F7YeyEesq5BqYXQqs/cMA8GA1UdEwEB\r\n\
/wQFMAMBAf8wHwYDVR0jBBgwFoAUv+al3NP0Xth7IR6yrkGphdCqz9wwSQYDVR0g\r\n\
BEIwQDA+BgwrBgEEAYGkBgIBAQEwLjAsBggrBgEFBQcCARYgaHR0cHM6Ly9xaHBw\r\n\
a2kuaG9uZXl3ZWxsLmNvbS9jcHMwDgYDVR0PAQH/BAQDAgEGMAoGCCqGSM49BAMC\r\n\
A0kAMEYCIQCSCRTqKd1hGkWUsYj4GGNH5ofz49uqOA20UZH7GG7VlgIhALUWaKKR\r\n\
82ERH8iEH/9roFGyJzwtGJCQmNAqV8wLMcFP\r\n\
-----END CERTIFICATE-----";*/
/**********************/

#endif //GWP_PRODUCTION

uint8_t cert[MAX_CERT_SIZE] = {0};
uint32_t cert_size = 0;
uint8_t cert_hash_buf[INIT_HASH_SIZE];
uint8_t init_random[INIT_RANDOM_SIZE];

cert_info_t cert_tmp_info;


static bool verify_cert_authentication(uint8_t *cert_buf, uint32_t cert_len)
{
    uint8_t hash_buf[INIT_HASH_SIZE];
    size_t len = INIT_HASH_SIZE;
    ret_code_t ret_code = GWP_SUCCESS;
//    gwpFunctions.rngVectorGenerate
    gwpFunctions.hash256Calculate(cert_buf, cert_len, hash_buf, &len);
    if(memcmp(hash_buf, cert_hash_buf, sizeof(hash_buf)))
    {
        return false;
    }
    else
    {
#ifdef GWP_PRODUCTION
        GWP_LOG_INFO("Initialize product CA in verify_cert_authentication");
        ret_code = gwpFunctions.x509CrtVerify (cert_buf, cert_len, product_ca_data, strlen((const char *)product_ca_data) + 1, NULL, 0);
#else
        //ret_code = gwp_x509_crt_verify(cert_buf,cert_len,hon_ca_data,strlen((const char *)hon_ca_data)+1,NULL,0);
//        gwpFunctions.rngVectorGenerate
        ret_code = gwpFunctions.x509CrtVerify(cert_buf, cert_len, ca_data, strlen((const char *)ca_data) + 1, NULL, 0);
#endif //GWP_PRODUCTION
        if(ret_code != GWP_SUCCESS)
        {
            return false;
        }
        return true;
    }
}


ret_code_t gwp_init_cmd_get_seq(uint8_t * result, uint16_t *out_len)
{
    ret_code_t ret_code = GWP_SUCCESS;
    VERIFY_TRUE((*out_len >= SEQ_SIZE), RPC_ERROR_MEMORY_OUT);
    memset(init_random, 0, sizeof(init_random));
#ifdef DEBUG_RPC_PROTOCOL
    const uint8_t fix_iv[16] = {0x51, 0xf2, 0xff, 0x13, 0x23, 0x77, 0x6a, 0x49, 0x6f, 0x36, 0x58, 0xa7, 0xd4, 0xcd, 0x99, 0xf8};
    memcpy((uint8_t *)&gwp_rpc.pdu_seq, fix_iv, sizeof(gwp_rpc.pdu_seq));
#else
    ret_code = gwpFunctions.rngVectorGenerate((uint8_t *)&gwp_rpc.pdu_seq, sizeof(gwp_rpc.pdu_seq));
#endif /* DEBUG_RPC_PROTOCOL */
    if(ret_code != GWP_SUCCESS)
    {
        return ret_code;
    }
    memcpy(result, &gwp_rpc.pdu_seq, sizeof(gwp_rpc.pdu_seq));
    gwp_rpc.rpc_state = RPC_STATE_INIT;
    gwp_rpc.rpc_init_status = INIT_PROBE_CAP;//INIT_CHECK_DEVICE;//INIT_PROBE_CAP
    return ret_code;
}
extern uint8_t salt_key[4];           // 4bytes random number from Random SQ
extern uint8_t salt_key_all[8];
ret_code_t gwp_init_cmd_check_device_random(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len)
{
    ret_code_t ret_code = GWP_SUCCESS;
#ifndef RPC_TOOL_PYTHON_SUPPORT
    int i;
    uint8_t verify_data[4] = {0x47, 0x57, 0x50, 0x0a}; //"GWP"
#endif
    VERIFY_TRUE((*out_len >= SEQ_SIZE), RPC_ERROR_MEMORY_OUT);
    if(gwp_rpc.rpc_init_status != INIT_CHECK_DEVICE)
    {
        return RPC_ERROR_INIT_STATUS;
    }
    if(gwp_rpc.pdu_seq != rpc_frame->u.payload_init.pdu_seq)
    {
        return RPC_ERROR_FRAME_SEQ;
    }
    gwp_rpc.pdu_seq ++;
    if(rpc_frame->payload_length != INIT_RANDOM_SIZE + SEQ_SIZE + INIT_CMD_SIZE)
    {
        return RPC_ERROR_FRAME_SIZE;
    }
#ifndef RPC_TOOL_PYTHON_SUPPORT
    for(i = 0; i < SEQ_SIZE; i++)
    {
        result[i] = rpc_frame->u.payload_init.payload[i] ^ verify_data[i];
    }
#else
    size_t result_length = 32; //SEQ_SIZE;  gwpFunctions.rngVectorGenerate
    ret_code = gwpFunctions.hash256Calculate(rpc_frame->u.payload_init.payload, SEQ_SIZE, result, &result_length);
    if(ret_code != GWP_SUCCESS)
        return ret_code;
    memcpy(salt_key, rpc_frame->u.payload_init.payload, 4); //save 4bytes random number
    memcpy(&salt_key_all[0], rpc_frame->u.payload_init.payload, 4); //save 4bytes random number
    print_hex(salt_key_all, sizeof(salt_key_all), "salt_key_all");
#endif
    *out_len = 32;
    gwp_rpc.rpc_init_status = INIT_CHECK_CERT_HASH;
    return ret_code;
}

//extern void print_hex(const void* data, size_t length, const char* debug_info);//debug only
ret_code_t gwp_init_cmd_check_cert_hash(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len)
{
    ret_code_t ret_code = GWP_SUCCESS;
    size_t len = INIT_HASH_SIZE;
    uint8_t hash_buf[INIT_HASH_SIZE] = {0};
    VERIFY_TRUE((*out_len >= 1), RPC_ERROR_MEMORY_OUT);
    if(gwp_rpc.rpc_init_status != INIT_CHECK_CERT_HASH)
    {
        return RPC_ERROR_INIT_STATUS;
    }
    if(gwp_rpc.pdu_seq != rpc_frame->u.payload_init.pdu_seq)
    {
        return RPC_ERROR_FRAME_SEQ;
    }
    gwp_rpc.pdu_seq ++;
    if(rpc_frame->payload_length != INIT_HASH_SIZE + SEQ_SIZE + INIT_CMD_SIZE)
    {
        return RPC_ERROR_FRAME_SIZE;
    }
    memset(cert_hash_buf, 0, sizeof(cert_hash_buf));
    memcpy(cert_hash_buf, rpc_frame->u.payload_init.payload, sizeof(cert_hash_buf));
    //read out certificate
#if defined(NRF52840_XXAA)
    gwp_read_cert(cert, sizeof(cert), &cert_size);
#elif defined(STM32WB55xx) || defined(CONFIG_IDF_TARGET_ESP32S3)
//  gwp_read_cert(cert, sizeof(cert), &cert_size);
#else
#error "Architecture not set."
#endif
    if(cert_size == 0 || cert_size > MAX_CERT_SIZE )
    {
        *result = FAIL;
        gwp_rpc.rpc_init_status = INIT_SEND_CERT;
        memset(&cert_tmp_info, 0, sizeof(cert_tmp_info));
    }
    else
    {
//        print_hex(cert, sizeof(cert), "gert cert from flesh"); gwpFunctions.rngVectorGenerate
        ret_code = gwpFunctions.hash256Calculate(cert, cert_size, hash_buf, &len);
//        print_hex(hash_buf, len, "cert hash 256 info");
        if(memcmp(hash_buf, cert_hash_buf, sizeof(hash_buf)) == 0)
        {
            *result = SUCCESS;//test .
            gwp_rpc.rpc_init_status = INIT_GET_RANDOM;
        }
        else
        {
            *result = FAIL;
            gwp_rpc.rpc_init_status = INIT_SEND_CERT;
            memset(&cert_tmp_info, 0, sizeof(cert_tmp_info));
        }
    }
    *out_len = 1;
    return ret_code;
}


ret_code_t gwp_init_cmd_send_cert(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len)
{
    ret_code_t ret_code = GWP_SUCCESS;
    uint16_t cert_len = 0;
    VERIFY_TRUE((*out_len >= 1), RPC_ERROR_MEMORY_OUT);
    if(gwp_rpc.rpc_init_status != INIT_SEND_CERT)
    {
        return RPC_ERROR_INIT_STATUS;
    }
    if(gwp_rpc.pdu_seq != rpc_frame->u.payload_init.pdu_seq)
    {
        return RPC_ERROR_FRAME_SEQ;
    }
    gwp_rpc.pdu_seq ++;
    uint8_t cert_count = rpc_frame->u.payload_init.payload[PAYLOAD_CERT_COUNT_OFFSET];
    uint8_t cert_seq = rpc_frame->u.payload_init.payload[PAYLOAD_CERT_SEQ_OFFSET];
    if(cert_seq != cert_tmp_info.cert_seq)
    {
        return RPC_ERROR_CERT_SEQ;
    }
    // check is first cert data
    if(cert_seq == 0)
    {
        cert_tmp_info.cert_count = cert_count;
    }
    cert_len = rpc_frame->payload_length - SEQ_SIZE - INIT_CMD_SIZE - PAYLOAD_CERT_COUNT_SIZE - PAYLOAD_CERT_SEQ_SIZE;
    memcpy(&cert_tmp_info.cert_payload[cert_tmp_info.cert_size],
           &rpc_frame->u.payload_init.payload[PAYLOAD_CERT_DATA_OFFSET], cert_len);
    cert_tmp_info.cert_size += cert_len;
    if(cert_tmp_info.cert_count == (cert_seq + 1))
    {
        if(verify_cert_authentication(cert_tmp_info.cert_payload, cert_tmp_info.cert_size))
        {
            memcpy(cert, cert_tmp_info.cert_payload, cert_tmp_info.cert_size);
            cert_size = cert_tmp_info.cert_size;
#if defined(NRF52840_XXAA)
#ifndef CONFIG_GWP_ECCHIP_SUPPORT
            gwp_write_cert(cert_tmp_info.cert_payload, cert_tmp_info.cert_size);
#endif
#elif defined(STM32WB55xx)  || defined(CONFIG_IDF_TARGET_ESP32S3)
//#ifndef CONFIG_GWP_ECCHIP_SUPPORT
//            gwp_write_cert(cert_tmp_info.cert_payload, cert_tmp_info.cert_size);
//#endif
#else
#error "Architecture not set."
#endif
            *result = SUCCESS;
            gwp_rpc.rpc_init_status = INIT_GET_RANDOM;
        }
        else
        {
            *result = FAIL;
            gwp_rpc.rpc_init_status = INIT_SEQ_NUM;
        }
    }
    else
    {
        cert_tmp_info.cert_seq ++;
        *result = SUCCESS;
    }
    *out_len = 1;
    return ret_code;
}


ret_code_t gwp_init_cmd_get_random(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len)
{
    ret_code_t ret_code = GWP_SUCCESS;
    VERIFY_TRUE((*out_len >= INIT_RANDOM_SIZE), RPC_ERROR_MEMORY_OUT);
    if(gwp_rpc.rpc_init_status != INIT_GET_RANDOM)
    {
        return RPC_ERROR_INIT_STATUS;
    }
    if(gwp_rpc.pdu_seq != rpc_frame->u.payload_init.pdu_seq)
    {
        return RPC_ERROR_FRAME_SEQ;
    }
    gwp_rpc.pdu_seq ++;
#ifdef DEBUG_RPC_PROTOCOL
    const uint8_t fix_iv[16] = {0x51, 0xf2, 0xff, 0x13, 0x23, 0x77, 0x6a, 0x49, 0x6f, 0x36, 0x58, 0xa7, 0xd4, 0xcd, 0x99, 0xf8};
    memcpy(init_random, fix_iv, INIT_RANDOM_SIZE);
#else
//    gwpFunctions
    ret_code = gwpFunctions.rngVectorGenerate(init_random, INIT_RANDOM_SIZE);
    print_hex(init_random, sizeof(init_random), "init_random");
    print_hex(salt_key_all, sizeof(salt_key_all), "salt_key_all");
#endif /* DEBUG_RPC_PROTOCOL */
    if(ret_code != GWP_SUCCESS)
    {
        return ret_code;
    }
    memcpy(&salt_key_all[4], init_random, 4); //save 4bytes random number
    print_hex(salt_key_all, sizeof(salt_key_all), "salt_key_all");
    memcpy(result, init_random, INIT_RANDOM_SIZE);
    *out_len = INIT_RANDOM_SIZE;
    gwp_rpc.rpc_init_status = INIT_VERIFY_HOST;
    return ret_code;
}


ret_code_t gwp_init_cmd_verify_random_signature(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len)
{
    ret_code_t ret_code = GWP_SUCCESS;
    uint8_t hash[INIT_HASH_SIZE] = {0};
    size_t hash_len = INIT_HASH_SIZE;
    uint8_t raw_public_key[GWP_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE] = {0};
    gwpFunctions.getPkFromCrt  (cert, cert_size, raw_public_key, sizeof(raw_public_key));
    VERIFY_TRUE((*out_len >= 1), RPC_ERROR_MEMORY_OUT);
    if(gwp_rpc.rpc_init_status != INIT_VERIFY_HOST)
    {
        return RPC_ERROR_INIT_STATUS;
    }
    if(gwp_rpc.pdu_seq != rpc_frame->u.payload_init.pdu_seq)
    {
        return RPC_ERROR_FRAME_SEQ;
    }
    gwp_rpc.pdu_seq ++;
    // calculate  random hash256
    ret_code = gwpFunctions.hash256Calculate (init_random, sizeof(init_random), hash, &hash_len);
    if(ret_code != GWP_SUCCESS)
    {
        return ret_code;
    }
    //verify signature gwpFunctions.aesCrypt
    ret_code = gwpFunctions.usingCrtVerifySignature(cert, cert_size, hash, INIT_HASH_SIZE,
               rpc_frame->u.payload_init.payload,
               INIT_SIGNATURE_SIZE);
    //ret_code = gwp_secp256r1_verify(raw_public_key,hash,rpc_frame->u.payload_init.payload);
    if(ret_code != GWP_SUCCESS)
    {
        *result = FAIL;
    }
    else
    {
        *result = SUCCESS;
    }
    *out_len = 1;
    gwp_rpc.rpc_init_status = INIT_SEQ_NUM;
    gwp_rpc.rpc_state = RPC_STATE_START;
    return GWP_SUCCESS;
}

//const uint8_t sw_version_desc[VERSION_SIZE] = "version 0.1.2.3";

const rpc_instance_t ins_rpc = {{0x13, 0x14, 0x15, 0x16}, {0x02, 0x00}, "version 0.1.2.3"};
ret_code_t gwp_init_cmd_probe_gwp_capacity(rpc_frame_t *rpc_frame, uint8_t * result, uint16_t *out_len)
{
    ret_code_t ret_code = GWP_SUCCESS;
    rpc_info_t ins_rpc_info = {0, 0, 1, 1, 1, 1};
    VERIFY_TRUE((*out_len >= INIT_RANDOM_SIZE), RPC_ERROR_MEMORY_OUT);
    if(gwp_rpc.rpc_init_status != INIT_PROBE_CAP)
    {
        return RPC_ERROR_INIT_STATUS;
    }//INIT_CHECK_DEVICE;//INIT_PROBE_CAP
    if(gwp_rpc.pdu_seq != rpc_frame->u.payload_init.pdu_seq)
    {
        return RPC_ERROR_FRAME_SEQ;
    }
    gwp_rpc.pdu_seq ++;
//    ret_code = gwp_rng_vector_generate(init_random, INIT_RANDOM_SIZE);
//    if(ret_code != GWP_SUCCESS)
//    {
//        return ret_code;
//    }
#ifdef CONFIG_GWP_ECCHIP_SUPPORT
    ins_rpc_info.gwp_auth_cap = 1;
#endif
    ins_rpc_info.gwp_wireless_cap = 0x01;
#if (defined NONE_SELF_CONTAIN)&&(defined BOOTLOADER_CODE )
    ins_rpc_info.run_stage = 0;
#else
    ins_rpc_info.run_stage = 1;
#endif
#ifdef NONE_SELF_CONTAIN
    ins_rpc_info.self_contain = 0;
#else
    ins_rpc_info.self_contain = 1;
#endif
    memcpy(result, &ins_rpc_info, sizeof(ins_rpc_info));
    *out_len = sizeof(ins_rpc_info);
    gwp_rpc.rpc_init_status = INIT_CHECK_DEVICE;
    return ret_code;
}
