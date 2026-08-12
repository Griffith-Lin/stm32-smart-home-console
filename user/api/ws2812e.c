#include "ws2812e.h"


//pb15
void ws2812e_ini(uint16_t rgb_num)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_OUT;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_15;
//    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_DOWN;上下拉电阻仅在输入模式下有效
    gpio_InitTypeDef.GPIO_Speed=GPIO_High_Speed;
    
    GPIO_Init(GPIOB,&gpio_InitTypeDef);
    
    
    PB15_LOW();
    Delay_Us(2000);//复位灯带，清除脏数据
     
    ws2812e_open_reset(0,0,0,rgb_num);
    ws2812e_open_reset(0,0,0,rgb_num);
    //多调用一次ws2812e_open_reset，解决复位时第一个灯常亮绿灯，但是复位时第一个灯依旧有绿灯亮闪
    
/*
    MCU 复位 → PB15 浮空 → 噪声 → WS2812 第一个灯吞了 24bit → 锁存绿灯
                                                    ↓
                你的代码开始跑 → 复位 2ms → 发两遍黑数据 → 没用
                因为第一个灯已经锁存了噪声数据，不再接收新数据
                
                
你在代码层的操作（2ms 复位、发两遍黑数据）都发生在芯片的复位之后。但噪声发生在芯片的复位期间，那时 CPU 根本没在跑代码，你控制不了 PB15。

就像别人趁你不在家往信箱里塞了垃圾，你回家再锁门也清不掉已经塞进去的东西。

唯一解法：硬件上加 10kΩ 下拉电阻，让 PB15 在 CPU 不干活的时候也被拽在 GND。这是 WS2812 电路的常规做法，成本两分钱。
*/
    
    
    
    
    //查是否是中断的原因
    //__disable_irq();                // 关全局中断
    
//    rgb_num += 10;                // 冗余，确保覆盖所有灯

//    PB15_LOW();                   // 先复位，清除复位期间的噪声
//    Delay_Us(400);
//    
//    while(rgb_num--)
//    ws2812e_open(0,0,0);
//    
//    
//    PB15_LOW();
//    Delay_Us(400);
    
    //__enable_irq();                 // 开全局中断
}

void ws2812e_send_byte(uint8_t data)
{
     __disable_irq();                    // 关全局中断，保护时序
    
    for(int i=0;i<8;i++)
    {
        if(data & (0x80>>i))
        {
            PB15_HIGH();
            Delay_100Ns(7);
            PB15_LOW();
            Delay_100Ns(3);
        }
        else
        {
            PB15_HIGH();
            Delay_100Ns(3);
            PB15_LOW();
            Delay_100Ns(7);
        }
    }

    
    __enable_irq();                 // 开全局中断
}


void ws2812e_reset(void)
{
    
    PB15_LOW();
    Delay_Us(400);
}



void ws2812e_open(uint8_t green,uint8_t red,uint8_t blue)
{
    ws2812e_send_byte(green);
    ws2812e_send_byte(red);
    ws2812e_send_byte(blue);
}

volatile uint8_t green=0;
volatile uint8_t red=0;
volatile uint8_t blue=0;


void ws2812e_open_reset(uint8_t green,uint8_t red,uint8_t blue,uint8_t rgb_num)
{
    while(rgb_num--)
    ws2812e_open(green,red,blue);
    
    ws2812e_reset();
}

//uint8_t tmp_flag=1;

//void ws2812e_mode1(void)
//{
//    if(green!=255 && tmp_flag)
//    {
//    green++;
//    red++;
//    blue++;
//    }
//    else if(
//    {
//    green--;
//    red--;
//    blue--;   
//    tmp_flag=0;
//    }
//}


