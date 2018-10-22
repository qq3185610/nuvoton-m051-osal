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
#include "osal_timer.h"
#include "osal_config.h"
#include "osal_port.h"
#include "osal_task.h"
#include <stdint.h>
#include <string.h>

typedef struct osal_timer_t {
    struct osal_timer_t *p_timer_prev;
    struct osal_timer_t *p_timer_next;
    void ( *p_fxn )( void * );
    void *p_arg;
    uint32_t timeout;
} OSAL_TIMER_t;

static OSAL_TIMER_t *p_timers_head;
static OSAL_TIMER_t *p_timers_tail;
static uint32_t time_sec;
static uint32_t sysclock;
static uint16_t time_ms;
static uint8_t prev_systick;

static OSAL_TIMER_t *osal_timer_list_find( OSAL_TIMER_t *p_timer )
{
    OSAL_TIMER_t *p_timer_match;
    
    OSAL_ASSERT( p_timer != NULL );
    if( p_timers_head == NULL )
        return NULL;

    //find the timer at the tail
    p_timer_match = p_timers_head;
    while( p_timer != p_timer_match )
    {
        p_timer_match = p_timer_match->p_timer_next;
        if( p_timer_match == NULL )
            return NULL;
    }
    
    return p_timer_match;
}

static void osal_timer_list_del( OSAL_TIMER_t *p_timer )
{
    OSAL_ASSERT( p_timer != NULL );
    OSAL_ASSERT( p_timers_head != NULL );
    
    if( p_timer->p_timer_prev == NULL &&
        p_timer->p_timer_next == NULL )
    {
        p_timers_head = NULL;
        p_timers_tail = NULL;
    }
    else
    {
        if( p_timer->p_timer_prev != NULL )
        {
            p_timer->p_timer_prev->p_timer_next = p_timer->p_timer_next;
            if( p_timer->p_timer_prev->p_timer_next == NULL )
            {
                p_timers_tail = p_timer->p_timer_prev;
            }
        }

        if( p_timer->p_timer_next != NULL )
        {
            p_timer->p_timer_next->p_timer_prev = p_timer->p_timer_prev;
            if( p_timer->p_timer_next->p_timer_prev == NULL )
            {
                p_timers_head = p_timer->p_timer_next;
            }
        }
    }
}

static void osal_timer_list_add( OSAL_TIMER_t *p_timer_new )
{
    p_timer_new->p_timer_next = NULL;
    if( p_timers_tail )
    {
        //append the new timer to the tail
        p_timer_new->p_timer_prev = p_timers_tail;
        p_timers_tail->p_timer_next = p_timer_new;
        p_timers_tail = p_timer_new;
    }
    else
    {
        p_timer_new->p_timer_prev = NULL;
        OSAL_ASSERT( p_timers_head == NULL );
        p_timers_head = p_timer_new;
        p_timers_tail = p_timer_new;
    }
}

static void osal_timer_event_kernel ( void *p_arg )
{
    uint16_t u16tmp;
    uint8_t task_id;
    uint8_t event_id;

    u16tmp = (uint16_t)((uint32_t)p_arg);
    task_id = HI_UINT16(u16tmp);
    event_id = LO_UINT16(u16tmp);
    osal_event_set(task_id, event_id);
}

static OSAL_TIMER_t *osal_timer_event_find( uint8_t task_id, uint8_t event_id )
{
    OSAL_TIMER_t *p_timer_match;
    void *p_arg;
    
    if( p_timers_head == NULL )
        return NULL;

    //find the timer at the tail
    p_timer_match = p_timers_head;
    p_arg = (void *)BUILD_UINT16( event_id, task_id );
    while( p_timer_match->p_fxn != osal_timer_event_kernel || p_timer_match->p_arg != p_arg )
    {
        p_timer_match = p_timer_match->p_timer_next;
        if( p_timer_match == NULL )
            return NULL;
    }
    
    return p_timer_match;
}

extern void     osal_timer_init         ( void )
{
    p_timers_head = NULL;
    p_timers_tail = NULL;
    time_sec = 0;
    sysclock = 0;
    time_ms = 0;
    prev_systick = 0;
}


extern void     osal_timer_update       ( void )
{
    OSAL_TIMER_t *p_timer_curr;
    OSAL_TIMER_t *p_timer_next;
    OSAL_TIMER_t *p_timer_head;
    OSAL_TIMER_t *p_timer_tail;
    void (*p_fxn)( void * );
    void *p_arg;
    uint8_t curr_systick;
    uint8_t delta_systick;
    
    curr_systick = osal_get_systick();

    if( curr_systick != prev_systick )
    {
        delta_systick = ( curr_systick > prev_systick ) ? ( curr_systick - prev_systick ) : ( UINT8_MAX - prev_systick + curr_systick );
        prev_systick = curr_systick;
        
        time_ms += delta_systick;
        sysclock += delta_systick;
        if( time_ms >= 1000 )
        {
            time_ms -= 1000;
            time_sec++;
        }


        if( p_timers_head )
        {
            OSAL_ASSERT( p_timers_tail != NULL );
            //save the head and tail to local
            p_timer_head = p_timers_head;
            p_timer_tail = p_timers_tail;
            
            p_timer_curr = NULL;
            p_timer_next = p_timer_head;
            while( p_timer_curr != p_timer_tail )
            {
                p_timer_curr = p_timer_next;
                p_timer_next = p_timer_next->p_timer_next;
                p_timer_curr->timeout = ( p_timer_curr->timeout >= delta_systick ) ? ( p_timer_curr->timeout - delta_systick ) : 0;
                if( p_timer_curr->timeout == 0 )
                {
                    p_fxn = p_timer_curr->p_fxn;
                    p_arg = p_timer_curr->p_arg;
                    osal_timer_list_del( p_timer_curr );
                    osal_mem_free( p_timer_curr );
                    p_fxn( p_arg );
                }
            }
            
        }
    }
}

