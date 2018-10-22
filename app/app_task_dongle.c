/******************************************************************************

 @file  app_task_dongle.c

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
#include "app_task_dongle.h"
#include "app_event_por.h"

/**************************************************************************************************
 * TYPES
 **************************************************************************************************/

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/


/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 * LOCAL FUNCTION IMPLEMENTATIONS
 **************************************************************************************************/


/**************************************************************************************************
 * PUBLIC FUNCTION IMPLEMENTATIONS
 **************************************************************************************************/
 
/**************************************************************************************************
 * @fn      AppTask_Dongle_Init
 *
 * @brief   Dongle task init
 *
 * @param   None
 *          
 * @return  None
 **************************************************************************************************/
extern void AppTask_Dongle_Init ( void )
{
    osal_timer_event_create( OSAL_TASK_ID_APP_DONGLE, APP_TASK_DONGLE_EVENT_POR, APP_POR_DELAY_TIME );
}


/**************************************************************************************************
 * @fn      AppTask_Dongle
 *
 * @brief   Dongle process task
 *
 * @param   task_id - Dongle Task Id
 *          events - events
 *
 * @return  None
 **************************************************************************************************/
void AppTask_Dongle( uint8_t task_id, uint8_t event_id )
{
    void *pMsg;

    switch ( event_id )
    {
        case OSAL_TASK_EVENT_MSG:
        {
            pMsg = osal_msg_recv( task_id );
            while ( pMsg )
            {
                osal_msg_delete( pMsg );
                pMsg = osal_msg_recv( task_id );
            }
        }
        break;

        case APP_TASK_DONGLE_EVENT_POR:
        {
            AppEventPowerOnReset();
        }
        break;

#if 0
        case APP_TASK_DONGLE_EVENT_DSP_INIT:
        {
            HalDspSendInitCode( HAL_DSP_INIT_CODE_0 );
            HalDspSetVol(0);
            HAL_DSP_SET_AUDIOSRC_NONE();
            AppInfo.dspInited = TRUE;
            HalTerminalPrintStr( "DSP Initialized" );
            HalTerminalPrintStr( ".\r\n" );
        }
        break;

        case APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL:
        {
            HalLuciCmdReset();
            HalTerminalPrintStr( "[ERROR]: LS boot failed! Rebooting.." );
            HalTerminalPrintStr( ".\r\n" );
        }
        break;

        case APP_TASK_DONGLE_EVENT_LS_ROUTER_NOT_FOUND:
        {
            HalLuciCmdSendSetWiFiModeCFG();
            HalTerminalPrintStr( "Router not found. Entering into CFG mode.." );
            HalTerminalPrintStr( ".\r\n" );
        }
        break;
#endif

        default:
            OSAL_ASSERT_FORCED();
        break;
    }
}

/**************************************************************************************************
**************************************************************************************************/

