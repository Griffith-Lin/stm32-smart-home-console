#ifndef __RELAY_
#define __RELAY_

#include "stm32f4xx.h"

#define RELAY_ON GPIO_SetBits(GPIOE,GPIO_Pin_0);
#define RELAY_OFF GPIO_ResetBits(GPIOE,GPIO_Pin_0);

void relay_ini(void);

#endif


