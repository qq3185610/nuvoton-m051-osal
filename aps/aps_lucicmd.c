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
#include "hal_lucicmd.h"
#include "hal_assert.h"
#include "hal_uart.h"
#include "hal_terminal.h"

#include "app_task_lucicop.h"

#include <string.h>
#include "stringx.h"

#define HAL_LUCI_CMD_HEAD_LEN             10
#define HAL_LUCI_CMD_BUFFER_SIZE          (256+2)
#define HAL_LUCI_CMD_TASK_EVENT_ENABLE    3

#define HAL_LUCI_RESET_LOW()        P11 = 0
#define HAL_LUCI_RESET_HIGH()       P11 = 1

typedef struct rx_ctrl_t {
    uint16_t cnt;
    uint8_t pb[HAL_LUCI_CMD_BUFFER_SIZE];
} RX_CTRL_t;


static OSAL_FIFO_t txFIFO;
static RX_CTRL_t *rxCtrl;

static void halLuciCmdTask ( uint8_t task_id, uint8_t event_id );
static void halLuciCmdProcessRx( void );
static void halLuciCmdProcessTx( void );
static void halLuciCmdSendByte( uint8_t b );
static void halLuciCmdSend( const HAL_LUCI_CMD_t *pLuciCmd );
static uint16_t halLuciCmdCRC16_ccitt( const HAL_LUCI_CMD_t *pLuciCmd );

static void halLuciCmdTask ( uint8_t task_id, uint8_t event_id )
{
    switch ( event_id )
    {
        case HAL_UART1_TASK_EVENT_POLL_RX:
            halLuciCmdProcessRx();
        break;

        case HAL_UART1_TASK_EVENT_POLL_TX:
            halLuciCmdProcessTx();
        break;

        case HAL_LUCI_CMD_TASK_EVENT_ENABLE:
            HalLuciCmdEnable();
        break;
        
        default:
            HAL_ASSERT_FORCED();
        break;
    }
}


static void halLuciCmdProcessRx( void )
{
    HAL_LUCI_CMD_t *pLuciCmd;
    uint16_t dataLen;
    uint16_t crc;
    uint8_t b;
    uint8_t len;
    uint8_t i;
    
    len = HalUartQryRxBuf( HAL_UART_PORT_1 );

    for( i = 0; i < len; i++ )
    {
        b = HalUartGet( HAL_UART_PORT_1 );
        
        if( rxCtrl == NULL )
        {
            //check if the first byte is 0xAA
            if( b == 0xAA )
            {
                //YES! Maybe received a new luci command, allocate the RX ctrl space
                rxCtrl = osal_mem_alloc( sizeof(RX_CTRL_t) );
                HAL_ASSERT( rxCtrl != NULL );
                //memset( rxCtrl, 0, sizeof(RX_CTRL_t) );
                rxCtrl->cnt = 0;
                rxCtrl->pb[rxCtrl->cnt++] = b;
            }
        }
        else
        {
            //head receive process
            if( rxCtrl->cnt < HAL_LUCI_CMD_BUFFER_SIZE )
            {
                rxCtrl->pb[rxCtrl->cnt++] = b;
                if( rxCtrl->cnt == 2 )
                {
                    if( b != 0xAA )
                    {
                        osal_mem_free( rxCtrl );
                        rxCtrl = NULL;
                    }
                }
            }
            else
            {
                //overflow, need to enlarge rx buffer
                //HAL_ASSERT_FORCED();
                //data corruption
                osal_mem_free( rxCtrl );
                rxCtrl = NULL;

                HalTerminalPrintStr("[ERROR]: Luci CMD overflow!\r\n");
            }

            if( rxCtrl != NULL )
            {
                if( rxCtrl->cnt >= HAL_LUCI_CMD_HEAD_LEN )
                {
                    dataLen = BUILD_UINT16( rxCtrl->pb[9], rxCtrl->pb[8] );
                    if( rxCtrl->cnt == HAL_LUCI_CMD_HEAD_LEN + dataLen )
                    {
                        //cmd receive finished
                        if( dataLen )
                        {
                            pLuciCmd = (HAL_LUCI_CMD_t *)osal_msg_create( sizeof(HAL_LUCI_CMD_t) + dataLen + 1 );//+1 for '\0'
                        }
                        else
                        {
                            pLuciCmd = (HAL_LUCI_CMD_t *)osal_msg_create( sizeof(HAL_LUCI_CMD_t) );
                        }
                        
                        pLuciCmd->remoteID = BUILD_UINT16( rxCtrl->pb[1], rxCtrl->pb[0] );
                        pLuciCmd->type = rxCtrl->pb[2];
                        pLuciCmd->msgBox = BUILD_UINT16( rxCtrl->pb[4], rxCtrl->pb[3] );
                        pLuciCmd->status = rxCtrl->pb[5];
                        pLuciCmd->crc = BUILD_UINT16( rxCtrl->pb[7], rxCtrl->pb[6] );
                        pLuciCmd->len = dataLen;
                        if( dataLen )
                        {
                            pLuciCmd->pData = (char *)((uint8_t *)pLuciCmd + sizeof(HAL_LUCI_CMD_t));
                            memcpy( pLuciCmd->pData, rxCtrl->pb + HAL_LUCI_CMD_HEAD_LEN, dataLen );
                            pLuciCmd->pData[dataLen] = (uint8_t)'\0';
                        }
                        else
                        {
                            pLuciCmd->pData = NULL;
                        }

                        osal_mem_free( rxCtrl );
                        rxCtrl = NULL;

                        crc = halLuciCmdCRC16_ccitt( pLuciCmd );
                        if( crc == pLuciCmd->crc )
                        {
                            //received LUCI command OK, send it to LUCI command processor
                            osal_msg_send( pLuciCmd, OSAL_TASK_ID_APP_LUCI_COP );
                        }
                        else
                        {
                            //CRC check failed
                            osal_mem_free( pLuciCmd );
                        }
                    }
                    else if( rxCtrl->cnt > HAL_LUCI_CMD_HEAD_LEN + dataLen )
                    {
                        HalTerminalPrintStr("[ERROR]: Luci CMD length wrong!\r\n");
                        osal_mem_free( rxCtrl );
                        rxCtrl = NULL;
                    }
                }
            }
        }
    }
}

