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
#include "hal_gpio.h"

#include <string.h>

#define GPIO_RW_ADDR(port, pin)    ((volatile uint32_t *)((GPIO_PIN_DATA_BASE+(0x20*(port))) + ((pin)<<2)))

typedef struct hal_gpio_item_t {
    GPIO_T *port;
    uint32_t pin;
    volatile uint32_t *rwaddr;
} HAL_GPIO_ITEM_t;

typedef struct hal_gpio_callback_item_t {
    uint32_t gpio;
    HAL_GPIO_CALLBACK_t cb;
} HAL_GPIO_CALLBACK_ITEM_t;

static const HAL_GPIO_ITEM_t gpioList[] = {
    { P3, BIT2, GPIO_RW_ADDR(3, 2) },   //IO0, I2C IRQ pin
    { P2, BIT2, GPIO_RW_ADDR(2, 2) },   //IO1, multiplexer 0 ctrl pin
    { P2, BIT3, GPIO_RW_ADDR(2, 3) },   //IO2, multiplexer 1 ctrl pin
    { P1, BIT1, GPIO_RW_ADDR(1, 1) },   //IO3, ls5b reset pin
    { P4, BIT2, GPIO_RW_ADDR(4, 2) },   //IO4, ncpa110p reset pin
};

static HAL_GPIO_CALLBACK_ITEM_t gpioCallbackList[HAL_GPIO_CALLBACK_MAX];
static uint32_t oldValue;

extern void     HalGpioInit         ( void )
{
    memset( gpioCallbackList, sizeof(gpioCallbackList), 0 );
    oldValue = 0xFFFFFFFF;
}

extern void     HalGpioSetMode      ( uint32_t gpio, uint8_t mode )
{
    uint8_t gpio_idx = 0;

    while( gpio )
    {
        if( gpio & 0x1 )
        {
            if( gpio_idx < sizeof(gpioList)/sizeof(HAL_GPIO_ITEM_t) )
                GPIO_SetMode( gpioList[gpio_idx].port, gpioList[gpio_idx].pin, (uint32_t)mode );
        }
        gpio >>= 1;
        gpio_idx++;
    }
}

extern uint32_t HalGpioRead         ( uint32_t gpio )
{
    uint32_t value = 0;
    uint8_t gpio_idx = 0;

    while( gpio )
    {
        if( gpio & 0x1 )
        {
            if( gpio_idx < sizeof(gpioList)/sizeof(HAL_GPIO_ITEM_t) )
            {
                if( *(gpioList[gpio_idx].rwaddr) )
                    value |= (1<<gpio_idx);
            }
        }
        gpio >>= 1;
        gpio_idx++;
    }
    
    return value;
}

extern void     HalGpioWrite        ( uint32_t gpio, uint8_t value )
{
    uint8_t gpio_idx = 0;
    if( value )
        value = 1;

    while( gpio )
    {
        if( gpio & 0x1 )
        {
            if( gpio_idx < sizeof(gpioList)/sizeof(HAL_GPIO_ITEM_t) )
            {
                *(gpioList[gpio_idx].rwaddr) = (uint32_t)value;
            }
        }
        gpio >>= 1;
        gpio_idx++;
    }
}

extern uint32_t HalGpioRegCallback  ( uint32_t gpio, const HAL_GPIO_CALLBACK_t *cb )
{
    uint8_t i;
    
    for( i = 0; i < HAL_GPIO_CALLBACK_MAX; i++ )
    {
        if( gpioCallbackList[i].gpio )
        {
            if( gpio & gpioCallbackList[i].gpio )
                return 0;
        }
        else
        {
            gpioCallbackList[i].gpio = gpio;
            gpioCallbackList[i].cb.pfnGpioUpdate = cb->pfnGpioUpdate;
            break;
        }
    }

    return gpio;
}


extern void     HalGpioPoll( void )
{
    uint32_t newValue;
    uint32_t updGpio;
    uint32_t gpio = 0;
    uint8_t i;

    for( i = 0; i < HAL_GPIO_CALLBACK_MAX; i++ )
    {
        if( gpioCallbackList[i].gpio )
        {
            gpio |= gpioCallbackList[i].gpio;
        }
        else
        {
            break;
        }
    }

    newValue = HalGpioRead( gpio );

    if( newValue != oldValue )
    {
        updGpio = newValue ^ oldValue;
        updGpio &= gpio;
        for( i = 0; i < HAL_GPIO_CALLBACK_MAX; i++ )
        {
            if( gpioCallbackList[i].gpio & updGpio )
            {
                gpioCallbackList[i].cb.pfnGpioUpdate( gpioCallbackList[i].gpio & updGpio, (uint8_t)( newValue & gpioCallbackList[i].gpio & updGpio ) );
            }
        }

        newValue = oldValue;
    }
    
}

