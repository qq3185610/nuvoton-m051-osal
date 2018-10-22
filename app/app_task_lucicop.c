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
#include "hal.h"

#include "app_task_lucicop.h"
#include "app_event_luci.h"

#include "stringx.h"

static void luciCommandProcessor( const HAL_LUCI_CMD_t *pLuciCmd );

extern void AppTask_LuciCoP_Init     ( void )
{
}

extern void AppTask_LuciCoP          ( uint8_t task_id, uint8_t event_id )
{
    HAL_LUCI_CMD_t *pMsg;
    
    switch( event_id )
    {
        case OSAL_TASK_EVENT_MSG:

            pMsg = (HAL_LUCI_CMD_t *)osal_msg_recv( task_id );
            while ( pMsg )
            {
                luciCommandProcessor( pMsg );
                osal_msg_delete( pMsg );
                pMsg = osal_msg_recv( task_id );
            }
        break;

        default:
            OSAL_ASSERT_FORCED();
        break;
    }
}

static void luciCommandProcessor( const HAL_LUCI_CMD_t *pLuciCmd )
{   
    uint32_t u32tmp;
    char cActiveNetwork;
    char wlanState;
    uint8_t mode;

    switch ( pLuciCmd->msgBox )
    {
        case HAL_LUCICMD_MSGBOX_CurrentPlayState:
            decstr2uint( pLuciCmd->pData, &u32tmp );
            AppEventLuciUpdatePlayStatus( (uint8_t)u32tmp );
        break;

        case HAL_LUCICMD_MSGBOX_UartInterfaceReady:
            HalLuciCmdSendIsHostPresent();
        break;

        case HAL_LUCICMD_MSGBOX_IsHostPresent:
            AppEventLuciDevBootUp();
        break;

        case HAL_LUCICMD_MSGBOX_VolumeCtrl:
            if( pLuciCmd->status == HAL_LUCICMD_STATUS_NA )
            {
                //Volume from APP
                u32tmp = UINT32_MAX;
                HAL_ASSERT( pLuciCmd->pData != NULL );
                decstr2uint( pLuciCmd->pData, &u32tmp );
                HAL_ASSERT( u32tmp <= 100 );
                AppEventLuciUpdateDevVolume( (uint8_t)u32tmp );
            }
            else if( pLuciCmd->status == HAL_LUCICMD_STATUS_OK )
            {
                //Volume from HW, APP confirmed this volume
            }
            else
            {
                HAL_ASSERT_FORCED();
            }
        break;

        case HAL_LUCICMD_MSGBOX_ZoneVolumeCtrl:
            if( pLuciCmd->status == HAL_LUCICMD_STATUS_NA &&
                pLuciCmd->type   == HAL_LUCICMD_TYPE_SET )
            {
                //Volume from APP
                u32tmp = UINT32_MAX;
                HAL_ASSERT( pLuciCmd->pData != NULL );
                decstr2uint( pLuciCmd->pData, &u32tmp );
                HAL_ASSERT( u32tmp <= 100 );
                AppEventLuciUpdateZoneVolume( (uint8_t)u32tmp );
            }
        break;

        case HAL_LUCICMD_MSGBOX_CurrentSource:
            //Debug
            AppEventLuciDefault( pLuciCmd );
            if( decstr2uint( pLuciCmd->pData, &u32tmp ) == 0 )
                HAL_ASSERT_FORCED();
            AppEventLuciUpdateAudioSrc( (uint8_t)u32tmp );            
        break;

        //case HAL_LUCICMD_MSGBOX_IsAllowed:
        //    HAL_ASSERT( pLuciCmd->status == HAL_LUCICMD_STATUS_OK );
        //break;

        case HAL_LUCICMD_MSGBOX_UpdateSource:
            if( decstr2uint( pLuciCmd->pData, &u32tmp ) == 0 )
                HAL_ASSERT_FORCED();
            if( AppEventLuciRequestAudioSrc( (uint8_t)u32tmp ) == FALSE )
            {
                HalLuciCmdSendIsNotAllowed();
            }
            else
            {
                HalLuciCmdSendIsAllowed();
            }
        break;

        case HAL_LUCICMD_MSGBOX_RebootRequest:
            if( AppEventLuciRequestReboot() == FALSE )
            {
                
            }
            else
            {
                HalLuciCmdReset();
            }
        break;

        case HAL_LUCICMD_MSGBOX_NetworkConnectionStatus:
            cActiveNetwork = pLuciCmd->pData[0];
            wlanState = pLuciCmd->pData[4];

            if(cActiveNetwork == '3' || cActiveNetwork == '1')
            {
                if(wlanState == '0')        mode = HAL_LUCICMD_WIFIMODE_SA;
                else                        mode = HAL_LUCICMD_WIFIMODE_HN;
            }
            else if(cActiveNetwork == '0')  mode = HAL_LUCICMD_WIFIMODE_NONE;
            else if(cActiveNetwork == '4')  mode = HAL_LUCICMD_WIFIMODE_CFG;
            else                            mode = HAL_LUCICMD_WIFIMODE_NONE;

            AppEventLuciUpdateWiFiMode( mode );
        break;


        case HAL_LUCICMD_MSGBOX_NetworkConfigurationStatus:
            if( decstr2uint( pLuciCmd->pData, &u32tmp ) == 0 )
            {
                HAL_ASSERT_FORCED();
            }

            AppEventLuciUpdateNwConfigStatus((uint8_t)u32tmp);
            
        break;
        //case HAL_LUCICMD_MSGBOX_AuxInputStart:
        //break;

        //case HAL_LUCICMD_MSGBOX_AuxInputStop:
        //break;

        //case HAL_LUCICMD_MSGBOX_NvReadWrite:
        //break;

        //case HAL_LUCICMD_MSGBOX_NetworkConfigurationStatus:
        //break;

        //case HAL_LUCICMD_MSGBOX_MACID:
        //break;
        
        default:
            AppEventLuciDefault( pLuciCmd );
        break;
    }
    
}



