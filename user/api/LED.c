#include "LED.h"

void led_ini(void)
{
    RCC->AHB1ENR |= (1<<3);
    
    //pd 12 13 14
    GPIOD->MODER &=~(0x3f<<24);
    GPIOD->MODER |=(0x15<<24);//通用输出
    GPIOD->OTYPER &=~(7<<12);//推挽
    GPIOD->ODR |=(7<<12);//端口输出数据寄存器，全置1，主动设置为输出高电平（因为输出低电平的时候led灯亮）
    
}

