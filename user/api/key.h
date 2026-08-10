#ifndef _KEY_
#define _KEY_

#include "stm32f4xx.h"
#include "delay.h"
#include "debug.h"
#include "ws2812e.h"


extern volatile uint32_t motor_ccr;

void key_ini(void);
void Exti_key_ini(void);
#endif

