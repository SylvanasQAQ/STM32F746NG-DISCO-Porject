#ifndef __OS_THREADS_H
#define __OS_THREADS_H
#include "main.h"

/* Exported variables ---------------------------------------------*/
extern GUI_HWIN hCurrentWindow;
extern GUI_HWIN hDesktop;
extern GUI_HWIN hHomeWindow;
extern GUI_HWIN hClockWindow;
extern GUI_HWIN hAlarmWindow;
extern GUI_HWIN hTaskBar;



// GUI thread
extern void gui_thread(void *argument);
extern const osThreadAttr_t guiTask_attributes;
extern osThreadId_t guiTaskHandle;


// APP alarm thread
extern void app_alarm_thread(void *argument);
extern osThreadId_t app_alarmTaskHandle;
extern const osThreadAttr_t app_alarmTask_attributes;


#endif