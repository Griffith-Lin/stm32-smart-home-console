#ifndef _SYS_H
#define _SYS_H

#include "stm32f4xx.h"
#include "stdio.h"

void Sys_delay_us(uint32_t ustime);
void Sys_delay_ms(uint32_t mstime);
void sysnvic_delay(uint32_t mstime);
void sysnvic_delay1(uint32_t mstime);

extern uint32_t time1[2];
extern uint32_t time2[2];

#endif
