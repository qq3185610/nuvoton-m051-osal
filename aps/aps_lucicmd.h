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
#ifndef __HAL_LUCICMD_H__
#define __HAL_LUCICMD_H__


#include <stdint.h>

#define HAL_LUCICMD_MSGBOX_NULL                                 0
#define HAL_LUCICMD_MSGBOX_VersionInfo                          5
#define HAL_LUCICMD_MSGBOX_HostVersionInfo                      6
#define HAL_LUCICMD_MSGBOX_IsHostPresent                        9
#define HAL_LUCICMD_MSGBOX_UpdateSource                         10
#define HAL_LUCICMD_MSGBOX_IsAllowed                            11
#define HAL_LUCICMD_MSGBOX_Airplay                              12
#define HAL_LUCICMD_MSGBOX_ACPShareCmd                          14
#define HAL_LUCICMD_MSGBOX_ACPShareResponse                     15
#define HAL_LUCICMD_MSGBOX_PowerSaveDeepSleepStart              20
#define HAL_LUCICMD_MSGBOX_PowerSaveDeepSleepEnd                21
#define HAL_LUCICMD_MSGBOX_PowerSaveStandByStart                22
#define HAL_LUCICMD_MSGBOX_PowerSaveStandByEnd                  23
#define HAL_LUCICMD_MSGBOX_PowerSaveStatus                      24
#define HAL_LUCICMD_MSGBOX_DeviceDeattachmentStatus             36
#define HAL_LUCICMD_MSGBOX_GracefulShutdown                     37
#define HAL_LUCICMD_MSGBOX_DeviceAttachmentStatus               38
#define HAL_LUCICMD_MSGBOX_PlayCtrlCmd                          40
#define HAL_LUCICMD_MSGBOX_BrowseCtrlCmd                        41
#define HAL_LUCICMD_MSGBOX_GetUI                                42
#define HAL_LUCICMD_MSGBOX_CurrentTime                          49
#define HAL_LUCICMD_MSGBOX_CurrentSource                        50
#define HAL_LUCICMD_MSGBOX_CurrentPlayState                     51
#define HAL_LUCICMD_MSGBOX_IsPlayStatus                         54
#define HAL_LUCICMD_MSGBOX_VolumeCtrl                           64
#define HAL_LUCICMD_MSGBOX_FirmwareUpgradeRequest               65
#define HAL_LUCICMD_MSGBOX_FirmwareUpgradeProcess               66
#define HAL_LUCICMD_MSGBOX_HostImagePresent                     68
#define HAL_LUCICMD_MSGBOX_RequestFirmwareUpgrade               69
#define HAL_LUCICMD_MSGBOX_AppCtrl                              70
#define HAL_LUCICMD_MSGBOX_SDCardStatus                         71
#define HAL_LUCICMD_MSGBOX_SpotifyPresetActions                 75
#define HAL_LUCICMD_MSGBOX_PlayAudioIndex                       80
#define HAL_LUCICMD_MSGBOX_DeviceName                           90
#define HAL_LUCICMD_MSGBOX_MACID                                91
#define HAL_LUCICMD_MSGBOX_AuxInputStart                        95
#define HAL_LUCICMD_MSGBOX_AuxInputStop                         96
#define HAL_LUCICMD_MSGBOX_ExternalPlayback                     97
#define HAL_LUCICMD_MSGBOX_DDMS_Trigger                         100
#define HAL_LUCICMD_MSGBOX_StandAloneMode                       101
#define HAL_LUCICMD_MSGBOX_SearchLsModule                       102
#define HAL_LUCICMD_MSGBOX_QueryDDMS                            103
#define HAL_LUCICMD_MSGBOX_SetZoneID                            104
#define HAL_LUCICMD_MSGBOX_DDMS_SSID                            105
#define HAL_LUCICMD_MSGBOX_SpeakerType                          106
#define HAL_LUCICMD_MSGBOX_SceneName                            107
#define HAL_LUCICMD_MSGBOX_TcpipTunnelingStart                  111
#define HAL_LUCICMD_MSGBOX_TunelData                            112
#define HAL_LUCICMD_MSGBOX_MiracastCtrl                         113
#define HAL_LUCICMD_MSGBOX_RebootRequest                        114
#define HAL_LUCICMD_MSGBOX_RebootCmd                            115
#define HAL_LUCICMD_MSGBOX_NetworkConnectionStatus              124
#define HAL_LUCICMD_MSGBOX_ConfigureNetwork                     125
#define HAL_LUCICMD_MSGBOX_iDeviceWifiSettingsSharing           126
#define HAL_LUCICMD_MSGBOX_WPS_ConfigurationStatus              140
#define HAL_LUCICMD_MSGBOX_WPS_Configuration                    141
#define HAL_LUCICMD_MSGBOX_NetworkConfiguration                 142
#define HAL_LUCICMD_MSGBOX_NetworkConfigurationStatus           143
#define HAL_LUCICMD_MSGBOX_StopWAC                              144
#define HAL_LUCICMD_MSGBOX_FactoryReset                         150
#define HAL_LUCICMD_MSGBOX_NvReadWrite                          208
#define HAL_LUCICMD_MSGBOX_BluetoothCtrlCmd                     209
#define HAL_LUCICMD_MSGBOX_DMR_Restart                          210
#define HAL_LUCICMD_MSGBOX_StartFirmwareUpgrade                 211
#define HAL_LUCICMD_MSGBOX_NamePasswordNotifier                 213
#define HAL_LUCICMD_MSGBOX_EnShareMode                          214
#define HAL_LUCICMD_MSGBOX_EnPairMode                           215
#define HAL_LUCICMD_MSGBOX_ZoneVolumeCtrl                       219
#define HAL_LUCICMD_MSGBOX_PairStatus                           221
#define HAL_LUCICMD_MSGBOX_CastOTAUpgrade                       222
#define HAL_LUCICMD_MSGBOX_FirmwareUpgradeInternet              223
#define HAL_LUCICMD_MSGBOX_CastIsEnabled                        224
#define HAL_LUCICMD_MSGBOX_GoogleCastSettingInfo                226
#define HAL_LUCICMD_MSGBOX_GetNTPTime                           229
#define HAL_LUCICMD_MSGBOX_AC_Powered                           232
#define HAL_LUCICMD_MSGBOX_UartInterfaceReady                   0xAAAA
#define HAL_LUCICMD_MSGBOX_CASTSetupStart                       0x01EE
    
