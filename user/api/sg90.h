#ifndef __SG90_
#define __SG90_

#include "stm32f4xx.h"

void sg90_PWM(uint32_t psc,uint32_t arr);
void sg90_set_angle(float angle);

#endif
