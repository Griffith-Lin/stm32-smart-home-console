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



volatile Key_t keys[3]={0};
//声明为keys[10]报了好多错。   L6407E = 链接器在 RAM 区找不到能放下这些 section 的地方。"报了好多错"是正常的：Keil 对每个放不下的 section 各报一条 L6407E，0x54（84 字节）是这些 section 的总和——看着吓人，其实是同一个根因：RAM 不够了
//修复：把内存池砍到够用的尺寸（一行改动）mymalloc.h:20：#define MEM1_MAX_SIZE   100*1024    // 改前   #define MEM1_MAX_SIZE   60*1024     // 改后

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
    
    
    
    keys[1].port = GPIOB;
    keys[1].pin  = GPIO_Pin_1;
    keys[1].state       = KEY_STATE_IDLE;
    keys[1].event_flag  = KEY_EVENT_NONE;
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
    exti_InitTypeDef.EXTI_Trigger=EXTI_Trigger_Rising;
    exti_InitTypeDef.EXTI_LineCmd=ENABLE;
    
    EXTI_Init(&exti_InitTypeDef);
    
    
    //EXTI1
//    SYSCFG->EXTICR[0] &= ~(0xf<<4);
//    SYSCFG->EXTICR[0] |= (1<<4);
    
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource1);
      
    
    
    
    
//    EXTI->RTSR |= (1<<1);//EXTI1（对应的是尾号为1的io口）
//    EXTI->IMR |= (1<<1);
    
//    exti_InitTypeDef.EXTI_Line=EXTI_Line1; 
//    EXTI_Init(&exti_InitTypeDef);
        
        
        
        
        
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
        
        
        
//        green+=50;
//        red+=50;
//        blue+=50;
        
        
//        Delay_Ms(300);//消抖阻塞整个系统：延时期间CPU被锁死在中断里，所有其他中断（包括更高优先级的SysTick、UART、DMA）都无法响应。智能家居设备可能因此丢失传感器数据、通信超时、看门狗复位。
//          抖动并未真正消除：机械按键抖动持续5-20ms。你在ISR入口延时300ms后退出，此时按键可能仍在抖动，下一次电平变化又会触发新的中断，导致多次进入ISR，green/red/blue 被累加多次——消抖完全失效。
//          违反中断设计原则：ISR应“快进快出”，只做最少必要工作（读状态、清标志、设标志位），耗时操作必须移到主循环。
    
    
    

    }
        
        
//        beep_one();
    
    
    
}






void key_scan_tim_ini(void)   // main 里调用
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

    TIM_TimeBaseInitTypeDef t = {0};
    t.TIM_Prescaler         = 840 - 1;    // 84MHz/840 = 100kHz
    t.TIM_Period            = 1000 - 1;   // 1000 tick → 10ms 中断
    t.TIM_CounterMode       = TIM_CounterMode_Up;
    t.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM10, &t);

    TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef n = {0};
    n.NVIC_IRQChannel                   = TIM1_UP_TIM10_IRQn;
    n.NVIC_IRQChannelPreemptionPriority = 2;   // 高于 EXTI0(3),低于播放DMA,扫描不丢
    n.NVIC_IRQChannelSubPriority        = 0;
    n.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&n);

    TIM_Cmd(TIM10, ENABLE);
}




void ProcessKey(volatile Key_t* key)
{
    uint8_t level = GPIO_ReadInputDataBit(key->port, key->pin);

    switch(key->state)
    {
    case KEY_STATE_IDLE:
        if(level == 0) {
            key->state = KEY_STATE_DEBOUNCE;
            key->debounce_count = 0;
        }
        break;

    case KEY_STATE_DEBOUNCE:        // 按下消抖
        if(level == 0) {
            if(++key->debounce_count >= 3) {        // 30ms 连续低
                key->state = KEY_STATE_PRESSED;
                key->press_start_time = GetTim6Tick();
                key->event_flag = KEY_EVENT_PRESS;
            }
        } else {
            key->state = KEY_STATE_IDLE;            // 抖动,作废
            key->debounce_count = 0;
        }
        break;

    case KEY_STATE_PRESSED:         // 已确认按下,检测释放
        if(level == 1) {
            key->debounce_count = 0;
            key->state = KEY_STATE_RELEASE;         // 先进释放消抖,别直接判事件
        }
        break;

    case KEY_STATE_RELEASE:         // 释放消抖:连续3次为高才确认释放
        if(level == 1) {
            if(++key->debounce_count >= 3) {
                uint32_t duration = GetTim6Tick() - key->press_start_time;
                key->event_flag = (duration > LONG_PRESS_THRESHOLD)
                                ? KEY_EVENT_LONG_PRESS : KEY_EVENT_SHORT_PRESS;
                key->state = KEY_STATE_IDLE;
            }
        } else {
            key->state = KEY_STATE_PRESSED;         // 又按回去了
            key->debounce_count = 0;
        }
        break;
    }
}



void TIM1_UP_TIM10_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET) 
    {
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
        ProcessKey(&keys[1]);       // 只扫 PB1;keys[0] 别放 PA0(已被EXTI0占用)
    }
    
    if(keys[1].event_flag == KEY_EVENT_SHORT_PRESS) 
    {
    keys[1].event_flag = KEY_EVENT_NONE;
    status_dev.PlayState = PLAY_NEXT;           // 短按:下一首
    }
    
    if(keys[1].event_flag == KEY_EVENT_LONG_PRESS) 
    {
    keys[1].event_flag = KEY_EVENT_NONE;        // 消费事件
        
     if(status_dev.PlayState == PLAY_CLEAR) // 命令空闲时才发新命令
     {           
        if(audiodev.status & 0X01)          // 正在播放 → 发暂停命令
         status_dev.PlayState = PLAY_PAUSE;
        else  
        {// 已暂停 → 发继续命令
         status_dev.PlayState = PLAY_PLAY;
        }
      }

    }
}