#define HAL_LUCICMD_TYPE_GET                                    0x01
#define HAL_LUCICMD_TYPE_SET                                    0x02

#define HAL_LUCICMD_STATUS_NA                                   0x00
#define HAL_LUCICMD_STATUS_OK                                   0x01
#define HAL_LUCICMD_STATUS_GENERIC_ERR                          0x02
#define HAL_LUCICMD_STATUS_DEV_NOT_RDY                          0x03
#define HAL_LUCICMD_STATUS_CRC_ERR                              0x04

#define HAL_LUCICMD_PLAYCTRL_PLAY     .0
#define HAL_LUCICMD_PLAYCTRL_PAUSE     1
#define HAL_LUCICMD_PLAYCTRL_STOP      2
#define HAL_LUCICMD_PLAYCTRL_RESUME    3
#define HAL_LUCICMD_PLAYCTRL_NEXT      4
#define HAL_LUCICMD_PLAYCTRL_PREV      5
#define HAL_LUCICMD_PLAYCTRL_MUTE      6
#define HAL_LUCICMD_PLAYCTRL_UNMUTE    7

#define HAL_LUCICMD_AUDIOSRC_NONE                  0
#define HAL_LUCICMD_AUDIOSRC_Airplay               1
#define HAL_LUCICMD_AUDIOSRC_DMR                   2
#define HAL_LUCICMD_AUDIOSRC_DMP                   3
#define HAL_LUCICMD_AUDIOSRC_Spotify               4
#define HAL_LUCICMD_AUDIOSRC_USB                   5
#define HAL_LUCICMD_AUDIOSRC_SDCard                6
#define HAL_LUCICMD_AUDIOSRC_Melon                 7
#define HAL_LUCICMD_AUDIOSRC_vTuner                8
#define HAL_LUCICMD_AUDIOSRC_TuneIn                9
#define HAL_LUCICMD_AUDIOSRC_Miracast              10
#define HAL_LUCICMD_AUDIOSRC_Reserved11            11
#define HAL_LUCICMD_AUDIOSRC_DDMS_Slave            12
#define HAL_LUCICMD_AUDIOSRC_Reserved13            13
#define HAL_LUCICMD_AUDIOSRC_LineIn                14
#define HAL_LUCICMD_AUDIOSRC_Reserved15            15
#define HAL_LUCICMD_AUDIOSRC_AppleDevice           16
#define HAL_LUCICMD_AUDIOSRC_DirectURL             17
#define HAL_LUCICMD_AUDIOSRC_Reserved18            18
#define HAL_LUCICMD_AUDIOSRC_Bluetooth             19
#define HAL_LUCICMD_AUDIOSRC_Reserved20            20
#define HAL_LUCICMD_AUDIOSRC_Deezer                21
#define HAL_LUCICMD_AUDIOSRC_Tidal                 22
#define HAL_LUCICMD_AUDIOSRC_Favorites             23
#define HAL_LUCICMD_AUDIOSRC_GoogleCast            24
#define HAL_LUCICMD_AUDIOSRC_ExtSrcCtrlByHostMCU   25

