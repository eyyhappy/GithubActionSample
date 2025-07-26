


#if defined(NRF52840_XXAA)


#include <stdint.h>
#include <string.h>
//#include "nrf_log.h"
#include "gwp_common.h"

#ifdef FREERTOS
    #include "FreeRTOS.h"
    #include "task.h"
#else
    #include "gwp_rpc_crypto.h"


#endif

#include "gwp_error_common.h"
#include "gwp_rpc.h"
#include "gwp_tasks_hal.h"
extern ret_code_t gwp_rpc_func_check_OTA_status(void);
#ifdef FREERTOS
TaskHandle_t pxRpcTask;
void secure_rpc_thread(void * arg)
{
    init_rpc_session_key();
    gwp_rpc_func_check_OTA_status();
    while(1)
    {
        gwp_rpc_run();
        vTaskSuspend(pxRpcTask);
    }
}
#else

uint32_t timeout_rpc = 100;
extern bool check_dfu_info(void);
void secure_rpc_thread(void)
{
    uint8_t ret;
    ret = check_dfu_info();
    GWP_LOG_INFO("check_dfu_info retern value is 0x%02x", ret);
    GWP_LOG_FLUSH();
    if ( ret == 1 )
    {
        GWP_LOG_INFO("enter bootloader rpc task");
        GWP_LOG_FLUSH();
        init_rpc_session_key();
        gwp_rpc_func_check_OTA_status();
        while(timeout_rpc)
        {
            gwp_rpc_run();
        }
    }
    GWP_LOG_INFO("exit bootloader rpc task");
    GWP_LOG_FLUSH();
}

void test_secure_rpc_thread(void)
{
    uint8_t ret;
//    ret = check_dfu_info();
    GWP_LOG_INFO("check_dfu_info retern value is 0x%02x", ret);
    GWP_LOG_FLUSH();
    if ( ret == 1 )
    {
        GWP_LOG_INFO("enter bootloader rpc task");
        GWP_LOG_FLUSH();
        init_rpc_session_key();
        gwp_rpc_func_check_OTA_status();
        while(timeout_rpc)
        {
            gwp_rpc_run();
        }
    }
    GWP_LOG_INFO("exit bootloader rpc task");
    GWP_LOG_FLUSH();
}

// do nothing h145764

#endif





//
//
//typedef enum task_enum
//{
//    simpleTask,
//    LOGGER,
//    IDLE,
//    PROV,
//    RPC,
//    BLE,
//    BLE_RECEIVE,
//    BLE_APP_SEND,
//    BLE_SECURITY_SERVICE,
//    BLE_SEND,
//    BLE_SEND_CALLBACK,
//    BLE_APP_RECEIVE,
//    BLE_OTA,
//    Tmr_Svc,
//} task_enum_def;
//
//typedef struct Task_all
//{
//    TaskFunction_t pxTaskCode;
//    const char * const pcName; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
//    const configSTACK_DEPTH_TYPE usStackDepth;
//    void * const pvParameters;
//    UBaseType_t uxPriority;
//    TaskHandle_t * const pxCreatedTask ;
//} task_all_str;
//
//const task_all_str ins_rpc_task;
//extern void periodical_simple_task_thread(void * arg);
//#define MAX_TASK_NUM       14
//const task_all_str task_all_def[MAX_TASK_NUM] =
//{
//
//    [simpleTask] =       {periodical_simple_task_thread, "simpleTask", 0x100, NULL, 1, NULL},
//    [LOGGER] =           {logger_thread, "LOGGER", 512, NULL, 1, &m_logger_thread},
//    [IDLE] =           {prvIdleTask,       configIDLE_TASK_NAME,
//                                                 ulIdleTaskStackSize,
//                                                 ( void * ) NULL,       /*lint !e961.  The cast is not redundant for all compilers. */
//                                                 portPRIVILEGE_BIT,     /* In effect ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), but tskIDLE_PRIORITY is zero. */
//                                                 pxIdleTaskStackBuffer,
//                                                 pxIdleTaskTCBBuffer},
//    [PROV] =             {prov_test_thread, "PROV", 402, NULL, 1, NULL},
//    [RPC] =              {secure_rpc_thread, "RPC", 5046, NULL, 1, &pxRpcTask},
//    [BLE] =              {softdevice_task, "BLE", GWP_BLE_FREERTOS_SDH_TASK_STACK, p_context, configMAX_PRIORITIES - 1, &m_softdevice_task},
//    [BLE_RECEIVE] =      {ble_secure_channel_thread, "BLE_RECEIVE", 1206, NULL, configMAX_PRIORITIES - 2, &m_ble_receive_handle},
//    [BLE_APP_SEND] =     {ble_app_send_data_thread, "BLE APP SEND", 512, NULL, configMAX_PRIORITIES - 2, &m_ble_app_send_handle},
//    [BLE_SECURITY_SERVIC] = {ble_defragmentation_thread, "BLE DEFRAGMENTATION SERVICE", 512, NULL, configMAX_PRIORITIES - 3, &m_defragmentation_service_handle},
//    [BLE_SEND] =         {ble_send_thread, "BLE SEND", 720, NULL, configMAX_PRIORITIES - 4, &m_ble_send_handle},
//    [BLE_SEND_CALLBACK] = {ble_send_callback_thread, "BLE SEND CALLBACK", 256, NULL, configMAX_PRIORITIES - 4, &m_ble_send_callback_handle},
//    [BLE_OTA] =          {ble_ota_thread, "BLE OAT", 2048, NULL, configMAX_PRIORITIES - 5, &m_ble_ota_handle},
//    [Tmr_Svc] =          {prvTimerTask, configTIMER_SERVICE_TASK_NAME, configTIMER_TASK_STACK_DEPTH, NULL, ( ( UBaseType_t ) configTIMER_TASK_PRIORITY ) | portPRIVILEGE_BIT, &xTimerTaskHandle}
//};



