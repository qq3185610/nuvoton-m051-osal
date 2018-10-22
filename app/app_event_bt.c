/******************************************************************************

 @file  app_event_luci.c

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
#include "hal.h"
#include "app.h"

#include "app_event_bt.h"

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


extern void AppEventBtStateUpdate( uint8_t dev, uint8_t state )
{
    uint8_t devid;
    uint8_t stateOld;
    
    const char *pNames[] = {
        "OFF",
        "CONNECTABLE",
        "DISCOVERABLE",
        "CONNECTED(PAUSED)",
        "CONNECTED(PALYING)"
    };

    devid = 0xFF;
    stateOld = 0xFF;
    if( dev == HAL_BT_DEVICE_0 )
    {
        devid = 0;
        stateOld = AppInfo.bt0State;
        AppInfo.bt0State = state;
    }
    else if( dev == HAL_BT_DEVICE_1 )
    {
        devid = 1;
        stateOld = AppInfo.bt1State;
        AppInfo.bt1State = state;
    }
    
    HalTerminalPrintStr( "BT" );
    HalTerminalPrintUint( devid );
    HalTerminalPrintStr( " state = " );
    HalTerminalPrintStr( pNames[state] );
    HalTerminalPrintStr( ".\r\n" );
    
    if( AppInfo.btMode == APP_BT_MODE_OFF )
    {
        if( state != HAL_BT_STATE_OFF )
        {
            HalBtCtrl( HAL_BT_DEVICE_0 + HAL_BT_DEVICE_1, HAL_BT_CTRL_OFF );
            HalTerminalPrintStr( "Turn OFF all BT.\r\n" );
        }
        return;
    }
    
    if( AppInfo.btMode == APP_BT_MODE_SINGLE && dev == HAL_BT_DEVICE_0 )
    {
        switch ( state )
        {
            case HAL_BT_STATE_PAUSED:
                HAL_LED_BT0_IND_CONNECTED_PAUSED();
            break;

            case HAL_BT_STATE_PLAYING:
                HAL_LED_BT0_IND_CONNECTED_PLAYING();
                if( AppInfo.dspAudioSrc != HAL_DSP_AUDIO_SRC_SDI0 || 
                    AppInfo.multiplexer0ch != HAL_MULTIPLEXER_CH1 ||
                    AppInfo.dspVol != HAL_DSP_VOL_MAX )
                {
                    AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_SDI0;
                    AppInfo.multiplexer0ch = HAL_MULTIPLEXER_CH1;
                    AppInfo.dspVol = HAL_DSP_VOL_MAX;
                    HAL_DSP_SET_AUDIOSRC_NONE();
                    HAL_DSP_SET_AUDIOSRC_BT0();
                    HalDspSetVol( AppInfo.dspVol );
                    HalTerminalPrintStr( "Only use BT0 audio channel.\r\n" );
                }
            break;

            case HAL_BT_STATE_CONNECTABLE:

                if( stateOld == HAL_BT_STATE_PAUSED || stateOld == HAL_BT_STATE_PLAYING )
                {
                    HAL_LED_BT0_IND_DISCOVERABLE();
                    HalBtCtrl( HAL_BT_DEVICE_0, HAL_BT_CTRL_OFF );
                    HalBtCtrl( HAL_BT_DEVICE_0, HAL_BT_CTRL_PAIRING );
                    HalTerminalPrintStr( "BT0 entering into pairing.\r\n" );
                }
            break;
        }

        return;
    }

    if( AppInfo.btMode == APP_BT_MODE_DJ )
    {
        switch ( state )
        {
            case HAL_BT_STATE_PAUSED:
                if( dev == HAL_BT_DEVICE_0 )
                    HAL_LED_BT0_IND_CONNECTED_PAUSED();
                else
                    HAL_LED_BT1_IND_CONNECTED_PAUSED();
            break;

            case HAL_BT_STATE_PLAYING:
                if( dev == HAL_BT_DEVICE_0 )
                    HAL_LED_BT0_IND_CONNECTED_PLAYING();
                else
                    HAL_LED_BT1_IND_CONNECTED_PLAYING();

                if( AppInfo.dspAudioSrc    != HAL_DSP_AUDIO_SRC_SDI0 + HAL_DSP_AUDIO_SRC_SDI1 ||
                    AppInfo.multiplexer0ch != HAL_MULTIPLEXER_CH1                             ||
                    AppInfo.multiplexer1ch != HAL_MULTIPLEXER_CH1                             ||
                    AppInfo.dspVol         != HAL_DSP_VOL_MAX )
                {
                    AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_SDI0 + HAL_DSP_AUDIO_SRC_SDI1;
                    AppInfo.multiplexer0ch = HAL_MULTIPLEXER_CH1;
                    AppInfo.multiplexer1ch = HAL_MULTIPLEXER_CH1;
                    AppInfo.dspVol = HAL_DSP_VOL_MAX;
                    HAL_DSP_SET_AUDIOSRC_NONE();
                    HAL_DSP_SET_AUDIOSRC_BT0();
                    HAL_DSP_SET_AUDIOSRC_BT1();
                    HalDspSetVol( AppInfo.dspVol );
                    HalTerminalPrintStr( "Only use BT0 + BT1 audio channel.\r\n" );
                }
            break;

            case HAL_BT_STATE_CONNECTABLE:

                if( stateOld == HAL_BT_STATE_PAUSED || stateOld == HAL_BT_STATE_PLAYING )
                {
                    if( dev == HAL_BT_DEVICE_0 )
                        HAL_LED_BT0_IND_DISCOVERABLE();
                    else
                        HAL_LED_BT1_IND_DISCOVERABLE();

                    HalBtCtrl( dev, HAL_BT_CTRL_OFF );
                    HalBtCtrl( dev, HAL_BT_CTRL_PAIRING );
                    HalTerminalPrintStr( "BT" );
                    HalTerminalPrintUint( devid );
                    HalTerminalPrintStr( " entering into pairing.\r\n" );
                }
            break;
        }

        return;
    }


    if( AppInfo.btMode == APP_BT_MODE_PARTY )
    {
        switch( state )
        {
            case HAL_BT_STATE_PAUSED:
                if( dev == HAL_BT_DEVICE_0 )
                {
                    HAL_LED_BT0_IND_CONNECTED_PAUSED();
                    if( AppInfo.bt1State != HAL_BT_STATE_DISCOVERABLE )
                    {
                        HalTerminalPrintStr( "BT0 is connected. Let BT1 enter pairing mode.\r\n" );
                        HalBtCtrl( HAL_BT_DEVICE_1, HAL_BT_CTRL_PAIRING );
                    }
                }
                else
                {
                    HAL_LED_BT1_IND_CONNECTED_PAUSED();
                    if( AppInfo.bt0State != HAL_BT_STATE_DISCOVERABLE )
                    {
                        HalTerminalPrintStr( "BT1 is connected. Let BT0 enter pairing mode.\r\n" );
                        HalBtCtrl( HAL_BT_DEVICE_0, HAL_BT_CTRL_PAIRING );
                    }
                }
            break;

            case HAL_BT_STATE_PLAYING:
                if( dev == HAL_BT_DEVICE_0 )
                {
                    HAL_LED_BT0_IND_CONNECTED_PLAYING();
                    if( AppInfo.dspAudioSrc    != HAL_DSP_AUDIO_SRC_SDI0  ||
                        AppInfo.multiplexer0ch != HAL_MULTIPLEXER_CH1     ||
                        AppInfo.dspVol         != HAL_DSP_VOL_MAX )
                    {
                        AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_SDI0;
                        AppInfo.multiplexer0ch = HAL_MULTIPLEXER_CH1;
                        AppInfo.dspVol = HAL_DSP_VOL_MAX;
                        HAL_DSP_SET_AUDIOSRC_NONE();
                        HAL_DSP_SET_AUDIOSRC_BT0();
                        HalDspSetVol( AppInfo.dspVol );
                        HalTerminalPrintStr( "Only use BT0 audio channel.\r\n" );
                    }
                }
                else
                {
                    HAL_LED_BT1_IND_CONNECTED_PLAYING();
                    if( AppInfo.dspAudioSrc    != HAL_DSP_AUDIO_SRC_SDI1  ||
                        AppInfo.multiplexer1ch != HAL_MULTIPLEXER_CH1     ||
                        AppInfo.dspVol         != HAL_DSP_VOL_MAX )
                    {
                        AppInfo.dspAudioSrc = HAL_DSP_AUDIO_SRC_SDI1;
                        AppInfo.multiplexer1ch = HAL_MULTIPLEXER_CH1;
                        AppInfo.dspVol = HAL_DSP_VOL_MAX;
                        HAL_DSP_SET_AUDIOSRC_NONE();
                        HAL_DSP_SET_AUDIOSRC_BT1();
                        HalDspSetVol( AppInfo.dspVol );
                        HalTerminalPrintStr( "Only use BT1 audio channel.\r\n" );
                    }
                }
            break;

            case HAL_BT_STATE_CONNECTABLE:
                if( stateOld == HAL_BT_STATE_PAUSED || stateOld == HAL_BT_STATE_PLAYING )
                {
                    if( dev == HAL_BT_DEVICE_0 )
                        HAL_LED_BT0_IND_DISCOVERABLE();
                    else
                        HAL_LED_BT1_IND_DISCOVERABLE();

                    HalBtCtrl( dev, HAL_BT_CTRL_OFF );
                    HalBtCtrl( dev, HAL_BT_CTRL_PAIRING );
                    HalTerminalPrintStr( "BT" );
                    HalTerminalPrintUint( devid );
                    HalTerminalPrintStr( " entering into pairing.\r\n" );
                }
            break;
        }
    }
}
 
/**************************************************************************************************
**************************************************************************************************/

