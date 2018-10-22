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
#include "osal.h"

#include "M051Series.h"
#include "hal_config.h"
#include "hal_dsp.h"
#include "hal_assert.h"



#define     DSP_WR_ADDR               (0xE6 | 0x00)
#define     DSP_RD_ADDR               (0xE6 | 0x01)

#define     RST_DSP                   P42

#define HAL_DSP_OPEN()                I2C_Open(I2C0, 100000)
// Use this macro instead of Nuvoton's BSP API "I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STO | I2C_I2CON_SI)"
// to generate I2C stop, since it has compatibility issue with some I2C slave device.
// The root cause is that SDA and SCL almost rise at the same time! The right stop timing is as below
// 1. SDA SCL are both low after data transmition
// 2. SCL rise to high and stable for a while
// 3. SDA rise to high
#define HAL_DSP_STOP_CLOSE()    st( P35 = 0;                                            \
                                    P34 = 0;                                            \
                                    SYS->P3_MFP &= ~(SYS_MFP_P35_Msk | SYS_MFP_P34_Msk);\
                                    I2C_Close( I2C0 );                                  \
                                    P35 = 1;                                            \
                                    CLK_SysTickDelay( 1 );                              \
                                    SYS->P3_MFP |= SYS_MFP_P34_SDA0 | SYS_MFP_P35_SCL0; )

static const uint8_t start[] = {0x00, 0xA4, 0x11};
static const uint8_t end[]   = {0x00, 0xC8, 0x00};
static uint8_t cmd[] = {
        0x00, 0x00, 0x00,   // M1 - SDI0 to Processing path 
        0x00, 0x00, 0x00,   // M2 - SDI1 to Processing path 
        0x00, 0x00, 0x00,   // M3 - SDI2 to Processing path 
        0x00, 0x00, 0x00,   // M4 - ADC  to Processing path 
        0x00, 0x00, 0x00,   // M5 - SDI0 to UnProcessing path 
        0x00, 0x00, 0x00,   // M6 - SDI1 to UnProcessing path 
        0x00, 0x00, 0x00,   // M7 - SDI2 to UnProcessing path 
        0x00, 0x00, 0x00,   // M8 - ADC  to UnProcessing path 
        0x00, 0x00, 0x00,   // M9 - SDO0 from Processed path 
        0x00, 0x00, 0x00,   // M10- SDO1 from Processed path 
        0x00, 0x00, 0x00,   // M11- DAC1 from Processed path 
        0x00, 0x00, 0x00,   // M12- SDO0 from UnProcessed path 
        0x00, 0x00, 0x00,   // M13- SDO1 from UnProcessed path 
        0x00, 0x00, 0x00,   // M14- DAC1 from UnProcessed path 
        0x00, 0x00, 0x00,   // M15- DAC2/SDO2 from Subwoofer path 
        0x00, 0x00, 0x00,   // M16- DAC2/SDO2 UnProcessed path 
        0x00, 0x00, 0x00    // M17- DAC2/SDO2 Processed path 
};