ret_code_t secure_rpc_task(void)
{
    ret_code_t ret_code = GWP_SUCCESS;
#ifdef FREERTOS
    if (pdPASS != xTaskCreate(secure_rpc_thread, "RPC", 3436, NULL, 1, &pxRpcTask))   //5120
    {
        ret_code = GWP_ERROR_NO_MEM;
    }
#endif
    return ret_code;
}




#ifdef SECURE_RPC_SUPPORT

GWP_TASK_DEFINE(gwp_tasks_t const secure_rpc) =
{
    .fn_handler = secure_rpc_task
};
#endif



#elif defined(STM32WB55xx) || defined(CONFIG_IDF_TARGET_ESP32S3)


#include <stdint.h>
#include <string.h>
//#include "nrf_log.h"
#include "gwp_common.h"

#ifdef FREERTOS
    #include "FreeRTOS.h"
    #include "task.h"
#else
    #include "gwp_rpc_crypto.h"


#endif

#include "gwp_error_common.h"
#include "gwp_rpc.h"
//#include "gwp_tasks_hal.h"
//extern ret_code_t gwp_rpc_func_check_OTA_status(void);
#ifdef FREERTOS
TaskHandle_t pxRpcTask;
void secure_rpc_thread(void * arg)
{
    init_rpc_session_key();
//    gwp_rpc_func_check_OTA_status();
    while(1)
    {
        gwp_rpc_run();
        vTaskSuspend(pxRpcTask);
    }
}
#else

uint32_t timeout_rpc = 100;
extern bool check_dfu_info(void);
void secure_rpc_thread(void)
{
    uint8_t ret;
    ret = check_dfu_info();
    GWP_LOG_INFO("check_dfu_info retern value is 0x%02x", ret);
    GWP_LOG_FLUSH();
    if ( ret == 1 )
    {
        GWP_LOG_INFO("enter bootloader rpc task");
        GWP_LOG_FLUSH();
        init_rpc_session_key();
//        gwp_rpc_func_check_OTA_status();
        while(timeout_rpc)
        {
            gwp_rpc_run();
        }
    }
    GWP_LOG_INFO("exit bootloader rpc task");
    GWP_LOG_FLUSH();
}

void test_secure_rpc_thread(void)
{
//    uint8_t ret;
//    ret = check_dfu_info();
    GWP_LOG_INFO("check_dfu_info retern value is 0x%02x", ret);
    GWP_LOG_FLUSH();
    GWP_LOG_INFO("enter bootloader rpc task");
    GWP_LOG_FLUSH();
    init_rpc_session_key();
//        gwp_rpc_func_check_OTA_status();
    while(1)//timeout_rpc
    {
        gwp_rpc_run();
    }
    GWP_LOG_INFO("exit bootloader rpc task");
    GWP_LOG_FLUSH();
}

