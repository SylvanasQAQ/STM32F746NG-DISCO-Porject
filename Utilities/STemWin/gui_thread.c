/**
 * @Brief   用于 GUI 的线程
 * @Dependency  
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "os_threads.h"
#include "DIALOG.h"
#include "fatfs.h"
#include "usb_device.h"
#include "resources.h"





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
 * @param  void *argument
 * @retval None
 */
void vGUITaskCreate()
{
#ifdef CMSIS_V1
	xTaskCreate(GUIThread, "GUI Task", 512, NULL, osPriorityHigh, &guiTaskHandle);
#endif

#ifdef CMSIS_V2
	guiTaskHandle = osThreadNew(GUIThread, NULL, &guiTask_attributes);
#endif
}


FRESULT ret[4];
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

	/* Initialize GUI */
	GUI_Init();

	
	MX_USB_DEVICE_Init();
  	MX_FATFS_Init();
	  
	ret[0] = f_mount(&SDFatFS, "", 0);
	ret[1] = f_open(&SDFile, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
	ret[2] = f_printf(&SDFile, "Hello %d\n", 112);
	ret[3] = f_close(&SDFile);

	hDesktop = CreateDesktop();
	hTaskBar = CreateTaskBar();
	hClockWindow = CreateClockWindow();
	hHomeWindow = CreateWindow();
	hAlarmWindow = CreateAlarmWindow();
	hAudioWindow = CreateAudioWindow();
	hFreqAnalysisWindow = CreateFreqAnalysisWindow();

	WM_AttachWindow(hClockWindow, hDesktop);
	WM_AttachWindow(hHomeWindow, hDesktop);
	WM_AttachWindow(hAlarmWindow, hDesktop);
	WM_AttachWindow(hAudioWindow, hDesktop);
	WM_AttachWindow(hFreqAnalysisWindow, hDesktop);

	WM_BringToTop(hCurrentWindow = hHomeWindow);


	
	/* Gui background Task */
	while (1)
	{
		GUI_Exec(); /* Do the background work ... Update windows etc.) */
		vTaskDelay(20); /* Nothing left to do for the moment ... Idle processing */
	}
}








