#ifndef __DELAY_
#define __DELAY_

#include "stm32f4xx.h"
#include "stdio.h" //ø…“‘”√printf
#include <stddef.h>

void TIM6_delay(uint32_t ms);
void TIM6_Task_Init(uint32_t psc,uint32_t arr);



#endif
