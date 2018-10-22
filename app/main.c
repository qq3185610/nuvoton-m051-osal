/******************************************************************************

 @file  main.c

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/

#include "osal.h"
#include "hal.h"
#include "app.h"


int main( void )
{
    /* Initialize hardware */
    // Initialize board I/O
    HalInit();
    
    /* Initialize NV system */
    
    /* Initialize the operating system */
    osal_system_init();
    
    AppInit();
    /* Start OSAL */
    osal_system_start(); // No Return from here

    return 0;

}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
