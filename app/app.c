/******************************************************************************

 @file  app.c

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "osal.h"
//#include "hal.h"
#include "app.h"

#include "aps_cli.h"
//#include "app_task_key.h"
#include "app_task_cli.h"
//#include "app_task_lucicop.h"
//#include "app_task_bt.h"
#include "app_task_dongle.h"
/**************************************************************************************************
 * TYPES
 **************************************************************************************************/

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/


/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/
//APP_INFO_t AppInfo;

extern void AppInit ( void )
{
    //osal_task_create( AppTask_CLI, OSAL_TASK_ID_APP_CLI );
    //AppTask_CLI_Init();
    
    //osal_task_create( AppTask_Key, OSAL_TASK_ID_APP_KEY );
    //AppTask_Key_Init();

    //osal_task_create( AppTask_LuciCoP, OSAL_TASK_ID_APP_LUCI_COP );
    //AppTask_LuciCoP_Init();

    //osal_task_create( AppTask_BT, OSAL_TASK_ID_APP_BT );
    //AppTask_BT_Init();
    
    //Application task init and create
    osal_task_create( AppTask_CLI, OSAL_TASK_ID_APP_CLI );
    AppTask_CLI_Init();

    osal_task_create( AppTask_Dongle, OSAL_TASK_ID_APP_DONGLE );
    AppTask_Dongle_Init();

    //Application Support Sublayer module init
    ApsCliInit();
    ApsCliRegisterTask( OSAL_TASK_ID_APP_CLI );
    
}




/**************************************************************************************************
**************************************************************************************************/

