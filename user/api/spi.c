#include "spi.h"


///*---------spi通信（硬件实现）----------*/
///*
//为什么不用pa4的spi1_nss？
//做嵌入式开发（尤其是驱动 W25Qxx Flash 这种），强烈建议使用软件 NSS 模式。
//把 NSS 引脚配置为普通推挽输出 GPIO。
//不要开启 SPI 外设的硬件 NSS 功能（除非你非常清楚自己在做什么）。
//这样你可以精确控制片选的时序，避免硬件自动拉高/拉低导致的通信失败。

//pb14 cs片选线

//AF5
//pa5 spi1_sck
//pa6 spi1_miso 
//pa7 spi1_mosi
//*/

////结合从机SPI的主机代码流程分析:
//void spi_ini(void)
//{
//    //打开时钟（SPI1 PA PB）
//    RCC->APB2ENR |=(1<<12);//使能spi1时钟
//    RCC->AHB1ENR |=(1<<0);//使能gpioa时钟
//    RCC->AHB1ENR |=(1<<1);//使能gpiob时钟
//    
//    //pa5 6 7
//    GPIOA->MODER &=~(0x3f<<10);//清零
//    GPIOA->MODER |=(0x2a<<10);//功能模式为复用
//    GPIOA->OTYPER &=~(7<<5);//推挽 
//    GPIOA->OSPEEDR &=~(0x3f<<10);
//    GPIOA->OSPEEDR |=(0x2a<<10);//引脚速度 快速
//    GPIOA->PUPDR &=~(0x3f<<10);//上拉下拉设置为无，和ODR、IDR有关    
//    GPIOA->AFR[0] &=~(0xfff<<20);
//    GPIOA->AFR[0] |=(0x555<<20);;//af5
//    
//    //pb14
//    GPIOB->MODER &=~(3<<28);
//    GPIOB->MODER |= (1<<28);//通用输出
//    GPIOB->OTYPER &=~(1<<14);//推挽
//    GPIOB->OSPEEDR &=~(3<<28);//低速，片选线不用高速
//    GPIOB->PUPDR &=~(3<<28);//无上下拉
//    SPI_CS = 1;   // 初始化完成后释放片选


//    SPI1->CR1 =0;
//    
//    /*
//    0(高位)    0(低位)     ==0 
//    CPOL       CPHA        sp0
//    */
//    SPI1->CR1 &=~(1<<0);//时钟相位CPHA,从第一个时钟边沿开始采样数据
//    SPI1->CR1 &=~(1<<1);//时钟极性CPOL,空闲状态时，SCK保持低电平
//    SPI1->CR1 |=(1<<2);//配置为主机 (Master)，告诉 SPI 硬件，由我来产生 SCK 时钟信号，并主导通信流程。
//    SPI1->CR1 &=~(7<<3);//波特率控制 fpclk/2，在仿真器或极短的 PCB 走线上，fPCLK/2 没问题。但在实际智能家居产品的外接模块中，线束寄生电容会导致高速信号失真。强烈建议在调试初期改为 3<<3 (即 011，分频系数 8 或 16)，通信稳定后再尝试提高速率。
//    SPI1->CR1 &=~(1<<7);//先发送MSB,先发送最高有效位 (MSB)
//    SPI1->CR1 |=(3<<8);//从器件管理,启用软件管理 NSS (片选) 信号。这意味着 STM32 不会自动控制硬件 NSS 引脚（如 PA4），而是把它释放出来，当作普通的 GPIO 让你手动控制（拉低选中，拉高释放）。这在驱动多个 SPI 设备时是必须的。
//    SPI1->CR1 &=~(1<<11);//数据帧格式---八位
//    SPI1->CR1 &=~(1<<15);//双线单向通信数据格式,即 MOSI 和 MISO 独立工作，这是标准 SPI 的形态
//    SPI1->CR1 |=(1<<6);//使能SPI,打开 SPI 模块的时钟树和逻辑电路
//   
//}


