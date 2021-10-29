/* Includes ------------------------------------------------------------------*/
#include "DIALOG.h"
#include "app_alarm.h"
#include "main.h"
#include "os_time.h"
#include <string.h>
#include "os_threads.h"
#include "fatfs.h"
#include "dialog_window.h"



/* Defines ------------------------------------------------------------------*/




/* Functions prototypes ---------------------------------------------*/
static void AlarmThread(void *argument);
static void CheckAlarm();
static void AlarmAlert();





/* Private variables -----------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;

uint8_t AlarmTriggered;        // alarm 触发指示       1 -- 有 alarm 触发， 0 -- 无 alarm 触发
uint8_t AlarmLightEffect;      // alarm 灯光效果指示    1 -- alarm 开启了灯光效果，0 -- alarm 未开启灯光效果
uint8_t AlarmVolume;           // alarm 音量

uint8_t Alarm_Thread_Exist = 0;
char    AlarmSongPath[128] = "/The Dawn.wav";




/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
TaskHandle_t app_alarmTaskHandle;
#endif


#ifdef CMSIS_V2
osThreadId_t app_alarmTaskHandle;
const osThreadAttr_t app_alarmTask_attributes = {
    .name = "Alarm Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
#endif





void vAlarmTaskCreate(void *argument)
{
#ifdef CMSIS_V1
    xTaskCreate(AlarmThread, "Alarm Task", 512, NULL, osPriorityNormal, &app_alarmTaskHandle);
#endif

#ifdef CMSIS_V2
    app_alarmTaskHandle = osThreadNew(AlarmThread, NULL, &app_alarmTask_attributes);
#endif
}


/**
 * @brief  Alarm thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void AlarmThread(void *argument)
{
    Alarm_Thread_Exist = 1;

    for (;;)
    {
        CheckAlarm();
        if (AlarmTriggered)
            AlarmAlert();


        if(alarm_enabled == 0)
        {
            Alarm_Thread_Exist = 0;
#ifdef CMSIS_V1
            vTaskDelete(app_alarmTaskHandle);
#endif

#ifdef CMSIS_V2
            osThreadTerminate(app_alarmTaskHandle);
#endif
        }

        // 延时一秒
        vTaskDelay(500);
    }
}



/**
 * @brief  检查是否有闹钟触发，如果有就将 AlarmTriggered 置 1
 * @param  None
 * @retval None
 */
static void CheckAlarm()
{
    static uint32_t day = 0;
    static alarm_t *p = NULL;

    // 新的一天，清空 trigger 标记
    if (day != os_date_day)
    {
        day = os_date_day;
        for (int i = 0; i < APP_ALARM_NUM; i++)
        {
            p = &(app_alarm_arr[i]);
            alarm_clearTriggered(p);
        }
        AlarmTriggered = 0;
    }

    // 检查是否有闹钟触发
    for (int i = 0; i < APP_ALARM_NUM; i++)
    {
        p = &(app_alarm_arr[i]);

        if (!alarm_isEnabled(p))            // 直接跳过未开启的闹钟
            continue;

        // 若闹钟开启，检查闹钟时间是否等于当前时间
        if (os_time_hour == p->hour && os_time_minute == p->minute && os_time_second == p->second)
        {
            if (alarm_isTriggered(p))       // 判断闹钟今天是否已经触发过
                continue;

            // 如果闹钟是周期性的，需要检查星期是否符合
            if (alarm_isPeriodic(p) && (p->weekday & (0x1 << os_date_weekday_index)))
            {

                alarm_setTriggered(p); // 周期性的闹钟设置已触发
                AlarmTriggered = 1;

                AlarmLightEffect = alarm_isLightEffect(p) ? 1 : 0;
                AlarmVolume = p->alarmVolume;
            }
            else if (!alarm_isPeriodic(p))
            {
                alarm_clearEnabled(p); // 若是一次性的闹钟，触发后自动关闭
                alarm_ifAnyEnabled();  // 更新 `alarm_enabled` 状态
                AlarmTriggered = 1;

                AlarmLightEffect = alarm_isLightEffect(p) ? 1 : 0;
                AlarmVolume = p->alarmVolume;
            }
        }
    }
}




/**
 * @brief  如果闹钟触发，播放音乐，开启灯光提示
 * @param  None
 * @retval None
 */
static void AlarmAlert()
{
    extern char       musicPath[100];
    extern uint16_t   Music_Play_Start;            // 音乐开始标志
    extern uint16_t Music_Play_Restart;            // 音乐开始标志
    extern uint16_t Music_Thread_Exist;            // 音乐后台线程运行标志
    extern uint32_t uiMusicVolumeN;                // 音量系数——分子
    extern uint32_t uiMusicVolumeD;                // 音量系数——分母
    FIL file;

    AlarmTriggered = 0;
    if (AlarmLightEffect)
    {
        __HAL_TIM_SET_PRESCALER(&htim3, 10800 - 1);
        __HAL_TIM_SET_AUTORELOAD(&htim3, 500 - 1);
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 100);
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    }

    strcpy(musicPath, AlarmSongPath);
    if (f_open(&file, AlarmSongPath, FA_READ) == FR_OK)
    {
        f_close(&file);
        Music_Play_Start = 1;
        Music_Play_Restart = 1;
        // set volume
        uiMusicVolumeD = 50;
        uiMusicVolumeN = AlarmVolume > 50 ? (AlarmVolume - 50) * 4 + 50 : AlarmVolume;
        if (Music_Thread_Exist == 0)
            vMusicTaskCreate(); // 启动音乐线程
    }
    else
    {
        f_close(&file);
        __HAL_TIM_SET_PRESCALER(&htim5, 1080 * 5 - 1);
        __HAL_TIM_SET_AUTORELOAD(&htim5, 200 - 1);
        __HAL_TIM_SetCounter(&htim5, 0);
        __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, AlarmVolume);
        HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);
    }

    CreateAlarmDialog_Self("Alarm", "Alarm is on", DIALOG_ALARM);
}