#ifndef __WS2812E_
#define __WS2812E_

#include "stm32f4xx.h"
#include "delay.h"

extern volatile uint8_t green;
extern volatile uint8_t red;
extern volatile uint8_t blue;


void ws2812e_ini(uint16_t rgb_num);
void ws2812e_open(uint8_t green,uint8_t red,uint8_t blue);
void ws2812e_open_reset(uint8_t green,uint8_t red,uint8_t blue,uint8_t rgb_num);

void ws2812e_mode1(void);

#define PB15_HIGH()  do { GPIOB->BSRR |= (1U<<15); } while(0)
#define PB15_LOW()  do { GPIOB->BSRR |= (1U<<31); } while(0)
//() 不是给编译器看的，是给你自己看的。它让宏的写法和你每天写函数的写法保持一致，减少手误。
//寄存器中没有"负数"的概念，用无符号数才和硬件行为一致。


#endif

