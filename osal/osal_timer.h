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
#ifndef __OSAL_TIMER_H__
#define __OSAL_TIMER_H__

#include <stdint.h>
#include "osal_comdef.h"

extern void     osal_timer_init         ( void );
extern void     osal_timer_update       ( void );
extern uint32_t osal_timer_sysclock     ( void );
extern void    *osal_timer_cback_create ( void ( *p_fxn )( void * ), void *p_arg, uint32_t timeout_ms );
extern void     osal_timer_cback_update ( void *timer_id, uint32_t timeout_ms );
extern void     osal_timer_cback_delete ( void *timer_id );
extern uint32_t osal_timer_cback_query  ( void *timer_id );

extern void    *osal_timer_event_create ( uint8_t task_id, uint8_t event_id, uint32_t timeout_ms );
extern void    *osal_timer_event_update ( uint8_t task_id, uint8_t event_id, uint32_t timeout_ms );
extern void    *osal_timer_event_delete ( uint8_t task_id, uint8_t event_id );
extern uint32_t osal_timer_event_query  ( uint8_t task_id, uint8_t event_id );



#endif //__OSAL_TIMER_H__

