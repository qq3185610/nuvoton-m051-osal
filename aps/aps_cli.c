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

#include "hal_uart.h"

#include "aps_cli.h"

#include <string.h>
#include "stringx.h"
#include "bufmgr.h"

#define HAL_UART0_RX_BUF_SIZE       16
#define HAL_UART0_TX_BUF_SIZE       16

#define APS_CLI_TASK_EVENT_RX_CHAR  1
#define APS_CLI_TASK_EVENT_TX_CHAR  2

static OSAL_FIFO_t txFIFO;
static uint8_t uart0RxBuf[HAL_UART0_RX_BUF_SIZE];
static uint8_t uart0TxBuf[HAL_UART0_TX_BUF_SIZE];
static RING_BUF_t rxRingBuf;
static RING_BUF_t txRingBuf;
static uint8_t reportTaskId;
//#define IS_CHAR(x)  ( x > 0 && x <= 127 )

static void     apsCliTask ( uint8_t task_id, uint8_t event_id );
static void     apsCliProcessRx( void );
static void     apsCliProcessTx( void );
static void     uart0RxDataCallback( uint8_t  byte );
static uint8_t  uart0TxDataCallback( uint8_t *byte );

static void apsCliTask ( uint8_t task_id, uint8_t event_id )
{
    switch ( event_id )
    {
        case APS_CLI_TASK_EVENT_RX_CHAR:
            apsCliProcessRx();
        break;

        case APS_CLI_TASK_EVENT_TX_CHAR:
            apsCliProcessTx();
        break;
        
        default:
            OSAL_ASSERT_FORCED();
        break;
    }
}


static void apsCliProcessRx( void )
{
    char c;
    uint8_t empty;
    uint8_t cnt;
    char *pBuf;
    void *pMsg;
    
    OSAL_ENTER_CRITICAL();
    empty = ring_buf_empty( &rxRingBuf );
    OSAL_EXIT_CRITICAL();

    pBuf = (char *)osal_mem_alloc( HAL_UART0_RX_BUF_SIZE );
    OSAL_ASSERT( pBuf != NULL );
    cnt = 0;
    
    while( !empty )
    {
        OSAL_ENTER_CRITICAL();
        c = (char)ring_buf_get( &rxRingBuf );
        OSAL_EXIT_CRITICAL();

        pBuf[cnt++] = c;

        OSAL_ENTER_CRITICAL();
        empty = ring_buf_empty( &rxRingBuf );
        OSAL_EXIT_CRITICAL();
    }

    if( cnt && reportTaskId != OSAL_TASK_ID_NONE )
    {
        pMsg = osal_msg_create( cnt );
        memcpy( pMsg, pBuf, cnt );
        osal_msg_send( pMsg, reportTaskId );
    }
    
    osal_mem_free( pBuf );
}

static void apsCliProcessTx( void )
{
    uint8_t full;
    uint8_t b;
    
    if( txFIFO != NULL )
    {
        OSAL_ENTER_CRITICAL();
        full = ring_buf_full(&txRingBuf);
        OSAL_EXIT_CRITICAL();
        
        while( !full )
        {
            if( osal_fifo_len( txFIFO ) )
            {
                b = osal_fifo_get(txFIFO);
                
                OSAL_ENTER_CRITICAL();
                ring_buf_put( &txRingBuf, b );
                OSAL_EXIT_CRITICAL();
            }
            else
            {
                osal_fifo_delete( txFIFO );
                txFIFO = NULL;
                break;
            }
            
            OSAL_ENTER_CRITICAL();
            full = ring_buf_full(&txRingBuf);
            OSAL_EXIT_CRITICAL();
        }

        HalUartSendStart( HAL_UART_PORT_0 );
    }
}