static const uint8_t MaxxAudio_Cmd_FULL_NoProcess[] = 
{ 
    0x00, 0xa5, 0x00, /* Command DETECT                             */
    0x00, 0xa5, 0x00, /* Command DETECT                             */

    /* System Configuration Begin */
    0x00, 0xc3, 0x00, /* Command SAMPLE, Value = 48 kHz             */
    0x00, 0xa4, 0x11, /* Command ANALOG_SETTING                     */
    0x00, 0x00, 0x00, /*                                            */
    0x0f, 0xbf, 0x58, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x7f, 0xff, 0xff, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x7f, 0xff, 0xff, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0xc8, 0x00, /* Command UPD_ANALOG                         */
    0xbf, 0xc4, 0xc5, /* Command DIG_IO_CONFIG, Value = 0000c5      */
    0xff, 0xab, 0xfb, 
    0x24, 0x20, 0x09, /*                                            */
    0xff, 0xab, 0xfa, 
    0x08, 0x00, 0x27, /*                                            */
    0xff, 0xab, 0xfa, 
    0x00, 0x00, 0x27, /*                                            */
    0xff, 0xab, 0xfb, 
    0xa4, 0x28, 0x29, /*                                            */
    0xff, 0xad, 0x84, 
    0x00, 0x00, 0x18, /*                                            */
    0xff, 0xad, 0x85, 
    0x00, 0x1f, 0xff, /*                                            */
    0xff, 0xad, 0x81, 
    0x00, 0x12, 0x12, /*                                            */
    0xff, 0xad, 0x82, 
    0x00, 0x14, 0x14, /*                                            */
    0xff, 0xad, 0x83, 
    0x00, 0x00, 0x88, /*                                            */
    0xff, 0xad, 0x80, 
    0x00, 0x54, 0x54, /*                                            */
    0xff, 0xad, 0x87, 
    0x00, 0x54, 0x54, /*                                            */
    0xff, 0xad, 0x86, 
    0x00, 0x00, 0x00, /*                                            */
    0xff, 0xad, 0x86, 
    0x00, 0x00, 0x01, /*                                            */
    0xff, 0xad, 0x86, 
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0xc2, 0x00, /* Command SPEAKER, Value = 2.0               */
    /* System Configuration End */

    /* Algorithms Configuration Begin */
    0x00, 0xc9, 0x78, /* Command BALANCE, Value = 0                 */
    0x00, 0xdf, 0x1e, /* Command LEVELER_TARGET, Value = -6.0       */
    0x00, 0xdb, 0x78, /* Command LEVELER_RANGE, Value = -24.0       */
    0x00, 0xdb, 0x78, /* Command LEVELER_RANGE, Value = -24.0       */
    0x00, 0xc7, 0x00, /* Command MUTE, Value = Out                  */
    0x00, 0xe0, 0x0a, /* Command MUTE, Value = 10                   */
    0x00, 0xd5, 0x30, /* Command MAXX_BASS_FREQUENCY, Value = 20.0  */
    0x00, 0xcf, 0x3d, /* Command Internal                           */
    0x00, 0xe1, 0x70, /* Command MAXX_BASS_NG_THRESHOLD, Value = -100 */
    0x00, 0xcb, 0x00, /* Command MAXX_BASS_ORIGINAL_BASS, Value = Off */
    0x00, 0xe2, 0x2f, /* Command MAXX_BASS_NG_GAP, Value = -47      */
    0x00, 0xd9, 0x7f, /* Command MAXX_TREBLE_FREQUENCY, Value = 20000.0 */
    0x00, 0xe5, 0x2f, /* Command MAXX_TREBLE_NG_GAP, Value = -47    */
    0x00, 0xe4, 0x70, /* Command MAXX_TREBLE_NG_THRESHOLD, Value = -100 */
    0x00, 0xd7, 0x78, /* Command SUB_FREQ, Value = 160.1            */
    0x01, 0xd0, 0x90, /* Command MAXX_VOLUME_SLIDER                 */
    0x00, 0xc6, 0x01, /* Command LEVELER_ON_OFF                     */
    0x00, 0xdd, 0x01, /* Command HARD_KNEE                          */
    0x00, 0xd4, 0x00, /* Command VOLUME_DYN_RANGE                   */
    0x00, 0xda, 0x00, /* Command VOLUME_LOW_GAIN                    */
    0x00, 0xdc, 0x00, /* Command VOLUME_GATE                        */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x01, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x00, 0x00, 0x16, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x01, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x18, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x02, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x24, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x03, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x30, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x04, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x3c, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x05, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x48, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x06, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x54, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x07, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x60, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x08, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x6c, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x09, /*                                            */
    0x00, 0x00, 0x02, /*                                            */
    0x00, 0x00, 0x78, /*                                            */
    0x00, 0x00, 0x16, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xd1, 0x4c, /*                                            */
    0x00, 0xd1, 0x3f, /*                                            */
    0x00, 0xd1, 0x4c, /*                                            */
    0x00, 0xd1, 0x36, /*                                            */
    0x00, 0xd1, 0x2d, /*                                            */
    0x00, 0xd1, 0x24, /*                                            */
    0x00, 0xd1, 0x1b, /*                                            */
    0x00, 0xd1, 0x12, /*                                            */
    0x00, 0xd1, 0x09, /*                                            */
    0x00, 0xd1, 0x00, /*                                            */
    0x00, 0xcf, 0x11, /*                                            */
    0x00, 0xcf, 0x3d, /* Command Internal                           */
    0x00, 0xcf, 0x3d, /* Command Internal                           */
    0x00, 0xcf, 0x3d, /* Command Internal                           */
    0x00, 0xcf, 0x3d, /* Command Internal                           */

    /* Patch Begin */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x01, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x30, 0x1a, 0x48, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x20, 0xee, 0x7d, /*                                            */
    0xff, 0xab, 0xf7, /*                                            */
    0x02, 0x90, 0x78, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x02, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x10, 0x1a, 0x4a, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x21, 0xe6, 0x00, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x03, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x10, 0x10, 0x95, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x4f, 0xdc, 0xba, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x04, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x10, 0x13, 0x9e, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x05, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x30, 0x0f, 0x6c, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x56, 0xf4, 0x00, /*                                            */
    0xff, 0xab, 0xf7, /*                                            */
    0x00, 0x00, 0xa9, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x06, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x30, 0x0f, 0x66, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x0a, 0xf0, 0x80, /*                                            */
    0xff, 0xab, 0xf7, /*                                            */
    0x00, 0x00, 0xb6, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x07, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x20, 0x19, 0xe4, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x63, 0xf4, 0x00, /*                                            */
    0xff, 0xab, 0xf7, /*                                            */
    0x00, 0x02, 0xe0, /*                                            */
    0x06, 0xaa, 0x06, /*                                            */
    0x00, 0xa2, 0xd4, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0xe7, /*                                            */
    0x61, 0x70, 0x00, /*                                            */
    0x00, 0x02, 0x47, /*                                            */
    0x3c, 0x0a, 0x00, /*                                            */
    0x38, 0x0a, 0x00, /*                                            */
    0x64, 0xf4, 0x00, /*                                            */
    0x00, 0x01, 0x34, /*                                            */
    0x60, 0xf4, 0x00, /*                                            */
    0x00, 0x01, 0xda, /*                                            */
    0x05, 0xf4, 0x20, /*                                            */
    0xff, 0xff, 0xff, /*                                            */
    0x04, 0x64, 0xa0, /*                                            */
    0x20, 0x4c, 0x00, /*                                            */
    0x20, 0x48, 0x00, /*                                            */
    0x44, 0xb8, 0x00, /*                                            */
    0x0c, 0xc4, 0xa0, /*                                            */
    0x00, 0x00, 0x16, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xf7, 0x05, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xff, 0x38, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x20, 0x5c, 0x00, /*                                            */
    0x20, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xff, 0xbc, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xc7, 0x29, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x0a, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0x78, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xf6, 0x3a, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xff, 0x27, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x20, 0x5c, 0x00, /*                                            */
    0x20, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xff, 0xb6, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xc2, 0x23, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x54, 0xf0, 0x00, /*                                            */
    0xff, 0xff, 0xf1, /*                                            */
    0x0c, 0x1e, 0xe2, /*                                            */
    0x01, 0x40, 0xc6, /*                                            */
    0x00, 0x00, 0x07, /*                                            */
    0x01, 0x40, 0xc5, /*                                            */
    0x00, 0x00, 0x01, /*                                            */
    0x0a, 0xf0, 0xa2, /*                                            */
    0x00, 0x00, 0x89, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0x00, 0xcd, 0xac, /*                                            */
    0x61, 0x70, 0x00, /*                                            */
    0x00, 0x02, 0x14, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0x00, 0xc8, 0x32, /*                                            */
    0x61, 0x70, 0x00, /*                                            */
    0x00, 0x01, 0xff, /*                                            */
    0x60, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0xef, /*                                            */
    0x60, 0x70, 0x00, /*                                            */
    0x00, 0x01, 0xf8, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0xef, /*                                            */
    0x6c, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x4e, /*                                            */
    0x74, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x43, /*                                            */
    0x4c, 0xec, 0x00, /*                                            */
    0x0c, 0xc4, 0xa0, /*                                            */
    0x00, 0x00, 0x06, /*                                            */
    0x61, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x51, /*                                            */
    0x0b, 0xe1, 0x80, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x44, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0xb2, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0xd2, 0xab, /*                                            */
    0x44, 0xf0, 0x00, /*                                            */
    0x00, 0x01, 0xb2, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0xd3, 0x01, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0xbd, /*                                            */
    0x61, 0x70, 0x00, /*                                            */
    0x00, 0x02, 0x42, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0xff, 0xff, 0xf4, /*                                            */
    0x44, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x07, /*                                            */
    0x44, 0x59, 0x00, /*                                            */
    0x44, 0xb8, 0x00, /*                                            */
    0x0c, 0xc4, 0xa0, /*                                            */
    0x00, 0x00, 0x06, /*                                            */
    0x44, 0xf4, 0x00, /*                                            */
    0x20, 0x19, 0xe4, /*                                            */
    0x0a, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0xb4, /*                                            */
    0x44, 0xf4, 0x00, /*                                            */
    0x30, 0x19, 0xe4, /*                                            */
    0x44, 0x61, 0x00, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x61, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x20, /*                                            */
    0x21, 0xba, 0x00, /*                                            */
    0x0b, 0xe1, 0x80, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0xc2, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0xd1, 0x39, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0xc2, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x6c, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x4e, /*                                            */
    0x74, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x0a, /*                                            */
    0x20, 0x4c, 0x00, /*                                            */
    0x4e, 0xe4, 0x00, /*                                            */
    0x20, 0xcf, 0x00, /*                                            */
    /* Algorithms Configuration End */
    0x6c, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x4e, /*                                            */
    0x74, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x03, /*                                            */
    0x20, 0x4c, 0x00, /*                                            */
    0x4e, 0xe4, 0x00, /*                                            */
    0x20, 0xce, 0x00, /*                                            */
    0x20, 0x00, 0x0d, /*                                            */
    0x0d, 0x10, 0x49, /*                                            */
    0x00, 0x00, 0x15, /*                                            */
    0x57, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0xb7, /*                                            */
    0x46, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x01, /*                                            */
    0x20, 0x00, 0x58, /*                                            */
    0x21, 0xe5, 0x3a, /*                                            */
    0x20, 0x00, 0x68, /*                                            */
    0x47, 0xf4, 0x00, /*                                            */
    0x00, 0x16, 0x00, /*                                            */
    0x46, 0xf0, 0x78, /*                                            */
    0x00, 0x00, 0x48, /*                                            */
    0x20, 0x00, 0x58, /*                                            */
    0x21, 0xf3, 0x00, /*                                            */
    0x4e, 0xdb, 0x00, /*                                            */
    0x4e, 0x5c, 0x00, /*                                            */
    0x4e, 0xdb, 0x00, /*                                            */
    0x4e, 0x5c, 0x00, /*                                            */
    0x4e, 0xdb, 0x00, /*                                            */
    0x4e, 0x64, 0x00, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0xd3, 0x68, /*                                            */
    0x46, 0xf4, 0x00, /*                                            */
    0x4d, 0xf8, 0xf0, /*                                            */
    0x74, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x1e, /*                                            */
    0x4e, 0x6c, 0x00, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x55, 0xbe, 0x00, /*                                            */
    0x01, 0x40, 0xce, /*                                            */
    0xff, 0xff, 0x3f, /*                                            */
    0x30, 0xff, 0x00, /*                                            */
    0x04, 0xcd, 0xcd, /*                                            */
    0x20, 0x00, 0x1b, /*                                            */
    0x22, 0x09, 0x00, /*                                            */
    0x00, 0x00, 0x0b, /*                                            */
    0x0d, 0x10, 0x4a, /*                                            */
    0x00, 0x00, 0x0a, /*                                            */
    0x0a, 0x9d, 0xa1, /*                                            */
    0x00, 0x00, 0xf6, /*                                            */
    0x22, 0x09, 0x00, /*                                            */
    0x00, 0x00, 0x0b, /*                                            */
    0x0d, 0x10, 0x4a, /*                                            */
    0x00, 0x00, 0x04, /*                                            */
    0x0a, 0x9d, 0x81, /*                                            */
    0x00, 0x00, 0xfc, /*                                            */
    0x08, 0x45, 0x1a, /*                                            */
    0x56, 0xf4, 0x00, /*                                            */
    0x00, 0x03, 0xff, /*                                            */
    0x20, 0x00, 0x62, /*                                            */
    0x08, 0xcc, 0x1a, /*                                            */
    0x20, 0xa4, 0x00, /*                                            */
    0x07, 0xf0, 0x2d, /*                                            */
    0x00, 0x00, 0x3e, /*                                            */
    0x57, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0xac, /*                                            */
    0x45, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x02, /*                                            */
    0x45, 0xf4, 0x6a, /*                                            */
    0xff, 0x0f, 0xff, /*                                            */
    0x20, 0x00, 0x6e, /*                                            */
    0x55, 0x70, 0x00, /*                                            */
    0x00, 0x02, 0xac, /*                                            */
    0x08, 0xc4, 0x1a, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x02, 0xa7, 0xe0, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x02, 0xa7, 0xe1, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x02, 0xa7, 0xe2, /*                                            */
    0x02, 0x55, 0x96, /*                                            */
    0x02, 0xa7, 0xe3, /*                                            */
    0x02, 0x79, 0x1f, /*                                            */
    0x02, 0xa7, 0xe4, /*                                            */
    0x02, 0x9e, 0xc5, /*                                            */
    0x02, 0xa7, 0xe5, /*                                            */
    0x02, 0xc6, 0xa8, /*                                            */
    0x02, 0xa7, 0xe6, /*                                            */
    0x02, 0xf0, 0xea, /*                                            */
    0x02, 0xa7, 0xe7, /*                                            */
    0x03, 0x1d, 0xaf, /*                                            */
    0x02, 0xa7, 0xe8, /*                                            */
    0x03, 0x4d, 0x1e, /*                                            */
    0x02, 0xa7, 0xe9, /*                                            */
    0x03, 0x7f, 0x5f, /*                                            */
    0x02, 0xa7, 0xea, /*                                            */
    0x03, 0xb4, 0x9d, /*                                            */
    0x02, 0xa7, 0xeb, /*                                            */
    0x03, 0xed, 0x05, /*                                            */
    0x02, 0xa7, 0xec, /*                                            */
    0x04, 0x28, 0xc8, /*                                            */
    0x02, 0xa7, 0xed, /*                                            */
    0x04, 0x68, 0x19, /*                                            */
    0x02, 0xa7, 0xee, /*                                            */
    0x04, 0xab, 0x2d, /*                                            */
    0x02, 0xa7, 0xef, /*                                            */
    0x04, 0xf2, 0x3f, /*                                            */
    0x02, 0xa7, 0xf0, /*                                            */
    0x05, 0x3d, 0x8a, /*                                            */
    0x02, 0xa7, 0xf1, /*                                            */
    0x05, 0x8d, 0x50, /*                                            */
    0x02, 0xa7, 0xf2, /*                                            */
    0x05, 0xe1, 0xd4, /*                                            */
    0x02, 0xa7, 0xf3, /*                                            */
    0x06, 0x3b, 0x5f, /*                                            */
    0x02, 0xa7, 0xf4, /*                                            */
    0x06, 0x9a, 0x3d, /*                                            */
    0x02, 0xa7, 0xf5, /*                                            */
    0x06, 0xfe, 0xbf, /*                                            */
    0x02, 0xa7, 0xf6, /*                                            */
    0x07, 0x69, 0x3b, /*                                            */
    0x02, 0xa7, 0xf7, /*                                            */
    0x07, 0xda, 0x0c, /*                                            */
    0x02, 0xa7, 0xf8, /*                                            */
    0x08, 0x51, 0x92, /*                                            */
    0x02, 0xa7, 0xf9, /*                                            */
    0x08, 0xd0, 0x34, /*                                            */
    0x02, 0xa7, 0xfa, /*                                            */
    0x09, 0x56, 0x5d, /*                                            */
    0x02, 0xa7, 0xfb, /*                                            */
    0x09, 0xe4, 0x81, /*                                            */
    0x02, 0xa7, 0xfc, /*                                            */
    0x0a, 0x7b, 0x19, /*                                            */
    0x02, 0xa7, 0xfd, /*                                            */
    0x0b, 0x1a, 0xa6, /*                                            */
    0x02, 0xa7, 0xfe, /*                                            */
    0x0b, 0xc3, 0xaf, /*                                            */
    0x02, 0xa7, 0xff, /*                                            */
    0x0c, 0x76, 0xc5, /*                                            */
    0x03, 0xa7, 0x00, /*                                            */
    0x0d, 0x34, 0x82, /*                                            */
    0x03, 0xa7, 0x01, /*                                            */
    0x0d, 0xfd, 0x88, /*                                            */
    0x03, 0xa7, 0x02, /*                                            */
    0x0e, 0xd2, 0x82, /*                                            */
    0x03, 0xa7, 0x03, /*                                            */
    0x0f, 0xb4, 0x26, /*                                            */
    0x03, 0xa7, 0x04, /*                                            */
    0x10, 0xa3, 0x36, /*                                            */
    0x03, 0xa7, 0x05, /*                                            */
    0x11, 0xa0, 0x7d, /*                                            */
    0x03, 0xa7, 0x06, /*                                            */
    0x12, 0xac, 0xd4, /*                                            */
    0x03, 0xa7, 0x07, /*                                            */

    /* Algorithms Configuration Begin */
    0x13, 0xc9, 0x21, /*                                            */
    0x03, 0xa7, 0x08, /*                                            */
    0x14, 0xf6, 0x56, /*                                            */
    0x03, 0xa7, 0x09, /*                                            */
    0x16, 0x35, 0x76, /*                                            */
    0x03, 0xa7, 0x0a, /*                                            */
    0x17, 0x87, 0x91, /*                                            */
    0x03, 0xa7, 0x0b, /*                                            */
    0x18, 0xed, 0xc8, /*                                            */
    0x03, 0xa7, 0x0c, /*                                            */
    0x1a, 0x69, 0x4d, /*                                            */
    0x03, 0xa7, 0x0d, /*                                            */
    0x1b, 0xfb, 0x66, /*                                            */
    0x03, 0xa7, 0x0e, /*                                            */
    0x1d, 0xa5, 0x6a, /*                                            */
    0x03, 0xa7, 0x0f, /*                                            */
    0x1f, 0x68, 0xc5, /*                                            */
    0x03, 0xa7, 0x10, /*                                            */
    0x21, 0x46, 0xfb, /*                                            */
    0x03, 0xa7, 0x11, /*                                            */
    0x23, 0x41, 0xa5, /*                                            */
    0x03, 0xa7, 0x12, /*                                            */
    0x25, 0x5a, 0x74, /*                                            */
    0x03, 0xa7, 0x13, /*                                            */
    0x27, 0x93, 0x34, /*                                            */
    0x03, 0xa7, 0x14, /*                                            */
    0x29, 0xed, 0xcd, /*                                            */
    0x03, 0xa7, 0x15, /*                                            */
    0x2c, 0x6c, 0x42, /*                                            */
    0x03, 0xa7, 0x16, /*                                            */
    0x2f, 0x10, 0xb8, /*                                            */
    0x03, 0xa7, 0x17, /*                                            */
    0x31, 0xdd, 0x73, /*                                            */
    0x03, 0xa7, 0x18, /*                                            */
    0x34, 0xd4, 0xda, /*                                            */
    0x03, 0xa7, 0x19, /*                                            */
    0x37, 0xf9, 0x78, /*                                            */
    0x03, 0xa7, 0x1a, /*                                            */
    0x3b, 0x4e, 0x02, /*                                            */
    0x03, 0xa7, 0x1b, /*                                            */
    0x3e, 0xd5, 0x54, /*                                            */
    0x03, 0xa7, 0x1c, /*                                            */
    0x01, 0x0a, 0x4a, /*                                            */
    0x03, 0xa7, 0x1d, /*                                            */
    0x01, 0x1a, 0x23, /*                                            */
    0x03, 0xa7, 0x1e, /*                                            */
    0x01, 0x2a, 0xed, /*                                            */
    0x03, 0xa7, 0x1f, /*                                            */
    0x01, 0x3c, 0xb8, /*                                            */
    0x03, 0xa7, 0x20, /*                                            */
    0x01, 0x4f, 0x92, /*                                            */
    0x03, 0xa7, 0x21, /*                                            */
    0x01, 0x63, 0x8d, /*                                            */
    0x03, 0xa7, 0x22, /*                                            */
    0x01, 0x78, 0xb9, /*                                            */
    0x03, 0xa7, 0x23, /*                                            */
    0x01, 0x8f, 0x28, /*                                            */
    0x03, 0xa7, 0x24, /*                                            */
    0x01, 0xa6, 0xef, /*                                            */
    0x03, 0xa7, 0x25, /*                                            */
    0x01, 0xc0, 0x21, /*                                            */
    0x03, 0xa7, 0x26, /*                                            */
    0x01, 0xda, 0xd6, /*                                            */
    0x03, 0xa7, 0x27, /*                                            */
    0x01, 0xf7, 0x24, /*                                            */
    0x03, 0xa7, 0x28, /*                                            */
    0x02, 0x15, 0x24, /*                                            */
    0x03, 0xa7, 0x29, /*                                            */
    0x02, 0x34, 0xf1, /*                                            */
    0x03, 0xa7, 0x2a, /*                                            */
    0x02, 0x56, 0xa6, /*                                            */
    0x03, 0xa7, 0x2b, /*                                            */
    0x02, 0x7a, 0x63, /*                                            */
    0x03, 0xa7, 0x2c, /*                                            */
    0x02, 0xa0, 0x46, /*                                            */
    0x03, 0xa7, 0x2d, /*                                            */
    0x02, 0xc8, 0x71, /*                                            */
    0x03, 0xa7, 0x2e, /*                                            */
    0x02, 0xf3, 0x0a, /*                                            */
    0x03, 0xa7, 0x2f, /*                                            */
    0x03, 0x20, 0x37, /*                                            */
    0x03, 0xa7, 0x30, /*                                            */
    0x03, 0x50, 0x20, /*                                            */
    0x03, 0xa7, 0x31, /*                                            */
    0x03, 0x82, 0xf4, /*                                            */
    0x03, 0xa7, 0x32, /*                                            */
    0x03, 0xb8, 0xe0, /*                                            */
    0x03, 0xa7, 0x33, /*                                            */
    0x03, 0xf2, 0x17, /*                                            */
    0x03, 0xa7, 0x34, /*                                            */
    0x04, 0x2e, 0xd1, /*                                            */
    0x03, 0xa7, 0x35, /*                                            */
    0x04, 0x6f, 0x48, /*                                            */
    0x03, 0xa7, 0x36, /*                                            */
    0x04, 0xb3, 0xba, /*                                            */
    0x03, 0xa7, 0x37, /*                                            */
    0x04, 0xfc, 0x6d, /*                                            */
    0x03, 0xa7, 0x38, /*                                            */
    0x05, 0x49, 0xa9, /*                                            */
    0x03, 0xa7, 0x39, /*                                            */
    0x05, 0x9b, 0xbf, /*                                            */
    0x03, 0xa7, 0x3a, /*                                            */
    0x05, 0xf3, 0x04, /*                                            */
    0x03, 0xa7, 0x3b, /*                                            */
    0x06, 0x4f, 0xd8, /*                                            */
    0x03, 0xa7, 0x3c, /*                                            */
    0x06, 0xb2, 0xa2, /*                                            */
    0x03, 0xa7, 0x3d, /*                                            */
    0x07, 0x1b, 0xd1, /*                                            */
    0x03, 0xa7, 0x3e, /*                                            */
    0x07, 0x8b, 0xe2, /*                                            */
    0x03, 0xa7, 0x3f, /*                                            */
    0x08, 0x03, 0x5e, /*                                            */
    0x03, 0xa7, 0x40, /*                                            */
    0x08, 0x82, 0xdc, /*                                            */
    0x03, 0xa7, 0x41, /*                                            */
    0x09, 0x0b, 0x05, /*                                            */
    0x03, 0xa7, 0x42, /*                                            */
    0x09, 0x9c, 0x95, /*                                            */
    0x03, 0xa7, 0x43, /*                                            */
    0x0a, 0x38, 0x5e, /*                                            */
    0x03, 0xa7, 0x44, /*                                            */
    0x0a, 0xdf, 0x51, /*                                            */
    0x03, 0xa7, 0x45, /*                                            */
    0x0b, 0x92, 0x7d, /*                                            */
    0x03, 0xa7, 0x46, /*                                            */
    0x0c, 0x53, 0x17, /*                                            */
    0x03, 0xa7, 0x47, /*                                            */
    0x0d, 0x22, 0x84, /*                                            */
    0x03, 0xa7, 0x48, /*                                            */
    0x0e, 0x02, 0x61, /*                                            */
    0x03, 0xa7, 0x49, /*                                            */
    0x0e, 0xf4, 0x91, /*                                            */
    0x03, 0xa7, 0x4a, /*                                            */
    0x0f, 0xfb, 0x4d, /*                                            */
    0x03, 0xa7, 0x4b, /*                                            */
    0x11, 0x19, 0x37, /*                                            */
    0x03, 0xa7, 0x4c, /*                                            */
    0x12, 0x51, 0x77, /*                                            */
    0x03, 0xa7, 0x4d, /*                                            */
    0x13, 0xa7, 0xe3, /*                                            */
    0x03, 0xa7, 0x4e, /*                                            */
    0x15, 0x21, 0x2b, /*                                            */
    0x03, 0xa7, 0x4f, /*                                            */
    0x16, 0xc3, 0x21, /*                                            */
    0x03, 0xa7, 0x50, /*                                            */
    0x18, 0x95, 0x18, /*                                            */
    0x03, 0xa7, 0x51, /*                                            */
    0x1a, 0xa0, 0x69, /*                                            */
    0x03, 0xa7, 0x52, /*                                            */
    0x1c, 0xf1, 0x39, /*                                            */
    0x03, 0xa7, 0x53, /*                                            */
    0x1f, 0x97, 0xa2, /*                                            */
    0x03, 0xa7, 0x54, /*                                            */
    0x22, 0xa9, 0x72, /*                                            */
    0x03, 0xa7, 0x55, /*                                            */
    0x26, 0x45, 0x00, /*                                            */
    0x03, 0xa7, 0x56, /*                                            */
    0x2a, 0x95, 0xce, /*                                            */
    0x03, 0xa7, 0x57, /*                                            */
    0x2f, 0xdc, 0xa1, /*                                            */
    0x03, 0xa7, 0x58, /*                                            */
    0x36, 0x7e, 0x5d, /*                                            */
    0x03, 0xa7, 0x59, /*                                            */
    0x3f, 0x21, 0x66, /*                                            */
    0x03, 0xa7, 0x5a, /*                                            */
    0x4a, 0xed, 0x42, /*                                            */
    0x03, 0xa7, 0x5b, /*                                            */
    0x5c, 0x26, 0x73, /*                                            */
    0x03, 0xa7, 0x5c, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x5d, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x5e, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x5f, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x60, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x61, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x62, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x63, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x64, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x65, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x66, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x67, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    /* Patch End */

    0x00, 0xac, 0x00, /* Command COMLET_SETTING                     */
    0x00, 0x85, 0x00, /* Command Internal                           */
};