// do nothing h145764

#endif





//
//
//typedef enum task_enum
//{
//    simpleTask,
//    LOGGER,
//    IDLE,
//    PROV,
//    RPC,
//    BLE,
//    BLE_RECEIVE,
//    BLE_APP_SEND,
//    BLE_SECURITY_SERVICE,
//    BLE_SEND,
//    BLE_SEND_CALLBACK,
//    BLE_APP_RECEIVE,
//    BLE_OTA,
//    Tmr_Svc,
//} task_enum_def;
//
//typedef struct Task_all
//{
//    TaskFunction_t pxTaskCode;
//    const char * const pcName; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
//    const configSTACK_DEPTH_TYPE usStackDepth;
//    void * const pvParameters;
//    UBaseType_t uxPriority;
//    TaskHandle_t * const pxCreatedTask ;
//} task_all_str;
//
//const task_all_str ins_rpc_task;
//extern void periodical_simple_task_thread(void * arg);
//#define MAX_TASK_NUM       14
//const task_all_str task_all_def[MAX_TASK_NUM] =
//{
//
//    [simpleTask] =       {periodical_simple_task_thread, "simpleTask", 0x100, NULL, 1, NULL},
//    [LOGGER] =           {logger_thread, "LOGGER", 512, NULL, 1, &m_logger_thread},
//    [IDLE] =           {prvIdleTask,       configIDLE_TASK_NAME,
//                                                 ulIdleTaskStackSize,
//                                                 ( void * ) NULL,       /*lint !e961.  The cast is not redundant for all compilers. */
//                                                 portPRIVILEGE_BIT,     /* In effect ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), but tskIDLE_PRIORITY is zero. */
//                                                 pxIdleTaskStackBuffer,
//                                                 pxIdleTaskTCBBuffer},
//    [PROV] =             {prov_test_thread, "PROV", 402, NULL, 1, NULL},
//    [RPC] =              {secure_rpc_thread, "RPC", 5046, NULL, 1, &pxRpcTask},
//    [BLE] =              {softdevice_task, "BLE", GWP_BLE_FREERTOS_SDH_TASK_STACK, p_context, configMAX_PRIORITIES - 1, &m_softdevice_task},
//    [BLE_RECEIVE] =      {ble_secure_channel_thread, "BLE_RECEIVE", 1206, NULL, configMAX_PRIORITIES - 2, &m_ble_receive_handle},
//    [BLE_APP_SEND] =     {ble_app_send_data_thread, "BLE APP SEND", 512, NULL, configMAX_PRIORITIES - 2, &m_ble_app_send_handle},
//    [BLE_SECURITY_SERVIC] = {ble_defragmentation_thread, "BLE DEFRAGMENTATION SERVICE", 512, NULL, configMAX_PRIORITIES - 3, &m_defragmentation_service_handle},
//    [BLE_SEND] =         {ble_send_thread, "BLE SEND", 720, NULL, configMAX_PRIORITIES - 4, &m_ble_send_handle},
//    [BLE_SEND_CALLBACK] = {ble_send_callback_thread, "BLE SEND CALLBACK", 256, NULL, configMAX_PRIORITIES - 4, &m_ble_send_callback_handle},
//    [BLE_OTA] =          {ble_ota_thread, "BLE OAT", 2048, NULL, configMAX_PRIORITIES - 5, &m_ble_ota_handle},
//    [Tmr_Svc] =          {prvTimerTask, configTIMER_SERVICE_TASK_NAME, configTIMER_TASK_STACK_DEPTH, NULL, ( ( UBaseType_t ) configTIMER_TASK_PRIORITY ) | portPRIVILEGE_BIT, &xTimerTaskHandle}
//};



ret_code_t secure_rpc_task(void)
{
    ret_code_t ret_code = GWP_SUCCESS;
#ifdef FREERTOS
    if (pdPASS != xTaskCreate(secure_rpc_thread, "RPC", 3436, NULL, 1, &pxRpcTask))   //5120
    {
        ret_code = GWP_ERROR_NO_MEM;
    }
#endif
    return ret_code;
}





//#ifdef SECURE_RPC_SUPPORT
//
//GWP_TASK_DEFINE(gwp_tasks_t const secure_rpc) =
//{
//    .fn_handler = secure_rpc_task
//};
//#endif

























#else

#error "Architecture not set."

#endif



