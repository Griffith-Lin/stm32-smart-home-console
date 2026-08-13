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
    
    
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_1;
    GPIO_Init(GPIOB,&gpio_InitTypeDef);
}




void Exti_key_ini(void)
{
    key_ini();

//    RCC->APB2ENR |= (1<<14); //使能APB2时钟总线上的SYSCFG时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
                           
//    //EXTI0
//    SYSCFG->EXTICR[0] &= ~(0xf<<0);//EXTI 0外部中断的源输入设置为pa0，EXTICR[0] 即手册中的 SYSCFG_EXTICR1
//                                   //EXTICR 1的EXTI 0 [3:0]，其中的4位决定是哪个GPIOx的px0
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
    
 
    
//    EXTI->RTSR |= (1<<0);//上升沿触发选择寄存器。 上升沿触发选择寄存器对应的0线置1，允许输入线（0线）上升沿触发
//    EXTI->IMR |= (1<<0);//中断屏蔽寄存器（选择性屏蔽/允许某些中断源）。 pa0是属于EXTI0上的，对应的是中断屏蔽寄存器0线，开放来自0线的中断请求
//    //上升沿触发指的是当信号从低电平（0）跳变到高电平（1）的瞬间，触发某个动作或事件。这个"沿"就是指电平变化的边沿。
    
    
    EXTI_InitTypeDef exti_InitTypeDef={0};
        
    //EXTI_Mode_Interrupt
    //该EXTI线配置为中断模式，触发后进入NVIC中断服务程序
    //EXTI_Mode_Event
    //该EXTI线配置为事件模式，触发后产生脉冲信号，可唤醒CPU但不进ISR，常用于触发ADC/DMA等外设
    exti_InitTypeDef.EXTI_Mode=EXTI_Mode_Interrupt;
    exti_InitTypeDef.EXTI_Line=EXTI_Line0;
    exti_InitTypeDef.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
    exti_InitTypeDef.EXTI_LineCmd=ENABLE;
    
    EXTI_Init(&exti_InitTypeDef);
    
    
    //EXTI1
//    SYSCFG->EXTICR[0] &= ~(0xf<<4);
//    SYSCFG->EXTICR[0] |= (1<<4);
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource1);
      
//    EXTI->RTSR |= (1<<1);//EXTI1（对应的是尾号为1的io口）
//    EXTI->IMR |= (1<<1);
    
    exti_InitTypeDef.EXTI_Line=EXTI_Line1; 
    EXTI_Init(&exti_InitTypeDef);
        
        
//    NVIC_SetPriority(EXTI0_IRQn,14);//配置EXTI0的优先级，111 0 占先位数的十进制值为2（最前面3位），次级位数的十进制值为0（后面1位）
//    NVIC_EnableIRQ(EXTI0_IRQn);//使能EXTI0响应中断
//    
//    NVIC_SetPriority(EXTI1_IRQn,14);//111 0
//    NVIC_EnableIRQ(EXTI1_IRQn);

    NVIC_InitTypeDef nvic_InitTypeDef={0};
    
    nvic_InitTypeDef.NVIC_IRQChannel=EXTI0_IRQn;
    nvic_InitTypeDef.NVIC_IRQChannelSubPriority=0;
    nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=3;
    nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;

    NVIC_Init(&nvic_InitTypeDef);
    

}

//void EXTI0_IRQHandler(void)
//{
//    if(EXTI->PR & (1<<0))
//    {
//       EXTI->PR |= (1<<0);
////       GPIO_SetBits(GPIOD,GPIO_Pin_15); 
////       Delay_Ms(100);
////       GPIO_ResetBits(GPIOD,GPIO_Pin_15); 
//        
//       if(TIM3->CCR3<1000) 
//       TIM3->CCR3+=100;
//    }
//      
//}

volatile uint32_t motor_ccr=0;

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0)==SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
//       GPIO_SetBits(GPIOD,GPIO_Pin_15); 
//       Delay_Ms(100);
//       GPIO_ResetBits(GPIOD,GPIO_Pin_15); 
        
       
        
//       if(TIM_GetCapture3(TIM3)<1000)
//       {
//           
//             motor_ccr+=100;
//            TIM_SetCompare3(TIM3,motor_ccr);    
//       }
//        
        
        
        
//        ADC_SoftwareStartConv(ADC1);
//        gl5506_flag=1;
        
        
        
        green+=50;
        red+=50;
        blue+=50;
        
        
//        Delay_Ms(300);//消抖阻塞整个系统：延时期间CPU被锁死在中断里，所有其他中断（包括更高优先级的SysTick、UART、DMA）都无法响应。智能家居设备可能因此丢失传感器数据、通信超时、看门狗复位。
//          抖动并未真正消除：机械按键抖动持续5-20ms。你在ISR入口延时300ms后退出，此时按键可能仍在抖动，下一次电平变化又会触发新的中断，导致多次进入ISR，green/red/blue 被累加多次——消抖完全失效。
//          违反中断设计原则：ISR应“快进快出”，只做最少必要工作（读状态、清标志、设标志位），耗时操作必须移到主循环。
    }
}


void EXTI1_IRQHandler(void)
{
    
}

