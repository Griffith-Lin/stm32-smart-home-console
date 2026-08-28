#include "pd333b.h"

//PD333B 是一款 5mm 插件式红外光敏二极管（Photodiode），常用于红外接收、对射检测或环境光感知。
//pc1 ADC1_IN11
void pd333b_adc_ini(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AN;
    //gpio_InitTypeDef.GPIO_OType//模拟模式下不用配输出模式
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_1;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    //gpio_InitTypeDef.GPIO_Speed//模拟模式不配速度
    
    GPIO_Init(GPIOC,&gpio_InitTypeDef);    
}