static void halLuciCmdProcessTx( void )
{
    uint16_t space;
    if( txFIFO != NULL )
    {
        space = HalUartQryTxBuf( HAL_UART_PORT_1 );
        while( osal_fifo_len( txFIFO ) )
        {
            if( space > 0 )
            {
                HalUartPut( HAL_UART_PORT_1, osal_fifo_get(txFIFO) );
                space--;
            }
            else
            {
                break;
            }
        }

        if( osal_fifo_len( txFIFO ) == 0 )
        {
            osal_fifo_delete( txFIFO );
            txFIFO = NULL;
        }
    }
}

static void halLuciCmdSendByte( uint8_t b )
{
    uint8_t *pc;
    if(txFIFO == NULL)
        txFIFO = osal_fifo_create();
    HAL_ASSERT( txFIFO != NULL );
    pc = osal_fifo_put( txFIFO, b );
    HAL_ASSERT( pc != NULL );
    osal_event_set( OSAL_TASK_ID_HAL_LUCI_CMD, HAL_UART1_TASK_EVENT_POLL_TX );
}


static void halLuciCmdSend( const HAL_LUCI_CMD_t *pLuciCmd )
{
    uint16_t i;
    
    halLuciCmdSendByte((pLuciCmd->remoteID         >> 8) & 0xFF );
    halLuciCmdSendByte((pLuciCmd->remoteID         >> 0) & 0xFF );
    halLuciCmdSendByte((uint8_t)pLuciCmd->type           & 0xFF );
    halLuciCmdSendByte(((uint16_t)pLuciCmd->msgBox >> 8) & 0xFF );
    halLuciCmdSendByte(((uint16_t)pLuciCmd->msgBox >> 0) & 0xFF );
    halLuciCmdSendByte((uint8_t)pLuciCmd->status         & 0xFF );
    halLuciCmdSendByte((pLuciCmd->crc      >> 8)         & 0xFF );
    halLuciCmdSendByte((pLuciCmd->crc      >> 0)         & 0xFF );
    halLuciCmdSendByte((pLuciCmd->len      >> 8)         & 0xFF );
    halLuciCmdSendByte((pLuciCmd->len      >> 0)         & 0xFF );

    for( i = 0; i < pLuciCmd->len; i++ )
    {
        halLuciCmdSendByte( pLuciCmd->pData[i] );
    }
}



static uint16_t halLuciCmdCRC16_ccitt( const HAL_LUCI_CMD_t *pLuciCmd )
{
	uint16_t i;
	uint16_t crc = 0;
    
    static const uint16_t crc16tab[256]= {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
    };
    
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ ((pLuciCmd->remoteID >> 8) & 0xFF))         & 0xFF];
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ ((pLuciCmd->remoteID >> 0) & 0xFF))         & 0xFF];
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ ((uint8_t)pLuciCmd->type   & 0xFF))         & 0xFF];
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ (((uint16_t)pLuciCmd->msgBox >> 8) & 0xFF)) & 0xFF];
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ (((uint16_t)pLuciCmd->msgBox >> 0) & 0xFF)) & 0xFF];
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ ((uint8_t)pLuciCmd->status & 0xFF))         & 0xFF];
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ ((pLuciCmd->len  >> 8)     & 0xFF))         & 0xFF];
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ ((pLuciCmd->len  >> 0)     & 0xFF))         & 0xFF];
    
    for( i = 0; i < pLuciCmd->len; i++ )
    {
        crc = (crc<<8) ^ crc16tab[((crc>>8) ^ pLuciCmd->pData[i])&0x00FF];
    }
	return crc;
}

