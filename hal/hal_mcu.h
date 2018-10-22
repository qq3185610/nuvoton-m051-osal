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

#ifndef __HAL_MCU_H__
#define __HAL_MCU_H__

#include "osal_comdef.h"
#include "M051Series.h"

#define HAL_SYSTEM_RESET()          st( HAL_DISABLE_INTERRUPTS(); SYS_UnlockReg(); SYS_ResetChip();)
#define HAL_DISABLE_INTERRUPTS()    __disable_irq()
#define HAL_ENABLE_INTERRUPTS()     __enable_irq()



#endif

