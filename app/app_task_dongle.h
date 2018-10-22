/******************************************************************************

 @file  app_task_dongle.h

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/
#ifndef __APP_TASK_DONGLE_H__
#define __APP_TASK_DONGLE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdint.h>

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/
#define APP_TASK_DONGLE_EVENT_POR                       1
//#define APP_TASK_DONGLE_EVENT_DSP_INIT                  2
//#define APP_TASK_DONGLE_EVENT_LS_BOOT_FAIL              3
//#define APP_TASK_DONGLE_EVENT_LS_ROUTER_NOT_FOUND       4


/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/


/**************************************************************************************************
 * FUNCTIONS - API
 **************************************************************************************************/

extern void AppTask_Dongle_Init ( void );
extern void AppTask_Dongle ( uint8_t task_id, uint8_t event_id );



#ifdef __cplusplus
}
#endif

#endif

/**************************************************************************************************
**************************************************************************************************/
