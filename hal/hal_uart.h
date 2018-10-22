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
#ifndef __HAL_UART_H__
#define __HAL_UART_H__


#include <stdint.h>

typedef struct hal_uart_callback_t {
    void     (*pfnRxData)( uint8_t  byte );
    uint8_t  (*pfnTxData)( uint8_t *byte );
} HAL_UART_CALLBACK_t;

#define     HAL_UART_PORT_0             0
#define     HAL_UART_PORT_1             1

extern void HalUartInit( uint8_t port, const HAL_UART_CALLBACK_t *cb );
extern void HalUartOpen ( uint8_t port );
extern void HalUartSendStart( uint8_t port );
extern void HalUartClose( uint8_t port );


#endif /* __HAL_UART_H__ */

