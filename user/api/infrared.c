#include "infrared.h"

//在STM32的时钟树（Clock Tree）中，APB1和APB2总线分频器之后，专门针对定时器挂载了一个硬件倍频器。
//当 APB 预分频系数 = 1 时（也就是APB没有对AHB分频时）：定时器时钟 = APB 总线时钟。
//当 APB 预分频系数 ≠ 1 时：定时器时钟 = APB 总线时钟 × 2。


//抢占优先级1 合理。NEC 数据位间隔 ~560μs，时间敏感，必须最高

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
//    tim_TimeBaseInitTypeDef.TIM_ClockDivision=TIM_CKD_DIV1;//不分频  它不是定时器计数时钟的主分频，而是 定时器内部采样时钟 DTS 的分频。
    
    TIM_TimeBaseInit(TIM9,&tim_TimeBaseInitTypeDef);
    
    
    TIM_ICInitTypeDef tim_ICInitTypeDef={0};
    
    tim_ICInitTypeDef.TIM_Channel=TIM_Channel_2;
    tim_ICInitTypeDef.TIM_ICPrescaler=TIM_ICPSC_DIV1;//几个事件触发一次
//    tim_ICInitTypeDef.TIM_ICPolarity=TIM_ICPolarity_BothEdge;//双边沿捕获  此方案麻烦！！
    tim_ICInitTypeDef.TIM_ICPolarity=TIM_ICPolarity_Rising;
    tim_ICInitTypeDef.TIM_ICSelection=TIM_ICSelection_DirectTI;
//    tim_ICInitTypeDef.TIM_ICFilter
    
   
    
    
    TIM_ICInit(TIM9,&tim_ICInitTypeDef);
    
         
    NVIC_InitTypeDef nvic_InitTypeDef={0};
    
    nvic_InitTypeDef.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn;
    nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=1;
    nvic_InitTypeDef.NVIC_IRQChannelSubPriority=0;
    nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;
    
    NVIC_Init(&nvic_InitTypeDef);
    
    
    
    TIM_ARRPreloadConfig(TIM9, DISABLE);
    TIM_GenerateEvent(TIM9, TIM_EventSource_Update);  
    TIM_ITConfig(TIM9,TIM_IT_CC2, ENABLE); //使能中断 (使能通道2捕获中断)
     TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
    
    
    TIM_Cmd(TIM9,ENABLE);
    
    
}




volatile uint32_t infrared_buf=0;//接收数据的缓存区

uint16_t high_cnt=0;
uint16_t count=0;//校验32位完整性
uint8_t ir_begin_flag=0;
uint8_t ir_command=0;

uint8_t deal_ir_flag=0;//接收的正确红外，处理标志

//红外接收端接收到红外后，返回给芯片的是，逻辑1是560us低+1680us高，逻辑0应该是560us低+560us 高 .(接收头接收到的信号的反向的)
//红外接收端空闲时是高电平
//×2 倍频器只对定时器生效，非定时器外设（包括 USART）的时钟始终严格等于 PCLK 本身。
//APB2的频率是84MHz（168MHz/84MHz！=1，定时器内部要倍频）   定时器内部倍频==84MHz*2==168MHz   定时器内部分频==168MHz/168==1MHz
//Tcnt==1us  Tperiod==1us  *  arr >=4500us+560us(加560us是因为遇到上升沿ccr才清0）  arr>=5100，arr要比理论值设置多的多
void TIM1_BRK_TIM9_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM9,TIM_IT_Update))
    {
        TIM_ClearITPendingBit(TIM9,TIM_IT_Update);   // ← 查了就要清,不然会一直在isr里面，跳不出去
        
       
        if(ir_begin_flag==1 && count==32)
        {
//            printf("total=%x\r\n",infrared_buf);     
            ir_command = (infrared_buf>>16) & 0xff ;   
               
//            printf("C=%d\r\n",ir_command);
            infrared_buf=0;//清空
            ir_begin_flag=0;//码头标志位清0
            
            deal_ir_flag=1;//处理一次
            
            //进入歌曲播放后，主函数就走不下去了，执行不到其它函数。除非中断函数
            if(ir_command==69)
            {
            beep_one();
            }
            else if(ir_command==8)
            {
                beep_one();
             status_dev.PlayState=PLAY_PREVIOUS;//上一曲
            }
            else if(ir_command==90)
            {
                beep_one();
             status_dev.PlayState=PLAY_NEXT;//下一曲
            }
            else if(ir_command==28)
            {
                beep_one();
             
             if(audiodev.status & 0X01)          // 正在播放 → 发暂停命令
             status_dev.PlayState = PLAY_PAUSE;
             else  
             {// 已暂停 → 发继续命令
             status_dev.PlayState = PLAY_PLAY;
             }
            }
            
        }
    }

    
    
    if(TIM_GetITStatus(TIM9,TIM_IT_CC2))
    {
        TIM_ClearITPendingBit(TIM9,TIM_IT_CC2);
        
                    
        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6))
        {
            TIM_SetCounter(TIM9,0);//遇到上升沿，ccr清0
            
            TIM9->CCER |=(1<<5);
            TIM9->CCER &=~(1<<7);//修改为下降沿触发           
            
        }
        else
        {
            TIM9->CCER &=~(1<<5);
            TIM9->CCER &=~(1<<7);//修改为上升沿触发          
             
            high_cnt=TIM_GetCapture2(TIM9);
            
            if(ir_begin_flag==0 && high_cnt > 4000 && high_cnt < 4600)
            {
                ir_begin_flag=1;
                count=0;
            }
            else if(ir_begin_flag==1)
            {
                infrared_buf>>=1;//先移位，后赋值，不然的话，最后整体会向右偏移一位
                
                if(high_cnt>500 && high_cnt<600)
                {
                    
                }
                else if(high_cnt>1500 && high_cnt<1700)
                {
                    infrared_buf |= 0x80000000;
                }
                 else
                {
                    //遇到非法脉宽，立即熔断，放弃本次接收！
                    ir_begin_flag = 0;
                    count = 0;
                    return; // 提前退出，防止执行下方的 count++
                }
                count++;
            }
            
//             printf("%d\r\n", high_cnt);
                       
        }
     
    }
}

//69 70 71 
//68 64 67 
// 7 21 9 
//22 25 13 
//   24 
// 8 28 90 
//   82
void deal_if(void)
{
    if(deal_ir_flag==0)
        return;
    
    if(ir_command==69)
    {
    beep_one();
    }
    else if(ir_command==8)
    {
        beep_one();
     status_dev.PlayState=PLAY_PREVIOUS;//上一曲,进入歌曲播放后，主函数就走不下去了，执行不到其它函数。除非中断函数
    }
    else if(ir_command==90)
    {
        beep_one();
     status_dev.PlayState=PLAY_NEXT;//下一曲
    }
    
    
    deal_ir_flag=0;
}

