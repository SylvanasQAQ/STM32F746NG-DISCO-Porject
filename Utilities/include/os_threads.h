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



// cmsis1
#ifdef CMSIS_V1
// GUI thread handle
extern TaskHandle_t guiTaskHandle;

// Alarm thread handle
extern TaskHandle_t app_alarmTaskHandle;

//Audio thread handle
extern TaskHandle_t app_audioTaskHandle;

//Frequency analysis thread handle
extern TaskHandle_t app_freqAnalysisTaskHandle;

//Storage thread handle
extern TaskHandle_t storageTaskHandle;
#endif


// cmsis2
#ifdef CMSIS_V2
// GUI thread handle
extern osThreadId_t guiTaskHandle;

// APP alarm thread handle
extern osThreadId_t app_alarmTaskHandle;

//Audio thread handle
extern osThreadId_t app_audioTaskHandle;

//Frequency analysis thread handle
extern osThreadId_t app_freqAnalysisTaskHandle;

//Storage thread handle
extern osThreadId_t storageTaskHandle;
#endif



void vGUITaskCreate();
void vAlarmTaskCreate();
void vAudioTaskCreate();
void vFreqAnalysisTaskCreate();
void vStorageTaskCreate();



#endif