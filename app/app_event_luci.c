/******************************************************************************

 @file  app_event_luci.c

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "osal.h"
#include "hal.h"
#include "app.h"

#include "app_event_luci.h"
#include "app_task_dongle.h"

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

static void luciCmdPrint( const HAL_LUCI_CMD_t *pLuciCmd )
{
    uint16_t i;

    const char *cmdStatusStr[] = {
        "NA",
        "OK",
        "GENERIC_ERR",
        "DEV_NOT_RDY",
        "CRC_ERR"
    };

    //print msgbox ID
    HalTerminalPrintStr("MSGBOX = ");
    if(pLuciCmd->msgBox > 255)
    {
        HalTerminalPrintStr("0x");
        HalTerminalPrintHex16( pLuciCmd->msgBox );
    }
    else
    {
        HalTerminalPrintUint( pLuciCmd->msgBox );
    }
    HalTerminalPrintStr("; ");

    //print TYPE
    HalTerminalPrintStr("TYPE = ");
    if(pLuciCmd->type == HAL_LUCICMD_TYPE_GET)
    {
        HalTerminalPrintStr("GET");
    }
    else if(pLuciCmd->type == HAL_LUCICMD_TYPE_SET)
    {
        HalTerminalPrintStr("SET");
    }
    else
    {
        HalTerminalPrintStr("0x");
        HalTerminalPrintHex8(pLuciCmd->type);
    }
    HalTerminalPrintStr("; ");

    //print status
    HalTerminalPrintStr("STATUS = ");
    if(pLuciCmd->status < sizeof(cmdStatusStr)/sizeof(char *))
        HalTerminalPrintStr( cmdStatusStr[(uint8_t)pLuciCmd->status] );
    else
        HalTerminalPrintStr( "UNKNOWN" );
    HalTerminalPrintStr("; ");
    
    //print data
    if( pLuciCmd->len )
    {
        HalTerminalPrintStr("DATA = ");
        for(i = 0; i < pLuciCmd->len; i++)
        {
            //if(cmd->msgbox > 255)
            if( pLuciCmd->pData[i] > 127 || pLuciCmd->pData[i] < 32 )
            {
                HalTerminalPrintStr( "0x" );
                HalTerminalPrintHex8( pLuciCmd->pData[i] );
                HalTerminalPrintChar(' ');
            }
            else
            {
                HalTerminalPrintChar(pLuciCmd->pData[i]);
            }
        }
    }
}

static void lsRebootProcess( void )
{
    //HalDspResetHold();
    //AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_NONE;
    //AppInfo.dspVol = 0;
    //AppInfo.dspInitied = FALSE;
    AppInfo.lsAudioSrc = HAL_LUCICMD_AUDIOSRC_NONE;
    AppInfo.lsPlayState = HAL_LUCICMD_PLAYSTAT_STOPPED;
    AppInfo.lsReady = FALSE;
    AppInfo.lsVolDevice = 0xFF;                         //impossible value 0xFF, work as not initialized flag
    //AppInfo.lsVolDeviceNv = APP_NV_LS_VOL_DEVICE_DEF; //load from eeprom
    //AppInfo.lsVolZone = 0;
    AppInfo.lsWiFiMode = HAL_LUCICMD_WIFIMODE_NONE;
    //AppInfo.multiplexer0ch = HAL_MULTIPLEXER_CH1;
    //AppInfo.multiplexer1ch = HAL_MULTIPLEXER_CH1;
    AppInfo.volAuxIn = 0xFF;                            //impossible value 0xFF, work as not initialized flag
    //AppInfo.volAuxInNv = APP_NV_VOL_AUXIN_DEF;          //load from eeprom
    AppInfo.volDisp = 0xFF;                             //impossible value 0xFF, work as not initialized flag
}

extern void AppEventLuciDefault( const HAL_LUCI_CMD_t *pLuciCmd )
{
    if( AppInfo.lsReady == FALSE )
    {
        if( osal_timer_event_query( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL ) == 0 )
            osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL, APP_LS_BOOT_TIMEOUT );
    }
    
    luciCmdPrint( pLuciCmd );
    HalTerminalPrintStr( "\r\n" );
}

extern void     AppEventLuciDevBootUp( void )
{
    HalTerminalPrintStr( "Device boot up" );
    HalTerminalPrintStr( ".\r\n" );
    AppInfo.lsReady = TRUE;

    if( AppInfo.dspInited == FALSE )
    {
        HalDspResetFree();
        osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_DSP_INIT, APP_DSP_INIT_DELAY_TIME );
    }
    osal_timer_event_delete( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL );
    osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_ROUTER_NOT_FOUND, APP_LS_ROUTER_SEARCH_TIMEOUT );
}

extern void     AppEventLuciUpdateDevVolume( uint8_t vol )
{   
    if( AppInfo.lsReady == FALSE )
    {
        if( osal_timer_event_query( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL ) == 0 )
            osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL, APP_LS_BOOT_TIMEOUT );
        return;
    }
    
    if( AppInfo.dspAudioSrc == HAL_DSP_AUDIO_SRC_ADC )
    {
        if( AppInfo.volAuxIn > 100 )
        {
            AppInfo.volAuxIn = AppInfo.volAuxInNv;
            HalLuciCmdSendSetVolume( AppInfo.volAuxIn );
        }
        else
        {
            AppInfo.volAuxIn = vol;
            AppInfo.volAuxInNv = vol;
            AppInfo.volAuxInNv = ( AppInfo.volAuxInNv < APP_NV_VOL_AUXIN_MIN ) ? APP_NV_VOL_AUXIN_MIN : AppInfo.volAuxInNv;
            AppInfo.volAuxInNv = ( AppInfo.volAuxInNv > APP_NV_VOL_AUXIN_MAX ) ? APP_NV_VOL_AUXIN_MAX : AppInfo.volAuxInNv;
        }
        AppInfo.volDisp = AppInfo.volAuxIn;
        //if AUX-In mode, use DSP to control volume
        AppInfo.dspVol = 0;
        if( AppInfo.volAuxIn == 100 )
            AppInfo.dspVol = HAL_DSP_VOL_MAX;
        else
            AppInfo.dspVol = (HAL_DSP_VOL_MAX * AppInfo.volAuxIn)/100;
        HalDspSetVol( AppInfo.dspVol );

        HalTerminalPrintStr( "DSP volume = " );
        HalTerminalPrintHex32( AppInfo.dspVol );
        HalTerminalPrintStr( ".\r\n" );
    }
    else
    {
        if( AppInfo.lsVolDevice > 100 )
        {
            AppInfo.lsVolDevice = AppInfo.lsVolDeviceNv;
            HalLuciCmdSendSetVolume( AppInfo.lsVolDevice );
        }
        else
        {
            AppInfo.lsVolDevice = vol;
            AppInfo.lsVolDeviceNv = vol;
            AppInfo.lsVolDeviceNv = ( AppInfo.lsVolDeviceNv < APP_NV_LS_VOL_DEVICE_MIN ) ? APP_NV_LS_VOL_DEVICE_MIN : AppInfo.lsVolDeviceNv;
            AppInfo.lsVolDeviceNv = ( AppInfo.lsVolDeviceNv > APP_NV_LS_VOL_DEVICE_MAX ) ? APP_NV_LS_VOL_DEVICE_MAX : AppInfo.lsVolDeviceNv;
        }

        AppInfo.volDisp = AppInfo.lsVolDevice;
        
        HalTerminalPrintStr( "LS device volume = " );
        HalTerminalPrintUint( AppInfo.lsVolDevice );
        HalTerminalPrintStr( ".\r\n" );
    }
}

extern void     AppEventLuciUpdateZoneVolume( uint8_t vol )
{
    if( AppInfo.lsReady == FALSE )
    {
        if( osal_timer_event_query( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL ) == 0 )
            osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL, APP_LS_BOOT_TIMEOUT );
        return;
    }
    
    AppInfo.lsVolZone = vol;
    HalTerminalPrintStr( "LS zone volume = " );
    HalTerminalPrintUint( AppInfo.lsVolZone );
    HalTerminalPrintStr( ".\r\n" );
}

extern void     AppEventLuciUpdateAudioSrc( uint8_t src )
{
    const char *pNames[] = {
        "None",                  // = 0,
        "Airplay",               // = 1,
        "DMR",                   // = 2,
        "DMP",                   // = 3,
        "Spotify",               // = 4,
        "USB",                   // = 5,
        "SDCard",                // = 6,
        "Melon",                 // = 7,
        "vTuner",                // = 8,
        "TuneIn",                // = 9,
        "Miracast",              // = 10,
        "Reserved",              // = 11
        "DDMS_Slave",            // = 12,
        "Reserved",              // = 13
        "LineIn",                // = 14,
        "Reserved",              // = 15
        "AppleDevice",           // = 16,
        "DirectURL",             // = 17,
        "Reserved",              // = 18
        "Bluetooth",             // = 19,
        "Reserved",              // = 20
        "Deezer",                // = 21,
        "Tidal",                 // = 22,
        "Favorites",             // = 23,
        "GoogleCast",            // = 24,
        "ExtSrcCtrlByHostMCU",   // = 25
    };

    if( AppInfo.lsReady == FALSE )
    {
        if( osal_timer_event_query( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL ) == 0 )
            osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL, APP_LS_BOOT_TIMEOUT );
        return;
    }

    AppInfo.lsAudioSrc = src;
    
    HalTerminalPrintStr( "Audio source = " );
    HalTerminalPrintStr( pNames[src] );
    HalTerminalPrintStr( ".\r\n" );

    if( src == HAL_LUCICMD_AUDIOSRC_NONE )
    {
        HAL_DSP_SET_AUDIOSRC_NONE();
        AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_NONE;
        
        HalLedSet( HAL_LED_AUX, HAL_LED_MODE_OFF );
    }
    else if( src == HAL_LUCICMD_AUDIOSRC_LineIn )
    {
        HAL_DSP_SET_AUDIOSRC_NONE();
        
        if( AppInfo.volAuxIn > 100 )
            AppInfo.volAuxIn = AppInfo.volAuxInNv;
        
        if( AppInfo.volAuxIn == 100 )
            AppInfo.dspVol = HAL_DSP_VOL_MAX;
        else
            AppInfo.dspVol = (HAL_DSP_VOL_MAX * AppInfo.volAuxIn)/100;
        HalDspSetVol( AppInfo.dspVol );
        HAL_DSP_SET_AUDIOSRC_AUX();
        AppInfo.volDisp = AppInfo.volAuxIn;
        AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_ADC;
        //Update display volume
        HalLuciCmdSendSetVolume( AppInfo.volDisp );

        HalLedSet( HAL_LED_AUX, HAL_LED_MODE_ON );
    }
    else
    {
        HAL_DSP_SET_AUDIOSRC_NONE();
        AppInfo.dspVol = HAL_DSP_VOL_MAX;
        AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_SDI0;
        AppInfo.multiplexer0ch = HAL_MULTIPLEXER_CH0;
        AppInfo.volDisp = AppInfo.lsVolDevice;
        HalDspSetVol( AppInfo.dspVol );
        HAL_DSP_SET_AUDIOSRC_WIFI();
        //Update display volume
        HalLuciCmdSendSetVolume( AppInfo.volDisp );
        HalLedSet( HAL_LED_AUX, HAL_LED_MODE_OFF );
    }
}

extern uint8_t  AppEventLuciRequestAudioSrc( uint8_t src )
{
    if( AppInfo.lsReady == FALSE )
    {
        if( osal_timer_event_query( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL ) == 0 )
            osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL, APP_LS_BOOT_TIMEOUT );
    }
    
    return TRUE;
}

extern uint8_t  AppEventLuciRequestReboot( void )
{
    HAL_LED_WIFI_IND_BOOTING();

    lsRebootProcess();
    
    HalTerminalPrintStr("LS rebooting...\r\n");
    return TRUE;
}

extern void     AppEventLuciUpdateWiFiMode( uint8_t mode )
{
    const char *pNames[] = {
            "NONE",
            "SA",
            "HN",
            "CONFIG"
    };
    
    if( AppInfo.lsReady == FALSE )
    {
        if( osal_timer_event_query( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL ) == 0 )
            osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL, APP_LS_BOOT_TIMEOUT );
        return;
    }
    
    HalTerminalPrintStr( "Wi-Fi mode = " );
    HalTerminalPrintStr( pNames[mode] );
    HalTerminalPrintStr( ".\r\n" );

    if( mode != HAL_LUCICMD_WIFIMODE_NONE )
    {
        osal_timer_event_delete( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_ROUTER_NOT_FOUND );
    }
    
    switch( mode )
    {
        case HAL_LUCICMD_WIFIMODE_NONE:
            
            
            
            if( AppInfo.lsWiFiMode == HAL_LUCICMD_WIFIMODE_CFG )
            {
                HalTerminalPrintStr( "Searching router...\r\n" );
                HAL_LED_WIFI_IND_SEARCH_ROUTER();
            }
            else
            {
                if( AppInfo.lsWiFiMode == HAL_LUCICMD_WIFIMODE_NONE )
                    //very seldom meet this case
                    HalLuciCmdSendSetWiFiModeCFG();
                if( mode != AppInfo.lsWiFiMode )
                    HAL_LED_WIFI_IND_MODE_NONE();
            }
               
        break;

        case HAL_LUCICMD_WIFIMODE_SA:
            if( mode != AppInfo.lsWiFiMode )
                HAL_LED_WIFI_IND_MODE_SA();
        break;

        case HAL_LUCICMD_WIFIMODE_HN:
            if( mode != AppInfo.lsWiFiMode )
                HAL_LED_WIFI_IND_MODE_HN();
        break;

        case HAL_LUCICMD_WIFIMODE_CFG:
            if( mode != AppInfo.lsWiFiMode )
                HAL_LED_WIFI_IND_MODE_CFG();
        break;

        default:
            HAL_ASSERT_FORCED();
        break;
    }

    AppInfo.lsWiFiMode = mode;
}

extern void     AppEventLuciUpdatePlayStatus( uint8_t status )
{
    const char *pNames[] = {
        "Playing",      // = 0,
        "Stopped",      // = 1,
        "Paused",       // = 2,
        "Connecting",   // = 3,
        "Receiving",    // = 4,
        "Buffering"     // = 5,
    };
    
    if( AppInfo.lsReady == FALSE )
    {
        if( osal_timer_event_query( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL ) == 0 )
            osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL, APP_LS_BOOT_TIMEOUT );
        return;
    }

    HalTerminalPrintStr( "Play Status = " );
    HalTerminalPrintStr( pNames[status] );
    HalTerminalPrintStr( ".\r\n" );

    AppInfo.lsPlayState = status;

    if( AppInfo.lsPlayState == HAL_LUCICMD_PLAYSTAT_PLAYING )
    {
        if( AppInfo.lsAudioSrc == HAL_LUCICMD_AUDIOSRC_LineIn )
        {
            HAL_DSP_SET_AUDIOSRC_NONE();
            
            if( AppInfo.volAuxIn > 100 )
                AppInfo.volAuxIn = AppInfo.volAuxInNv;
            
            if( AppInfo.volAuxIn == 100 )
                AppInfo.dspVol = HAL_DSP_VOL_MAX;
            else
                AppInfo.dspVol = (HAL_DSP_VOL_MAX * AppInfo.volAuxIn)/100;
            HalDspSetVol( AppInfo.dspVol );
            HAL_DSP_SET_AUDIOSRC_AUX();
            AppInfo.volDisp = AppInfo.volAuxIn;
            AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_ADC;
            HalLuciCmdSendSetVolume( AppInfo.volDisp );
        }
        else
        {
            if( AppInfo.dspAudioSrc != HAL_DSP_AUDIO_SRC_SDI0 ||
                AppInfo.multiplexer0ch != HAL_MULTIPLEXER_CH0 ||
                AppInfo.dspVol != HAL_DSP_VOL_MAX )
            {
                AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_SDI0;
                AppInfo.multiplexer0ch = HAL_MULTIPLEXER_CH0;
                AppInfo.dspVol = HAL_DSP_VOL_MAX;
                HAL_DSP_SET_AUDIOSRC_NONE();
                HAL_DSP_SET_AUDIOSRC_WIFI();
                HalDspSetVol( AppInfo.dspVol );
                HalTerminalPrintStr( "Only enable LS module audio channel.\r\n" );
            }
        }
    }
    
}

extern void     AppEventLuciUpdateNwConfigStatus( uint8_t status )
{
    const char *pNames[] = {
        "Initializing",                 // = 0,
        "Ready",                        // = 1,
        "Configuration Start",          // = 2,
        "Configuration Received",       // = 3,
        "Configuration Complete",       // = 4,
        "Stop",                         // = 5,
        "Error",                        // = 6,
        "WAC_Timeout_Message_Timeout",  // = 7,
        "WAC_Message_Config_Failed",    // = 8,
        "SAC Failed"                    // = 9,
    };

    if( AppInfo.lsReady == FALSE )
    {
        if( osal_timer_event_query( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL ) == 0 )
            osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL, APP_LS_BOOT_TIMEOUT );
        return;
    }

    HalTerminalPrintStr( "Network configuration status = " );
    HalTerminalPrintStr( pNames[status] );
    HalTerminalPrintStr( ".\r\n" );

    //if( status == HAL_LUCICMD_NWCFGSTAT_Stop )
    //{
    //    HalTerminalPrintStr( "Searching router...\r\n" );
    //    HAL_LED_WIFI_INDICATE_SEARCH_ROUTER();
    //}

    if( status >= HAL_LUCICMD_NWCFGSTAT_Error )
    {
        HAL_LED_WIFI_IND_BOOTING();
        HalTerminalPrintStr( "CFG failed. Rebooting...\r\n" );
        HalLuciCmdReset();
        lsRebootProcess();
    }
}

/**************************************************************************************************
**************************************************************************************************/

