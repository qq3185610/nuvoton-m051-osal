/******************************************************************************

 @file  app_task_bt.c

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

#include "hal_assert.h"
#include "hal_uibrd.h"
#include "hal_bt.h"

#include "app_task_bt.h"
#include "app_event_bt.h"
/**************************************************************************************************
 * TYPES
 **************************************************************************************************/

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/


/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/
static uint8_t oldBT0State;
static uint8_t oldBT1State;

/**************************************************************************************************
 * LOCAL FUNCTION IMPLEMENTATIONS
 **************************************************************************************************/
static uint8_t appTaskBtStateRemapping( uint8_t uibrdBtStateCode )
{
    switch ( uibrdBtStateCode )
    {
        case HAL_UIBRD_BTSTATE_Off:
            return HAL_BT_STATE_OFF;
        case HAL_UIBRD_BTSTATE_Connectable:
            return HAL_BT_STATE_CONNECTABLE;
        case HAL_UIBRD_BTSTATE_Discoverable:
            return HAL_BT_STATE_DISCOVERABLE;
        case HAL_UIBRD_BTSTATE_Paused:
            return HAL_BT_STATE_PAUSED;
        case HAL_UIBRD_BTSTATE_Playing:
            return HAL_BT_STATE_PLAYING;
    }

    HAL_ASSERT_FORCED();
    return 0;
}

static void appTaskBtMsgHandler( uint8_t btStates )
{
    uint8_t newBT0State;
    uint8_t newBT1State;
    
    newBT0State = appTaskBtStateRemapping( HAL_UIBRD_BT0_STATE(btStates) );
    newBT1State = appTaskBtStateRemapping( HAL_UIBRD_BT1_STATE(btStates) );

    if( newBT0State != oldBT0State )
    {
        AppEventBtStateUpdate( HAL_BT_DEVICE_0, newBT0State );
        oldBT0State = newBT0State;
    }

    if( newBT1State != oldBT1State )
    {
        AppEventBtStateUpdate( HAL_BT_DEVICE_1, newBT1State );
        oldBT1State = newBT1State;
    }
}

/**************************************************************************************************
 * PUBLIC FUNCTION IMPLEMENTATIONS
 **************************************************************************************************/
 
/**************************************************************************************************
 * @fn      AppTask_BT_Init
 *
 * @brief   Bluetooth task init
 *
 * @param   None
 *          
 * @return  None
 **************************************************************************************************/
extern void AppTask_BT_Init ( void )
{
    oldBT0State = HAL_BT_STATE_OFF;
    oldBT1State = HAL_BT_STATE_OFF;
}


/**************************************************************************************************
 * @fn      AppTask_BT
 *
 * @brief   Bluetooth process task
 *
 * @param   task_id - BT Task Id
 *          events - events
 *
 * @return  None
 **************************************************************************************************/
void AppTask_BT( uint8_t task_id, uint8_t event_id )
{
    void *pMsg;

    if( event_id == OSAL_TASK_EVENT_MSG )
    {
        pMsg = osal_msg_recv( task_id );
        
        while ( pMsg )
        {
            appTaskBtMsgHandler( *((uint8_t *)pMsg) );
            osal_msg_delete( pMsg );
            pMsg = osal_msg_recv( task_id );
        }
    }
}

/**************************************************************************************************
**************************************************************************************************/

