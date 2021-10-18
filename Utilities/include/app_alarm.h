#ifndef __APP_ALARM_H
#define __APP_ALARM_H

#include <stdint.h>

/*------------------- defines -------------------*/
#define APP_ALARM_SUNDAY                 (0x1)
#define APP_ALARM_MONDAY                 (0x1 << 1)
#define APP_ALARM_TUESDAY                (0x1 << 2)
#define APP_ALARM_WEDNESDAY              (0x1 << 3)
#define APP_ALARM_THURSDAY               (0x1 << 4)
#define APP_ALARM_FRIDAY                 (0x1 << 5)
#define APP_ALARM_SATURDAY               (0x1 << 6)

#define APP_ALARM_ENABLED                (0x1)
#define APP_ALARM_PERIODIC               (0x1 << 1)
#define APP_ALARM_LIGHT                  (0x1 << 2)
#define APP_ALARM_TRIGGERED              (0x1 << 3)


#define APP_ALARM_NUM                   5




/*------------------- typedef -------------------*/
/**
 * @brief   闹钟结构体，定义了一些闹钟的属性
 */
typedef struct alarm_struct
{
    uint32_t hour;
    uint32_t minute;
    uint8_t weekday;                  // [bitmap] weekday 的 bit 0~6 分别对应 周日, 周一, ...，bit 7未使用
    uint8_t alarmVolume;              // 闹钟音量
    uint8_t property;                 // [bitmap] 闹钟属性
                                     //  bit 0 表示是否开启闹钟，       1 -- 开启               0 -- 关闭
                                      // bit 1 闹钟是否为周期性的       1 -- 是周期性的         0 -- 一次性
                                      // bit 2 表示是否开启灯光效果，其他位暂时未使用
                                      // bit 3 表示今天是否被触发过     1 -- 触发过             0 -- 未触发过
    uint8_t musicType;                // not in use
}alarm_t;





/*------------------- externs -------------------*/
extern alarm_t app_alarm_arr[APP_ALARM_NUM];
extern uint8_t alarm_enabled;


/*------------------- PF -------------------*/
uint8_t alarm_isLightEffect(alarm_t* alarm);
void alarm_setLightEffect(alarm_t* alarm);
void alarm_clearLightEffect(alarm_t* alarm);


uint8_t alarm_isPeriodic(alarm_t* alarm);

uint8_t alarm_isEnabled(alarm_t* alarm);
void alarm_setEnabled(alarm_t* alarm);
void alarm_clearEnabled(alarm_t* alarm);

uint8_t alarm_isTriggered(alarm_t* alarm);
void alarm_setTriggered(alarm_t* alarm);
void alarm_clearTriggered(alarm_t* alarm);

uint8_t alarm_ifAnyEnabled(void);

#endif