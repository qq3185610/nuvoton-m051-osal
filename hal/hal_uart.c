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
#include "M051Series.h"
#include "hal_config.h"
#include "hal_uart.h"

#if HAL_UART0_EN > 0
static HAL_UART_CALLBACK_t Uart0Callback;
#endif

#if HAL_UART1_EN > 0
static HAL_UART_CALLBACK_t Uart1Callback;
#endif

extern void HalUartInit( uint8_t port, const HAL_UART_CALLBACK_t *cb )
{
#if HAL_UART0_EN > 0
    if( port == HAL_UART_PORT_0 )
    {
        Uart0Callback.pfnRxData = cb->pfnRxData;
        Uart0Callback.pfnTxData = cb->pfnTxData;
        return;
    }
#endif

#if HAL_UART1_EN > 0
    if( port == HAL_UART_PORT_1 )
    {
        Uart1Callback.pfnRxData = cb->pfnRxData;
        Uart1Callback.pfnTxData = cb->pfnTxData;
        return;
    }
#endif
}

extern void HalUartOpen ( uint8_t port )
{   
#if HAL_UART0_EN > 0
    if( port == HAL_UART_PORT_0 )
    {
        /* Reset UART0 */
        SYS_ResetModule( UART0_RST );
        /* Init UART0 to 115200-8n1 for print message */
        UART_Open( UART0, HAL_UART0_BAUDRATE );
        /* Enable Interrupt and install the call back function */
        //UART_ENABLE_INT(UART0, (UART_IER_RDA_IEN_Msk | UART_IER_THRE_IEN_Msk | UART_IER_RTO_IEN_Msk));
        UART_ENABLE_INT( UART0, ( UART_IER_RDA_IEN_Msk | UART_IER_RTO_IEN_Msk ));
        NVIC_EnableIRQ( UART0_IRQn );
        return;
    }
#endif

#if HAL_UART1_EN > 0
    if( port == HAL_UART_PORT_1 )
    {
        /* Reset UART1 */
        SYS_ResetModule( UART1_RST );
        /* Init UART1 to 115200-8n1 for print message */
        UART_Open( UART1, HAL_UART1_BAUDRATE );
        /* Enable Interrupt and install the call back function */
        //UART_ENABLE_INT(UART1, (UART_IER_RDA_IEN_Msk | UART_IER_THRE_IEN_Msk | UART_IER_RTO_IEN_Msk));
        UART_ENABLE_INT( UART1, (UART_IER_RDA_IEN_Msk | UART_IER_RTO_IEN_Msk) );
        NVIC_EnableIRQ ( UART1_IRQn );
        return;
    }
#endif
}

extern void HalUartSendStart( uint8_t port )
{
    UART_ENABLE_INT( port == HAL_UART_PORT_0 ? UART0 : UART1 , UART_IER_THRE_IEN_Msk);
}

extern void HalUartClose( uint8_t port )
{
#if HAL_UART0_EN > 0
    if( port == HAL_UART_PORT_0 )
    {
        UART_DISABLE_INT( UART0, (UART_IER_RDA_IEN_Msk | UART_IER_THRE_IEN_Msk | UART_IER_RTO_IEN_Msk) );
        NVIC_DisableIRQ( UART0_IRQn );
        UART_Close( UART0 );
        return;
    }
#endif

#if HAL_UART1_EN > 0
    if(port == HAL_UART_PORT_1)
    {
        UART_DISABLE_INT(UART1, (UART_IER_RDA_IEN_Msk | UART_IER_THRE_IEN_Msk | UART_IER_RTO_IEN_Msk));
        NVIC_DisableIRQ(UART1_IRQn);
        UART_Close(UART1);
        return;
    }
#endif
}

extern void HalUart0Isr( void )
{
	uint8_t u8tmp = 0xFF;
    uint32_t u32IntSts;
    
    u32IntSts = UART0->ISR;
    
    if( u32IntSts & UART_ISR_RDA_INT_Msk )
    {
        /* Get all the input characters */
        while( UART_IS_RX_READY( UART0 ) )
        {
            /* Get the character from UART Buffer */
            u8tmp = UART_READ( UART0 );
            if( Uart0Callback.pfnRxData != NULL )
                Uart0Callback.pfnRxData( u8tmp );
        }
    }
    
    
    if(u32IntSts & UART_ISR_THRE_INT_Msk)
    {
        if( Uart0Callback.pfnTxData != NULL )
        {
            if( Uart0Callback.pfnTxData( &u8tmp ) )
            {
                while( UART_IS_TX_FULL( UART0 ) );
                UART_WRITE( UART0, u8tmp );
                return;
            }
        }
        UART_DISABLE_INT( UART0, UART_IER_THRE_IEN_Msk );
    }
}

extern void HalUart1Isr( void )
{
	uint8_t u8tmp = 0xFF;
    uint32_t u32IntSts;
    
    u32IntSts = UART1->ISR;
    
    if( u32IntSts & UART_ISR_RDA_INT_Msk )
    {
        /* Get all the input characters */
        while( UART_IS_RX_READY( UART1 ) )
        {
            /* Get the character from UART Buffer */
            u8tmp = UART_READ( UART1 );
            if( Uart1Callback.pfnRxData != NULL )
                Uart1Callback.pfnRxData( u8tmp );
        }
    }
    
    
    if(u32IntSts & UART_ISR_THRE_INT_Msk)
    {
        if( Uart1Callback.pfnTxData != NULL )
        {
            if( Uart1Callback.pfnTxData( &u8tmp ) )
            {
                while( UART_IS_TX_FULL( UART1 ) );
                UART_WRITE( UART1, u8tmp );
                return;
            }
        }
        UART_DISABLE_INT( UART1, UART_IER_THRE_IEN_Msk );
    }
}



