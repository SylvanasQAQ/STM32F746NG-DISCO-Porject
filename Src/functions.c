#include "functions.h"
#include <stdio.h>
#include <string.h>

char taskBarTitle[] = "10/11/2021  09:48:00 Mon    Group4  @MianXu @WangHe";
char taskBarSuffix[] = "    Group4  @MianXu @WangHe";

// 系统时间-秒
int time_second = 50;
// 系统时间-分
int time_minute = 59;
// 系统时间-时
int time_hour = 23;
// 系统时间-日
int date_day = 11;
// 系统时间-月
int date_month = 10;
// 系统时间-年
int date_year = 2021;
char date_weekday[3] = "Mon";

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

    sprintf(taskBarTitle, "%02d/%02d/%d  %02d:%02d:%02d %s%s",
            date_month, date_day, date_year, time_hour, time_minute, time_second, date_weekday, taskBarSuffix);
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
    if (date_month == 1 || date_month == 2)
    {
        year--;
        month += 12;
    }
    c = year / 100;
    y = year - c * 100;
    week = (c / 4) - 2 * c + (y + y / 4) + (13 * (month + 1) / 5) + date_day - 1;
    while (week < 0)
        week += 7;
    week %= 7;
    switch (week)
    {
    case 1:
        strcpy(date_weekday, "Mon");
        break;
    case 2:
        strcpy(date_weekday, "Tue");
        break;
    case 3:
        strcpy(date_weekday, "Wen");
        break;
    case 4:
        strcpy(date_weekday, "Thu");
        break;
    case 5:
        strcpy(date_weekday, "Fri");
        break;
    case 6:
        strcpy(date_weekday, "Sat");
        break;
    case 7:
        strcpy(date_weekday, "Sun");
        break;
    default:
        break;
    }
}