static const uint8_t MaxxAudio_Cmd_FULL_MaxxBase[] = 
{ 
    0x00, 0xa5, 0x00, /* Command DETECT                             */
    0x00, 0xa5, 0x00, /* Command DETECT                             */

    /* System Configuration Begin */
    0x00, 0xc3, 0x00, /* Command SAMPLE, Value = 48 kHz             */
    0x00, 0xa4, 0x11, /* Command ANALOG_SETTING                     */
    0x00, 0x00, 0x00, /*                                            */
    0x0f, 0xbf, 0x58, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x7f, 0xff, 0xff, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x7f, 0xff, 0xff, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0xc8, 0x00, /* Command UPD_ANALOG                         */
    0xbf, 0xc4, 0xc5, /* Command DIG_IO_CONFIG, Value = 0000c5      */
    0xff, 0xab, 0xfb, 
    0x24, 0x20, 0x09, /*                                            */
    0xff, 0xab, 0xfa, 
    0x08, 0x00, 0x27, /*                                            */
    0xff, 0xab, 0xfa, 
    0x00, 0x00, 0x27, /*                                            */
    0xff, 0xab, 0xfb, 
    0xa4, 0x28, 0x29, /*                                            */
    0xff, 0xad, 0x84, 
    0x00, 0x00, 0x18, /*                                            */
    0xff, 0xad, 0x85, 
    0x00, 0x1f, 0xff, /*                                            */
    0xff, 0xad, 0x81, 
    0x00, 0x12, 0x12, /*                                            */
    0xff, 0xad, 0x82, 
    0x00, 0x14, 0x14, /*                                            */
    0xff, 0xad, 0x83, 
    0x00, 0x00, 0x88, /*                                            */
    0xff, 0xad, 0x80, 
    0x00, 0x54, 0x54, /*                                            */
    0xff, 0xad, 0x87, 
    0x00, 0x54, 0x54, /*                                            */
    0xff, 0xad, 0x86, 
    0x00, 0x00, 0x00, /*                                            */
    0xff, 0xad, 0x86, 
    0x00, 0x00, 0x01, /*                                            */
    0xff, 0xad, 0x86, 
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0xc2, 0x00, /* Command SPEAKER, Value = 2.0               */
    /* System Configuration End */

    /* Algorithms Configuration Begin */
    0x00, 0xc9, 0x78, /* Command BALANCE, Value = 0                 */
    0x00, 0xdf, 0x1e, /* Command LEVELER_TARGET, Value = -6.0       */
    0x00, 0xdb, 0x78, /* Command LEVELER_RANGE, Value = -24.0       */
    0x00, 0xdb, 0x78, /* Command LEVELER_RANGE, Value = -24.0       */
    0x00, 0xc7, 0x00, /* Command MUTE, Value = Out                  */
    0x00, 0xe0, 0x0a, /* Command MUTE, Value = 10                   */
    0x00, 0xd5, 0x30, /* Command MAXX_BASS_FREQUENCY, Value = 20.0  */
    0x00, 0xcf, 0x3d, /* Command Internal                           */
    0x00, 0xe1, 0x70, /* Command MAXX_BASS_NG_THRESHOLD, Value = -100 */
    0x00, 0xcb, 0x00, /* Command MAXX_BASS_ORIGINAL_BASS, Value = Off */
    0x00, 0xe2, 0x2f, /* Command MAXX_BASS_NG_GAP, Value = -47      */
    0x00, 0xd9, 0x7f, /* Command MAXX_TREBLE_FREQUENCY, Value = 20000.0 */
    0x00, 0xe5, 0x2f, /* Command MAXX_TREBLE_NG_GAP, Value = -47    */
    0x00, 0xe4, 0x70, /* Command MAXX_TREBLE_NG_THRESHOLD, Value = -100 */
    0x00, 0xd7, 0x78, /* Command SUB_FREQ, Value = 160.1            */
    0x01, 0xd0, 0x90, /* Command MAXX_VOLUME_SLIDER                 */
    0x00, 0xc6, 0x01, /* Command LEVELER_ON_OFF                     */
    0x00, 0xdd, 0x01, /* Command HARD_KNEE                          */
    0x00, 0xd4, 0x00, /* Command VOLUME_DYN_RANGE                   */
    0x00, 0xda, 0x00, /* Command VOLUME_LOW_GAIN                    */
    0x00, 0xdc, 0x00, /* Command VOLUME_GATE                        */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x01, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x00, 0x00, 0x16, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x01, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x18, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x02, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x24, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x03, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x30, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x04, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x3c, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x05, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x48, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x06, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x54, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x07, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x60, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x08, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x00, 0x00, 0x6c, /*                                            */
    0x00, 0x00, 0x3a, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xa1, 0x05, /* Command SET_PEQ                            */
    0x00, 0x00, 0x09, /*                                            */
    0x00, 0x00, 0x02, /*                                            */
    0x00, 0x00, 0x78, /*                                            */
    0x00, 0x00, 0x16, /*                                            */
    0x00, 0x00, 0x40, /*                                            */
    0x00, 0xd8, 0x00, /* Command UPD_PEQ                            */
    0x00, 0xd1, 0x4c, /*                                            */
    0x00, 0xd1, 0x3f, /*                                            */
    0x00, 0xd1, 0x4c, /*                                            */
    0x00, 0xd1, 0x36, /*                                            */
    0x00, 0xd1, 0x2d, /*                                            */
    0x00, 0xd1, 0x24, /*                                            */
    0x00, 0xd1, 0x1b, /*                                            */
    0x00, 0xd1, 0x12, /*                                            */
    0x00, 0xd1, 0x09, /*                                            */
    0x00, 0xd1, 0x00, /*                                            */
    0x00, 0xcf, 0x11, /*                                            */
    0x00, 0xcf, 0x3d, /* Command Internal                           */
    0x00, 0xcf, 0x3d, /* Command Internal                           */
    0x00, 0xcf, 0x3d, /* Command Internal                           */
    0x00, 0xcf, 0x3d, /* Command Internal                           */

    /* Patch Begin */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x01, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x30, 0x1a, 0x48, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x20, 0xee, 0x7d, /*                                            */
    0xff, 0xab, 0xf7, /*                                            */
    0x02, 0x90, 0x78, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x02, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x10, 0x1a, 0x4a, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x21, 0xe6, 0x00, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x03, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x10, 0x10, 0x95, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x4f, 0xdc, 0xba, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x04, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x10, 0x13, 0x9e, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x05, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x30, 0x0f, 0x6c, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x56, 0xf4, 0x00, /*                                            */
    0xff, 0xab, 0xf7, /*                                            */
    0x00, 0x00, 0xa9, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x06, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x30, 0x0f, 0x66, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x0a, 0xf0, 0x80, /*                                            */
    0xff, 0xab, 0xf7, /*                                            */
    0x00, 0x00, 0xb6, /*                                            */
    0xff, 0xab, 0xf4, /*                                            */
    0x00, 0x00, 0x07, /*                                            */
    0xff, 0xab, 0xf5, /*                                            */
    0x20, 0x19, 0xe4, /*                                            */
    0xff, 0xab, 0xf6, /*                                            */
    0x63, 0xf4, 0x00, /*                                            */
    0xff, 0xab, 0xf7, /*                                            */
    0x00, 0x02, 0xe0, /*                                            */
    0x06, 0xaa, 0x06, /*                                            */
    0x00, 0xa2, 0xd4, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0xe7, /*                                            */
    0x61, 0x70, 0x00, /*                                            */
    0x00, 0x02, 0x47, /*                                            */
    0x3c, 0x0a, 0x00, /*                                            */
    0x38, 0x0a, 0x00, /*                                            */
    0x64, 0xf4, 0x00, /*                                            */
    0x00, 0x01, 0x34, /*                                            */
    0x60, 0xf4, 0x00, /*                                            */
    0x00, 0x01, 0xda, /*                                            */
    0x05, 0xf4, 0x20, /*                                            */
    0xff, 0xff, 0xff, /*                                            */
    0x04, 0x64, 0xa0, /*                                            */
    0x20, 0x4c, 0x00, /*                                            */
    0x20, 0x48, 0x00, /*                                            */
    0x44, 0xb8, 0x00, /*                                            */
    0x0c, 0xc4, 0xa0, /*                                            */
    0x00, 0x00, 0x16, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xf7, 0x05, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xff, 0x38, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x20, 0x5c, 0x00, /*                                            */
    0x20, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xff, 0xbc, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xc7, 0x29, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x0a, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0x78, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xf6, 0x3a, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xff, 0x27, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x20, 0x5c, 0x00, /*                                            */
    0x20, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xff, 0xb6, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x57, 0xf4, 0x00, /*                                            */
    0x7f, 0xc2, 0x23, /*                                            */
    0x5f, 0x5c, 0x00, /*                                            */
    0x5f, 0x58, 0x00, /*                                            */
    0x54, 0xf0, 0x00, /*                                            */
    0xff, 0xff, 0xf1, /*                                            */
    0x0c, 0x1e, 0xe2, /*                                            */
    0x01, 0x40, 0xc6, /*                                            */
    0x00, 0x00, 0x07, /*                                            */
    0x01, 0x40, 0xc5, /*                                            */
    0x00, 0x00, 0x01, /*                                            */
    0x0a, 0xf0, 0xa2, /*                                            */
    0x00, 0x00, 0x89, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0x00, 0xcd, 0xac, /*                                            */
    0x61, 0x70, 0x00, /*                                            */
    0x00, 0x02, 0x14, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0x00, 0xc8, 0x32, /*                                            */
    0x61, 0x70, 0x00, /*                                            */
    0x00, 0x01, 0xff, /*                                            */
    0x60, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0xef, /*                                            */
    0x60, 0x70, 0x00, /*                                            */
    0x00, 0x01, 0xf8, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0xef, /*                                            */
    0x6c, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x4e, /*                                            */
    0x74, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x43, /*                                            */
    0x4c, 0xec, 0x00, /*                                            */
    0x0c, 0xc4, 0xa0, /*                                            */
    0x00, 0x00, 0x06, /*                                            */
    0x61, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x51, /*                                            */
    0x0b, 0xe1, 0x80, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x44, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0xb2, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0xd2, 0xab, /*                                            */
    0x44, 0xf0, 0x00, /*                                            */
    0x00, 0x01, 0xb2, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0xd3, 0x01, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0xbd, /*                                            */
    0x61, 0x70, 0x00, /*                                            */
    0x00, 0x02, 0x42, /*                                            */
    0x61, 0xf4, 0x00, /*                                            */
    0xff, 0xff, 0xf4, /*                                            */
    0x44, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x07, /*                                            */
    0x44, 0x59, 0x00, /*                                            */
    0x44, 0xb8, 0x00, /*                                            */
    0x0c, 0xc4, 0xa0, /*                                            */
    0x00, 0x00, 0x06, /*                                            */
    0x44, 0xf4, 0x00, /*                                            */
    0x20, 0x19, 0xe4, /*                                            */
    0x0a, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0xb4, /*                                            */
    0x44, 0xf4, 0x00, /*                                            */
    0x30, 0x19, 0xe4, /*                                            */
    0x44, 0x61, 0x00, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x61, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x20, /*                                            */
    0x21, 0xba, 0x00, /*                                            */
    0x0b, 0xe1, 0x80, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0xc2, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0xd1, 0x39, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0x00, 0xc2, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x6c, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x4e, /*                                            */
    0x74, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x0a, /*                                            */
    0x20, 0x4c, 0x00, /*                                            */
    0x4e, 0xe4, 0x00, /*                                            */
    0x20, 0xcf, 0x00, /*                                            */
    /* Algorithms Configuration End */
    0x6c, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0x4e, /*                                            */
    0x74, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x03, /*                                            */
    0x20, 0x4c, 0x00, /*                                            */
    0x4e, 0xe4, 0x00, /*                                            */
    0x20, 0xce, 0x00, /*                                            */
    0x20, 0x00, 0x0d, /*                                            */
    0x0d, 0x10, 0x49, /*                                            */
    0x00, 0x00, 0x15, /*                                            */
    0x57, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0xb7, /*                                            */
    0x46, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x01, /*                                            */
    0x20, 0x00, 0x58, /*                                            */
    0x21, 0xe5, 0x3a, /*                                            */
    0x20, 0x00, 0x68, /*                                            */
    0x47, 0xf4, 0x00, /*                                            */
    0x00, 0x16, 0x00, /*                                            */
    0x46, 0xf0, 0x78, /*                                            */
    0x00, 0x00, 0x48, /*                                            */
    0x20, 0x00, 0x58, /*                                            */
    0x21, 0xf3, 0x00, /*                                            */
    0x4e, 0xdb, 0x00, /*                                            */
    0x4e, 0x5c, 0x00, /*                                            */
    0x4e, 0xdb, 0x00, /*                                            */
    0x4e, 0x5c, 0x00, /*                                            */
    0x4e, 0xdb, 0x00, /*                                            */
    0x4e, 0x64, 0x00, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x0b, 0xf0, 0x80, /*                                            */
    0x00, 0xd3, 0x68, /*                                            */
    0x46, 0xf4, 0x00, /*                                            */
    0x4d, 0xf8, 0xf0, /*                                            */
    0x74, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x1e, /*                                            */
    0x4e, 0x6c, 0x00, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x55, 0xbe, 0x00, /*                                            */
    0x01, 0x40, 0xce, /*                                            */
    0xff, 0xff, 0x3f, /*                                            */
    0x30, 0xff, 0x00, /*                                            */
    0x04, 0xcd, 0xcd, /*                                            */
    0x20, 0x00, 0x1b, /*                                            */
    0x22, 0x09, 0x00, /*                                            */
    0x00, 0x00, 0x0b, /*                                            */
    0x0d, 0x10, 0x4a, /*                                            */
    0x00, 0x00, 0x0a, /*                                            */
    0x0a, 0x9d, 0xa1, /*                                            */
    0x00, 0x00, 0xf6, /*                                            */
    0x22, 0x09, 0x00, /*                                            */
    0x00, 0x00, 0x0b, /*                                            */
    0x0d, 0x10, 0x4a, /*                                            */
    0x00, 0x00, 0x04, /*                                            */
    0x0a, 0x9d, 0x81, /*                                            */
    0x00, 0x00, 0xfc, /*                                            */
    0x08, 0x45, 0x1a, /*                                            */
    0x56, 0xf4, 0x00, /*                                            */
    0x00, 0x03, 0xff, /*                                            */
    0x20, 0x00, 0x62, /*                                            */
    0x08, 0xcc, 0x1a, /*                                            */
    0x20, 0xa4, 0x00, /*                                            */
    0x07, 0xf0, 0x2d, /*                                            */
    0x00, 0x00, 0x3e, /*                                            */
    0x57, 0xf0, 0x00, /*                                            */
    0x00, 0x02, 0xac, /*                                            */
    0x45, 0xf4, 0x00, /*                                            */
    0x00, 0x00, 0x02, /*                                            */
    0x45, 0xf4, 0x6a, /*                                            */
    0xff, 0x0f, 0xff, /*                                            */
    0x20, 0x00, 0x6e, /*                                            */
    0x55, 0x70, 0x00, /*                                            */
    0x00, 0x02, 0xac, /*                                            */
    0x08, 0xc4, 0x1a, /*                                            */
    0x00, 0x00, 0x0c, /*                                            */
    0x02, 0xa7, 0xe0, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x02, 0xa7, 0xe1, /*                                            */
    0x00, 0x00, 0x00, /*                                            */
    0x02, 0xa7, 0xe2, /*                                            */
    0x02, 0x55, 0x96, /*                                            */
    0x02, 0xa7, 0xe3, /*                                            */
    0x02, 0x79, 0x1f, /*                                            */
    0x02, 0xa7, 0xe4, /*                                            */
    0x02, 0x9e, 0xc5, /*                                            */
    0x02, 0xa7, 0xe5, /*                                            */
    0x02, 0xc6, 0xa8, /*                                            */
    0x02, 0xa7, 0xe6, /*                                            */
    0x02, 0xf0, 0xea, /*                                            */
    0x02, 0xa7, 0xe7, /*                                            */
    0x03, 0x1d, 0xaf, /*                                            */
    0x02, 0xa7, 0xe8, /*                                            */
    0x03, 0x4d, 0x1e, /*                                            */
    0x02, 0xa7, 0xe9, /*                                            */
    0x03, 0x7f, 0x5f, /*                                            */
    0x02, 0xa7, 0xea, /*                                            */
    0x03, 0xb4, 0x9d, /*                                            */
    0x02, 0xa7, 0xeb, /*                                            */
    0x03, 0xed, 0x05, /*                                            */
    0x02, 0xa7, 0xec, /*                                            */
    0x04, 0x28, 0xc8, /*                                            */
    0x02, 0xa7, 0xed, /*                                            */
    0x04, 0x68, 0x19, /*                                            */
    0x02, 0xa7, 0xee, /*                                            */
    0x04, 0xab, 0x2d, /*                                            */
    0x02, 0xa7, 0xef, /*                                            */
    0x04, 0xf2, 0x3f, /*                                            */
    0x02, 0xa7, 0xf0, /*                                            */
    0x05, 0x3d, 0x8a, /*                                            */
    0x02, 0xa7, 0xf1, /*                                            */
    0x05, 0x8d, 0x50, /*                                            */
    0x02, 0xa7, 0xf2, /*                                            */
    0x05, 0xe1, 0xd4, /*                                            */
    0x02, 0xa7, 0xf3, /*                                            */
    0x06, 0x3b, 0x5f, /*                                            */
    0x02, 0xa7, 0xf4, /*                                            */
    0x06, 0x9a, 0x3d, /*                                            */
    0x02, 0xa7, 0xf5, /*                                            */
    0x06, 0xfe, 0xbf, /*                                            */
    0x02, 0xa7, 0xf6, /*                                            */
    0x07, 0x69, 0x3b, /*                                            */
    0x02, 0xa7, 0xf7, /*                                            */
    0x07, 0xda, 0x0c, /*                                            */
    0x02, 0xa7, 0xf8, /*                                            */
    0x08, 0x51, 0x92, /*                                            */
    0x02, 0xa7, 0xf9, /*                                            */
    0x08, 0xd0, 0x34, /*                                            */
    0x02, 0xa7, 0xfa, /*                                            */
    0x09, 0x56, 0x5d, /*                                            */
    0x02, 0xa7, 0xfb, /*                                            */
    0x09, 0xe4, 0x81, /*                                            */
    0x02, 0xa7, 0xfc, /*                                            */
    0x0a, 0x7b, 0x19, /*                                            */
    0x02, 0xa7, 0xfd, /*                                            */
    0x0b, 0x1a, 0xa6, /*                                            */
    0x02, 0xa7, 0xfe, /*                                            */
    0x0b, 0xc3, 0xaf, /*                                            */
    0x02, 0xa7, 0xff, /*                                            */
    0x0c, 0x76, 0xc5, /*                                            */
    0x03, 0xa7, 0x00, /*                                            */
    0x0d, 0x34, 0x82, /*                                            */
    0x03, 0xa7, 0x01, /*                                            */
    0x0d, 0xfd, 0x88, /*                                            */
    0x03, 0xa7, 0x02, /*                                            */
    0x0e, 0xd2, 0x82, /*                                            */
    0x03, 0xa7, 0x03, /*                                            */
    0x0f, 0xb4, 0x26, /*                                            */
    0x03, 0xa7, 0x04, /*                                            */
    0x10, 0xa3, 0x36, /*                                            */
    0x03, 0xa7, 0x05, /*                                            */
    0x11, 0xa0, 0x7d, /*                                            */
    0x03, 0xa7, 0x06, /*                                            */
    0x12, 0xac, 0xd4, /*                                            */
    0x03, 0xa7, 0x07, /*                                            */

    /* Algorithms Configuration Begin */
    0x13, 0xc9, 0x21, /*                                            */
    0x03, 0xa7, 0x08, /*                                            */
    0x14, 0xf6, 0x56, /*                                            */
    0x03, 0xa7, 0x09, /*                                            */
    0x16, 0x35, 0x76, /*                                            */
    0x03, 0xa7, 0x0a, /*                                            */
    0x17, 0x87, 0x91, /*                                            */
    0x03, 0xa7, 0x0b, /*                                            */
    0x18, 0xed, 0xc8, /*                                            */
    0x03, 0xa7, 0x0c, /*                                            */
    0x1a, 0x69, 0x4d, /*                                            */
    0x03, 0xa7, 0x0d, /*                                            */
    0x1b, 0xfb, 0x66, /*                                            */
    0x03, 0xa7, 0x0e, /*                                            */
    0x1d, 0xa5, 0x6a, /*                                            */
    0x03, 0xa7, 0x0f, /*                                            */
    0x1f, 0x68, 0xc5, /*                                            */
    0x03, 0xa7, 0x10, /*                                            */
    0x21, 0x46, 0xfb, /*                                            */
    0x03, 0xa7, 0x11, /*                                            */
    0x23, 0x41, 0xa5, /*                                            */
    0x03, 0xa7, 0x12, /*                                            */
    0x25, 0x5a, 0x74, /*                                            */
    0x03, 0xa7, 0x13, /*                                            */
    0x27, 0x93, 0x34, /*                                            */
    0x03, 0xa7, 0x14, /*                                            */
    0x29, 0xed, 0xcd, /*                                            */
    0x03, 0xa7, 0x15, /*                                            */
    0x2c, 0x6c, 0x42, /*                                            */
    0x03, 0xa7, 0x16, /*                                            */
    0x2f, 0x10, 0xb8, /*                                            */
    0x03, 0xa7, 0x17, /*                                            */
    0x31, 0xdd, 0x73, /*                                            */
    0x03, 0xa7, 0x18, /*                                            */
    0x34, 0xd4, 0xda, /*                                            */
    0x03, 0xa7, 0x19, /*                                            */
    0x37, 0xf9, 0x78, /*                                            */
    0x03, 0xa7, 0x1a, /*                                            */
    0x3b, 0x4e, 0x02, /*                                            */
    0x03, 0xa7, 0x1b, /*                                            */
    0x3e, 0xd5, 0x54, /*                                            */
    0x03, 0xa7, 0x1c, /*                                            */
    0x01, 0x0a, 0x4a, /*                                            */
    0x03, 0xa7, 0x1d, /*                                            */
    0x01, 0x1a, 0x23, /*                                            */
    0x03, 0xa7, 0x1e, /*                                            */
    0x01, 0x2a, 0xed, /*                                            */
    0x03, 0xa7, 0x1f, /*                                            */
    0x01, 0x3c, 0xb8, /*                                            */
    0x03, 0xa7, 0x20, /*                                            */
    0x01, 0x4f, 0x92, /*                                            */
    0x03, 0xa7, 0x21, /*                                            */
    0x01, 0x63, 0x8d, /*                                            */
    0x03, 0xa7, 0x22, /*                                            */
    0x01, 0x78, 0xb9, /*                                            */
    0x03, 0xa7, 0x23, /*                                            */
    0x01, 0x8f, 0x28, /*                                            */
    0x03, 0xa7, 0x24, /*                                            */
    0x01, 0xa6, 0xef, /*                                            */
    0x03, 0xa7, 0x25, /*                                            */
    0x01, 0xc0, 0x21, /*                                            */
    0x03, 0xa7, 0x26, /*                                            */
    0x01, 0xda, 0xd6, /*                                            */
    0x03, 0xa7, 0x27, /*                                            */
    0x01, 0xf7, 0x24, /*                                            */
    0x03, 0xa7, 0x28, /*                                            */
    0x02, 0x15, 0x24, /*                                            */
    0x03, 0xa7, 0x29, /*                                            */
    0x02, 0x34, 0xf1, /*                                            */
    0x03, 0xa7, 0x2a, /*                                            */
    0x02, 0x56, 0xa6, /*                                            */
    0x03, 0xa7, 0x2b, /*                                            */
    0x02, 0x7a, 0x63, /*                                            */
    0x03, 0xa7, 0x2c, /*                                            */
    0x02, 0xa0, 0x46, /*                                            */
    0x03, 0xa7, 0x2d, /*                                            */
    0x02, 0xc8, 0x71, /*                                            */
    0x03, 0xa7, 0x2e, /*                                            */
    0x02, 0xf3, 0x0a, /*                                            */
    0x03, 0xa7, 0x2f, /*                                            */
    0x03, 0x20, 0x37, /*                                            */
    0x03, 0xa7, 0x30, /*                                            */
    0x03, 0x50, 0x20, /*                                            */
    0x03, 0xa7, 0x31, /*                                            */
    0x03, 0x82, 0xf4, /*                                            */
    0x03, 0xa7, 0x32, /*                                            */
    0x03, 0xb8, 0xe0, /*                                            */
    0x03, 0xa7, 0x33, /*                                            */
    0x03, 0xf2, 0x17, /*                                            */
    0x03, 0xa7, 0x34, /*                                            */
    0x04, 0x2e, 0xd1, /*                                            */
    0x03, 0xa7, 0x35, /*                                            */
    0x04, 0x6f, 0x48, /*                                            */
    0x03, 0xa7, 0x36, /*                                            */
    0x04, 0xb3, 0xba, /*                                            */
    0x03, 0xa7, 0x37, /*                                            */
    0x04, 0xfc, 0x6d, /*                                            */
    0x03, 0xa7, 0x38, /*                                            */
    0x05, 0x49, 0xa9, /*                                            */
    0x03, 0xa7, 0x39, /*                                            */
    0x05, 0x9b, 0xbf, /*                                            */
    0x03, 0xa7, 0x3a, /*                                            */
    0x05, 0xf3, 0x04, /*                                            */
    0x03, 0xa7, 0x3b, /*                                            */
    0x06, 0x4f, 0xd8, /*                                            */
    0x03, 0xa7, 0x3c, /*                                            */
    0x06, 0xb2, 0xa2, /*                                            */
    0x03, 0xa7, 0x3d, /*                                            */
    0x07, 0x1b, 0xd1, /*                                            */
    0x03, 0xa7, 0x3e, /*                                            */
    0x07, 0x8b, 0xe2, /*                                            */
    0x03, 0xa7, 0x3f, /*                                            */
    0x08, 0x03, 0x5e, /*                                            */
    0x03, 0xa7, 0x40, /*                                            */
    0x08, 0x82, 0xdc, /*                                            */
    0x03, 0xa7, 0x41, /*                                            */
    0x09, 0x0b, 0x05, /*                                            */
    0x03, 0xa7, 0x42, /*                                            */
    0x09, 0x9c, 0x95, /*                                            */
    0x03, 0xa7, 0x43, /*                                            */
    0x0a, 0x38, 0x5e, /*                                            */
    0x03, 0xa7, 0x44, /*                                            */
    0x0a, 0xdf, 0x51, /*                                            */
    0x03, 0xa7, 0x45, /*                                            */
    0x0b, 0x92, 0x7d, /*                                            */
    0x03, 0xa7, 0x46, /*                                            */
    0x0c, 0x53, 0x17, /*                                            */
    0x03, 0xa7, 0x47, /*                                            */
    0x0d, 0x22, 0x84, /*                                            */
    0x03, 0xa7, 0x48, /*                                            */
    0x0e, 0x02, 0x61, /*                                            */
    0x03, 0xa7, 0x49, /*                                            */
    0x0e, 0xf4, 0x91, /*                                            */
    0x03, 0xa7, 0x4a, /*                                            */
    0x0f, 0xfb, 0x4d, /*                                            */
    0x03, 0xa7, 0x4b, /*                                            */
    0x11, 0x19, 0x37, /*                                            */
    0x03, 0xa7, 0x4c, /*                                            */
    0x12, 0x51, 0x77, /*                                            */
    0x03, 0xa7, 0x4d, /*                                            */
    0x13, 0xa7, 0xe3, /*                                            */
    0x03, 0xa7, 0x4e, /*                                            */
    0x15, 0x21, 0x2b, /*                                            */
    0x03, 0xa7, 0x4f, /*                                            */
    0x16, 0xc3, 0x21, /*                                            */
    0x03, 0xa7, 0x50, /*                                            */
    0x18, 0x95, 0x18, /*                                            */
    0x03, 0xa7, 0x51, /*                                            */
    0x1a, 0xa0, 0x69, /*                                            */
    0x03, 0xa7, 0x52, /*                                            */
    0x1c, 0xf1, 0x39, /*                                            */
    0x03, 0xa7, 0x53, /*                                            */
    0x1f, 0x97, 0xa2, /*                                            */
    0x03, 0xa7, 0x54, /*                                            */
    0x22, 0xa9, 0x72, /*                                            */
    0x03, 0xa7, 0x55, /*                                            */
    0x26, 0x45, 0x00, /*                                            */
    0x03, 0xa7, 0x56, /*                                            */
    0x2a, 0x95, 0xce, /*                                            */
    0x03, 0xa7, 0x57, /*                                            */
    0x2f, 0xdc, 0xa1, /*                                            */
    0x03, 0xa7, 0x58, /*                                            */
    0x36, 0x7e, 0x5d, /*                                            */
    0x03, 0xa7, 0x59, /*                                            */
    0x3f, 0x21, 0x66, /*                                            */
    0x03, 0xa7, 0x5a, /*                                            */
    0x4a, 0xed, 0x42, /*                                            */
    0x03, 0xa7, 0x5b, /*                                            */
    0x5c, 0x26, 0x73, /*                                            */
    0x03, 0xa7, 0x5c, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x5d, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x5e, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x5f, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x60, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x61, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x62, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x63, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x64, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x65, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x66, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    0x03, 0xa7, 0x67, /*                                            */
    0x77, 0xf3, 0xd5, /*                                            */
    /* Patch End */

    0x00, 0xac, 0x00, /* Command COMLET_SETTING                     */
    0x00, 0x85, 0x00, /* Command Internal                           */
};

