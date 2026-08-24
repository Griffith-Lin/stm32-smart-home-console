#include "beep.h"

void beep_ini(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef;

    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_OUT;
    gpio_InitTypeDef .GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef .GPIO_Pin=GPIO_Pin_15;
    gpio_InitTypeDef .GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef .GPIO_Speed=GPIO_Low_Speed;
    
    GPIO_ResetBits(GPIOD,GPIO_Pin_15);//Ä¬ÈÏÀ­µÍ
    
    GPIO_Init(GPIOD,&gpio_InitTypeDef);
    
       
}


void beep_one(void)
{
    BEEP_ON();
    Delay_Ms(30);
    BEEP_OFF();
}

