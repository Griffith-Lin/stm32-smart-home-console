#ifndef __DEBUG_
#define __DEBUG_

#include "stm32f4xx.h"
#include "tim.h"
#include "W25Qxx.h"
#include "delay.h"
#include "MLX90614.h"

#include <stdio.h>
#include <stdint.h> 


extern uint8_t tmp;
extern uint8_t gl5506_flag;


void adc_debug_manual(void);
void adc_debug_auto(void);
void W25Qxx_test(void);
#endif
