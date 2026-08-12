#include "tim.h"


///*
//如果 APB 预分频系数为 1（即 APB 时钟 = AHB 时钟）：
//定时器时钟频率 = APB 时钟频率。
//如果 APB 预分频系数不为 1（即 APB 时钟被分频了）：
//定时器时钟频率 = APB 时钟频率 × 2。
//*/
//void TIM6_Task_Init(uint32_t psc,uint32_t arr)   
//{
//    RCC->APB1ENR |= (1<<4);//使能时钟
//    
//    TIM6->PSC = psc-1;//预分频系数 (PSC) = (时钟源频率 / 目标计数频率) - 1, 每 1μs 计数一次（即 1MHz 计数频率）,84MHz/1Mhz -1
//    TIM6->ARR = arr-1;//1ms,24位
//     
//    TIM6->CR1 &=~(1<<1);//使能UEV事件（更新事件）。更新事件通过以下三种情况产生：1、计数器上溢/下溢  2、将UG位置1  3、通过从模式控制器生成的更新事件
//    TIM6->CR1 &=~ (1<<2);//使能更新请求源。 产生更新事件后，会生成更新中断或dma请求
//    TIM6->CR1 &=~(1<<3);//单脉冲模式,计数到0后自动停止，无需软件关闭
//    TIM6->CR1 &=~(1<<7);//禁能自动重载预装载（arr无缓冲）
//    
//    TIM6->DIER |= (1<<0);//使能更新中断
//    
//    TIM6->EGR |= (1<<0);//手动置一，生成更新事件
//    
//    TIM6->CR1 |= (1<<0);//CEN:使能定时器
//    
//    NVIC_SetPriority(TIM6_DAC_IRQn,7);//占先=3 次级=1
//	NVIC_EnableIRQ(TIM6_DAC_IRQn);
//}


//delay意味着阻塞，tim中断最好不要用delay命名
void TIM6_Task_Init(uint32_t psc,uint32_t arr) 
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
    
    TIM_TimeBaseInitTypeDef tim_TimeBaseInitTypeDef;
    
    
    // 标准库的 TIM_TimeBaseInit 内部默认将 UDIS(位1)=0, URS(位2)=0, OPM(位3)=0。
    // 因此你注释中的前三行寄存器操作，在调用此函数时已自动完成。
    // 注：TIM6 是基本定时器，不支持时钟分频和计数模式选择，后两个参数填默认值即可。
    tim_TimeBaseInitTypeDef.TIM_Period = arr-1;                  // 对应 ARR
    tim_TimeBaseInitTypeDef.TIM_Prescaler = psc-1;               // 对应 PSC
    tim_TimeBaseInitTypeDef.TIM_ClockDivision = TIM_CKD_DIV1;  // TIM6 忽略此项
    tim_TimeBaseInitTypeDef.TIM_CounterMode = TIM_CounterMode_Up; // TIM6 只能向上计数，忽略此项
    
    TIM_TimeBaseInit(TIM6, &tim_TimeBaseInitTypeDef);

    // 禁能自动重载预装载 (ARPE=0) 
    // 对应: TIM6->CR1 &=~(1<<7);
    TIM_ARRPreloadConfig(TIM6, DISABLE);

    //使能更新中断 
    //对应: TIM6->DIER |= (1<<0);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    //手动生成更新事件 
    // 对应: TIM6->EGR |= (1<<0);
    // 作用：将当前设置的 PSC 和 ARR 立即更新到影子寄存器，确保首次启动时参数生效
    TIM_GenerateEvent(TIM6, TIM_EventSource_Update);

    
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//次优先级
    
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
    
    
//    TIM_Cmd(TIM6, ENABLE);//在延时函数里使能和禁能，能减小能耗
}






//volatile uint32_t tim6=0;

//void TIM6_delay(uint32_t ms)
//{ 
//    TIM6->CR1 |= (1<<0);
//    
//    tim6=ms;
//    while(tim6);
//    
//    TIM6->CR1 &=~(1<<0);
//    /*
//    while(tim6) 是忙等（busy-wait），CPU 被占着、main 后面的代码不执行。
//    但它不像 while(1) 那样死循环，因为 ISR 在背后偷偷改 tim6，最终会让条件变假。
//    这就是 volatile 为什么重要——
//    没有它编译器会把 while(tim6) 当成 while(1) 处理，因为编译器"看不见"ISR。
//    */
//}

volatile uint32_t tim6=0;

void TIM6_delay(uint32_t ms)
{
    TIM_Cmd(TIM6,ENABLE);
    
    tim6=ms;
    while(tim6);    
    
    TIM_Cmd(TIM6,DISABLE);
}




////有中断必配优先级
//void TIM6_DAC_IRQHandler(void)
//{ 
//    if(TIM6->SR & (1<<0))// 等 UIF 变 1
//    {
//        TIM6->SR &=~(1<<0);//清空状态
//        tim6--;
//        //如果定时器不会停，中断会一直触发，这里不加判断，tim6 会从 0 继续递减变成 0xFFFFFFFF, 0xFFFFFFFE... while(tim6) 永远为真 → 主线程永久死锁
//    }
//}


//中断外面用TIM_GetFlagStatus,中断里面用TIM_GetITStatus
void TIM6_DAC_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6, TIM_IT_Update)==SET)
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        
        if (tim6 > 0) 
        {tim6--;}
    }
}


//pb0配置为输入捕获模式






