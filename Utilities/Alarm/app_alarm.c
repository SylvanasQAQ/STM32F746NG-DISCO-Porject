#include "app_alarm.h"



// alarm 结构体数组
alarm_t app_alarm_arr[APP_ALARM_NUM];

// alarm 开启指示       1 -- 有 alarm 开启， 0 -- 无 alarm 开启
uint8_t alarm_enabled;
// alarm 触发指示       1 -- 有 alarm 触发， 0 -- 无 alarm 触发
uint8_t alarm_triggered;





/**
 * @brief   判断闹钟是否开启
 * @param   alarm_t* alarm
 * @retval  0: 未开启       非 0：开启
 */
uint8_t alarm_isEnabled(alarm_t* alarm)
{
    return alarm->property & APP_ALARM_ENABLED;
}


void alarm_setEnabled(alarm_t* alarm)
{
    alarm->property |= APP_ALARM_ENABLED;
    alarm->property &= ~APP_ALARM_TRIGGERED;            // 清除触发位
    alarm_ifAnyEnabled();
}


void alarm_clearEnabled(alarm_t* alarm)
{
    alarm->property &= ~APP_ALARM_ENABLED;
    alarm_ifAnyEnabled();
}





/**
 * @brief   判断闹钟是否是周期性的
 * @param   alarm_t* alarm
 * @retval  0: 不是       非 0: 是周期性的
 */
uint8_t alarm_isPeriodic(alarm_t* alarm)
{
    return alarm->property & APP_ALARM_PERIODIC;
}



/**
 * @brief   判断闹钟触发时是否开启灯光效果
 * @param   alarm_t* alarm
 * @retval  0: 不开启       非 0：开启
 */
uint8_t alarm_isLightEffect(alarm_t* alarm)
{
    return alarm->property & APP_ALARM_LIGHT;
}


void alarm_setLightEffect(alarm_t* alarm)
{
    alarm->property |= APP_ALARM_LIGHT;
}

void alarm_clearLightEffect(alarm_t* alarm)
{
    alarm->property &= ~APP_ALARM_LIGHT;
}



/**
 * @brief   判断闹钟今天是否触发过
 * @param   alarm_t* alarm
 * @retval  0: 未触发过       非 0：触发过
 */
uint8_t alarm_isTriggered(alarm_t* alarm)
{
    return alarm->property & APP_ALARM_TRIGGERED;
}


void alarm_setTriggered(alarm_t* alarm)
{
    alarm->property |= APP_ALARM_TRIGGERED;
}

void alarm_clearTriggered(alarm_t* alarm)
{
    alarm->property &= ~APP_ALARM_TRIGGERED;
}




/**
 * @brief   判断是否有闹钟开启
 * @param   void
 * @retval  0: 无闹钟开启       1：至少有一个闹钟开启
 */
uint8_t alarm_ifAnyEnabled(void)
{
    for(int i = 0; i < APP_ALARM_NUM; i++)
    {
        if(alarm_isEnabled(&(app_alarm_arr[i]))){
            alarm_enabled = 1;
            return 1;
        }
    }
    alarm_enabled = 0;
    return 0;
}