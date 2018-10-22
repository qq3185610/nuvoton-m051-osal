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
#ifndef __HAL_DSP_H__
#define __HAL_DSP_H__


#include <stdint.h>


#define HAL_DSP_INIT_CODE_0       0         //No process
#define HAL_DSP_INIT_CODE_1       1         //Added Maxx Base

#define HAL_DSP_AUDIO_SRC_NONE    0x00
#define HAL_DSP_AUDIO_SRC_SDI0    0x01
#define HAL_DSP_AUDIO_SRC_SDI1    0x02
#define HAL_DSP_AUDIO_SRC_ADC     0x04

#define HAL_DSP_CTRL_OFF          0
#define HAL_DSP_CTRL_ON           1      

#define HAL_DSP_VOL_MAX           0x7FFFFF

extern void    HalDspInit( void );
extern void    HalDspResetHold(void);
extern void    HalDspResetFree(void);
extern void    HalDspSendInitCode(uint8_t initCode);
extern void    HalDspRead(const uint8_t cmd[3], uint8_t val[3]);
extern void    HalDspWrite(const uint8_t *buf, uint16_t len);
extern void    HalDspAudioSrcCtrl(uint8_t src, uint8_t ctrl );
extern void    HalDspSetVol(uint32_t vol);


#endif