extern void HalDspInit( void )
{
    HalDspResetHold();
}

extern void HalDspResetHold(void)
{
    RST_DSP = 0;
}
extern void HalDspResetFree(void)
{
    RST_DSP = 1;
}

extern void HalDspSendInitCode(uint8_t initCode)
{
    uint8_t txbuf[3] = {0x00, 0xB7, 0x00};
    uint8_t swVersion[3] = {0xFF, 0xFF, 0xFF};
    uint8_t chipVersion[3] = {0xFF, 0xFF, 0xFF};
    
    HalDspRead(txbuf, swVersion);
    txbuf[1] = 0xB8;
    HalDspRead(txbuf, chipVersion);
    
    switch (initCode)
    {
        case HAL_DSP_INIT_CODE_0:
            HalDspWrite(MaxxAudio_Cmd_FULL_NoProcess, sizeof(MaxxAudio_Cmd_FULL_NoProcess));
        break;
        
        case HAL_DSP_INIT_CODE_1:
            HalDspWrite(MaxxAudio_Cmd_FULL_MaxxBase, sizeof(MaxxAudio_Cmd_FULL_MaxxBase));
        break;

        default:
            HAL_ASSERT_FORCED();
        break;
    }
    
    if(swVersion[0] != 0x00 || swVersion[1] != 0x06 || swVersion[2] != 0x06 ||
       chipVersion[0] != 0x00 || chipVersion[1] != 0x60 || chipVersion[2] != 0x00)
    {
        HAL_ASSERT_FORCED();
    }
}

