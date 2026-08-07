#include "delay.h"


/*
如果 APB 预分频系数为 1（即 APB 时钟 = AHB 时钟）：
定时器时钟频率 = APB 时钟频率。
如果 APB 预分频系数不为 1（即 APB 时钟被分频了）：
定时器时钟频率 = APB 时钟频率 × 2。
*/
void TIM6_Delay_Init(uint32_t psc,uint32_t arr)   
{
    RCC->APB1ENR |= (1<<4);//使能时钟
    
    TIM6->PSC = psc-1;//预分频系数 (PSC) = (时钟源频率 / 目标计数频率) - 1, 每 1μs 计数一次（即 1MHz 计数频率）,84MHz/1Mhz -1
    TIM6->ARR = arr-1;//1ms,24位
     
    TIM6->CR1 &=~(1<<1);//使能UEV事件（更新事件）。更新事件通过以下三种情况产生：1、计数器上溢/下溢  2、将UG位置1  3、通过从模式控制器生成的更新事件
    TIM6->CR1 &=~ (1<<2);//使能更新请求源。 产生更新事件后，会生成更新中断或dma请求
    TIM6->CR1 &=~(1<<3);//单脉冲模式,计数到0后自动停止，无需软件关闭
    TIM6->CR1 &=~(1<<7);//禁能自动重载预装载（arr无缓冲）
    
    TIM6->DIER |= (1<<0);//使能中断
    
    TIM6->EGR |= (1<<0);//手动置一，生成更新事件
    
    TIM6->CR1 |= (1<<0);//CEN:使能定时器
    
    NVIC_SetPriority(TIM6_DAC_IRQn,7);//占先=3 次级=1
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

volatile uint32_t tim6=0;

void TIM6_delay(uint32_t ms)
{ 
   TIM6->CR1 |= (1<<0);
    
   tim6=ms;
    while(tim6);
    /*
    while(tim6) 是忙等（busy-wait），CPU 被占着、main 后面的代码不执行。
    但它不像 while(1) 那样死循环，因为 ISR 在背后偷偷改 tim6，最终会让条件变假。
    这就是 volatile 为什么重要——
    没有它编译器会把 while(tim6) 当成 while(1) 处理，因为编译器"看不见"ISR。
    */
}


//有中断必配优先级
void TIM6_DAC_IRQHandler(void)
{
    
   
    if(TIM6->SR & (1<<0))// 等 UIF 变 1
    {
        TIM6->SR &=~(1<<0);//清空状态
        tim6--;
        //如果定时器不会停，中断会一直触发，这里不加判断，tim6 会从 0 继续递减变成 0xFFFFFFFF, 0xFFFFFFFE... while(tim6) 永远为真 → 主线程永久死锁
    }
}


