/******************************************************************************

 @file  hal_uibrd.c

 @brief This file contains the interface to the UI Board control.

 Group: 
 Target Device: 

 ******************************************************************************


 ******************************************************************************
 Release Name: 
 Release Date: 
 *****************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "osal.h"

#include "hal_config.h"
#include "hal_uibrd.h"
#include "hal_assert.h"
#include "M051Series.h"

#include "app_task_key.h"
#include "app_task_bt.h"

/***************************************************************************************************
 * TYPEDEFS
 ***************************************************************************************************/


/***************************************************************************************************
 * GLOBAL VARIABLES
 ***************************************************************************************************/

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
#define WR_ADDR                     (0x94 | 0x00)
#define RD_ADDR                     (0x94 | 0x01)

#define HAL_UIBRD_TASK_EVENT_IRQ    1

/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/
#define HAL_UIBRD_OPEN()        I2C_Open( I2C0, 100000 )

// Use this macro instead of Nuvoton's BSP API "I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STO | I2C_I2CON_SI)"
// to generate I2C stop, since it has compatibility issue with some I2C slave device.
// The root cause is that SDA and SCL almost rise at the same time! The right stop timing is as below
// 1. SDA SCL are both low after data transmition
// 2. SCL rise to high and stable for a while
// 3. SDA rise to high
#define HAL_UIBRD_STOP_CLOSE()  st( P35 = 0;                                            \
                                    P34 = 0;                                            \
                                    SYS->P3_MFP &= ~(SYS_MFP_P35_Msk | SYS_MFP_P34_Msk);\
                                    I2C_Close( I2C0 );                                  \
                                    P35 = 1;                                            \
                                    CLK_SysTickDelay( 1 );                              \
                                    SYS->P3_MFP |= SYS_MFP_P34_SDA0 | SYS_MFP_P35_SCL0; )
    

#define HAL_UIBRD_IRQ_SET()     (P32 == 0)

/***************************************************************************************************
 * LOCAL FUNCTION
 ***************************************************************************************************/
static void halUiBrdTask ( uint8_t task_id, uint8_t event_id )
{
    uint8_t *pMsg;
    uint8_t irqs;
    uint8_t keyValue;
    uint8_t btState;
    
    switch ( event_id )
    {
        case HAL_UIBRD_TASK_EVENT_IRQ:
            HalUiBrdRegRead( HAL_UIBRD_REG_IRQ_EVENT, &irqs, sizeof(uint8_t) );
            if( irqs & BV(0) )  //key update event
            {
                HalUiBrdRegRead( HAL_UIBRD_REG_KEY_VALUE, &keyValue, sizeof(uint8_t) );
                pMsg = osal_msg_create( sizeof(uint8_t) );
                *pMsg = keyValue;
                osal_msg_send( pMsg, OSAL_TASK_ID_APP_KEY );
            }

            if( irqs & BV(1) ) //bt update event
            {
                HalUiBrdRegRead( HAL_UIBRD_REG_BT_STATE, &btState, sizeof(uint8_t) );
                pMsg = osal_msg_create( sizeof(uint8_t) );
                *pMsg = btState;
                osal_msg_send( pMsg, OSAL_TASK_ID_APP_BT );
            }
        break;

        default:
            OSAL_ASSERT_FORCED();
        break;
    }
}


/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      HalUibrdInit
 *
 * @brief   Initialize UI board including initial LED status etc.
 *
 * @param   
 *
 * @return  None
 ***************************************************************************************************/
extern void HalUiBrdInit( void )
{
    osal_task_create( halUiBrdTask, OSAL_TASK_ID_HAL_UIBRD );
}

/***************************************************************************************************
 * @fn      HalUibrdPollIRQ
 *
 * @brief   
 *
 * @param   
 *
 * @return  None
 ***************************************************************************************************/
extern uint8_t HalUiBrdPollIRQ ( void )
{
    if( HAL_UIBRD_IRQ_SET() )
    {
        osal_event_set( OSAL_TASK_ID_HAL_UIBRD, HAL_UIBRD_TASK_EVENT_IRQ );
        return TRUE;
    }
    return FALSE;
}

