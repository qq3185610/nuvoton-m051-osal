/******************************************************************************

 @file  hal_config.h

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/

#ifndef __HAL_CONFIG_H__
#define __HAL_CONFIG_H__

#define     HAL_ASSERT_EN       1
//#define     HAL_ASSERT_RESET    0
//#define     HAL_ASSERT_LIGHTS   1
//#define     HAL_ASSERT_SPIN     0

//#define     HAL_LED_EN          1
//#define     HAL_LED_BLINK_EN    1

#define HAL_UART0_EN                        1
#define HAL_UART1_EN                        1
#define HAL_UART0_BAUDRATE                  115200
#define HAL_UART1_BAUDRATE                  57600

#define HAL_GPIO_EN                         1
#define HAL_GPIO_CALLBACK_MAX               1

#define HAL_I2C_EN                          1
#define HAL_I2C0_CLOCK                      100000
#define HAL_I2C1_CLOCK                      100000


/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/

#endif

