/******************************************************************************

 @file  app_event_key.c

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

#include "app_event_key.h"

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


extern void AppEventKeyUpdate( uint8_t keyValue, uint8_t keyEvent )
{
    uint8_t i;
    uint8_t cnt;
    
    const char *pKeyEvent[] = {
        "LEAVE",
        "ENTER",
        "SHORT",
        "LONG",
        "VLONG"
    };

    const char *pKeyName[] = {
        "WIFI",
        "BT",
        "AUX",
        "VOL-",
        "VOL+"
    };
    
    HalTerminalPrintStr( "KEY=" );
    for( i = 0, cnt = 0; i < sizeof(pKeyName)/sizeof(const char *); i++ )
    {
        if( keyValue & BV(i) )
        {
            if( cnt != 0 )
            {
                HalTerminalPrintChar( '+' );
            }
            HalTerminalPrintStr( pKeyName[i] );
            cnt++;
        }
    }
    HalTerminalPrintChar( ' ' );
    HalTerminalPrintStr( pKeyEvent[keyEvent] );
    HalTerminalPrintStr( "\r\n" );

    if( AppInfo.lsReady == FALSE )
        return;

    switch( keyValue )
    {
        case HAL_KEY_AUX:
        {
            if( keyEvent == APP_EVENT_KEY_SHORT )
            {
                if( AppInfo.dspAudioSrc != HAL_DSP_AUDIO_SRC_ADC )
                {
                    HalLuciCmdSendAuxInStart();
                    
                }
                else
                {
                    HalLuciCmdSendAuxInStop();
                }
            }
        }
        break;

        case HAL_KEY_WIFI:
        {
            if( keyEvent == APP_EVENT_KEY_SHORT )
            {
                if( AppInfo.lsWiFiMode == HAL_LUCICMD_WIFIMODE_SA )
                {
                    HalLuciCmdSendSetWiFiModeHN();
                }
                else if( AppInfo.lsWiFiMode == HAL_LUCICMD_WIFIMODE_HN )
                {
                    HalLuciCmdSendSetWiFiModeSA();
                }
                else if( AppInfo.lsWiFiMode == HAL_LUCICMD_WIFIMODE_CFG )
                {
                    HalLuciCmdSendSetWiFiModeSA();
                }
                else
                {
                    //HAL_LED_WIFI_INDICATE_MODE_NONE();
                    //AppInfo.lsWiFiMode = HAL_LUCICMD_WIFIMODE_NONE;
                    //HalLuciCmdSendSetWiFiModeSA();
                }
            }
            else if( keyEvent == APP_EVENT_KEY_LONG )
            {
                if( AppInfo.lsWiFiMode == HAL_LUCICMD_WIFIMODE_HN )
                {
                    HalLuciCmdSendSetWiFiModeCFG();
                }
                else if( AppInfo.lsWiFiMode == HAL_LUCICMD_WIFIMODE_SA )
                {
                    HalLuciCmdSendSetWiFiModeCFG();
                }
                else if( AppInfo.lsWiFiMode == HAL_LUCICMD_WIFIMODE_CFG )
                {
                    HalLuciCmdSendSetWiFiModeSA();
                }
                else
                {
                    //HAL_LED_WIFI_INDICATE_MODE_NONE();
                    //AppInfo.lsWiFiMode = HAL_LUCICMD_WIFIMODE_NONE;
                    //HalLuciCmdSendSetWiFiModeHN();
                }
            }
        }
        break;

        case HAL_KEY_BT:
        {
            if( keyEvent == APP_EVENT_KEY_SHORT )
            {
                if( AppInfo.btMode != APP_BT_MODE_OFF )
                {
                    AppInfo.btMode++;
                    if( AppInfo.btMode == APP_BT_MODE_PARTY )
                    {
                        HalBtCtrl( HAL_BT_DEVICE_0 + HAL_BT_DEVICE_1, HAL_BT_CTRL_PAIRING );
                        HAL_LED_BTMODE_IND_PARTY();
                        HAL_LED_BT0_IND_DISCOVERABLE();
                        HAL_LED_BT1_IND_DISCOVERABLE();
                        
                        HalTerminalPrintStr( "Bluetooth mode = " );
                        HalTerminalPrintStr( "PARTY.\r\n" );
                        
                    }
                    else if( AppInfo.btMode == APP_BT_MODE_DJ )
                    {
                        HalBtCtrl( HAL_BT_DEVICE_0 + HAL_BT_DEVICE_1, HAL_BT_CTRL_PAIRING );
                        HAL_LED_BTMODE_IND_DJ();
                        HAL_LED_BT0_IND_DISCOVERABLE();
                        HAL_LED_BT1_IND_DISCOVERABLE();
                        
                        HalTerminalPrintStr( "Bluetooth mode = " );
                        HalTerminalPrintStr( "DJ.\r\n" );
                    }
                    else
                    {
                        AppInfo.btMode = APP_BT_MODE_SINGLE;
                        HalBtCtrl( HAL_BT_DEVICE_0, HAL_BT_CTRL_PAIRING );
                        HalBtCtrl( HAL_BT_DEVICE_1, HAL_BT_CTRL_OFF );
                        HAL_LED_BTMODE_IND_SINGLE();
                        HAL_LED_BT0_IND_DISCOVERABLE();
                        HAL_LED_BT1_IND_OFF();
                        
                        HalTerminalPrintStr( "Bluetooth mode = " );
                        HalTerminalPrintStr( "SINGLE.\r\n" );
                    }
                    
                    
                }     
            }
            else if( keyEvent == APP_EVENT_KEY_LONG )
            {
                if( AppInfo.btMode == APP_BT_MODE_OFF )
                {
                    AppInfo.btMode = APP_BT_MODE_SINGLE;
                    HalBtCtrl( HAL_BT_DEVICE_0, HAL_BT_CTRL_PAIRING );
                    HalBtCtrl( HAL_BT_DEVICE_1, HAL_BT_CTRL_OFF );
                    HAL_LED_BTMODE_IND_SINGLE();
                    HAL_LED_BT0_IND_DISCOVERABLE();
                    HAL_LED_BT1_IND_OFF();
                    
                    HalTerminalPrintStr( "Bluetooth mode = " );
                    HalTerminalPrintStr( "SINGLE.\r\n" );
                }
                else
                {
                    AppInfo.btMode = APP_BT_MODE_OFF;
                    HalBtCtrl( HAL_BT_DEVICE_0 + HAL_BT_DEVICE_1, HAL_BT_CTRL_OFF );
                    HAL_LED_BTMODE_IND_OFF();
                    HAL_LED_BT0_IND_OFF();
                    HAL_LED_BT1_IND_OFF();
                    
                    HalTerminalPrintStr( "Bluetooth mode = " );
                    HalTerminalPrintStr( "OFF.\r\n" );
                }
            }

            if( keyEvent == APP_EVENT_KEY_SHORT || keyEvent == APP_EVENT_KEY_LONG )
            {
                if( AppInfo.dspAudioSrc != HAL_DSP_AUDIO_SRC_NONE )
                {
                    HAL_DSP_SET_AUDIOSRC_NONE();

                    //Stop AUX-IN
                    if( AppInfo.dspAudioSrc == HAL_DSP_AUDIO_SRC_ADC )
                        HalLuciCmdSendAuxInStop();

                    //Stop Wi-Fi
                    AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_NONE;
                }
            }
        }
        break;

        case HAL_KEY_VOLDN:
            if( keyEvent == APP_EVENT_KEY_SHORT )
            {
                if( AppInfo.lsAudioSrc == HAL_LUCICMD_AUDIOSRC_LineIn )
                {
                    if( AppInfo.volAuxIn > 100 )
                        AppInfo.volAuxIn = AppInfo.volAuxInNv;
                    if( AppInfo.volAuxIn > 0 )
                        AppInfo.volAuxIn--;
                    
                    if( AppInfo.volAuxIn == 100 )
                        AppInfo.dspVol = HAL_DSP_VOL_MAX;
                    else
                        AppInfo.dspVol = (HAL_DSP_VOL_MAX * AppInfo.volAuxIn)/100;
                    HalDspSetVol( AppInfo.dspVol );
                    
                    AppInfo.volDisp = AppInfo.volAuxIn;
                    HalLuciCmdSendSetVolume( AppInfo.volDisp );
                }
                else
                {
                    if( AppInfo.lsVolDevice > 100 )
                        AppInfo.lsVolDevice = AppInfo.lsVolDeviceNv;
                    if( AppInfo.lsVolDevice > 0 )
                        AppInfo.lsVolDevice--;

                    AppInfo.volDisp = AppInfo.lsVolDevice;
                    HalLuciCmdSendSetVolume( AppInfo.volDisp );
                }

                HalTerminalPrintStr( "Update display volume = " );
                HalTerminalPrintUint( AppInfo.volDisp );
                HalTerminalPrintStr( ".\r\n" );
            }
        break;

        case HAL_KEY_VOLUP:
            if( keyEvent == APP_EVENT_KEY_SHORT )
            {
                if( AppInfo.lsAudioSrc == HAL_LUCICMD_AUDIOSRC_LineIn )
                {
                    if( AppInfo.volAuxIn > 100 )
                        AppInfo.volAuxIn = AppInfo.volAuxInNv;
                    if( AppInfo.volAuxIn < 100 )
                        AppInfo.volAuxIn++;
                    
                    if( AppInfo.volAuxIn == 100 )
                        AppInfo.dspVol = HAL_DSP_VOL_MAX;
                    else
                        AppInfo.dspVol = (HAL_DSP_VOL_MAX * AppInfo.volAuxIn)/100;
                    HalDspSetVol( AppInfo.dspVol );
                    
                    AppInfo.volDisp = AppInfo.volAuxIn;
                    HalLuciCmdSendSetVolume( AppInfo.volDisp );
                }
                else
                {
                    if( AppInfo.lsVolDevice > 100 )
                        AppInfo.lsVolDevice = AppInfo.lsVolDeviceNv;
                    if( AppInfo.lsVolDevice < 100 )
                        AppInfo.lsVolDevice++;
                    

                    AppInfo.volDisp = AppInfo.lsVolDevice;
                    HalLuciCmdSendSetVolume( AppInfo.volDisp );
                }
                HalTerminalPrintStr( "Update display volume = " );
                HalTerminalPrintUint( AppInfo.volDisp );
                HalTerminalPrintStr( ".\r\n" );
            }
        break;
        
    }
}




/**************************************************************************************************
**************************************************************************************************/

