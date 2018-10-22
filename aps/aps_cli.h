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
 
#ifndef __HAL_TERMINAL_H__
#define __HAL_TERMINAL_H__


#include <stdint.h>

extern void ApsCliInit         ( void );
extern void ApsCliRegisterTask ( uint8_t task_id );
extern void ApsCliPrintChar    ( char c );
extern void ApsCliPrintStr     ( const char *s );
extern void ApsCliPrintSint    ( int32_t num );
extern void ApsCliPrintUint    ( uint32_t num );
extern void ApsCliPrintHex8    ( uint8_t num );
extern void ApsCliPrintHex16   ( uint16_t num );
extern void ApsCliPrintHex32   ( uint32_t num );

#endif /* __HAL_TERMINAL_H__ */
