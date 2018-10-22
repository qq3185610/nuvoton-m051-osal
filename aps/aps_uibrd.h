/******************************************************************************

 @file  hal_uibrd.h

 @brief This file contains the interface to the LED Service.

 Group: 
 Target Device: 

 ******************************************************************************

 ******************************************************************************
 Release Name: 
 Release Date: 
 *****************************************************************************/

#ifndef __HAL_UIBRD_H__
#define __HAL_UIBRD_H__

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
#define HAL_UIBRD_REG_HW_VER        0x00 //READ ONLY,  len = 1, check hardware version by this REG
#define HAL_UIBRD_REG_SW_VER        0x01 //READ ONLY,  len = 1, check software version by this REG
#define HAL_UIBRD_REG_IRQ_EVENT     0x02 //READ ONLY,  len = 1, check the interrupt request events by this REG
#define HAL_UIBRD_REG_KEY_VALUE     0x03 //READ ONLY,  len = 1, check the key press/release status by this REG
#define HAL_UIBRD_REG_BT_STATE      0x04 //READ ONLY,  len = 1, check the bluetooth module working status by this REG
#define HAL_UIBRD_REG_BT_CTRL       0x05 //WRITE ONLY, len = 2, set the bluetooth working state
#define HAL_UIBRD_REG_LED_CTRL      0x06 //WRITE ONLY, len = 5, set the LED ON/OFF/BLINKING state

#define HAL_UIBRD_IRQ_KeyUpdate     0x01
#define HAL_UIBRD_IRQ_BtUpdate      0x02

#define HAL_UIBRD_BTSTATE_Off          0x07
#define HAL_UIBRD_BTSTATE_Connectable  0x03
#define HAL_UIBRD_BTSTATE_Discoverable 0x05
#define HAL_UIBRD_BTSTATE_Paused       0x01
#define HAL_UIBRD_BTSTATE_Playing      0x06

#define HAL_UIBRD_BT0_STATE(x)         (x&0xF)
#define HAL_UIBRD_BT1_STATE(x)         (x>>4)

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

 /*********************************************************************
 * FUNCTION APIs
 */
extern void HalUiBrdInit    ( void );
extern uint8_t HalUiBrdPollIRQ ( void );
extern void HalUiBrdRegRead ( uint8_t reg, uint8_t *pb, uint8_t len );
extern void HalUiBrdRegWrite( uint8_t reg, const uint8_t *pb, uint8_t len );
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
