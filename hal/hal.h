/******************************************************************************

 @file  hal.h

 @brief This file contains the interface to the Drivers service.

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 
 *****************************************************************************/
#ifndef __HAL_H__
#define __HAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdint.h>
//#include "hal_config.h"

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/


/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 * MACROS
 **************************************************************************************************/
//#define HAL_LED_WIFI_IND_BOOTING()            st( HalLedBlink( HAL_LED_WIFIR, 0, 50,  300 );\
//                                                  HalLedBlink( HAL_LED_WIFIB, 0, 100, 0   );)
//#define HAL_LED_WIFI_IND_MODE_NONE()          st( HalLedBlink( HAL_LED_WIFIR, 0, 50,  1000);\
//                                                  HalLedBlink( HAL_LED_WIFIB, 0, 100, 0   );)
//#define HAL_LED_WIFI_IND_MODE_HN()            st( HalLedBlink( HAL_LED_WIFIR, 0, 100, 1   );\
//                                                  HalLedBlink( HAL_LED_WIFIB, 0, 100, 1   );)
//#define HAL_LED_WIFI_IND_MODE_SA()            st( HalLedBlink( HAL_LED_WIFIR, 0, 100, 0   );\
//                                                  HalLedBlink( HAL_LED_WIFIB, 0, 100, 1   );)
//#define HAL_LED_WIFI_IND_MODE_CFG()           st( HalLedBlink( HAL_LED_WIFIR, 0, 100, 1   );\
//                                                  HalLedBlink( HAL_LED_WIFIB, 0, 100, 0   );)
//#define HAL_LED_WIFI_IND_SEARCH_ROUTER()     HalLedBlink( HAL_LED_WIFIR + HAL_LED_WIFIB, 0, 50, 500 )
//
//#define HAL_LED_BT0_IND_OFF()                     HalLedSet  ( HAL_LED_BT0, HAL_LED_MODE_OFF )
//#define HAL_LED_BT0_IND_CONNECTABLE()             HalLedBlink( HAL_LED_BT0, 0, 50, 1000 )
//#define HAL_LED_BT0_IND_DISCOVERABLE()            HalLedBlink( HAL_LED_BT0, 0, 50, 300 )
//#define HAL_LED_BT0_IND_CONNECTED_PAUSED()        HalLedSet  ( HAL_LED_BT0, HAL_LED_MODE_ON )
//#define HAL_LED_BT0_IND_CONNECTED_PLAYING()       HalLedBlink( HAL_LED_BT0, 0, 5, 2000 )
//
//#define HAL_LED_BT1_IND_OFF()                     HalLedSet  ( HAL_LED_BT1, HAL_LED_MODE_OFF )
//#define HAL_LED_BT1_IND_CONNECTABLE()             HalLedBlink( HAL_LED_BT1, 0, 50, 1000 )
//#define HAL_LED_BT1_IND_DISCOVERABLE()            HalLedBlink( HAL_LED_BT1, 0, 50, 300 )
//#define HAL_LED_BT1_IND_CONNECTED_PAUSED()        HalLedSet  ( HAL_LED_BT1, HAL_LED_MODE_ON )
//#define HAL_LED_BT1_IND_CONNECTED_PLAYING()       HalLedBlink( HAL_LED_BT1, 0, 5, 2000 )
//
//#define HAL_LED_BTMODE_IND_OFF()                  HalLedSet  ( HAL_LED_PARTY + HAL_LED_DJ, HAL_LED_MODE_OFF )
//#define HAL_LED_BTMODE_IND_SINGLE()               HAL_LED_BTMODE_IND_OFF()
//#define HAL_LED_BTMODE_IND_PARTY()            st( HalLedSet  ( HAL_LED_PARTY + HAL_LED_DJ, HAL_LED_MODE_OFF );\
//                                                  HalLedSet  ( HAL_LED_PARTY, HAL_LED_MODE_ON );              )
//#define HAL_LED_BTMODE_IND_DJ()               st( HalLedSet  ( HAL_LED_PARTY + HAL_LED_DJ, HAL_LED_MODE_OFF );\
//                                                  HalLedSet  ( HAL_LED_DJ, HAL_LED_MODE_ON );              )
//
//
//
//#define HAL_DSP_SET_AUDIOSRC_NONE()               HalDspAudioSrcCtrl( HAL_DSP_AUDIO_SRC_SDI0 + HAL_DSP_AUDIO_SRC_SDI1 + HAL_DSP_AUDIO_SRC_ADC, HAL_DSP_CTRL_OFF )
//#define HAL_DSP_SET_AUDIOSRC_WIFI()           st( HalMultiplexerSet ( HAL_MULTIPLEXER_DEV0, HAL_MULTIPLEXER_CH0 ); \
//                                                  HalDspAudioSrcCtrl( HAL_DSP_AUDIO_SRC_SDI0, HAL_DSP_CTRL_ON );   )
//#define HAL_DSP_SET_AUDIOSRC_AUX()                HalDspAudioSrcCtrl( HAL_DSP_AUDIO_SRC_ADC, HAL_DSP_CTRL_ON )
//#define HAL_DSP_SET_AUDIOSRC_BT0()            st( HalMultiplexerSet ( HAL_MULTIPLEXER_DEV0, HAL_MULTIPLEXER_CH1 ); \
//                                                  HalDspAudioSrcCtrl( HAL_DSP_AUDIO_SRC_SDI0, HAL_DSP_CTRL_ON );   )
//#define HAL_DSP_SET_AUDIOSRC_BT1()            st( HalMultiplexerSet ( HAL_MULTIPLEXER_DEV1, HAL_MULTIPLEXER_CH1 ); \
//                                                  HalDspAudioSrcCtrl( HAL_DSP_AUDIO_SRC_SDI1, HAL_DSP_CTRL_ON );   )



/**************************************************************************************************
 * FUNCTIONS - API
 **************************************************************************************************/
/*
 * Initialize HW
 */
extern void HalInit (void);
/*
 * Process Polls
 */
extern void HalProcessPoll (void);



#ifdef __cplusplus
}
#endif

#endif

/**************************************************************************************************
**************************************************************************************************/
