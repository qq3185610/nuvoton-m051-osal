/******************************************************************************

 @file  hal_bt.h

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 
 *****************************************************************************/

#ifndef __HAL_BT_H__
#define __HAL_BT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "M051Series.h"
#include "osal_comdef.h"

/*********************************************************************
 * MACROS
 */   


/*********************************************************************
 * CONSTANTS
 */
/* BT DEVICES */
#define HAL_BT_DEVICE_0             0x01
#define HAL_BT_DEVICE_1             0x02

/* BT STATES */
#define HAL_BT_STATE_OFF            0x00
#define HAL_BT_STATE_CONNECTABLE    0x01
#define HAL_BT_STATE_DISCOVERABLE   0x02
#define HAL_BT_STATE_PAUSED         0x03
#define HAL_BT_STATE_PLAYING        0x04

/* BT CTRL */
#define HAL_BT_CTRL_OFF             0x00
#define HAL_BT_CTRL_ON              0x01
#define HAL_BT_CTRL_PAIRING         0x02

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Initialize BT Service.
 */
extern void HalBtInit( void );
extern void HalBtCtrl( uint8_t dev, uint8_t ctrl );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
