#include "relay.h"


//pe0  
void relay_ini(void)
{
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef;

    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_OUT;
    gpio_InitTypeDef .GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef .GPIO_Pin=GPIO_Pin_0;
    gpio_InitTypeDef .GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef .GPIO_Speed=GPIO_Low_Speed;
    
    GPIO_ResetBits(GPIOE,GPIO_Pin_0);//Ä¬ÈÏÀ­µÍ
    
    GPIO_Init(GPIOE,&gpio_InitTypeDef);
}
