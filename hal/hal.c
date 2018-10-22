/******************************************************************************

 @file  hal.c

 @brief This file contains the interface to the Drivers Service.

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 
 *****************************************************************************/

/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include "osal.h"
#include "M051Series.h"
/**************************************************************************************************
 *                                         CONSTANTS
 **************************************************************************************************/
#define 	SYSTICK_FREQ		1000
/**************************************************************************************************
 *                                      GLOBAL VARIABLES
 **************************************************************************************************/


/**************************************************************************************************
 * @fn      HalInit
 *
 * @brief   Initialize HW - These need to be initialized before anyone.
 *
 * @param   
 *
 * @return  None
 **************************************************************************************************/
void HalInit (void)
{   
    /* Unlock protected registers */
    SYS_UnlockReg();
	
	/*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);
	CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);
	
	CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));
	
	CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    CLK_SetCoreClock(50000000);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Enable 12.288MHz Clock output */
    CLK_EnableModuleClock(FDIV_MODULE);
    SYS->P3_MFP &= ~SYS_MFP_P36_Msk;
    SYS->P3_MFP |= SYS_MFP_P36_CKO;
    CLK_EnableCKO(CLK_CLKSEL2_FRQDIV_S_HXT, 0, 1);
    
    /* Enable UART0 */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Set UART0 clock */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));
    /* Set P3 multi-function pins for UART0 RXD and TXD  */
	SYS->P3_MFP &= ~(SYS_MFP_P30_Msk | SYS_MFP_P31_Msk);
    SYS->P3_MFP |= (SYS_MFP_P30_RXD0 | SYS_MFP_P31_TXD0);

    /* Enable UART1 */
    CLK_EnableModuleClock(UART1_MODULE);
    /* Set UART1 clock */
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));
    /* Set P1 multi-function pins for UART1 RXD and TXD  */
	SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
    SYS->P1_MFP |= (SYS_MFP_P12_RXD1 | SYS_MFP_P13_TXD1);

    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, 0);

    /* Enable I2C0 */
    CLK_EnableModuleClock(I2C0_MODULE);
    /* Set P3 multi-function pins as the SDA0 & SCL0 of I2C0 pins */
    SYS->P3_MFP &= ~(SYS_MFP_P34_Msk | SYS_MFP_P35_Msk);
    SYS->P3_MFP |= SYS_MFP_P34_SDA0 | SYS_MFP_P35_SCL0;

    /*  Set PWM01 clock */
    CLK_SetModuleClock(PWM01_MODULE, CLK_CLKSEL1_PWM01_S_HXT, 0);
    /*  Enable PWM01 */
    CLK_EnableModuleClock(PWM01_MODULE);
    GPIO_SetMode(P2, BIT0 , GPIO_PMD_OUTPUT);
    GPIO_SetMode(P2, BIT1 , GPIO_PMD_OUTPUT);
    /* Set P2 multi-function pins for PWM0 and PWM1  */
    SYS->P2_MFP &= ~(SYS_MFP_P20_PWM0 | SYS_MFP_P21_PWM1);
    SYS->P2_MFP |= (SYS_MFP_P20_PWM0 | SYS_MFP_P21_PWM1);
    
    /* Enable PWM0 PWM1 output, output 3.072MHz and 48kHz */
    PWM_EnableOutput(PWMA, 0x3);
    PWM_ConfigOutputChannel(PWMA, PWM_CH0, 3072000, 50);
    PWM_ConfigOutputChannel(PWMA, PWM_CH1, 48000, 50);
    PWM_Start(PWMA, 0x3);

    /* Start systick timer */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, SYSTICK_FREQ);
    TIMER_EnableInt(TIMER0);
	NVIC_EnableIRQ(TMR0_IRQn);
	TIMER_Start(TIMER0);

    /* Lock protected registers */
    SYS_LockReg();
}


/**************************************************************************************************
 * @fn      Hal_ProcessPoll
 *
 * @brief   This routine will be called by OSAL to poll UART, TIMER...
 *
 * @param   task_id - Hal TaskId
 *
 * @return  None
 **************************************************************************************************/
void HalProcessPoll ( void )
{
	
}


/**************************************************************************************************
**************************************************************************************************/

