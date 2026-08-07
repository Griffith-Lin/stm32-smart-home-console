#include "key.h"

////key1(pa0) key2(pb0) key3(pb1)
//void key_ini(void)
//{
//	RCC->AHB1ENR |=(1<<0);//PA时钟
//	RCC->AHB1ENR |=(1<<1);//PB时钟
//	
//	GPIOA->MODER &=~(3<<0);//输入模式
//	GPIOB->MODER &=~(0xf<<0);//输入模式
//	
//	GPIOA->PUPDR &=~(3<<0);//无上下拉
//	GPIOB->PUPDR &=~(0xf<<0);//无上下拉
//}

void key_ini(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_IN;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_0;
    
    GPIO_Init(GPIOA,&gpio_InitTypeDef);
    
    
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB,&gpio_InitTypeDef);
}




void Exti_key_ini(void)
{
    key_ini();

    RCC->APB2ENR |= (1<<14); //使能APB2时钟总线上的SYSCFG时钟
                           
    //EXTI0
    SYSCFG->EXTICR[0] &= ~(0xf<<0);//EXTI 0外部中断的源输入设置为pa0，EXTICR[0] 即手册中的 SYSCFG_EXTICR1
                                   //EXTICR 1的EXTI 0 [3:0]，其中的4位决定是哪个GPIOx的px0
    
    EXTI->RTSR |= (1<<0);//上升沿触发选择寄存器。 上升沿触发选择寄存器对应的0线置1，允许输入线（0线）上升沿触发
    EXTI->IMR |= (1<<0);//中断屏蔽寄存器（选择性屏蔽/允许某些中断源）。 pa0是属于EXTI0上的，对应的是中断屏蔽寄存器0线，开放来自0线的中断请求
    //上升沿触发指的是当信号从低电平（0）跳变到高电平（1）的瞬间，触发某个动作或事件。这个"沿"就是指电平变化的边沿。
    
    
    
    
    //EXTI1
    SYSCFG->EXTICR[0] &= ~(0xf<<4);
    SYSCFG->EXTICR[0] |= (1<<4);
    
    EXTI->RTSR |= (1<<1);//EXTI1（对应的是尾号为1的io口）
    EXTI->IMR |= (1<<1);
    
    
    
        
    NVIC_SetPriority(EXTI0_IRQn,4);//配置EXTI0的优先级，010 0 占先位数的十进制值为2（最前面3位），次级位数的十进制值为0（后面1位）
    NVIC_EnableIRQ(EXTI0_IRQn);//使能EXTI0响应中断
    
    NVIC_SetPriority(EXTI1_IRQn,6);//011 0
    NVIC_EnableIRQ(EXTI1_IRQn);
    

}

void EXTI0_IRQHandler(void)
{
    if(EXTI->PR & (1<<0))
    {
       EXTI->PR |= (1<<0);
            
    }
    
    
}