extern void HalDspWrite(const uint8_t *buf, uint16_t len)
{
    uint8_t err;
    uint16_t i;

    err = 0;

    HAL_DSP_OPEN();
    
    /* Send start */
    I2C_START(I2C0);
    I2C_WAIT_READY(I2C0);

    /* Send control byte */
    I2C_SET_DATA(I2C0, DSP_WR_ADDR);
    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    I2C_WAIT_READY(I2C0);
    if(I2C_GET_STATUS(I2C0) == 0x18)
    {
        /* ACK */
        for(i = 0; i < len; i++)
        {
            /* Send data */
            I2C_SET_DATA(I2C0, buf[i]); // data
            I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
            I2C_WAIT_READY(I2C0);
            if(I2C_GET_STATUS(I2C0) == 0x30)
            {
                /* NACK */
                /* Send data error */
                err = 4;
            }
        }
    }
    else
    {
        /* NACK */
        /* Send control error */
        err = 1;
    }

    HAL_DSP_STOP_CLOSE();
    HAL_ASSERT( err == 0 );
    
}

extern void HalDspRead(const uint8_t cmd[3], uint8_t val[3])
{
    uint8_t err;
    uint16_t i;
    
    HAL_ASSERT( cmd != NULL );
    HAL_ASSERT( val != NULL );

    err = 0;

    HAL_DSP_OPEN();
    
    /* Send start */
    I2C_START(I2C0);
    I2C_WAIT_READY(I2C0);

    /* Send control byte */
    I2C_SET_DATA(I2C0, DSP_WR_ADDR);
    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    I2C_WAIT_READY(I2C0);
    if(I2C_GET_STATUS(I2C0) == 0x18)
    {
        /* ACK */

        /* Send high address */
        
        for(i = 0; i < 3; i++)
        {
            I2C_SET_DATA(I2C0, cmd[i]);
            I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
            I2C_WAIT_READY(I2C0);
            if(I2C_GET_STATUS(I2C0) != 0x28)
            {
                break;
            }
        }
        
        if(i == 3)
        {
            /* ACK */
            /* Send restart */
            I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STA | I2C_I2CON_SI);
            I2C_WAIT_READY(I2C0);
            if(I2C_GET_STATUS(I2C0) == 0x10)
            {
                /* ACK */

                /* Send control byte */
                I2C_SET_DATA(I2C0, DSP_RD_ADDR);
                I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
                I2C_WAIT_READY(I2C0);
                if(I2C_GET_STATUS(I2C0) == 0x40)
                {
                    for(i = 0; i < 3 - 1; i++)
                    {
                        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI | I2C_I2CON_AA);
                        I2C_WAIT_READY(I2C0);

                        /* Read data */
                        val[i] = I2C_GET_DATA(I2C0);
                    }

                    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
                    I2C_WAIT_READY(I2C0);
                    val[i] = I2C_GET_DATA(I2C0);
                }
                else
                {
                    /* NACK */

                    /* Send control read error */
                    err = 4;
                }
            }
            else
            {
                /* NACK */

                /* Send start error */
                err = 3;
            }
        }
        else
        {
            /* NACK */

            /* Send high address error */
            err = 2;
        }
    }
    else
    {
        /* NACK */

        /* Send control write error */
        err = 1;

    }
    
    HAL_DSP_STOP_CLOSE();
    HAL_ASSERT( err == 0 );
}