static void     uart0RxDataCallback( uint8_t  byte )
{
    if( ring_buf_full( &rxRingBuf ) )
    {
        OSAL_ASSERT_FORCED();
    }
    else
    {
        ring_buf_put( &rxRingBuf, byte );
    }
    osal_event_set( OSAL_TASK_ID_APS_CLI, APS_CLI_TASK_EVENT_RX_CHAR );
}


static uint8_t  uart0TxDataCallback( uint8_t *byte )
{
    if( ring_buf_empty( &txRingBuf ) )
    {
        return FALSE;
    }

    *byte = ring_buf_get( &txRingBuf );
    osal_event_set( OSAL_TASK_ID_APS_CLI, APS_CLI_TASK_EVENT_TX_CHAR );
    return TRUE;
}


extern void ApsCliInit( void )
{
    HAL_UART_CALLBACK_t uartcb;
    
    txFIFO = NULL;
    reportTaskId = OSAL_TASK_ID_NONE;
    
    rxRingBuf.buf = uart0RxBuf;
    rxRingBuf.head = 0;
    rxRingBuf.tail = 0;
    rxRingBuf.size = HAL_UART0_RX_BUF_SIZE;

    txRingBuf.buf = uart0TxBuf;
    txRingBuf.head = 0;
    txRingBuf.tail = 0;
    txRingBuf.size = HAL_UART0_TX_BUF_SIZE;

    osal_task_create( apsCliTask, OSAL_TASK_ID_APS_CLI );

    uartcb.pfnRxData = uart0RxDataCallback;
    uartcb.pfnTxData = uart0TxDataCallback;
    HalUartInit( HAL_UART_PORT_0, &uartcb );
    HalUartOpen( HAL_UART_PORT_0 );
}

extern void ApsCliRegisterTask ( uint8_t task_id )
{
    if( reportTaskId == OSAL_TASK_ID_NONE )
    {
        reportTaskId = task_id;
    }
}

extern void ApsCliPrintChar(char c)
{
    uint8_t *pc;
    if(txFIFO == NULL)
        txFIFO = osal_fifo_create();
    OSAL_ASSERT( txFIFO != NULL );
    pc = osal_fifo_put( txFIFO, (uint8_t)c );
    OSAL_ASSERT( pc != NULL );
    osal_event_set( OSAL_TASK_ID_APS_CLI, APS_CLI_TASK_EVENT_TX_CHAR );
}

extern void ApsCliPrintStr(const char *s)
{
    while(*s)
    {
        ApsCliPrintChar(*s++);
    }
}

extern void ApsCliPrintSint(int32_t num)
{
    char str[SINT_STR_LEN_MAX];
    uint8_t len;
    uint8_t i;
    
    len = sintstr(num, str);
    
    for(i = 0; i < len; i++)
    {
        ApsCliPrintChar(str[i]);
    }
    
}

extern void ApsCliPrintUint(uint32_t num)
{
    char str[UINT_STR_LEN_MAX];
    uint8_t len;
    uint8_t i;
    
    len = uintstr(num, str);
    
    for(i = 0; i < len; i++)
    {
        ApsCliPrintChar(str[i]);
    }
}

extern void ApsCliPrintHex8(uint8_t num)
{
    char str[HEX8_STR_LEN_MAX];
    uint8_t len;
    uint8_t i;
    
    len = hex8str(num, str);
    
    for(i = 0; i < len; i++)
    {
        ApsCliPrintChar(str[i]);
    }
}


extern void ApsCliPrintHex16(uint16_t num)
{
    char str[HEX16_STR_LEN_MAX];
    uint8_t len;
    uint8_t i;
    
    len = hex16str(num, str);
    
    for(i = 0; i < len; i++)
    {
        ApsCliPrintChar(str[i]);
    }
}

extern void ApsCliPrintHex32(uint32_t num)
{
    char str[HEX32_STR_LEN_MAX];
    uint8_t len;
    uint8_t i;
    
    len = hex32str(num, str);
    
    for(i = 0; i < len; i++)
    {
        ApsCliPrintChar(str[i]);
    }
}

