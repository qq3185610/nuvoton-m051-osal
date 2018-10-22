/******************************************************************************

 @file  app_task_cli.c

 @brief This file contains the command line interface service.

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

#include "app_task_cli.h"
#include "app_event_cli.h"

#include <string.h>
#include "stringx.h"
/**************************************************************************************************
 * TYPES
 **************************************************************************************************/
typedef struct cli_cmd_t {
    char *str;
    uint16_t lenStr;
    uint16_t lenBuf;
} CLI_CMD_t;

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/
#define CLI_CMD_LEN_MIN     32
#define CLI_CMD_LEN_MAX     (8*CLI_CMD_LEN_MIN)

/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/
static CLI_CMD_t *pCliCmd;



extern void AppTask_CLI_Init ( void )
{
     pCliCmd = NULL;
}



static void cliProcessTerminalMsg( char *pc, uint16_t len )
{
    char c;

    while( len )
    {
        len--;
        c = *pc++;

        if( c != '\n' )
            ApsCliPrintChar( c );

        if( c != '\r' && c != '\n' )
        {
            if( pCliCmd == NULL )
            {
                pCliCmd = osal_mem_alloc( sizeof( CLI_CMD_t ) );
                OSAL_ASSERT( pCliCmd != NULL );
                pCliCmd->str = osal_mem_alloc( CLI_CMD_LEN_MIN );
                OSAL_ASSERT( pCliCmd->str != NULL);
                pCliCmd->lenStr = 0;
                pCliCmd->lenBuf = CLI_CMD_LEN_MIN;
            }

            //check if enlarger the buffer is needed
            if( pCliCmd->lenStr >= pCliCmd->lenBuf - 1 )
            {
                if( pCliCmd->lenStr < CLI_CMD_LEN_MAX )
                {
                    pCliCmd->lenBuf <<= 1;
                    pCliCmd->str = osal_mem_realloc( pCliCmd->str, pCliCmd->lenBuf );
                    OSAL_ASSERT( pCliCmd->str != NULL);
                }
            }

            if( pCliCmd->lenStr < pCliCmd->lenBuf - 1 )
            {
                pCliCmd->str[pCliCmd->lenStr++] = c;
            }
        }
        else if( c == '\r' )
        {
            ApsCliPrintChar( '\n' );
            
            if( pCliCmd != NULL )
            {
                pCliCmd->str[pCliCmd->lenStr] = '\0';

                AppEventCliRxCmd( pCliCmd->str );
                
                osal_mem_free( pCliCmd->str );
                osal_mem_free( pCliCmd );
                pCliCmd = NULL;
            }
        }
    }
    
}

/**************************************************************************************************
 * @fn      AppTask_CLI
 *
 * @brief   AppTask CLI
 *
 * @param   task_id - CLI Task Id
 *          events - events
 *
 * @return  None
 **************************************************************************************************/
void AppTask_CLI( uint8_t task_id, uint8_t event_id )
{
    void *pMsg;
    uint16_t len;

    switch ( event_id )
    {
        case OSAL_TASK_EVENT_MSG:
        {
            
            pMsg = osal_msg_recv( task_id );
            while ( pMsg )
            {
                len = osal_msg_len( pMsg );
                cliProcessTerminalMsg( (char *)pMsg, len );
                osal_msg_delete( pMsg );
                pMsg = osal_msg_recv( task_id );
            }
        }
        break;
        
        default:
            OSAL_ASSERT_FORCED();
        break;
    }
}

/**************************************************************************************************
**************************************************************************************************/

