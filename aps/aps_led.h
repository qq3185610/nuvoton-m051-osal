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

#ifndef __HAL_LED_H__
#define __HAL_LED_H__

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
#define HAL_LED_IO_INIT()       


/*********************************************************************
 * CONSTANTS
 */

#define HAL_LED_MAX_LEDS      7

/* LEDS - The LED number is the same as the bit position */
#define HAL_LED_0     BV(0)
#define HAL_LED_1     BV(1)
#define HAL_LED_2     BV(2)
#define HAL_LED_3     BV(3)
#define HAL_LED_4     BV(4)
#define HAL_LED_5     BV(5)
#define HAL_LED_6     BV(6)
//#define HAL_LED_7     BV(7)

#define HAL_LED_PARTY   HAL_LED_0
#define HAL_LED_DJ      HAL_LED_1
#define HAL_LED_BT0     HAL_LED_2
#define HAL_LED_BT1     HAL_LED_3
#define HAL_LED_AUX     HAL_LED_4
#define HAL_LED_WIFIR   HAL_LED_5
#define HAL_LED_WIFIB   HAL_LED_6

#define HAL_LED_ALL   ( HAL_LED_0 | HAL_LED_1 | HAL_LED_2 | HAL_LED_3 | HAL_LED_4 | HAL_LED_5 | HAL_LED_6 )

/* Modes */
#define HAL_LED_MODE_OFF     0x00
#define HAL_LED_MODE_ON      0x01

/* Defaults */

#define HAL_LED_DEFAULT_DUTY_CYCLE    5
#define HAL_LED_DEFAULT_FLASH_COUNT   50
#define HAL_LED_DEFAULT_FLASH_TIME    1000

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Initialize LED Service.
 */
extern void HalLedInit( void );

/*
 * Set the LED ON/OFF.
 */
extern void HalLedSet( uint8_t led, uint8_t mode );

/*
 * Blink the LED.
 */
extern void HalLedBlink( uint8_t leds, uint8_t cnt, uint8_t duty, uint16_t time );

/*
 * Put LEDs in sleep state - store current values
 */
extern void HalLedEnterSleep( void );

/*
 * Retore LEDs from sleep state
 */
extern void HalLedExitSleep( void );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
