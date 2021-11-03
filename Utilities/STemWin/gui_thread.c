/**
 * @Brief   用于 GUI 的线程
 * @Dependency  
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DIALOG.h"
#include "os_state.h"
#include "os_threads.h"





/* Functions prototypes ---------------------------------------------*/
static void GUIThread(void *argument);
static void RecoverFromReboot();
static void StartupScreen();

extern WM_HWIN CreateWindow(void);
extern WM_HWIN CreateTaskBar(void);
extern WM_HWIN CreateDesktop(void);
extern WM_HWIN CreateClockWindow(void);
extern WM_HWIN CreateAlarmWindow(void);
extern WM_HWIN CreateAudioWindow(void);
extern WM_HWIN CreateFreqAnalysisWindow(void);
extern WM_HWIN CreateMusicWindow(void);



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
	xTaskCreate(GUIThread, "GUI Task", 824, NULL, (UBaseType_t)osPriorityLow, &guiTaskHandle);
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
	/* Initialize GUI */
	GUI_Init();

	StartupScreen();


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

	RecoverFromReboot();

	
	/* Gui background Task */
	while (1)
	{
		GUI_Exec(); 	/* Do the background work ... Update windows etc.) */
		vTaskDelay(5); /* Nothing left to do for the moment ... Idle processing */
	}
}


static void RecoverFromReboot()
{
	extern U16      Music_Item_Current;
    extern char     musicPath[100];
    extern WM_HWIN  hListView;
	extern uint32_t uiWavPlayIndex;
	extern U16      Music_Play_Start;
	extern U16      Music_Play_Restart;
	extern void 	MoveToMusicWindow(WM_HWIN hWin);
	extern void 	MusicWindowPlayButtonHit(WM_HWIN hWin);

    static uint16_t i, numRows;
    static os_reboot_info_t   *pRebootInfo = (os_reboot_info_t*)REBOOT_STORAGE_AREA;

	if(pRebootInfo->magic == 0x1234567887654321)		// 重启标识
	{
		uiWavPlayIndex = pRebootInfo->uiWavPlayIndex;
		Music_Item_Current = pRebootInfo->Music_Item_Current;
        Music_Play_Restart = pRebootInfo->Music_Play_Restart;
		strcpy(musicPath, pRebootInfo->musicPath);

		numRows = pRebootInfo->musicListLen;
		for(i = 0; i < numRows; i++)
		{
			LISTVIEW_AddRow(hListView, NULL);
			LISTVIEW_SetItemText(hListView, 0, i, (pRebootInfo->musicPathList)[i]);
			LISTVIEW_SetItemText(hListView, 1, i, (pRebootInfo->musicDurationList)[i]);
		}
		LISTVIEW_SetSel(hListView, Music_Item_Current);

		memset(pRebootInfo, 0, REBOOT_STORAGE_AREA_SIZE);
		WM_BringToTop(hCurrentWindow = hMusicWindow);
		MoveToMusicWindow(hMusicWindow);
		Music_Play_Start = 1;
		MusicWindowPlayButtonHit(hMusicWindow);
	}
}




char startup_image[20][100] = {
"                           _ooOoo_                             ",
"                          o8888888o                            ",
"                          88' . '88                            ",
"                          (| ^_^ |)                            ",
"                          O\\  =  /O                            ",
"                       ____/`---'\\____                         ",
"                     .'  \\|     |//  `.                       ",
"                    /  \\|||  :  |||//  \\                      ",
"                   /  _||||| -:- |||||-  \\                     ",
"                   |   | \\\\  -  /// |   |                     ",
"                   | \\_|  ''\\---/''  |   |                     ",
"                   \\  .-\\__  `-`  ___/-. /                     ",
"                 ___`. .'  /--.--\\  `. . __                    ",
"              ."" '<  `.___\\_<|>_/___.'  >'"".                 ",
"             | | :  `- \\`.;`\\ _ /`;.`/ - ` : | |               ",
"             \\  \\ `-.   \\_ __\\ /__ _/   .-` /  /               ",
"        ======`-.____`-.___\\_____/___.-`____.-'======          ",
"                           `=---='                             ",
"        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^          ",
"               // NO BUG FOR GOODNESS' SAKE //                  "
};

static void StartupScreen()
{
	static uint16_t i;
	static os_reboot_info_t   *pRebootInfo = (os_reboot_info_t*)REBOOT_STORAGE_AREA;

	if(pRebootInfo->magic == 0x1234567887654321)
		return;

	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_SetFont(GUI_FONT_13B_ASCII);
	GUI_SetColor(GUI_BLACK);
	for(i = 0; i < 20; i++)
		GUI_DispStringHCenterAt(startup_image[i], 240,  5 + i*13);
	vTaskDelay(3000);
}

