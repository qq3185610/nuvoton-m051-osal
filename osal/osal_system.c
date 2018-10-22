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
#include "osal.h"
#include <stdint.h>



extern OSAL_TCB_t osalTcbArr[ OSAL_TASK_MAX ];
extern void HalProcessPoll( void );


extern void osal_system_init ( void )
{
    osal_mem_init();
    osal_timer_init();
    osal_task_init();
    osal_msg_init();
}

extern void osal_system_start( void )
{
    OSAL_TASK_t task;
    uint32_t event;
    uint8_t event_id;
    uint8_t index;
    
    for(;;)
    {
        osal_timer_update();
        HalProcessPoll();
        
        for( index = 0; index < OSAL_TASK_MAX; index++ )
        {
            OSAL_ENTER_CRITICAL();
            event = osalTcbArr[index].event;
            task = osalTcbArr[index].task;
            OSAL_EXIT_CRITICAL();
            
            if( event )
            {
                for( event_id = 0; event_id < OSAL_EVENT_MAX; event_id++ )
                {
                    if( event & BV( event_id ) )
                    {
                        break;
                    }
                }
                
                task( index, event_id );

                event = BV( event_id );
                event = ~event;
                
                OSAL_ENTER_CRITICAL();
                if( osalTcbArr[index].task == task )
                {
                    osalTcbArr[index].event &= event;
                }
                OSAL_EXIT_CRITICAL();
                break;
            }
            
        }
    }
}





















