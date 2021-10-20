#ifndef __OS_TIME_H
#define __OS_TIME_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>


/* Exported functions ------------------------------------------------------- */
void updateSysTimeBySecond();
void updateSysTimeByDay();
void updateSysWeekday();


/* Exported variables -------------------------------------------------------- */
extern uint32_t os_time_second; 
extern uint32_t os_time_minute;
extern uint32_t os_time_hour;
extern uint32_t os_date_day;
extern uint32_t os_date_month;
extern uint32_t os_date_year;
extern uint32_t os_date_weekday_index;
extern char os_date_weekday[3];



#endif