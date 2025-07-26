


#include "gwp_common.h"




// Empty function declaration
ret_code_t EmptyFunction()
{
    printf("Empty function called\n");
    return 0;
}
// ret_code_t EmptyFunction(int num_params, ...)
// {
//     va_list args;
//     va_start(args, num_params);
//     printf("Empty function called with %d parameter(s).\n", num_params);
//     va_end(args);
//     return 0;
// }

// Initialize function pointer array structure with empty functions
GwpFunctionPointers gwpFunctions =
{
    .aesCrypt = (GwpAesCryptFunc)EmptyFunction,
    .aesCcmCrypt = (GwpAesCcmCryptFunc)EmptyFunction,
    .checksumCrc = (GwpChecksumCrcFunc)EmptyFunction,
    .checksumCrcOk = (GwpChecksumCrcOkFunc)EmptyFunction,
    .ecdhCompute = (GwpEcdhComputeFunc)EmptyFunction,
    .secp256r1Verify = (GwpSecp256r1VerifyFunc)EmptyFunction,
    .secp256r1Signature = (GwpSecp256r1SignatureFunc)EmptyFunction,
    .secp256r1GeneratePair = (GwpSecp256r1GeneratePairFunc)EmptyFunction,
    .hkdfCalculate = (GwpHkdfCalculateFunc)EmptyFunction,
    .hmac256Calculate = (GwpHmac256CalculateFunc)EmptyFunction,
    .rngVectorGenerate = (GwpRngVectorGenerateFunc)EmptyFunction,
    .hash256Init = (GwpHash256InitFunc)EmptyFunction,
    .hash256Update = (GwpHash256UpdateFunc)EmptyFunction,
    .hash256Finalize = (GwpHash256FinalizeFunc)EmptyFunction,
    .hash256Calculate = (GwpHash256CalculateFunc)EmptyFunction,
    .x509CrtVerify = (GwpX509CrtVerifyFunc)EmptyFunction,
    .usingCrtVerifySignature = (GwpUsingCrtVerifySignatureFunc)EmptyFunction,
    .getPkFromCrt = (GwpGetPkFromCrtFunc)EmptyFunction,
    .readCert = (GwpReadCertFunc)EmptyFunction,
    .writeCert = (GwpWriteCertFunc)EmptyFunction,
    .rpcPortInit = (GwpRpcPortInitFunc)EmptyFunction,
    .rpcPortSend = (GwpRpcPortSendFunc)EmptyFunction,
    .rpcPortReceive = (GwpRpcPortReceiveFunc)EmptyFunction,
};


void print_hex(const void* data, uint32_t length, const char* debug_info);
void print_hex(const void* data, uint32_t length, const char* debug_info)
{
    const unsigned char* p = (const unsigned char*) data;
    printf("%s", debug_info);
    printf(" (length=%lu):\n", (unsigned long) length);
    for (uint32_t i = 0; i < length; i++)
    {
        printf("%02x ", p[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    if (length % 16 != 0)
    {
        printf("\n");
    }
}