extern void HalLuciCmdInit( void )
{
    txFIFO = NULL;
    rxCtrl = NULL;

    HAL_LUCI_RESET_LOW();
    osal_task_create( halLuciCmdTask, OSAL_TASK_ID_HAL_LUCI_CMD );
    //osal_timer_event_create( OSAL_TASK_ID_HAL_LUCI_CMD, HAL_LUCI_CMD_TASK_EVENT_RESET, 100 );
    HalUartInit( HAL_UART_PORT_1 );
    HalUartOpen( HAL_UART_PORT_1 );
    
}

extern void HalLuciCmdEnable ( void )
{
    HAL_LUCI_RESET_HIGH();
}

extern void HalLuciCmdDisable    ( void )
{
    HAL_LUCI_RESET_LOW();
    
    if( txFIFO != NULL )
    {
        osal_fifo_delete( txFIFO );
        txFIFO = NULL;
    }

    if( rxCtrl != NULL )
    {
        osal_mem_free( rxCtrl );
        rxCtrl = NULL;
    }
}

extern void HalLuciCmdReset ( void )
{
    HalLuciCmdDisable();
    osal_timer_event_create( OSAL_TASK_ID_HAL_LUCI_CMD, HAL_LUCI_CMD_TASK_EVENT_ENABLE, 10 );
}

extern void HalLuciCmdSendIsHostPresent ( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_IsHostPresent;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = "1";
    luciCmd.len = 1; //strlen("PLAYUSB");
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}

extern void HalLuciCmdSendSetVolume( uint8_t vol )
{
    HAL_LUCI_CMD_t luciCmd;
    uint16_t len;
    char buf[UINT_STR_LEN_MAX];
    
    if( vol > 100 )
        vol = 100;
    
    len = uintstr( vol, buf );
    
    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_VolumeCtrl;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = buf;
    luciCmd.len = len;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
    
}

extern void HalLuciCmdSendQryVolume( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_VolumeCtrl;
    luciCmd.type = HAL_LUCICMD_TYPE_GET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = NULL;
    luciCmd.len = 0;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}

extern void HalLuciCmdSendSavVolume( uint8_t vol )
{
    HAL_LUCI_CMD_t luciCmd;
    uint16_t len;
    char *pData;

    if( vol > 100 )
        vol = 100;
    
    len = (uint16_t)strlen( "WRITE_current_volume," );
    pData = osal_mem_alloc( len + UINT_STR_LEN_MAX );
    HAL_ASSERT( pData != NULL );
    memcpy( pData, "WRITE_current_volume,", len );
    len += uintstr( vol, pData + len );
    
    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_NvReadWrite;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = pData;
    luciCmd.len = len;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );

    osal_mem_free( pData );
    
}

extern void HalLuciCmdSendAuxInStart( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_AuxInputStart;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = NULL;
    luciCmd.len = 0;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );

}

extern void HalLuciCmdSendAuxInStop( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_AuxInputStop;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = NULL;
    luciCmd.len = 0;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}

extern void HalLuciCmdSendPlayUSB( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_AppCtrl;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = "PLAYUSB";
    luciCmd.len = 7; //strlen("PLAYUSB");
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}

extern void HalLuciCmdSendSetWiFiModeHN( void )
{
    HalLuciCmdSendSetWiFiModeCFG();
}

extern void HalLuciCmdSendSetWiFiModeCFG( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_NetworkConfiguration;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = NULL;
    luciCmd.len = 0;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}

extern void HalLuciCmdSendSetWiFiModeSA( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_StandAloneMode;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = NULL;
    luciCmd.len = 0;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}

extern void HalLuciCmdSendPlayCtrl( uint8_t play_ctrl )
{
    HAL_LUCI_CMD_t luciCmd;
    
    char *pData[] = {
        "PLAY",
        "PAUSE",
        "STOP",
        "RESUME",
        "NEXT",
        "PREV",
        "MUTE",
        "UNMUTE",
    };

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_PlayCtrlCmd;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = pData[play_ctrl];
    luciCmd.len = strlen( pData[play_ctrl] );
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );

    
}

extern void HalLuciCmdSendQryMacID      ( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_MACID;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = NULL;
    luciCmd.len = 0;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}


extern void HalLuciCmdSendIsAllowed     ( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_IsAllowed;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = "1";
    luciCmd.len = 1;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}

extern void HalLuciCmdSendIsNotAllowed     ( void )
{
    HAL_LUCI_CMD_t luciCmd;

    luciCmd.remoteID = 0xAAAA;
    luciCmd.msgBox = HAL_LUCICMD_MSGBOX_IsAllowed;
    luciCmd.type = HAL_LUCICMD_TYPE_SET;
    luciCmd.status = HAL_LUCICMD_STATUS_NA;
    luciCmd.pData = "0";
    luciCmd.len = 1;
    luciCmd.crc = halLuciCmdCRC16_ccitt( &luciCmd );

    halLuciCmdSend( &luciCmd );
}


