/******************************************************************************

 @file  app_event_cli.c

 @brief This file contains the command line interface events handlers.

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
//#include "hal.h"
#include "aps_cli.h"
#include "app_event_cli.h"

#include <string.h>
#include "stringx.h"
/**************************************************************************************************
 * TYPE DEFINES
 **************************************************************************************************/

typedef struct cli_cmd_list_t {
    char *cmd;
    void ( *pFxn )( char *pArg );
} CLI_CMD_LIST_t;

 /**************************************************************************************************
 * LOCAL API DECLARATION
 **************************************************************************************************/
static void cmdHandler_test         ( char *pArg );
#if 0
static void cmdHandler_dsp          ( char *pArg );
static void cmdHandler_multiplexer  ( char *pArg );
static void cmdHandler_led          ( char *pArg );
static void cmdHandler_bt           ( char *pArg );
static void cmdHandler_luci         ( char *pArg );
#endif

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/
static const CLI_CMD_LIST_t cmdList[] = {
    { "test", cmdHandler_test },
#if 0
    { "dsp",  cmdHandler_dsp  },
    { "multiplexer", cmdHandler_multiplexer },
    { "led", cmdHandler_led },
    { "bt",  cmdHandler_bt },
    { "luci", cmdHandler_luci },
#endif
};



extern void AppEventCliRxCmd( char *s )
{
    char *pArg = NULL;
    char *cmd;
    uint16_t i;
    
    cmd = strtok_r( s, " ", &pArg);

    for ( i = 0; i < sizeof(cmdList)/sizeof(CLI_CMD_LIST_t); i++ )
    {
        if( strcmp( cmd, cmdList[i].cmd ) == 0 )
        {
            if( cmdList[i].pFxn != NULL )
            {
                cmdList[i].pFxn( pArg );
            }
            break;
        }
    }
}

static void cmdHandler_test( char *pArg )
{
    ApsCliPrintStr( "TEST\r\n\r\n" );
}

#if 0
static void cmdHandler_dsp( char *pArg )
{
    uint32_t u32tmp;
    char *opt;
    char *pStr;
    uint8_t src;
    uint8_t ctrl;

    pStr = NULL;
    opt = strtok_r( pArg, " ", &pStr );

    if( strcmp( opt, "-reset" ) == 0 )
    {
        pArg = strtok_r( NULL, " ", &pStr );
        if( decstr2uint( pArg, &u32tmp ) )
        {
            if( u32tmp == 0 )
            {
                HalDspResetHold();
                HalTerminalPrintStr( "OK!\r\n\r\n" );
                return;
            }

            if( u32tmp == 1 )
            {
                HalDspResetFree();
                HalTerminalPrintStr( "OK!\r\n\r\n" );
                return;
            }
        }
        
    }
    else if( strcmp( opt, "-init" ) == 0 )
    {
        pArg = strtok_r( NULL, " ", &pStr );
        if( decstr2uint( pArg, &u32tmp ) )
        {
            if( u32tmp == 0 || u32tmp == 1 )
            {
                //HalDspOpen();
                HalDspSendInitCode( (uint8_t)u32tmp );
                //HalDspClose();
                HalTerminalPrintStr( "OK!\r\n\r\n" );
                return;
            }
        }
        return;
    }
    else if( strcmp( opt, "-vol" ) == 0 )
    {
        pArg = strtok_r( NULL, " ", &pStr );
        if( decstr2uint( pArg, &u32tmp ) )
        {
            //HalDspOpen();
            HalDspSetVol( u32tmp );
            //HalDspClose();
            HalTerminalPrintStr( "OK!\r\n\r\n" );
            return;
        }
    }
    else if( strcmp( opt, "-src" ) == 0 )
    {
        pArg = strtok_r( NULL, " ", &pStr );
        if( strcmp( pArg, "SDI0" ) == 0 )
        {
            src = HAL_DSP_AUDIO_SRC_SDI0;
        }
        else if( strcmp( pArg, "SDI1" ) == 0 )
        {
            src = HAL_DSP_AUDIO_SRC_SDI1;
        }
        else if( strcmp( pArg, "ADC" ) == 0 )
        {
            src = HAL_DSP_AUDIO_SRC_ADC;
        }
        else
        {
            src = 0xFF;
        }

        if( strcmp( pStr, "ON" ) == 0 )
        {
            ctrl = HAL_DSP_CTRL_ON;
        }
        else if( strcmp( pStr, "OFF" ) == 0 )
        {
            ctrl = HAL_DSP_CTRL_OFF;
        }
        else
        {
            ctrl = 0xFF;
        }

        if( src != 0xFF && ctrl != 0xFF )
        {
            //HalDspOpen();
            HalDspAudioSrcCtrl( src, ctrl );
            //HalDspClose();
            HalTerminalPrintStr( "OK!\r\n\r\n" );
            return;
        }

        
    }

    HalTerminalPrintStr( "Bad command!\r\n\r\n" );
    
}


