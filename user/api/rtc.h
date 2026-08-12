#ifndef __RTC_
#define __RTC_

#include "stm32f4xx.h"
#include "stdio.h"


void RTC_Cal_Config(void);
void RTC_Set_Time(uint8_t H12,uint8_t Hours,uint8_t Minutes,uint8_t Seconds);
void RTC_Set_Date(uint8_t Year,uint8_t Month,uint8_t Date,uint8_t WeekDay);
void RTC_Show_Time(void);
uint8_t Compile_WeekDay(void);

#endif

