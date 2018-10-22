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
#include "hal_assert.h"
#include "hal_i2c.h"
          
// Use the API halI2cStopClose() instead of Nuvoton's BSP API "I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STO | I2C_I2CON_SI)"
// to generate I2C stop, since it has compatibility issue with some I2C slave device.
// The root cause is that SDA and SCL almost rise at the same time! The right stop timing is as below
// 1. SDA SCL are both low after data transmition
// 2. SCL rise to high and stable for a while
// 3. SDA rise to high
static void halI2cStopClose( I2C_T *I2Cx )
{
    if( I2Cx == I2C0 )
    {
        P35 = 0;
        P34 = 0;
        SYS->P3_MFP &= ~(SYS_MFP_P35_Msk | SYS_MFP_P34_Msk);
        I2C_Close( I2Cx );
        P35 = 1;
        CLK_SysTickDelay( 1 );
        SYS->P3_MFP |= SYS_MFP_P34_SDA0 | SYS_MFP_P35_SCL0;
        return;
    }

    if( I2Cx == I2C1 )
    {
        P25 = 0;
        P24 = 0;
        SYS->P2_MFP &= ~(SYS_MFP_P25_Msk | SYS_MFP_P24_Msk);
        I2C_Close( I2Cx );
        P24 = 1;
        CLK_SysTickDelay( 1 );
        SYS->P2_MFP |= SYS_MFP_P25_SDA1 | SYS_MFP_P24_SCL1;
        return;
    }
}

extern void HalI2cRw    ( uint8_t port, HAL_I2C_RW_t *rw )
{
    I2C_T *I2Cx;
    uint8_t err;
    uint16_t i;

    err = 0;
    I2Cx = ( port == HAL_I2C_PORT_0 ) ? I2C0 : I2C1;
    
    I2C_Open( I2Cx, ( port == HAL_I2C_PORT_0 ) ? HAL_I2C0_CLOCK : HAL_I2C1_CLOCK );
    
    /* Send start */
    I2C_START( I2Cx );
    I2C_WAIT_READY( I2Cx );

    if( rw->lenTxD )
    {
        /* Send control byte */
        I2C_SET_DATA( I2Cx, rw->addr & 0xFE );
        I2C_SET_CONTROL_REG( I2Cx, I2C_I2CON_SI );
        I2C_WAIT_READY( I2Cx );
        if( I2C_GET_STATUS( I2Cx ) != 0x18 )
        {
            /* NACK */
            /* Send control error */
            err = 1;
            goto HalI2cRw_final;
        }
        
        /* ACK */
        for( i = 0; i < rw->lenTxD; i++ )
        {
            /* Send data */
            I2C_SET_DATA( I2Cx, rw->pTxD[i] ); // data
            I2C_SET_CONTROL_REG( I2Cx, I2C_I2CON_SI );
            I2C_WAIT_READY( I2Cx );
            if( I2C_GET_STATUS( I2Cx ) != 0x20 )
            {
                /* NACK */
                /* Send data error */
                err = 2;
                goto HalI2cRw_final;
            }
        }
    }

    if( rw->lenRxD )
    {
        if( rw->lenTxD )
        {
            //send a repeat start is needed
            I2C_SET_CONTROL_REG( I2Cx, I2C_I2CON_STA | I2C_I2CON_SI );
            I2C_WAIT_READY( I2Cx );
            if( I2C_GET_STATUS( I2Cx ) != 0x10 )
            {
                err = 3;
                goto HalI2cRw_final;
            }
        }
        /* Send control byte */
        I2C_SET_DATA( I2Cx, rw->addr | 0x01 );
        I2C_SET_CONTROL_REG( I2Cx, I2C_I2CON_SI );
        I2C_WAIT_READY( I2Cx );
        if( I2C_GET_STATUS( I2Cx ) != 0x40 )
        {
            err = 4;
            goto HalI2cRw_final;
        }

        for( i = 0; i < rw->lenRxD - 1; i++ )
        {
            I2C_SET_CONTROL_REG( I2Cx, I2C_I2CON_SI | I2C_I2CON_AA );
            I2C_WAIT_READY( I2Cx );

            /* Read data */
            rw->pRxD[i] = I2C_GET_DATA( I2Cx );
        }

        I2C_SET_CONTROL_REG( I2Cx, I2C_I2CON_SI );
        I2C_WAIT_READY (I2Cx );
        rw->pRxD[i] = I2C_GET_DATA( I2Cx );
    }

    HalI2cRw_final:
    halI2cStopClose( I2Cx );
    HAL_ASSERT( err == 0 );
}

