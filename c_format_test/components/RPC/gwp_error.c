
#include <stdint.h>
#include <string.h>


#if defined(NRF52840_XXAA)

    #include "nrf_log.h"

#elif defined(STM32WB55xx)

#elif defined(STM32WB55xx)

#else

    //#error "Architecture not set."

#endif





#ifdef FREERTOS
    #include "FreeRTOS.h"
    #include "task.h"
#endif

#include "gwp_error_common.h"
#include "gwp_rpc.h"
//#include "gwp_tasks_hal.h"


typedef enum GWP_DEBUG_LEVEL
{
    debug_none,
    debug_info,
    debug_worning,
    debug_error,

} GWP_DEBUG_LEVEL_ENUM;







//#define print_warning(fmt, ...)   GWP_LOG_INFO("warning@%s %d: "fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__)
//#define print_info(fmt, ...)  GWP_LOG_INFO("info@%s %d: "fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__)
//#define print_err(fmt, ...)       GWP_LOG_INFO("err@%s %d: "fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__)

//#define PRINT_LOG(fmt,args...) do{printf("FILE:[%s] --> [%s] --> LINE:[%d]:"fmt,__FILE__,__FUNCTION__,__LINE__,##args);}while(0);


/*****************************************************************************
 Prototype    : gwp_msg_auth_process_error
 Description  : GWP M1-M4 related error message printing
 Input        : GWP_DEBUG_LEVEL_ENUM     dbg_level
                unsigned char * msg
                GPW_ERROR_CODE_ALL_ENUM err_code
 Output       : None
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2021/8/12(16:24:14)
    Author       : Derek Chen
    Modification : Created function

*****************************************************************************/
void  gwp_msg_auth_process_error(  unsigned char * msg, GPW_ERROR_CODE_ALL_ENUM err_code)
{
//  GWP_DEBUG_LEVEL_ENUM dbg_level_tmp;
//  switch ( dbg_level )
//  {
//      case debug_none :
//
//          break;
//      case debug_info :
//           GWP_LOG_INFO(__VA_ARGS__);
//          break;
//      case debug_worning :
//
//          break;
//      case debug_error :
//
//          break;
//      default:
//           break;
//  }
//
}


/*****************************************************************************
 Prototype    : test_print_all
 Description  : gwp error file test function
 Input        : void
 Output       : None
 Return Value : void
 Calls        :
 Called By    :

  History        :
  1.Date         : 2021/8/13(11:03:12)
    Author       : Derek Chen
    Modification : Created function

*****************************************************************************/
void test_print_all( void )
{
//    print_warning("warning test");
    LOG_GWP_ERR(2, "ERROR2");
    LOG_GWP_ERR(4, "ERROR4");
    LOG_GWP_ERR(err_atecc608_sign, "sign error:%d", 8);
}

