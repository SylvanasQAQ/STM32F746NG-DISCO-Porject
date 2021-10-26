/**
 * @Brief   用于 GUI 的线程
 * @Dependency  
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DIALOG.h"





/* Functions prototypes ---------------------------------------------*/
static void GUIThread(void *argument);





/* Private variables -----------------------------------------------------------*/
GUI_HWIN hCurrentWindow;
GUI_HWIN hDesktop;
GUI_HWIN hTaskBar;
GUI_HWIN hHomeWindow;
GUI_HWIN hClockWindow;
GUI_HWIN hAlarmWindow;
GUI_HWIN hAudioWindow;
GUI_HWIN hFreqAnalysisWindow;
GUI_HWIN hMusicWindow;
GUI_HWIN hFileDialog;





/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
TaskHandle_t guiTaskHandle;
#endif

#ifdef CMSIS_V2
osThreadId_t guiTaskHandle;
const osThreadAttr_t guiTask_attributes = {
	.name = "GUI Task",
	.stack_size = 1024 * 4,
	.priority = (osPriority_t)osPriorityHigh,
};
#endif


/**
 * @brief  GUI Thread 的包装函数，用于 osThreadNew()
 * @param  None
 * @retval None
 */
void vGUITaskCreate()
{
#ifdef CMSIS_V1
	xTaskCreate(GUIThread, "GUI Task", 824, NULL, osPriorityLow, &guiTaskHandle);
#endif

#ifdef CMSIS_V2
	guiTaskHandle = osThreadNew(GUIThread, NULL, &guiTask_attributes);
#endif
}



/**
 * @brief  Start GUI task
 * @param  argument: pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void GUIThread(void *argument)
{
	extern WM_HWIN CreateWindow(void);
	extern WM_HWIN CreateTaskBar(void);
	extern WM_HWIN CreateDesktop(void);
	extern WM_HWIN CreateClockWindow(void);
	extern WM_HWIN CreateAlarmWindow(void);
	extern WM_HWIN CreateAudioWindow(void);
	extern WM_HWIN CreateFreqAnalysisWindow(void);
	extern WM_HWIN CreateMusicWindow(void);

	/* Initialize GUI */
	GUI_Init();

	hDesktop = CreateDesktop();
	hTaskBar = CreateTaskBar();
	hClockWindow = CreateClockWindow();
	hHomeWindow = CreateWindow();
	hAlarmWindow = CreateAlarmWindow();
	hAudioWindow = CreateAudioWindow();
	hFreqAnalysisWindow = CreateFreqAnalysisWindow();
	hMusicWindow = CreateMusicWindow();
	

	WM_AttachWindow(hClockWindow, hDesktop);
	WM_AttachWindow(hHomeWindow, hDesktop);
	WM_AttachWindow(hAlarmWindow, hDesktop);
	WM_AttachWindow(hAudioWindow, hDesktop);
	WM_AttachWindow(hFreqAnalysisWindow, hDesktop);
	WM_AttachWindow(hMusicWindow, hDesktop);

	WM_BringToTop(hCurrentWindow = hHomeWindow);


	
	/* Gui background Task */
	while (1)
	{
		GUI_Exec(); /* Do the background work ... Update windows etc.) */
		vTaskDelay(20); /* Nothing left to do for the moment ... Idle processing */
	}
}








