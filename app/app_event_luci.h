/******************************************************************************

 @file  app_event_luci.h

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/
#ifndef __APP_EVENT_LUCI_H__
#define __APP_EVENT_LUCI_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdint.h>
#include "hal_lucicmd.h"

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/


/**************************************************************************************************
 * FUNCTIONS - API
 **************************************************************************************************/

extern void     AppEventLuciDefault( const HAL_LUCI_CMD_t *pLuciCmd );
extern void     AppEventLuciDevBootUp( void );
extern void     AppEventLuciUpdateDevVolume( uint8_t vol );
extern void     AppEventLuciUpdateZoneVolume( uint8_t vol );
extern void     AppEventLuciUpdateAudioSrc( uint8_t src );
extern uint8_t  AppEventLuciRequestAudioSrc( uint8_t src );
extern uint8_t  AppEventLuciRequestReboot( void );
extern void     AppEventLuciUpdateWiFiMode( uint8_t mode );
extern void     AppEventLuciUpdatePlayStatus( uint8_t status );
extern void     AppEventLuciUpdateNwConfigStatus( uint8_t status ); 

#ifdef __cplusplus
}
#endif

#endif

/**************************************************************************************************
**************************************************************************************************/
