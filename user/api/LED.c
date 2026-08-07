#include "LED.h"

//void led_ini(void)
//{
//    RCC->AHB1ENR |= (1<<3);
//    
//    //pd 12 13 14
//    GPIOD->MODER &=~(0x3f<<24);
//    GPIOD->MODER |=(0x15<<24);//通用输出
//    GPIOD->OTYPER &=~(7<<12);//推挽
//    GPIOD->ODR |=(7<<12);//端口输出数据寄存器，全置1，主动设置为输出高电平（因为输出低电平的时候led灯亮）
//    
//}

void led_ini(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_OUT;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Speed=GPIO_Low_Speed;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    
    GPIO_SetBits(GPIOD,GPIO_Pin_12);
    GPIO_SetBits(GPIOD,GPIO_Pin_13);
    GPIO_SetBits(GPIOD,GPIO_Pin_14);
    
    GPIO_Init(GPIOD,&gpio_InitTypeDef);
       
}


