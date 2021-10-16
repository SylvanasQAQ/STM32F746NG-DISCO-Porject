#include "main.h"
#include "os_time.h"
#include "app_alarm.h"
#include "os_threads.h"
#include "DIALOG.h"
#include "resources.h"


/*------------------ private function prototype ------------------*/
extern void updateTaskBarTitle();

void rainbowEffect(WM_HWIN hItem, int second);
static void GUIThread(void *argument);
static void updateTaskBarAlarmState(WM_HWIN hAlarmImage);





/*------------------- private variables --------------------------*/

osThreadId_t guiTaskHandle;
const osThreadAttr_t guiTask_attributes = {
	.name = "guiTask",
	.stack_size = 1024 * 3,
	.priority = (osPriority_t)osPriorityHigh,
};

GUI_HWIN hCurrentWindow;
GUI_HWIN hDesktop;
GUI_HWIN hHomeWindow;
GUI_HWIN hClockWindow;
GUI_HWIN hAlarmWindow;
GUI_HWIN hTaskBar;






/*------------------- implementation ----------------------------*/
void gui_thread(void *argument)
{
	GUIThread(argument);
}


/**
 * @brief  Start GUI task
 * @param  argument: pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void GUIThread(void *argument)
{
	extern WM_HWIN CreateWindow_Self(void);
	extern WM_HWIN CreateTaskBar_Self(void);
	extern WM_HWIN CreateDesktop(void);
	extern WM_HWIN CreateClockWindow(void);
	extern WM_HWIN CreateAlarmWindow_Self(void);

	uint32_t 	second;
	WM_HWIN 	hHomeText;
	WM_HWIN 	hAlarmState;

	/* Initialize GUI */
	GUI_Init();

	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();

	hDesktop = CreateDesktop();
	hTaskBar = CreateTaskBar_Self();
	hClockWindow = CreateClockWindow();
	hHomeWindow = CreateWindow_Self();
	hAlarmWindow = CreateAlarmWindow_Self();

	WM_AttachWindow(hClockWindow, hDesktop);
	WM_AttachWindow(hHomeWindow, hDesktop);
	WM_AttachWindow(hAlarmWindow, hDesktop);

	WM_BringToBottom(hClockWindow);
	WM_BringToBottom(hAlarmWindow);
	//WM_BringToTop(hCurrentWindow = hHomeWindow);


	// #define ID_IMAGE_0 (GUI_ID_USER + 0x05)
	hAlarmState = WM_GetDialogItem(hTaskBar, GUI_ID_USER + 0x05);
	hHomeText = WM_GetDialogItem(hHomeWindow, 0x809);

	
	/* Gui background Task */
	while (1)
	{
		if (second != time_second)
		{
			second = time_second;

			// 更新任务栏
			updateTaskBarTitle();
			

			// 在桌面的底部文本使用彩虹特效
			if (hCurrentWindow == hHomeWindow)
				rainbowEffect(hHomeText, second);
			// 更新任务栏闹钟图标
			updateTaskBarAlarmState(hAlarmState);


			WM_Paint(hTaskBar);
		}
		else
			GUI_Exec(); /* Do the background work ... Update windows etc.) */
		vTaskDelay(20); /* Nothing left to do for the moment ... Idle processing */
	}
}

/**
 * @brief  Apply rainbow effect to object `hItem`
 * @param  WM_HWIN hItem
 * @param  int second
 * @retval None
 */
void rainbowEffect(WM_HWIN hItem, int second)
{
	switch (second % 7)
	{
	case 0:
		TEXT_SetTextColor(hItem, GUI_RED);
		break;
	case 1:
		TEXT_SetTextColor(hItem, GUI_ORANGE);
		break;
	case 2:
		TEXT_SetTextColor(hItem, GUI_YELLOW);
		break;
	case 3:
		TEXT_SetTextColor(hItem, GUI_GREEN);
		break;
	case 4:
		TEXT_SetTextColor(hItem, GUI_CYAN);
		break;
	case 5:
		TEXT_SetTextColor(hItem, GUI_BLUE);
		break;
	case 6:
		TEXT_SetTextColor(hItem, GUI_MAGENTA);
		break;
	};
}



/**
 * @brief  update the alarm state image of taskbar
 * @param  WM_HWIN hItem
 * @param  int second
 * @retval None
 */
static void updateTaskBarAlarmState(WM_HWIN hAlarmImage)
{
	static uint8_t		alarmState = 0;

	if (alarmState != alarm_enabled)
	{
		alarmState = alarm_enabled;
		if (alarmState == 1)
		{
			IMAGE_SetBitmap(hAlarmImage, &bmAlarm_enable);
			app_alarmTaskHandle = osThreadNew(app_alarm_thread, NULL, &app_alarmTask_attributes);
		}
		else
		{
			IMAGE_SetBitmap(hAlarmImage, &bmAlarm_disable);
			osThreadTerminate(app_alarmTaskHandle);
		}
	}
}



