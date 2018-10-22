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
#include "hal_interrupt.h"
#include "hal_uart.h"




extern uint8_t OsalSysTick;
extern void HalUart0Isr( void );
extern void HalUart1Isr( void );

extern void UART0_IRQHandler(void)
{
    HalUart0Isr();
}

extern void UART1_IRQHandler(void)
{
    HalUart1Isr();
}


extern void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
        OsalSysTick++;
    }
}


