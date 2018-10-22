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
#ifndef __OSAL_CONFIG_H__
#define __OSAL_CONFIG_H__



#define     OSAL_ASSERT_EN          1
#define     OSAL_TASK_MAX           10
#define     OSAL_EVENT_MAX          32      //must be 8, 16 or 32
#define     OSAL_FIFO_PAGE_SIZE     64




#define     OSAL_TASK_ID_0          0
#define     OSAL_TASK_ID_1          1
#define     OSAL_TASK_ID_2          2
#define     OSAL_TASK_ID_3          3
#define     OSAL_TASK_ID_4          4
#define     OSAL_TASK_ID_5          5
#define     OSAL_TASK_ID_6          6
#define     OSAL_TASK_ID_7          7
#define     OSAL_TASK_ID_8          8
#define     OSAL_TASK_ID_9          9


#define     OSAL_TASK_ID_APS_LUCI_CMD   OSAL_TASK_ID_0
#define     OSAL_TASK_ID_APS_CLI        OSAL_TASK_ID_1
#define     OSAL_TASK_ID_APS_UIBRD      OSAL_TASK_ID_2
#define     OSAL_TASK_ID_APS_LUCI_COP   OSAL_TASK_ID_5

#define     OSAL_TASK_ID_APP_BT         OSAL_TASK_ID_6
#define     OSAL_TASK_ID_APP_KEY        OSAL_TASK_ID_7
#define     OSAL_TASK_ID_APP_CLI        OSAL_TASK_ID_8
#define     OSAL_TASK_ID_APP_DONGLE     OSAL_TASK_ID_9
























#endif //__OSAL_CONFIG_H__
