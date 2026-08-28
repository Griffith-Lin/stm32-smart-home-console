#include "gl5506.h"

//GL5506 是一款非常经典的 硫化镉（CdS）光敏电阻，也称为“光导管”或“光敏电阻器”。
//pc0 adc1
void adc_GL5506_ini(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AN;
    //gpio_InitTypeDef.GPIO_OType//模拟模式下不用配输出模式
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_0;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    //gpio_InitTypeDef.GPIO_Speed//模拟模式不配速度
    
    GPIO_Init(GPIOC,&gpio_InitTypeDef);
    
    

}

