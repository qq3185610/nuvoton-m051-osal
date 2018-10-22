/******************************************************************************

 @file  app_event_por.c

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 
 *****************************************************************************/

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "osal.h"
//#include "hal.h"
//#include "app.h"

#include "aps_cli.h"
#include "app_event_por.h"

#include <string.h>
#include "stringx.h"
/**************************************************************************************************
 * TYPE DEFINES
 **************************************************************************************************/

 /**************************************************************************************************
 * LOCAL API DECLARATION
 **************************************************************************************************/

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/


/**************************************************************************************************
 * MACROS
 **************************************************************************************************/

extern void AppEventPowerOnReset( void )
{

    ApsCliPrintStr( "\r\n\r\nPower On Reset.\r\n" );
#if 0
    AppInfo.bt0State = HAL_BT_STATE_OFF;
    AppInfo.bt1State = HAL_BT_STATE_OFF;
    AppInfo.btMode = APP_BT_MODE_OFF;
    AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_NONE;
    AppInfo.dspVol = 0;
    AppInfo.dspInited = FALSE;
    AppInfo.lsAudioSrc = HAL_LUCICMD_AUDIOSRC_NONE;
    AppInfo.lsPlayState = HAL_LUCICMD_PLAYSTAT_STOPPED;
    AppInfo.lsReady = FALSE;
    AppInfo.lsVolDevice = 0xFF;                         //impossible value 0xFF, work as not initialized flag
    AppInfo.lsVolDeviceNv = APP_NV_LS_VOL_DEVICE_DEF;   //load from eeprom
    AppInfo.lsVolZone = 0;
    AppInfo.lsWiFiMode = HAL_LUCICMD_WIFIMODE_NONE;
    AppInfo.multiplexer0ch = HAL_MULTIPLEXER_CH1;
    AppInfo.multiplexer1ch = HAL_MULTIPLEXER_CH1;
    AppInfo.volAuxIn = 0xFF;                            //impossible value 0xFF, work as not initialized flag
    AppInfo.volAuxInNv = APP_NV_VOL_AUXIN_DEF;          //load from eeprom
    AppInfo.volDisp = 0xFF;                             //impossible value 0xFF, work as not initialized flag
    
    HalLuciCmdEnable();

    HalLedSet( HAL_LED_AUX + HAL_LED_BT0 + HAL_LED_BT1 + HAL_LED_PARTY + HAL_LED_DJ, HAL_LED_MODE_OFF );
    HalBtCtrl( HAL_BT_DEVICE_0 + HAL_BT_DEVICE_1, HAL_BT_CTRL_OFF );
    HAL_LED_WIFI_IND_BOOTING();
#endif

}
 
/**************************************************************************************************
**************************************************************************************************/

