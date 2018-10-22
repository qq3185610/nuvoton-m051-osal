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
#ifndef __OSAL_TASK_H__
#define __OSAL_TASK_H__

#include <stdint.h>
#include "osal_comdef.h"

extern void    osal_task_init       ( void );
extern void    osal_task_create     ( OSAL_TASK_t pfn_task, uint8_t task_id );
extern void    osal_task_delete     ( uint8_t task_id );

extern void    osal_event_set       ( uint8_t task_id, uint8_t event_id );
extern void    osal_event_clr       ( uint8_t task_id, uint8_t event_id );























#endif //__OSAL_TASK_H__