extern void HalDspAudioSrcCtrl(uint8_t src, uint8_t ctrl )
{
    if( src & HAL_DSP_AUDIO_SRC_SDI0 )
    {
        if( ctrl == HAL_DSP_CTRL_OFF )
        {
            cmd[0] = 0x00;
            cmd[1] = 0x00;
            cmd[2] = 0x00;
        }
        else
        {
            cmd[0] = 0x0F;
            cmd[1] = 0xBF;
            cmd[2] = 0x58;
        }
    }

    if( src & HAL_DSP_AUDIO_SRC_SDI1 )
    {
        if( ctrl == HAL_DSP_CTRL_OFF )
        {
            cmd[3] = 0x00;
            cmd[4] = 0x00;
            cmd[5] = 0x00;
        }
        else
        {
            cmd[3] = 0x0F;
            cmd[4] = 0xBF;
            cmd[5] = 0x58;
        }
    }

    if( src & HAL_DSP_AUDIO_SRC_ADC )
    {
        if( ctrl == HAL_DSP_CTRL_OFF )
        {
            cmd[9] = 0x00;
            cmd[10] = 0x00;
            cmd[11] = 0x00;
        }
        else
        {
            cmd[9] = 0x10;
            cmd[10] = 0x1D;
            cmd[11] = 0x3F;
        }
    }

    
    HalDspWrite(start, sizeof(start));
    HalDspWrite(cmd, sizeof(cmd));
    HalDspWrite(end, sizeof(end)); 
}


extern void HalDspSetVol(uint32_t vol)
{   
    if(vol > 0x7FFFFF)
    {
        vol = 0x7FFFFF;
    }

    //M9 - SDO0 from Processed path
    cmd[24] = (uint8_t)((vol >> 16) & 0xFF);
    cmd[25] = (uint8_t)((vol >>  8) & 0xFF);
    cmd[26] = (uint8_t)((vol >>  0) & 0xFF);

    //M11 - DAC1 from Processed path
    cmd[30] = (uint8_t)((vol >> 16) & 0xFF);
    cmd[31] = (uint8_t)((vol >>  8) & 0xFF);
    cmd[32] = (uint8_t)((vol >>  0) & 0xFF);
    
    HalDspWrite(start, sizeof(start));
    HalDspWrite(cmd, sizeof(cmd));
    HalDspWrite(end, sizeof(end));
}

