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
#ifndef __HAL_MULTIPLEXER_H__
#define __HAL_MULTIPLEXER_H__


#include <stdint.h>

#define HAL_MULTIPLEXER_DEV0        0x01
#define HAL_MULTIPLEXER_DEV1        0x02

#define HAL_MULTIPLEXER_CH0         0
#define HAL_MULTIPLEXER_CH1         1
     

extern void HalMultiplexerInit( void );
extern void HalMultiplexerSet( uint8_t dev, uint8_t ch );


#endif