static void cmdHandler_multiplexer( char *pArg )
{
    uint32_t u32tmp;
    char *opt;
    char *pStr;
    uint8_t dev;
    uint8_t ch;

    pStr = NULL;
    opt = strtok_r( pArg, " ", &pStr );
    
    if( strcmp( opt, "-dev" ) == 0 )
    {
        pArg = strtok_r( NULL, " ", &pStr );

        if( decstr2uint( pArg, &u32tmp ) == 0 )
        {
            HalTerminalPrintStr( "Bad command!\r\n\r\n" );
            return;
        }
        
        if( u32tmp > 1 )
        {
            HalTerminalPrintStr( "Bad command!\r\n\r\n" );
            return;
        }
        dev = BV( u32tmp );
    }
    else
    {
        HalTerminalPrintStr( "Bad command!\r\n\r\n" );
        return;
    }


    opt = strtok_r( NULL, " ", &pStr );
    if( strcmp( opt, "-ch" ) == 0 )
    {
        pArg = strtok_r( NULL, " ", &pStr );

        if( decstr2uint( pArg, &u32tmp ) == 0 )
        {
            HalTerminalPrintStr( "Bad command!\r\n\r\n" );
            return;
        }
        
        if( u32tmp > 1 )
        {
            HalTerminalPrintStr( "Bad command!\r\n\r\n" );
            return;
        }
        ch = u32tmp;
    }
    else
    {
        HalTerminalPrintStr( "Bad command!\r\n\r\n" );
        return;
    }

    HalMultiplexerSet( dev, ch );
    HalTerminalPrintStr( "OK!\r\n\r\n" );
    
}

static void cmdHandler_led( char *pArg )
{
    uint32_t u32tmp;
    char *pOpt;
    char *pStr;
    uint16_t arg_time;
    uint8_t arg_id;
    uint8_t arg_blink;
    uint8_t arg_duty;
    uint8_t argFlags;

    argFlags = 0;
    pStr = NULL;

    pOpt = strtok_r( pArg, " ", &pStr );
    pArg = strtok_r( NULL, " ", &pStr );
    
    while( pOpt != NULL )
    {
        if( strcmp( pOpt, "-id" ) == 0 )
        {
            if( hexstr2uint( pArg, &u32tmp ) == 0 )
            {
                HalTerminalPrintStr( "Bad command!\r\n\r\n" );
                return;
            }
            arg_id = (uint8_t)u32tmp;
            argFlags |= BV(0);
        }
        else if( strcmp( pOpt, "-b" ) == 0 )
        {
            if( decstr2uint( pArg, &u32tmp ) == 0 )
            {
                HalTerminalPrintStr( "Bad command!\r\n\r\n" );
                return;
            }
            arg_blink = (uint8_t)u32tmp;
            argFlags |= BV(1);
        }
        else if( strcmp( pOpt, "-d" ) == 0 )
        {
            if( decstr2uint( pArg, &u32tmp ) == 0 )
            {
                HalTerminalPrintStr( "Bad command!\r\n\r\n" );
                return;
            }
            arg_duty = (uint8_t)u32tmp;
            argFlags |= BV(2);
        }
        else if( strcmp( pOpt, "-t" ) == 0 )
        {
            if( decstr2uint( pArg, &u32tmp ) == 0 )
            {
                HalTerminalPrintStr( "Bad command!\r\n\r\n" );
                return;
            }
            arg_time = (uint16_t)u32tmp;
            argFlags |= BV(3);
        }
        else
        {
            HalTerminalPrintStr( "Bad command!\r\n\r\n" );
            return;
        }
        pOpt = strtok_r( NULL, " ", &pStr );
        pArg = strtok_r( NULL, " ", &pStr );
    }

    if( argFlags != BV(0) + BV(1) + BV(2) + BV(3) )
    {
        HalTerminalPrintStr( "Bad command!\r\n\r\n" );
        return;
    }

    HalLedBlink( arg_id, arg_blink, arg_duty, arg_time );
    HalTerminalPrintStr( "OK!\r\n\r\n" );
    
}