extern uint32_t osal_timer_sysclock      ( void )
{
    return sysclock;
}

extern void    *osal_timer_cback_create ( void ( *p_fxn )( void * ), void *p_arg, uint32_t timeout_ms )
{
    OSAL_TIMER_t *p_timer_new;

    OSAL_ASSERT( p_fxn != NULL );
    OSAL_ASSERT( timeout_ms != 0 );

    p_timer_new = osal_mem_alloc( sizeof(OSAL_TIMER_t) );
    OSAL_ASSERT( p_timer_new != NULL );
    p_timer_new->p_fxn = p_fxn;
    p_timer_new->p_arg = p_arg;
    p_timer_new->timeout = timeout_ms;
    
    osal_timer_list_add( p_timer_new );
    
    return p_timer_new;
}

extern void osal_timer_cback_update ( void *timer_id, uint32_t timeout_ms )
{
    OSAL_ASSERT( timer_id != NULL );
    OSAL_ASSERT( timeout_ms != 0 );
    OSAL_ASSERT( osal_timer_list_find( (OSAL_TIMER_t *)timer_id ) != NULL );
    
    ((OSAL_TIMER_t *)timer_id)->timeout = timeout_ms;
}

extern void osal_timer_cback_delete ( void *timer_id )
{
    OSAL_ASSERT( timer_id != NULL );
    OSAL_ASSERT( osal_timer_list_find( (OSAL_TIMER_t *)timer_id ) != NULL );
    
    osal_timer_list_del( (OSAL_TIMER_t *)timer_id );
    osal_mem_free( timer_id );
}

extern uint32_t osal_timer_cback_query  ( void *timer_id )
{
    OSAL_ASSERT( timer_id != NULL );
    OSAL_ASSERT( osal_timer_list_find( (OSAL_TIMER_t *)timer_id ) != NULL );
    
    return ( (OSAL_TIMER_t *)timer_id )->timeout;
}

extern void *osal_timer_event_create ( uint8_t task_id, uint8_t event_id, uint32_t timeout_ms )
{
    OSAL_TIMER_t *p_timer_match;

    OSAL_ASSERT( task_id < OSAL_TASK_MAX );
    OSAL_ASSERT( event_id < OSAL_EVENT_MAX );
    OSAL_ASSERT( timeout_ms != 0 );
    
    p_timer_match = osal_timer_event_find( task_id, event_id );
    
    if( p_timer_match )
    {
        //if found, update it
        p_timer_match->timeout = timeout_ms;
    }
    else
    {
        //if not found, create it
        p_timer_match = (void *)osal_timer_cback_create( osal_timer_event_kernel, (void *)BUILD_UINT16( event_id, task_id ), timeout_ms);
    }
    return p_timer_match;
}

extern void *osal_timer_event_update ( uint8_t task_id, uint8_t event_id, uint32_t timeout_ms )
{
    OSAL_TIMER_t *p_timer_match;

    OSAL_ASSERT( task_id < OSAL_TASK_MAX);
    OSAL_ASSERT( event_id < OSAL_EVENT_MAX );
    OSAL_ASSERT( timeout_ms != 0 );
    
    p_timer_match = osal_timer_event_find( task_id, event_id );
    if( p_timer_match )
    {
        p_timer_match->timeout = timeout_ms;
    }
    return (void *)p_timer_match;
}

extern void  *osal_timer_event_delete ( uint8_t task_id, uint8_t event_id )
{
    OSAL_TIMER_t *p_timer_match;

    OSAL_ASSERT( task_id < OSAL_TASK_MAX);
    OSAL_ASSERT( event_id < OSAL_EVENT_MAX );
    p_timer_match = osal_timer_event_find( task_id, event_id );

    if( p_timer_match )
    {
        osal_timer_cback_delete( p_timer_match );
    }

    return (void *)p_timer_match;
}

extern uint32_t osal_timer_event_query  ( uint8_t task_id, uint8_t event_id )
{
    OSAL_TIMER_t *p_timer_match;
    
    OSAL_ASSERT( task_id < OSAL_TASK_MAX);
    OSAL_ASSERT( event_id < OSAL_EVENT_MAX );

    p_timer_match = osal_timer_event_find( task_id, event_id );
    if( p_timer_match )
    {
        return p_timer_match->timeout;
    }
    
    return 0;
}














