#include "motor.h"


//pc8 AF2 TIM3_CH3
void motor_pwm_ini(uint32_t psc,uint32_t arr)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AF;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_8;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef.GPIO_Speed=GPIO_Low_Speed;
    
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);
    
    GPIO_Init(GPIOC,&gpio_InitTypeDef);
    
    
    TIM_TimeBaseInitTypeDef tim_TimeBaseInitTypeDef={0};
    
    tim_TimeBaseInitTypeDef.TIM_Prescaler=psc-1;
    tim_TimeBaseInitTypeDef.TIM_Period=arr-1;
    tim_TimeBaseInitTypeDef.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_TimeBaseInitTypeDef.TIM_CounterMode = TIM_CounterMode_Up;
    tim_TimeBaseInitTypeDef.TIM_RepetitionCounter = 0; // 通用定时器此项无效，
    
    TIM_TimeBaseInit(TIM3,&tim_TimeBaseInitTypeDef);
    
    TIM_OCInitTypeDef tim_OCInitTypeDef={0};
    
    tim_OCInitTypeDef.TIM_OCMode=TIM_OCMode_PWM1;
    tim_OCInitTypeDef.TIM_OutputState=TIM_OutputState_Enable;
    tim_OCInitTypeDef.TIM_OCPolarity=TIM_OCPolarity_High;
    tim_OCInitTypeDef.TIM_Pulse=0;//初始值为0
    
    TIM_OC3Init(TIM3,&tim_OCInitTypeDef);
    
    // 5. 使能CCR预装载(影子寄存器) & 使能ARR预装载 
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    
    TIM_GenerateEvent(TIM3,TIM_EventSource_Update);
    
    
    
    TIM_Cmd(TIM3, ENABLE);
    
}

void mortor_minspeed_open(void)
{
    TIM_SetCompare3(TIM3,500);
    Delay_Ms(100);
    TIM_SetCompare3(TIM3,200);
}

void Motor_Control(uint16_t speed)//arr=1000
{
    TIM_SetCompare3(TIM3,speed);
}

