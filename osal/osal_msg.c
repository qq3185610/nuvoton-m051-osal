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
#include "osal_msg.h"
#include "osal_config.h"
#include "osal_port.h"
#include "osal_task.h"
#include <stdint.h>
#include <string.h>

static OSAL_MSG_t *p_msg_list_head;
static OSAL_MSG_t *p_msg_list_tail;


static OSAL_MSG_t *osal_msg_list_find( OSAL_MSG_t *p_node )
{
    OSAL_MSG_t *p_node_match;
    
    if( p_msg_list_head == NULL || p_msg_list_tail == NULL )
    {
        return NULL;
    }
    
    //find the timer at the tail
    p_node_match = p_msg_list_head;
    while( p_node != p_node_match )
    {
        p_node_match = p_node_match->p_node_next;
        if( p_node_match == NULL )
            return NULL;
    }
    
    return p_node_match;
}


static void osal_msg_list_del( OSAL_MSG_t *p_node )
{
    OSAL_ASSERT( p_msg_list_head != NULL && p_msg_list_tail != NULL );
    
    if( p_node->p_node_prev == NULL &&
        p_node->p_node_next == NULL )
    {
        p_msg_list_head = NULL;
        p_msg_list_tail = NULL;
    }
    else
    {
        if( p_node->p_node_prev != NULL )
        {
            p_node->p_node_prev->p_node_next = p_node->p_node_next;
            if( p_node->p_node_prev->p_node_next == NULL )
            {
                p_msg_list_tail = p_node->p_node_prev;
            }
        }

        if( p_node->p_node_next != NULL )
        {
            p_node->p_node_next->p_node_prev = p_node->p_node_prev;
            if( p_node->p_node_next->p_node_prev == NULL )
            {
                p_msg_list_head = p_node->p_node_next;
            }
        }
    }
}

static void osal_msg_list_add( OSAL_MSG_t *p_node_new )
{
    p_node_new->p_node_next = NULL;
    if( p_msg_list_tail )
    {
        OSAL_ASSERT( p_msg_list_head != NULL );
        //append the new timer to the tail
        p_node_new->p_node_prev = p_msg_list_tail;
        p_msg_list_tail->p_node_next = p_node_new;
        p_msg_list_tail = p_node_new;
    }
    else
    {
        OSAL_ASSERT( p_msg_list_head == NULL );
        p_node_new->p_node_prev = NULL;
        p_msg_list_head = p_node_new;
        p_msg_list_tail = p_node_new;
    }
}



extern void     osal_msg_init         ( void )
{
    p_msg_list_head = NULL;
    p_msg_list_tail = NULL;
}

extern void *osal_msg_create ( uint16_t len )
{
    return osal_msg_createEx( len, 0 );
}

extern void *osal_msg_createEx ( uint16_t len, uint8_t type )
{
    OSAL_MSG_t *p_node_new;
    void *p_msg;

    if( len == 0 )
        return NULL;
    
    p_node_new = (OSAL_MSG_t *)osal_mem_alloc( sizeof(OSAL_MSG_t) + len );

    OSAL_ASSERT( p_node_new != NULL );
    
    p_msg = (void *)( (uint8_t *)p_node_new + sizeof( OSAL_MSG_t ) );
    
    p_node_new->len = len;
    p_node_new->type = type;
    
    return p_msg;
}

extern void osal_msg_delete ( void *p_msg )
{
    OSAL_MSG_t *p_node;

    OSAL_ASSERT( p_msg != NULL );

    p_node = (OSAL_MSG_t *)((uint8_t *)p_msg - sizeof(OSAL_MSG_t));
    
    OSAL_ASSERT( osal_msg_list_find( p_node ) != NULL );
    
    //kick it out of the list
    osal_msg_list_del( p_node );
    
    //free the RAM of the msg node
    osal_mem_free( p_node );
}

extern void osal_msg_send ( void *p_msg, uint8_t task_id )
{
    OSAL_MSG_t *p_node;
    
    OSAL_ASSERT( p_msg != NULL );
    p_node = (OSAL_MSG_t *)((uint8_t *)p_msg - sizeof(OSAL_MSG_t));
    OSAL_ASSERT( osal_msg_list_find( p_node ) == NULL );
    
    p_node->task_id = task_id;
    osal_msg_list_add( p_node );
    osal_event_set( task_id, OSAL_TASK_EVENT_MSG );
}

extern void osal_msg_fwrd ( void *p_msg, uint8_t task_id )
{
    OSAL_MSG_t *p_node;
    
    OSAL_ASSERT( p_msg != NULL );
    p_node = (OSAL_MSG_t *)((uint8_t *)p_msg - sizeof(OSAL_MSG_t));
    OSAL_ASSERT( osal_msg_list_find( p_node ) != NULL );
    p_node->task_id = task_id;
    osal_event_set( task_id, OSAL_TASK_EVENT_MSG );
}

extern void *osal_msg_recv ( uint8_t task_id )
{
    OSAL_MSG_t *p_node_match;
    void *p_msg;
    
    if( p_msg_list_head == NULL || p_msg_list_tail == NULL )
        return NULL;

    //find the timer at the tail
    p_node_match = p_msg_list_head;
    while( p_node_match->task_id != task_id )
    {
        p_node_match = p_node_match->p_node_next;
        if( p_node_match == NULL )
            return NULL;
    }

    p_msg = (void *)((uint8_t *)p_node_match + sizeof(OSAL_MSG_t));
    return p_msg;
}

/*
extern void osal_msg_write_head ( void *p_msg, uint8_t head )
{
    OSAL_ASSERT( p_msg != NULL );
    
    ((OSAL_MSG_t *)p_msg)->head = head;
}

extern void osal_msg_write_body ( void *p_msg, void *p_body )
{
    OSAL_ASSERT( p_msg != NULL );
    OSAL_ASSERT( p_body != NULL );
    
    if( ((OSAL_MSG_t *)p_msg)->p_body == NULL )
    {
        ((OSAL_MSG_t *)p_msg)->p_body = p_body;
    }
    else
    {
        memcpy( ((OSAL_MSG_t *)p_msg)->p_body, p_body, ((OSAL_MSG_t *)p_msg)->len );
    }
    
}

extern uint8_t osal_msg_read_head ( void *p_msg )
{
    OSAL_ASSERT( p_msg != NULL );
    return ((OSAL_MSG_t *)p_msg)->head;
}

extern void *osal_msg_read_body ( void *p_msg )
{
    OSAL_ASSERT( p_msg != NULL );
    return ((OSAL_MSG_t *)p_msg)->p_body;
}
*/


extern uint16_t osal_msg_len ( void *p_msg )
{
    OSAL_MSG_t *p_node;
    
    OSAL_ASSERT( p_msg != NULL );
    p_node = (OSAL_MSG_t *)((uint8_t *)p_msg - sizeof(OSAL_MSG_t));
    
    return p_node->len;
}

extern uint8_t osal_msg_type ( void *p_msg )
{
    OSAL_MSG_t *p_node;
    
    OSAL_ASSERT( p_msg != NULL );
    p_node = (OSAL_MSG_t *)((uint8_t *)p_msg - sizeof(OSAL_MSG_t));
    
    return p_node->type;
}