/*
pc7 flash_cs
pa5 SPI1_SCK
pa6 SPI1_MISO
pa7 SPI1_MOSI

*/
void spi_ini(void)
{
    RCC->APB2ENR |=(1<<12);//使能spi1时钟
    RCC->AHB1ENR |=(1<<0);//使能gpioa时钟
    RCC->AHB1ENR |=(1<<2);//使能gpioc时钟
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AF;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Speed=GPIO_High_Speed;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    
    GPIO_Init(GPIOA,&gpio_InitTypeDef);
    
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_7;
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_OUT;
    gpio_InitTypeDef.GPIO_Speed=GPIO_Low_Speed;
    
    GPIO_Init(GPIOC,&gpio_InitTypeDef);
    
    
    GPIO_PinAFConfig(GPIOA,GPIO_Pin_5,GPIO_AF6_SPI1);
    GPIO_PinAFConfig(GPIOA,GPIO_Pin_6,GPIO_AF6_SPI1);
    GPIO_PinAFConfig(GPIOA,GPIO_Pin_7,GPIO_AF6_SPI1);    
    
    
    SPI_InitTypeDef spi_InitTypeDef={0};
    
    spi_InitTypeDef.SPI_CPOL=SPI_CPOL_Low;
    spi_InitTypeDef.SPI_CPHA=SPI_CPHA_1Edge;
    spi_InitTypeDef.SPI_DataSize=SPI_DataSize_8b;
    spi_InitTypeDef.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
    spi_InitTypeDef.SPI_Mode=SPI_Mode_Master;
    spi_InitTypeDef.SPI_FirstBit=SPI_FirstBit_MSB;
    spi_InitTypeDef.SPI_NSS=SPI_NSSInternalSoft_Set;
    spi_InitTypeDef.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
    
    SPI_Init(SPI1,&spi_InitTypeDef);
    
    

}




//SPI数据收发一体函数 先发后收
uint8_t SPI_Echo(uint8_t data)
{
	uint32_t timeout = 0xFFFF;//如果硬件出问题（MISO 短路、W25Qxx 损坏），两个 while 会永久死等。加超时
    while((SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)==SET) && --timeout);//等待发送缓冲区为空
    SPI_SendData(SPI1,data);
    timeout = 0xFFFF;
    while((SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)==SET) && --timeout);//等待接收缓冲区为空
    return SPI_ReceiveData(SPI1);
}






/*-----------spi通信（软件模拟）------------*/
//pb14片选
//pa5
void spi_simulation_ini(void)
{
    //打开时钟（SPI1 PA PB）
    RCC->APB2ENR |=(1<<12);//使能spi1时钟
    RCC->AHB1ENR |=(1<<0);//使能gpioa时钟
    RCC->AHB1ENR |= (1<<1);   // GPIOB 时钟
    
    //pa5(sck，推挽输出) pa7(mosi，推挽输出)
    GPIOA->MODER &=~((0x03<<10) | (0x03<<14));//清零
    GPIOA->MODER |=((1<<10) | (1<<14));//功能模式为推挽输出
    GPIOA->OTYPER &=~(7<<5);//推挽 
    GPIOA->OSPEEDR &=~(0x3f<<10);
    GPIOA->OSPEEDR |=(0x2a<<10);//引脚速度 快速
    GPIOA->PUPDR &=~(0x3f<<10);//上拉下拉设置为无，和ODR、IDR有关   
    
    //pa6(miso，浮空输入)
    GPIOA->MODER &=~(0x03<<12);//清零，功能模式为输入
    GPIOA->OTYPER &=~(7<<5);//开漏
    GPIOA->OSPEEDR &=~(0x3f<<10);
    GPIOA->OSPEEDR |=(0x2a<<10);//引脚速度 快速
    GPIOA->PUPDR &=~(0x3f<<10);//上拉下拉设置为无，和ODR、IDR有关 
    
    //pb14(cs，通用输出）
    GPIOB->MODER &=~(3<<28);
    GPIOB->MODER |= (1<<28);//通用输出
    GPIOB->OTYPER &=~(1<<14);//推挽
    GPIOB->OSPEEDR &=~(3<<28);//低速，片选线不用高速
    GPIOB->PUPDR &=~(3<<28);//无上下拉
    SPI_CS = 1;   // 初始化完成后释放片选,输出电平为高，非选中状态    
}



/*
物理层:
同时收发（全双工）

代码层:
先发后收（因为要先触发时钟）

逻辑层:
发送和接收是同一个原子操作

关键点：不是"为什么要先发后收"，而是SPI机制决定了你必须通过发送来触发接收
*/
//从机W25Qxx需要SP0模式（99%都用sp0 sp3） 下降沿发送 上升沿接收。先发后收。
uint8_t SPI_simulation_Echo(uint8_t data)
{
	u8 i,rev_data=0;
	for(i=0;i<8;i++)
	{
		SPI_SCK=0;//下降沿   拉低SCK → 产生下降沿（数据输出边沿）  主机输出MOSI
        
		if(data & (0x80>>i))
		{
			SPI_MOSI=1;
		}
		else
		{
			SPI_MOSI=0;
		}
        
		SPI_SCK=1;//上升沿     拉高SCK → 产生上升沿（数据采样边沿）  主机采样MISO
		rev_data<<=1;         
		if(SPI_MISO)          
		{                     
			rev_data |=1;     
		}                     
        
	}
	return rev_data;    
}



