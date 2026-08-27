#ifndef __MOTOR_
#define __MOTOR_

#include "stm32f4xx.h"
#include "delay.h"

#include <stdint.h>

void motor_pwm_ini(uint32_t psc,uint32_t arr);


void mortor_minspeed_open(uint16_t ccr);
void Motor_Control(uint16_t speed);
#endif