static void cmdHandler_bt ( char *pArg )
{
    uint32_t u32tmp;
    char *pOpt;
    char *pStr;
    uint8_t arg_device;
    uint8_t arg_ctrl;
    uint8_t argFlags;
    const char *pCtrl[] = {
        "OFF",
        "ON",
        "PAIRING"
    };

    const uint8_t devices[] = {
        HAL_BT_DEVICE_0,
        HAL_BT_DEVICE_1
    };
    
    argFlags = 0;
    pStr = NULL;

    pOpt = strtok_r( pArg, " ", &pStr );
    pArg = strtok_r( NULL, " ", &pStr );
    
    while( pOpt != NULL )
    {
        if( strcmp( pOpt, "-d" ) == 0 )
        {
            if( decstr2uint( pArg, &u32tmp ) == 0 )
            {
                HalTerminalPrintStr( "Bad command!\r\n\r\n" );
                return;
            }

            if( u32tmp >= sizeof(devices) )
            {
                HalTerminalPrintStr( "Bad command!\r\n\r\n" );
                return;
            }
            arg_device = devices[u32tmp];
            argFlags |= BV(0);
        }
        else if( strcmp( pOpt, "-c" ) == 0 )
        {
            for( arg_ctrl = 0; arg_ctrl < sizeof(pCtrl)/sizeof(char *); arg_ctrl++ )
            {
                if( strcmp( pArg, pCtrl[arg_ctrl] ) == 0 )
                    break;
            }

            if( arg_ctrl == sizeof(pCtrl)/sizeof(char *) )
            {
                HalTerminalPrintStr( "Bad command!\r\n\r\n" );
                return;
            }
            
            argFlags |= BV(1);
        }
        else
        {
            HalTerminalPrintStr( "Bad command!\r\n\r\n" );
            return;
        }
        pOpt = strtok_r( NULL, " ", &pStr );
        pArg = strtok_r( NULL, " ", &pStr );
    }

    if( argFlags != BV(0) + BV(1) )
    {
        HalTerminalPrintStr( "Bad command!\r\n\r\n" );
        return;
    }

    HalBtCtrl( arg_device, arg_ctrl );
    HalTerminalPrintStr( "OK!\r\n\r\n" );
}

static void cmdHandler_luci ( char *pArg )
{
    char *pOpt;
    char *pStr;
    uint8_t arg_ctrl;
    uint8_t argFlags;
    const char *pCtrl[] = {
        "OFF",
        "ON",
    };
    
    
    argFlags = 0;
    pStr = NULL;

    pOpt = strtok_r( pArg, " ", &pStr );
    pArg = strtok_r( NULL, " ", &pStr );
    
    while( pOpt != NULL )
    {   
        if( strcmp( pOpt, "-c" ) == 0 )
        {
            for( arg_ctrl = 0; arg_ctrl < sizeof(pCtrl)/sizeof(char *); arg_ctrl++ )
            {
                if( strcmp( pArg, pCtrl[arg_ctrl] ) == 0 )
                    break;
            }

            if( arg_ctrl == sizeof(pCtrl)/sizeof(char *) )
            {
                HalTerminalPrintStr( "Bad command!\r\n\r\n" );
                return;
            }
            
            argFlags |= BV(0);
        }
        else
        {
            HalTerminalPrintStr( "Bad command!\r\n\r\n" );
            return;
        }
        pOpt = strtok_r( NULL, " ", &pStr );
        pArg = strtok_r( NULL, " ", &pStr );
    }

    if( argFlags != BV(0) )
    {
        HalTerminalPrintStr( "Bad command!\r\n\r\n" );
        return;
    }

    if( arg_ctrl == 0 )
        HalLuciCmdDisable();
    else
        HalLuciCmdEnable();
    
    HalTerminalPrintStr( "OK!\r\n\r\n" );
}
#endif

/**************************************************************************************************
**************************************************************************************************/

