#include "sg90.h"

/*
void sg90_PWM(uint32_t psc,uint32_t arr)
{
    //    舵机的代码设计流程:
    RCC->AHB1ENR |=(1<<2);
    RCC->APB1ENR |=(1<<1);//    打开时钟（PC TIM3）
    
    //    PC6复用推挽输出
    GPIOC->MODER &=~(0x3<<12);//清零
    GPIOC->MODER |=(0x2<<12);//复用
    GPIOC->AFR[0] &=~(0xf<<24);//清零
    GPIOC->AFR[0] |=(0x2<<24);//复用功能
    GPIOC->OTYPER &=~(1<<6);//推挽
    
    
    TIM3->CR1   = 0;                 // 先清空
    TIM3->CR2   = 0;
    TIM3->CCMR1 = 0;
    TIM3->CCER  = 0;
    
    
	TIM3->CR1 &=~(1<<1);//   更新禁止---主要让UG能产生一次更新事件    
    TIM3->CR1 &=~(1<<3);//    单脉冲模式（发生更新事件不停止计数）
    TIM3->CR1 &=~(1<<4);//    计数方向—递增
    TIM3->CR1 &=~(1<<7);//    ARR无缓冲
    
    TIM3->SMCR &=~(1<<0);//    禁止从模式 选择内部时钟源
    TIM3->EGR |=(1<<0);//    UG置一 手动产生一次更新事件
    
    
    TIM3->CCMR1 |=(1<<3);//    CCR预装载使能----影子寄存器
    TIM3->CCMR1 |=(6<<4);//OC1M   OC1（输出比较1）开头的位是通道1，OC2是通道2   PWM模式1 （cnt<ccr1为有效，否则为无效）
    TIM3->CCMR1 &=~(3<<0);//    通道配置为输出
    TIM3->CCER |=(1<<0);//   通道使能—开启通道
    TIM3->CCER &=~(1<<1);//CC1P     有效电平为高电平 
    
    TIM3->CNT =0;//    配置时基单元（CNT PSC ARR）
    
    TIM3->PSC =42000 - 1;
    分频系数psc==42k，fck==84MHz，arr=1000。基准时钟==84MHz/42K==2KHz，
    计数一次0.5ms，周期==arr*计数一次的时间==500ms，
    高电平时间为（cnt>ccr时）==100*0.5ms==50ms，占空比50ms/500ms==10%
    
    TIM3->PSC =psc-1;//84,基准时钟==84MHz/84==1MHz（每秒1K次），计数一次1us
    TIM3->ARR = arr-1;//20000,舵机的周期要20ms，1MHz / arr = 50Hz = 20ms 周期
    //
    
    TIM3->CR1 |=(1<<0);//使能定时器
    
    //TIM3->CCR1 =1000;//高电平为1000，时长为1000*1us==1ms，转45度
    //ccr可调节的范围越大，精度的上限也越大。想要ccr的范围变大，arr的范围就要变大。
    //arr的范围要变大，psc的值就要变大
}
*/

//pa1 TIM2_CH2
void sg90_PWM(uint32_t psc,uint32_t arr)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AF;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_1;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
    
    GPIO_Init(GPIOA,&gpio_InitTypeDef);
    
    

    // 3. 配置时基单元 (对应原代码中的PSC, ARR, CNT=0, CR1使能)  
    TIM_TimeBaseInitTypeDef tim_TimeBaseInitTypeDef;

    tim_TimeBaseInitTypeDef.TIM_Period            = arr - 1;   // ARR: 
    tim_TimeBaseInitTypeDef.TIM_Prescaler         = psc - 1;   // PSC: 84-1 -> 1MHz计数频率
    tim_TimeBaseInitTypeDef.TIM_ClockDivision     = TIM_CKD_DIV1;
    tim_TimeBaseInitTypeDef.TIM_CounterMode       = TIM_CounterMode_Up; // 递增计数
    tim_TimeBaseInitTypeDef.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tim_TimeBaseInitTypeDef);

    //TIM2频率等于两倍APB1频率，fcnt为1MHz,Tcnt为1us
    //Tcnt * ARR=Tperiod=20ms=20000us
    //ARR=20000
    
    // 4. 配置OC1通道为PWM模式1 (对应原代码CCMR1, CCER配置)
    TIM_OCInitTypeDef tim_OCInitTypeDef;
    
    tim_OCInitTypeDef.TIM_OCMode      = TIM_OCMode_PWM1;        // ← PWM1模式的核心设置
    tim_OCInitTypeDef.TIM_OutputState = TIM_OutputState_Enable;  // 通道输出使能
    tim_OCInitTypeDef.TIM_Pulse       = 0;                     // 初始比较值
    tim_OCInitTypeDef.TIM_OCPolarity  = TIM_OCPolarity_High;     // 有效电平极性
    
    TIM_OC2Init(TIM2, &tim_OCInitTypeDef);             
    

    // 5. 使能CCR预装载(影子寄存器) & 使能ARR预装载
    // 对应原代码: CCMR1 |= (1<<3) 和 CR1 &= ~(1<<7)的反向操作(标准库默认开启ARPE)
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    // 6. 手动产生一次更新事件，将影子寄存器值加载到活动寄存器
    // 对应原代码: EGR |= (1<<0)
    TIM_GenerateEvent(TIM2, TIM_EventSource_Update);

    // 7. 使能定时器
    TIM_Cmd(TIM2, ENABLE);
}











/*
 * SG90 舵机角度控制
 * PSC=84, ARR=20000 → 1 tick = 1μs, 周期 = 20ms
 *
 * 脉冲范围: 0.5ms ~ 2.5ms → 500 ~ 2500 tick
 *         0° ~ 180° 线性映射
 *
 * 精度: 2000 tick / 180° ≈ 11 tick/° → 0.09°/tick
 * float 运算 + 四舍五入，消除整数截断误差
 */

#define SG90_MIN   500    // 0°:  0.5ms
#define SG90_MAX   2500   // 180°: 2.5ms

void sg90_set_angle(float angle)
{
    /* 限幅 */
    if (angle < 0.0f)   angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    /*
     * pulse = 500 + (angle / 180) × (2500 - 500)
     *         = 500 + angle × 2000 / 180
     *         = 500 + angle × 11.111...
     *
     * 先乘再除 + 0.5f → 四舍五入到最近整数
     */
    uint32_t pulse = SG90_MIN+ (uint32_t)(angle * (SG90_MAX - SG90_MIN) / 180.0f + 0.5f);

    TIM2->CCR2 = pulse;
}

