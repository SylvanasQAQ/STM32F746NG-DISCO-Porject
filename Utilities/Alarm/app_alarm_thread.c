#include "app_alarm.h"
#include "main.h"
#include "os_time.h"
#include "DIALOG.h"


/*------------------- private variables --------------------------*/
// alarm 触发指示       1 -- 有 alarm 触发， 0 -- 无 alarm 触发
uint8_t alarm_triggered;






/*------------------- implementation ----------------------------*/
#ifdef CMSIS_V1
//osThreadId app_alarmTaskHandle;
TaskHandle_t app_alarmTaskHandle;


static void AlarmThread(void *argument);
void app_alarm_thread(void *argument)
{
    AlarmThread(argument);
}



/**
 * @brief  Alarm thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void AlarmThread(void *argument)
{
    extern TIM_HandleTypeDef htim3;
    extern WM_HWIN CreateAlarmDialog(void);
    alarm_t     *p = NULL;
    uint32_t     day = 0;
    uint8_t      lightEffect = 0;

    for (alarm_triggered = 0; ; alarm_triggered = 0, lightEffect = 0)
    {
        // 新的一天，清空 trigger 标记
        if(day != os_date_day)
        {
            day = os_date_day;
            for (int i = 0; i < APP_ALARM_NUM; i++)
            {
                p = &(app_alarm_arr[i]);
                alarm_clearTriggered(p);
            }
            alarm_triggered = 0;
        }


        // 检查是否有闹钟触发
        for (int i = 0; i < APP_ALARM_NUM; i++)
        {
            p = &(app_alarm_arr[i]);

            if (!alarm_isEnabled(p))
                continue;

            if (os_time_hour == p->hour && os_time_minute == p->minute)
            {
                if (alarm_isTriggered(p))
                    continue;

                if (alarm_isPeriodic(p) && (p->weekday & (0x1 << os_date_weekday_index)))
                {
                    
                    alarm_setTriggered(p);          // 周期性的闹钟设置已触发
                    alarm_triggered = 1;

                    if(alarm_isLightEffect(p))
                        lightEffect = 1;
                }
                else if(!alarm_isPeriodic(p))
                {
                    alarm_clearEnabled(p);          // 若是一次性的闹钟，触发后自动关闭
                    alarm_ifAnyEnabled();                       // 更新 `alarm_enabled` 状态
                    alarm_triggered = 1;

                    if(alarm_isLightEffect(p))
                        lightEffect = 1;
                }
            }
        }

        // 触发闹钟
        if(alarm_triggered)
        {
            alarm_triggered = 0;
            if(lightEffect){
                HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
                __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 3000);
            }
            CreateAlarmDialog();
        }

        //if(vTask)

        // 延时一秒
        vTaskDelay(1000);
    }
}
#endif




#ifdef CMSIS_V2
osThreadId_t app_alarmTaskHandle;
const osThreadAttr_t app_alarmTask_attributes = {
    .name = "Alarm Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityBelowNormal,
};



static void AlarmThread(void *argument);
void app_alarm_thread(void *argument)
{
    AlarmThread(argument);
}

/**
 * @brief  Alarm thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void AlarmThread(void *argument)
{
    extern TIM_HandleTypeDef htim3;
    extern WM_HWIN CreateAlarmDialog(void);
    alarm_t     *p = NULL;
    uint32_t     day = 0;
    uint8_t      lightEffect = 0;

    for (alarm_triggered = 0; ; alarm_triggered = 0, lightEffect = 0)
    {
        // 新的一天，清空 trigger 标记
        if(day != date_day)
        {
            day = date_day;
            for (int i = 0; i < APP_ALARM_NUM; i++)
            {
                p = &(app_alarm_arr[i]);
                alarm_clearTriggered(p);
            }
            alarm_triggered = 0;
        }


        // 检查是否有闹钟触发
        for (int i = 0; i < APP_ALARM_NUM; i++)
        {
            p = &(app_alarm_arr[i]);

            if (!alarm_isEnabled(p))
                continue;

            if (time_hour == p->hour && time_minute == p->minute)
            {
                if (alarm_isTriggered(p))
                    continue;

                if (alarm_isPeriodic(p) && (p->weekday & (0x1 << os_date_weekday_index)))
                {
                    
                    alarm_setTriggered(p);          // 周期性的闹钟设置已触发
                    alarm_triggered = 1;

                    if(alarm_isLightEffect(p))
                        lightEffect = 1;
                }
                else if(!alarm_isPeriodic(p))
                {
                    alarm_clearEnabled(p);          // 若是一次性的闹钟，触发后自动关闭
                    alarm_ifAnyEnabled();                       // 更新 `alarm_enabled` 状态
                    alarm_triggered = 1;

                    if(alarm_isLightEffect(p))
                        lightEffect = 1;
                }
            }
        }

        // 触发闹钟
        if(alarm_triggered)
        {
            alarm_triggered = 0;
            if(lightEffect){
                HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
                __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 3000);
            }
            CreateAlarmDialog();
        }

        // 延时一秒
        vTaskDelay(1000);
    }
}
#endif

