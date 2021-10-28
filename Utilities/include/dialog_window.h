/**
 * @brief   提醒对话框的头文件，需用用到提醒对话框时，包含该头文件即可
 * 
 * 
 */
#ifndef __DIALOG_ALARM_H
#define __DIALOG_ALARM_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DIALOG.h"



/* Defines ------------------------------------------------------------------*/
typedef uint16_t    DIALOG_TYPE;

#define DIALOG_NOTHING         0
#define DIALOG_ALARM           1



/* Functions prototypes ---------------------------------------------*/
WM_HWIN CreateAlarmDialog_Self(char * title, char * text, DIALOG_TYPE type);




/* Exported variables -----------------------------------------------------------*/
extern uint16_t       DIALOG_TOP_EXIST;
extern WM_HWIN        hStayOnTopWindow;

#endif