/***************************************************************************************************
 * @fn      HalUibrdRegRead
 *
 * @brief   
 *
 * @param   
 *          
 * @return  None
 ***************************************************************************************************/
extern void HalUiBrdRegRead ( uint8_t reg, uint8_t *pb, uint8_t len )
{
    uint8_t err;
    uint8_t i;
    
    HAL_UIBRD_OPEN();

    err = 0;
    
    /* Send start */
    I2C_START(I2C0);
    I2C_WAIT_READY(I2C0);

    /* Send write device address byte */
    I2C_SET_DATA(I2C0, WR_ADDR);
    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    I2C_WAIT_READY(I2C0);

    if(I2C_GET_STATUS(I2C0) == 0x18)
    {
        /* Send reg address byte */
        I2C_SET_DATA(I2C0, reg);
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
        I2C_WAIT_READY(I2C0);
        if(I2C_GET_STATUS(I2C0) == 0x28)
        {
            /* Send restart */
            I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STA | I2C_I2CON_SI);
            I2C_WAIT_READY(I2C0);
            if(I2C_GET_STATUS(I2C0) == 0x10)
            {
                /* Send read device address byte */ 
                I2C_SET_DATA(I2C0, RD_ADDR);
                I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
                I2C_WAIT_READY(I2C0);
                if(I2C_GET_STATUS(I2C0) == 0x40)
                {
                    if( len )
                    {
                        for( i = 0; i < len - 1; i++ )
                        {
                            I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI | I2C_I2CON_AA);
                            I2C_WAIT_READY(I2C0);
                            /* Read data */
                            pb[i] = I2C_GET_DATA(I2C0);
                        }

                        //NACK for the last byte
                        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
                        I2C_WAIT_READY(I2C0);
                        pb[i] = I2C_GET_DATA(I2C0);
                    }
                }
                else
                {
                    //NACK
                    err = 4;
                }
            }
            else
            {
                /* Send Start error */
                err = 3;
            }
        }
        else
        {
            /* NACK */
            err = 2;
        }
    }
    else
    {
        /* NACK */
        err = 1;
    }
    HAL_ASSERT( err == 0 );
    /* Send stop and close */
    HAL_UIBRD_STOP_CLOSE();
}

/***************************************************************************************************
 * @fn      HalUiBrdWrite
 *
 * @brief   
 *
 * @param   
 *          
 *          
 *                       
 *          
 *
 * @return  None
 ***************************************************************************************************/
extern void HalUiBrdRegWrite ( uint8_t reg, const uint8_t *pb, uint8_t len )
{
    uint8_t err;
    uint8_t i;
    uint8_t status;
    
    HAL_UIBRD_OPEN();

    err = 0;
    
    /* Send start */
    I2C_START(I2C0);
    I2C_WAIT_READY(I2C0);

    /* Send write device address byte */
    I2C_SET_DATA(I2C0, WR_ADDR);
    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    I2C_WAIT_READY(I2C0);

    if(I2C_GET_STATUS(I2C0) == 0x18)
    {
        /* Send reg address byte */
        I2C_SET_DATA(I2C0, reg);
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
        I2C_WAIT_READY(I2C0);
        if(I2C_GET_STATUS(I2C0) == 0x28)
        {
            for ( i = 0; i < len; i++ )
            {
                /* Send data */
                I2C_SET_DATA(I2C0, pb[i]);
                I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
                I2C_WAIT_READY(I2C0);
                status = I2C_GET_STATUS(I2C0);
                if( status != 0x28 )
                {
                    if( status == 0x30 )
                    {
                        //NACK
                        //if this is not the last byte, then error
                        if( i + 1 != len )
                        {
                            err = 4;
                        }
                    }
                    else
                    {
                        err = 3;
                    }
                    break;
                }
            }
        }
        else
        {
            /* NACK */
            err = 2;
        }
    }
    else
    {
        /* NACK */
        err = 1;
    }

    HAL_ASSERT( err == 0 );
    /* Send stop and close */
    HAL_UIBRD_STOP_CLOSE();
    
    
}



/***************************************************************************************************
***************************************************************************************************/




