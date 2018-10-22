/******************************************************************************

 @file  app.h

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/
#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdint.h>

/**************************************************************************************************
 * TYPEDEF
 **************************************************************************************************/
#if 0
typedef struct app_info_t {
    uint32_t dspVol;
    uint8_t  dspAudioSrc;
    uint8_t  dspInited;
    uint8_t  multiplexer0ch;
    uint8_t  multiplexer1ch;
    uint8_t  lsReady;
    uint8_t  lsVolZone;
    uint8_t  lsVolDevice;
    uint8_t  lsVolDeviceNv;
    uint8_t  lsWiFiMode;
    uint8_t  lsAudioSrc;
    uint8_t  lsPlayState;
    uint8_t  bt0State;
    uint8_t  bt1State;
    uint8_t  btMode;
    uint8_t  volAuxIn;
    uint8_t  volAuxInNv;
    uint8_t  volDisp;
 } APP_INFO_t;
#endif 
/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

#define APP_POR_DELAY_TIME              1000

#if 0
#define APP_DSP_INIT_DELAY_TIME         100
#define APP_LS_BOOT_TIMEOUT             20000
#define APP_LS_ROUTER_SEARCH_TIMEOUT    10000

#define APP_NV_VOL_AUXIN_DEF        50
#define APP_NV_VOL_AUXIN_MIN        20
#define APP_NV_VOL_AUXIN_MAX        100
#define APP_NV_LS_VOL_DEVICE_DEF    50
#define APP_NV_LS_VOL_DEVICE_MIN    20
#define APP_NV_LS_VOL_DEVICE_MAX    100


#define APP_BT_MODE_OFF             0
#define APP_BT_MODE_SINGLE          1
#define APP_BT_MODE_PARTY           2
#define APP_BT_MODE_DJ              3
#endif


/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/
//extern APP_INFO_t AppInfo;

/**************************************************************************************************
 * FUNCTIONS - API
 **************************************************************************************************/

extern void AppInit ( void );


#ifdef __cplusplus
}
#endif

#endif

/**************************************************************************************************
**************************************************************************************************/
