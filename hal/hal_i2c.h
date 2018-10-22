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
#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__


#include <stdint.h>

typedef struct hal_i2c_rw_t {
    uint8_t *pTxD;
    uint8_t *pRxD;
    uint16_t lenTxD;
    uint16_t lenRxD;
    uint8_t addr;
} HAL_I2C_RW_t;

#define HAL_I2C_PORT_0      0
#define HAL_I2C_PORT_1      1


extern void HalI2cRw    ( uint8_t port, HAL_I2C_RW_t *rw );

#endif /* __HAL_I2C_H__ */

