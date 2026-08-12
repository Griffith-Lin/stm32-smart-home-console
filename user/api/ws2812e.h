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

#define PB15_HIGH()  do { GPIOB->BSRR = (1U<<15); } while(0)
#define PB15_LOW()  do { GPIOB->BSRR = (1U<<31); } while(0)
//() 不是给编译器看的，是给你自己看的。它让宏的写法和你每天写函数的写法保持一致，减少手误。
//寄存器中没有"负数"的概念，用无符号数才和硬件行为一致。

//BSRR 是 STM32F4 上的只写寄存器。用 |= 会触发一次读-改-写操作：
//多消耗 2~3 个 APB2 桥接等待周期，让本已紧张的时序更雪上加霜
//BSRR 的读返回值是未定义的，在某些硅片版本上可能非零，导致同时置位 BS15 和 BR15（BS15 优先级更高，引脚会卡死在高电平）

#endif

