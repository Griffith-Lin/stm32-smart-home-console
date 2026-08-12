#ifndef __RTC_
#define __RTC_

#include "stm32f4xx.h"
#include "stdio.h"

struct COMPILE_data
{
    uint16_t year;
    uint8_t day;
    uint8_t month;  
    uint8_t weekday;
};

struct COMPILE_time
{
    uint8_t hour;
    uint8_t min;
    uint8_t second;
};
extern volatile struct COMPILE_time compile_time;
extern volatile struct COMPILE_data compile_date;


void RTC_Cal_Config(void);
void RTC_Set_Time(uint8_t H12,uint8_t Hours,uint8_t Minutes,uint8_t Seconds);
void RTC_Set_Date(uint8_t Year,uint8_t Month,uint8_t Date,uint8_t WeekDay);
void RTC_Show_Time(void);
uint8_t Compile_WeekDay(void);
void Compile_Time(void);

#endif

