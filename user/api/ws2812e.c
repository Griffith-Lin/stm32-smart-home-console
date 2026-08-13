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
//    gpio_InitTypeDef.GPIO_Speed=GPIO_High_Speed;
    
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




复位按下瞬间，PB15 进入悬空输入，被 WS2812E DIN 内部 ~100k 上拉拉高。灯的译码器看到这个上升沿，开始计第一个 bit 的高电平时间：
这一个下降沿完成了 bit：高电平时间 = 你按住复位的整个时长（几百 ms）>> 1 码 0.9μs 阈值 → 被判为 "1"。

这个 "1" 是新帧的第一个 bit，落在 G7（绿的最高位）→ green = 0b1000_0000 = 128，半亮绿。
之后 PB15 持续低电平（SystemInit 配 HSE/PLL + C 初始化 + 各外设初始化，好几 ms），超过 WS2812E 的复位阈值 → 灯锁存 → 第一个灯亮半绿。
直到 ws2812e.c:23-24 的两帧全 0 数据 + 结尾复位重新锁存 → 熄灭。这就是"闪一下"——约 5~15ms。
注意：这个"1"是确定性的，不是你之前判断的纯噪声——所以它和复位按住时长、SystemInit 的拉低时机强相关。

RTC_Cal_Config() 为什么能影响它——诚实地说
我把 rtc.c 逐行查过了：这个函数不碰任何 GPIO，PB15 在它执行前后都是低电平，它也不改变下次启动的流程。它唯一能跨越复位残留的，是备份域：BDCR 里的 RTCEN/RTCSEL 不受系统复位影响，LSI 和 RTC 会在下次复位窗口内继续振荡/走时（case B）；而 case A 在断电重启后（无 VBAT 时备份域清零），复位窗口内只有 HSI，没有 LSI。

也就是说："复位窗口内锁存的垃圾模式"对芯片内部活动极其敏感——你自己之前的排查（"硬件噪声，只能加下拉解决"）已经证明这盏灯对悬空线上的扰动敏感到了离谱的程度。RTC_Cal_Config 改变的就是这个窗口内芯片内部的状态，而不是灯线本身。




rtc.c:105-110这段代码，注释掉，闪灯问题复现
这段循环不碰任何硬件，它对灯的"影响"只有一个途径：改变了编译产物。而绿灯的窗口是下载结束那一刻：


二进制变 → 烧录器擦除/编程/校验的字节序列变
→ 下载尾声的 VDD 电流瞬态 + SWD(PA13/PA14) 校验流量模式变
→ 复位瞬间 DIN 悬空（100k 上拉）采到的噪声模式变
→ 垃圾帧凑不凑得成"以 1 开头的绿色"跟着变
校验读回的字节就是你刚烧进去的镜像——垃圾帧的内容直接编码了你的二进制。这就是为什么动任何一行代码（哪怕纯函数）现象都会翻转：你改的不是逻辑，是烧进 flash 的字节流。

决定性实验（比上一个更锋利）
1.同一份固件不重新下载，按复位键 10 次。 预测：现象稳定不变（下载过程不在场，噪声源没了）。如果按复位键也会随机闪，说明噪声源另有其人，我的机制就错了。
2.改 Keil 下载设置，不改代码：同一份二进制，把 "Verify" 关掉、或 "Erase Sectors" 换成 "Erase Full Chip" 再烧。若现象跟着下载设置走，直接实锤。
3.示波器挂 PB15，看下载→复位的全过程，垃圾边沿一目了然。


Keil 下载设置，换成 "Erase Full Chip" 再烧，闪灯问题还是不能解决



*/
    
      
    
    //查是否是中断的原因
    //__disable_irq();                // 关全局中断
    
    //过程..........
    
    //__enable_irq();                 // 开全局中断
}

void ws2812e_send_byte(uint8_t data)
{
     __disable_irq();                    // 关全局中断，排除法，锁定问题用
    
    uint8_t j;
    
    //1码低电平300ns，0码高电平300ns
    for(int i=0;i<8;i++)
    {
        if(data & (0x80>>i))
        {
            PB15_HIGH();
            Delay_Us(1);
            PB15_LOW();
            Delay_nop(40);//1nop==1/168 us==5.952ns    300ns/5.952==50.40nop
                             
        }
        else
        {
            PB15_HIGH();
            Delay_nop(40);
            PB15_LOW();
            Delay_Us(1);
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


