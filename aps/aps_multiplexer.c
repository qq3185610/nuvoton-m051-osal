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
#include "osal.h"

#include "M051Series.h"
#include "hal_config.h"
#include "hal_multiplexer.h"
#include "hal_assert.h"

#define IO_CH_SEL_DEV0      P22
#define IO_CH_SEL_DEV1      P23

extern void HalMultiplexerInit( void )
{
    
    
}


extern void HalMultiplexerSet( uint8_t dev, uint8_t ch )
{
    if( dev & HAL_MULTIPLEXER_DEV0 )
    {
        IO_CH_SEL_DEV0 = ( ch == HAL_MULTIPLEXER_CH0 ) ? 0 : 1;
    }
    
    if( dev & HAL_MULTIPLEXER_DEV1 )
    {
        IO_CH_SEL_DEV1 = ( ch == HAL_MULTIPLEXER_CH0 ) ? 0 : 1;
    }
    
}



