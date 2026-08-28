#ifndef __INFRARED_
#define __INFRARED_

#include "stm32f4xx.h"
#include "stdio.h"
#include "beep.h"
#include "wavplay.h"

extern volatile uint32_t infrared_buf;//接收数据的缓存区

void irm_3638T_ini(uint16_t psc,uint16_t arr);

void deal_if(void);

#endif

