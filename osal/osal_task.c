/******************************************************************************

 @file  

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/
#include "osal_task.h"
#include "osal_config.h"
#include "osal_port.h"
#include "osal_msg.h"
#include <string.h>


OSAL_TCB_t osalTcbArr[ OSAL_TASK_MAX ];




extern void    osal_task_init   ( void )
{
    //OSAL_ENTER_CRITICAL();
    memset( osalTcbArr, 0, sizeof(osalTcbArr) );
    //OSAL_EXIT_CRITICAL();
}


extern void osal_task_create ( OSAL_TASK_t pfn_task, uint8_t task_id )
{
    OSAL_ASSERT( task_id < OSAL_TASK_MAX );
    OSAL_ASSERT( pfn_task != NULL );

    OSAL_ENTER_CRITICAL();
    OSAL_ASSERT( osalTcbArr[task_id].task == NULL );
    osalTcbArr[task_id].task = pfn_task;
    OSAL_EXIT_CRITICAL();
}


extern void osal_task_delete ( uint8_t task_id )
{
    void *p_msg;
    OSAL_ASSERT( task_id < OSAL_TASK_MAX );

    p_msg = osal_msg_recv( task_id );
    while( p_msg != NULL )
    {
        osal_msg_delete( p_msg );
        p_msg = osal_msg_recv( task_id );
    }
    
    OSAL_ENTER_CRITICAL();
    OSAL_ASSERT( osalTcbArr[task_id].task != NULL );
    osalTcbArr[task_id].task = NULL;
    osalTcbArr[task_id].event = 0;
    OSAL_EXIT_CRITICAL();
}



extern void osal_event_set   ( uint8_t task_id, uint8_t event_id )
{
    uint32_t event;
    
    OSAL_ASSERT( task_id < OSAL_TASK_MAX);
    OSAL_ASSERT( event_id < OSAL_EVENT_MAX );

    event = ( 1 << event_id );
    
    OSAL_ENTER_CRITICAL();
    OSAL_ASSERT( osalTcbArr[task_id].task != NULL );
    osalTcbArr[task_id].event |= event;
    OSAL_EXIT_CRITICAL();
}

extern void osal_event_clr   ( uint8_t task_id, uint8_t event_id )
{
    uint32_t event;
    
    OSAL_ASSERT( task_id < OSAL_TASK_MAX);
    OSAL_ASSERT( event_id < OSAL_EVENT_MAX );
    
    event = ( 1 << event_id );
    event = ~event;
    
    OSAL_ENTER_CRITICAL();
    OSAL_ASSERT( osalTcbArr[task_id].task != NULL );
    osalTcbArr[task_id].event &= event;
    OSAL_EXIT_CRITICAL();
    
}




