/*
硬件实现下，数据帧格式设为8，是什么意思
你写的是 SPI1->CR1 &=~(1<<11)，即 DFF=0，8 位数据帧格式。意思是：每写入一次 SPI1->DR，硬件自动产生恰好 8 个 SCK 时钟，不多不少，发完就停、自动置 RXNE。


写 DR(8bit) → SCK 自动产生 8 个 → 停止 → RXNE=1（即SPI 状态寄存器（SR） bit 0 — 接收缓冲区非空。）
写 DR(8bit) → SCK 自动产生 8 个 → 停止 → RXNE=1
...
如果设 16 位（|= (1<<11)）：


写 DR(16bit) → SCK 产生 16 个 → 停止
为什么你的场景必须用 8 位
W25Qxx 的指令全部是单字节粒度——命令 1 字节、地址 3 字节、数据 N 字节。8 位帧格式收发一字节正合适：SPI_Echo(0x90) 写一个 8 位进去，刚好发一个字节的命令。

如果用 16 位帧，SPI_Echo(0x90) 写了一个 8 位值到 DR，但 SPI 硬件会等凑满 16 位才产生时钟——然后就卡死了。
------------------------------------
为什么接收一字节数据要先发送0xff?

因为 SPI 时钟只能由主机产生。


从机（W25Qxx）不能主动发数据，它要靠主机给的 SCK 时钟才输出 bit

主机干的事：
  0xFF = 0b11111111 → 8 个 SCK 时钟被产生出来 →
  MOSI 上全是 1（无意义）→ 从机不理
  但 MISO 上从机借这 8 个时钟把数据推出来了 →
  主机读 DR 拿到从机数据
0xFF 在这里就是"空字节"——给从机制造时钟节拍、借它的 MISO 收数据，自己不传任何有意义的东西。主机发什么不重要（发 0x00 也行），关键是发这个动作产生了 8 个 SCK。


硬件 SPI（你的 SPI_Echo）：
  SPI1->DR = 0xFF;       ← 写 DR 触发 8 个 SCK
  data = SPI1->DR;       ← 同时收从机发来的 8 位

软件模拟（你的 SPI_simulation_Echo）：
  for(i=0; i<8; i++) {
    SPI_SCK=0; SPI_SCK=1; ← 手动造 8 个 SCK
    ...读 MISO...
  }
软件版不需要显式发 0xFF，因为 SCK 是你手动翻转的，和数据无关。硬件版必须写一次 DR 才有 SCK，所以才用 0xFF 当"傀儡字节"。

---------------------------------------
为什么是0xff，而不是其它值?

0xFF 不是必须，0x00 也行。用 0xFF 是电气安全考虑。

SPI 从机有输出驱动，如果你的 MOSI 在发送空字节时是 0 → MOSI 低电平 → 而 MISO 也正在输出数据（可能是高），两条线一高一低相邻走，驱动电位相反，瞬间形成对地回路：


MOSI = 0 ────────────────
                    └── 两条线相邻，电平相反时存在短时对地电流
MISO = 1 ────────────────
电流极小（几十微安），在低功耗场景或对噪声敏感的项目里才是问题。发 0xFF（MOSI 全高）让 MOSI 和 MISO 在同一电平方向，避免这种情况。

对你现在的学习项目：0xFF 和 0x00 都一样能用。这个习惯是从工业代码里继承过来的，照做就行。

*/



//1.先发送高位 发送八位数据
void send_data(uint8_t data)
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        if(data & (0x80>>i))
        {
            //发送1;
        }
        else
        {
            //发送0;
        }
        //data<<=1;
    }
}
//2.先发送低位 发送八位数据
//void send_data(uint8_t data)
//{
//    uint8_t i;
//    for(i=0;i<8;i++)
//    {
//        if(data & 0x01)
//        {
//            //发送1;
//        }
//        else
//        {
//            //发送0;
//        }
//        data>>=1;
//    }
//}

//3.先接收高位 接收八位数据
//uint8_t rev_data(void)
//{
//    uint8_t data=0,i;
//    for(i=0;i<8;i++)
//    {
//        data<<=1;
//        if()//判断数据线数据为1
//        {
//            data |=1;
//        }
//    }
//    return data;
//}

//4.先接收低位 接收八位数据
//uint8_t rev_data(void)
//{
//    uint8_t data=0,i;
//    for(i=0;i<8;i++)
//    {
//        data>>=1;
//        if(//判断数据线数据为1)
//        {
//            data |=0x80;
//        }
//    }
//    return data;
//}







