#include "os_time.h"


// 系统时间-秒
uint32_t time_second = 50; 
// 系统时间-分
uint32_t time_minute = 59;
// 系统时间-时
uint32_t time_hour = 23;
// 系统时间-日
uint32_t date_day = 11;
// 系统时间-月
uint32_t date_month = 10;
// 系统时间-年
uint32_t date_year = 2021;
// 系统时间-星期
char date_weekday[3] = "Mon";

// 系统时间-星期, 0 表示星期日，1 表示星期一 ...
uint32_t date_weekday_index = 0;
const char *weekday_arr[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


/**
 * @brief   将系统时间增加一秒，更新 `taskBarTitle`
 * @param   none
 * @return  void
 */
void updateSysTimeBySecond()
{
    if (time_second != 59)
        time_second++;
    else
    {
        time_second = 0;
        if (time_minute != 59)
            time_minute++;
        else
        {
            time_minute = 0;
            if (time_hour != 23)
                time_hour++;
            else
            {
                time_hour = 0;
                // 更新日期
                updateSysTimeByDay();
                //更新星期
                updateSysWeekday();
            }
        }
    }
}

/**
 * @brief   将系统时间增加一天
 * @param   none
 * @return  void
 */
void updateSysTimeByDay()
{
    switch (date_month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
        if (date_day < 31)
            date_day++;
        else
        {
            date_day = 1;
            date_month++;
        }
        break;
    case 12:
        if (date_day < 31)
            date_day++;
        else
        {
            date_day = 1;
            date_month = 1;
            date_year++;
        }
        break;
    case 4:
    case 6:
    case 9:
        if (date_day < 30)
            date_day++;
        else
        {
            date_day = 1;
            date_month++;
        }
        break;
    case 2:
        if ((date_year % 4 == 0 && date_year % 100 != 0) || (date_year % 400 == 0))
        {
            if (date_day < 29)
                date_day++;
            else
            {
                date_day = 1;
                date_month++;
            }
        }
        else
        {
            if (date_day < 28)
                date_day++;
            else
            {
                date_day = 1;
                date_month++;
            }
        }
        break;
    default:
        break;
    }
}



/**
 * @brief   更新系统的星期
 * @param   none
 * @return  void
 */
void updateSysWeekday()
{
    int year = date_year, month = date_month, c, y, week;
    if (month == 1 || month == 2)
    {
        year--;
        month += 12;
    }
    c = year / 100;
    y = year - c * 100;
    week = (c / 4) - 2 * c + (y + y / 4) + (26 * (month + 1) / 10) + date_day - 1;
    while (week < 0)
        week += 7;

    week %= 7;
    date_weekday_index = week;
    strcpy(date_weekday, weekday_arr[week]);
}
