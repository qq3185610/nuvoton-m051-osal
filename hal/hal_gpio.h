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
#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__


#include <stdint.h>

typedef struct hal_gpio_callback_t {
    void (*pfnGpioUpdate)( uint32_t gpio, uint8_t value );
} HAL_GPIO_CALLBACK_t;

#define HAL_GPIO_MODE_INPUT     0
#define HAL_GPIO_MODE_OUTPUT    1
#define HAL_GPIO_MODE_OD        2
#define HAL_GPIO_MODE_QUASI     3

#define HAL_GPIO_IO_0           (uint32_t)(1<<0)
#define HAL_GPIO_IO_1           (uint32_t)(1<<1)
#define HAL_GPIO_IO_2           (uint32_t)(1<<2)
#define HAL_GPIO_IO_3           (uint32_t)(1<<3)
#define HAL_GPIO_IO_4           (uint32_t)(1<<4)

extern void     HalGpioInit         ( void );
extern void     HalGpioSetMode      ( uint32_t gpio, uint8_t mode );
extern uint32_t HalGpioRead         ( uint32_t gpio );
extern void     HalGpioWrite        ( uint32_t gpio, uint8_t value );
extern uint32_t HalGpioRegCallback  ( uint32_t gpio, const HAL_GPIO_CALLBACK_t *cb );


#endif /* __HAL_GPIO_H__ */

