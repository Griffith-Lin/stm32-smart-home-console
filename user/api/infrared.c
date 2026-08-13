#include "infrared.h"

//PE6 IR_RX  tim9_ch2的输入捕获 
//有红外信号时3638T输出低电平，无信号则高电平
void irm_3638T_ini(uint16_t psc,uint16_t arr)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AF;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_OD;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_6;
    gpio_InitTypeDef.GPIO_Speed=GPIO_High_Speed;
    
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_TIM9);
    
    GPIO_Init(GPIOE,&gpio_InitTypeDef);
    
    
    TIM_TimeBaseInitTypeDef tim_TimeBaseInitTypeDef={0};
    
    tim_TimeBaseInitTypeDef.TIM_Prescaler=psc-1;
    tim_TimeBaseInitTypeDef.TIM_Period=arr-1;
    tim_TimeBaseInitTypeDef.TIM_CounterMode=TIM_CounterMode_Up;//将定时器配置为向上计数模式   因为直觉习惯
    tim_TimeBaseInitTypeDef.TIM_ClockDivision=TIM_CKD_DIV1;//不分频  它不是定时器计数时钟的主分频，而是 定时器内部采样时钟 DTS 的分频。
    
    TIM_TimeBaseInit(TIM9,&tim_TimeBaseInitTypeDef);
    
    
    TIM_ICInitTypeDef tim_ICInitTypeDef={0};
    
    tim_ICInitTypeDef.TIM_Channel=TIM_Channel_2;
    tim_ICInitTypeDef.TIM_ICPrescaler=TIM_ICPSC_DIV1;
    tim_ICInitTypeDef.TIM_ICPolarity=TIM_ICPolarity_BothEdge;//双边沿捕获
    tim_ICInitTypeDef.TIM_ICSelection=TIM_ICSelection_DirectTI;
//    tim_ICInitTypeDef.TIM_ICFilter
    
   
    
    
    TIM_ICInit(TIM9,&tim_ICInitTypeDef);
    
         
    NVIC_InitTypeDef nvic_InitTypeDef={0};
    
    nvic_InitTypeDef.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn;
    nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=3;
    nvic_InitTypeDef.NVIC_IRQChannelSubPriority=0;
    nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;
    
    NVIC_Init(&nvic_InitTypeDef);
    
    
    
    TIM_ARRPreloadConfig(TIM9, DISABLE);
    TIM_GenerateEvent(TIM9, TIM_EventSource_Update);  
    TIM_ITConfig(TIM9,TIM_IT_CC2, ENABLE); //使能中断 (使能通道2捕获中断)
     TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
    
    
    TIM_Cmd(TIM9,ENABLE);
    
    
}

uint16_t tim9_begin=0;
uint16_t tim9_count=0;
uint16_t T_tim9_sum=0;
uint8_t tim9_begin_flag=0;//同步码头标志位
uint8_t tim9_high_flag=0;//高电平标志位


uint32_t infrared_buf=0;//接收数据的缓存区

//红外接收端接收到红外后，返回给芯片的是，逻辑1是560us低+1680us高，逻辑0应该是560us低+560us 高 .(接收头接收到的信号的反向的)
void TIM1_BRK_TIM9_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM9,TIM_IT_Update))
    {
        TIM_ClearITPendingBit(TIM9,TIM_IT_Update);   // ← 查了就要清,不然会一直在isr里面，跳不出去
        
        if(tim9_high_flag==1)//高电平开始计周期
        tim9_count++;
        
    }

    if(TIM_GetITStatus(TIM9,TIM_IT_CC2))
    {
        TIM_ClearITPendingBit(TIM9,TIM_IT_CC2);
        
        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6) && tim9_begin_flag==0)//还未收到码头
        {
            
             //接收到高电平          
            tim9_begin=TIM_GetCapture3(TIM3);           
            tim9_count=0;
            
            tim9_high_flag=1;//代表接收了高电平
        }
        else
        {
            //进这里必须要先接收过高电平
            //接收到低电平时，也就是接收完高电平时，开始计算高电平时长 
            if(tim9_high_flag==1)
            {
                tim9_high_flag=0;    
                    
                T_tim9_sum=(tim9_count*1000-tim9_begin+TIM_GetCapture2(TIM9));
                 
                //接收到的是4.5ms高电平（同步码头）
                if(T_tim9_sum>4300)
                {
                tim9_begin_flag=1;
                }
                
                tim9_begin=0;
                tim9_count=0;
                T_tim9_sum=0;

            }
        }
        
        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6) && tim9_begin_flag==1)//收到码头
        {
            
             //接收到高电平          
            tim9_begin=TIM_GetCapture3(TIM3);           
            tim9_count=0;
            
            tim9_high_flag=1;//代表接收了高电平
        }
        else
        {

            if(tim9_high_flag==1)
            {
                tim9_high_flag=0;    
                    
                T_tim9_sum=(tim9_count*1000-tim9_begin+TIM_GetCapture2(TIM9));
                 
                //根据高电平时长，判断逻辑0 1，存入infrared_buf
                
                
                tim9_begin=0;
                tim9_count=0;
                T_tim9_sum=0;

            }
        }
        
        
        
        
    }
}

//tim9_begin_flag==1后，进入接收数据的函数，NEC遥控指令按照低位在前，高位在后的顺序发送，所以从低到高接收
// 返回一个存储 地址码（遥控ID）、地址反码、控制码（键值）、控制反码 的32位数
uint32_t infrared_begin(void)
{
    
}