#define HAL_LUCICMD_WIFIMODE_NONE                  0
#define HAL_LUCICMD_WIFIMODE_SA                    1
#define HAL_LUCICMD_WIFIMODE_HN                    2
#define HAL_LUCICMD_WIFIMODE_CFG                   3

#define HAL_LUCICMD_PLAYSTAT_PLAYING               0
#define HAL_LUCICMD_PLAYSTAT_STOPPED               1
#define HAL_LUCICMD_PLAYSTAT_PAUSED                2
#define HAL_LUCICMD_PLAYSTAT_CONNECTING            3
#define HAL_LUCICMD_PLAYSTAT_RECEIVING             4
#define HAL_LUCICMD_PLAYSTAT_BUFFERING             5

#define HAL_LUCICMD_NWCFGSTAT_Initializing         0
#define HAL_LUCICMD_NWCFGSTAT_Ready                1
#define HAL_LUCICMD_NWCFGSTAT_ConfigStart          2
#define HAL_LUCICMD_NWCFGSTAT_ConfigReceived       3
#define HAL_LUCICMD_NWCFGSTAT_ConfigCompleted      4
#define HAL_LUCICMD_NWCFGSTAT_Stop                 5
#define HAL_LUCICMD_NWCFGSTAT_Error                6
#define HAL_LUCICMD_NWCFGSTAT_WACMsgTimeout        7
#define HAL_LUCICMD_NWCFGSTAT_WACMsgConfigFailed   8
#define HAL_LUCICMD_NWCFGSTAT_SACFailed            9

typedef struct hal_luci_cmd_t {
    char *pData;
    uint16_t remoteID;
    uint16_t msgBox;
    uint16_t len;
    uint16_t crc;
    uint8_t type;
    uint8_t status;
} HAL_LUCI_CMD_t;

extern void HalLuciCmdInit     ( void );
extern void HalLuciCmdEnable   ( void );
extern void HalLuciCmdDisable  ( void );
extern void HalLuciCmdReset    ( void );

extern void HalLuciCmdSendIsHostPresent ( void );
extern void HalLuciCmdSendSetVolume     ( uint8_t vol );
extern void HalLuciCmdSendQryVolume     ( void );
extern void HalLuciCmdSendSavVolume     ( uint8_t vol );
extern void HalLuciCmdSendAuxInStart    ( void );
extern void HalLuciCmdSendAuxInStop     ( void );
extern void HalLuciCmdSendPlayUSB       ( void );
extern void HalLuciCmdSendSetWiFiModeHN ( void );
extern void HalLuciCmdSendSetWiFiModeCFG( void );
extern void HalLuciCmdSendSetWiFiModeSA ( void );
extern void HalLuciCmdSendPlayCtrl      ( uint8_t play_ctrl );
extern void HalLuciCmdSendQryMacID      ( void );
extern void HalLuciCmdSendIsAllowed     ( void );
extern void HalLuciCmdSendIsNotAllowed  ( void );


#endif /* __HAL_LUCICMD_H__ */
