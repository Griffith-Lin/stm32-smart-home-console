#include "dma.h"



/*

每个片上外设用哪个DMA的哪个数据流，都是规定好的，详见DMA->通道选择




存储器M0AR与M1AR

双缓冲模式（DBM=1，CR第18位） 下才同时用到两个存储器：


单缓冲（DBM=0）：    M0AR → 当前数据源
                     M1AR → 闲置，不起作用

双缓冲（DBM=1）：    M0AR → 当前正在搬的数据源
                     M1AR → 备用缓冲区地址


双缓冲全流程：
当前目标 = M0AR，DMA 使用 M0AR 指向的 buffer 传输
NDTR 减到 0
产生 Transfer Complete 标志
硬件切换 CT：当前目标从 M0AR 变成 M1AR
NDTR 自动重载为之前设定的数据个数
DMA 开始使用 M1AR 指向的 buffer 继续传输
此时 M0AR 指向的 buffer 变成备用 buffer
CPU 可以处理/填充 M0AR 指向的 buffer
等 M1AR 的 NDTR 也减到 0
当前目标再切回 M0AR
如此往复


传输完成时硬件自动切换 CT位，让 DMA 从当前缓冲区切到另一个缓冲区。



单缓冲模式下：
有效参与传输的通常只有 M0AR 指向的 buffer。
DMA 正在搬运这块 buffer 时，CPU 不能安全地同时读写同一块 buffer，
否则可能出现数据不一致。

双缓冲模式下：
M0AR 和 M1AR 分别指向两个不同的 buffer。
DMA 当前搬运其中一个 buffer，另一个 buffer 作为备用。
CPU 可以操作备用 buffer，因此可以实现 DMA 传输和 CPU 处理并行。


M0AR / M1AR 本身只是地址指针，
真正被“边搬边写”或“边搬边读”的是它们指向的两块 RAM。


双缓冲模式的本质，就是让 DMA 和 CPU 分别操作不同的 buffer。

*/

void dma2_stream7_ini(uint8_t ndtr,uint32_t m0ar)
{
    //    代码流程设计:USART1_TX
    USART1->CR3 |=(1<<7);//打开USART的DMA使能
    RCC->AHB1ENR |=(1<<22);//打开DMA2时钟
    
    DMA2_Stream7->CR =0;
    DMA2_Stream7->NDTR =0;
    DMA2_Stream7->PAR =0;
    DMA2_Stream7->M0AR =0;
    DMA2_Stream7->FCR=0;
    
    //数据流7 通道4
    DMA2_Stream7->CR &=~(1<<0);//禁止使能DMA2的数据流7
    DMA2_Stream7->CR |=(1<<6);//数据传输方向（存储器到外设）
    DMA2_Stream7->CR &=~(1<<8);//循环模式—禁止(只搬运一次,DMA 传完 10 字节后自动停，要再发需要重新设 NDTR 并重新使能 CR)
    DMA2_Stream7->CR &=~(1<<9);//外设地址是否递增 否 
    DMA2_Stream7->CR |=(1<<10);//存储器地址是否递增 是
    DMA2_Stream7->CR &=~(3<<11);//外设数据宽度 1字节
    DMA2_Stream7->CR &=~(3<<13);//存储器数据宽度 1字节
    DMA2_Stream7->CR |=(3<<16);//数据流优先级 非常高
    DMA2_Stream7->CR |=(4<<25);//通道选择 4
    
    DMA2_Stream7->NDTR |=ndtr;//数据项数目
    DMA2_Stream7->PAR =(uint32_t)&(USART1->DR);//外设地址，发送数据寄存器(TDR),当你写 USART1->DR 时，
    //硬件把数据塞进隐藏的 TDR，然后 TDR 里的数据会自动加载到发送移位寄存器，移位寄存器立刻开始按设定的波特率往外挪 bit（起始位 → 8 数据位 → 停止位）。    
    DMA2_Stream7->M0AR =m0ar;//存的是存储器的地址 
    DMA2_Stream7->FCR &=~(1<<2);//直接模式
    
    DMA2_Stream7->CR |=(1<<0);//使能DMA2数据流7
    
   
    
}


void dma2_stream0_ini(uint8_t ndtr,uint32_t m0ar)
{
    ADC1->CR2 |=(1<<8);//使能dma模式
    RCC->AHB1ENR |=(1<<22);//打开DMA2时钟
    
    DMA2_Stream0->CR =0;
    DMA2_Stream0->NDTR =0;
    DMA2_Stream0->PAR =0;
    DMA2_Stream0->M0AR =0;
    DMA2_Stream0->FCR=0;    
    
    //数据流0 通道0
    DMA2_Stream0->CR &=~(1<<0);//禁止使能DMA2的数据流0
    DMA2_Stream0->CR &=~(3<<6);//数据传输方向（外设到存储器）
    DMA2_Stream0->CR |=(1<<8);//循环模式—开启
    DMA2_Stream0->CR &=~(1<<9);//外设地址是否递增 否 
    DMA2_Stream0->CR |=(1<<10);//存储器地址是否递增 是
    
    DMA2_Stream0->CR &=~(3<<11);
    DMA2_Stream0->CR |=(1<<11);//外设数据宽度 2字节
    DMA2_Stream0->CR &=~(3<<13);
    DMA2_Stream0->CR |=(1<<13);//存储器数据宽度 2字节
    DMA2_Stream0->CR |=(3<<16);//数据流优先级 非常高
    DMA2_Stream0->CR &=~(7<<25);//通道选择 0   
    DMA2_Stream0->CR &=~(1<<18);//关闭双重缓冲区模式    
    DMA2_Stream0->NDTR |=ndtr;//数据项数目
    DMA2_Stream0->PAR =(uint32_t)&(ADC1->DR);//外设地址    
    DMA2_Stream0->M0AR =m0ar;//存储器地址    
    DMA2_Stream0->FCR &=~(1<<2);//直接模式
    
    DMA2_Stream0->CR |=(1<<0);//使能DMA2数据流0
}


void dma2_stream0_dbbuf_ini(uint8_t ndtr,uint32_t m0ar,uint32_t m1ar)
{
    ADC1->CR2 |=(1<<8);//使能dma模式
    RCC->AHB1ENR |=(1<<22);//打开DMA2时钟
    
    DMA2_Stream0->CR =0;
    DMA2_Stream0->NDTR =0;
    DMA2_Stream0->PAR =0;
    DMA2_Stream0->M0AR =0;
    DMA2_Stream0->FCR=0;    
    
    //数据流0 通道0
    DMA2_Stream0->CR &=~(1<<0);//禁止使能DMA2的数据流0
    DMA2_Stream0->CR &=~(3<<6);//数据传输方向（外设到存储器）
    DMA2_Stream0->CR |=(1<<8);//循环模式—开启
    DMA2_Stream0->CR &=~(1<<9);//外设地址是否递增 否 
    DMA2_Stream0->CR |=(1<<10);//存储器地址是否递增 是
    
    DMA2_Stream0->CR &=~(3<<11);
    DMA2_Stream0->CR |=(1<<11);//外设数据宽度 2字节
    DMA2_Stream0->CR &=~(3<<13);
    DMA2_Stream0->CR |=(1<<13);//存储器数据宽度 2字节
    DMA2_Stream0->CR |=(3<<16);//数据流优先级 非常高
    DMA2_Stream0->CR &=~(7<<25);//通道选择 0
    
    DMA2_Stream0->CR |=(1<<18);//打开双重缓冲区模式
    
    DMA2_Stream0->NDTR |=ndtr;//数据项数目
    DMA2_Stream0->PAR =(uint32_t)&(ADC1->DR);//外设地址    
    DMA2_Stream0->M0AR =m0ar;//存储器地址
    DMA2_Stream0->M1AR =m1ar;   // 双重缓冲模式新增存储器地址    
    DMA2_Stream0->FCR &=~(1<<2);//直接模式
    
    DMA2_Stream0->CR |=(1<<0);//使能DMA2数据流0
}

void dma2_stream2_ini(uint8_t ndtr,uint32_t m0ar)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//使能DMA2时钟
    DMA_DeInit(DMA2_Stream2);                          //复位数据流2(相当于手动清零CR/NDTR/PAR/M0AR)
    
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);       //USART1->CR3 |= (1<<7) -> 接收DMA请求使能
    //注意:bit7(DMAR)是"接收"DMA使能;bit6(DMAT)才是发送——你的stream7函数里设的也是bit7
    
    DMA_InitTypeDef dma_InitTypeDef={0};
    
    dma_InitTypeDef.DMA_Channel            = DMA_Channel_4;                      //通道4:USART1_RX固定
    dma_InitTypeDef.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;              //外设地址:数据寄存器DR
    dma_InitTypeDef.DMA_Memory0BaseAddr    = m0ar;                               //存储地址:缓冲区首地址
    dma_InitTypeDef.DMA_DIR                = DMA_DIR_PeripheralToMemory;         //传输方向:外设→存储(接收)
    dma_InitTypeDef.DMA_BufferSize         = ndtr;                               //一次搬运的数据个数
    dma_InitTypeDef.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;          //外设地址不自增(始终是DR)
    dma_InitTypeDef.DMA_MemoryInc          = DMA_MemoryInc_Enable;               //存储地址自增(装满缓冲区)
    dma_InitTypeDef.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;        //外设数据宽度:1字节(USART8位数据)
    dma_InitTypeDef.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;            //存储数据宽度:1字节
    dma_InitTypeDef.DMA_Mode               = DMA_Mode_Normal;                    //正常模式:搬完ndtr个就停(需重新装载再使能)
    dma_InitTypeDef.DMA_Priority           = DMA_Priority_High;                  //优先级:高
    dma_InitTypeDef.DMA_FIFOMode           = DMA_FIFOMode_Disable;               //直连模式(相当于 FCR &=~(1<<2))
    dma_InitTypeDef.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull;
    dma_InitTypeDef.DMA_MemoryBurst        = DMA_MemoryBurst_Single;             //单次突发
    dma_InitTypeDef.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    
    DMA_Init(DMA2_Stream2,&dma_InitTypeDef);
    
    DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);//传输完成中断使能
    
    NVIC_InitTypeDef nvic_InitTypeDef={0};
    
    nvic_InitTypeDef.NVIC_IRQChannel                   = DMA2_Stream2_IRQn;
    nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_InitTypeDef.NVIC_IRQChannelSubPriority        = 0;
    nvic_InitTypeDef.NVIC_IRQChannelCmd                = ENABLE;
    
    NVIC_Init(&nvic_InitTypeDef);
    
    DMA_Cmd(DMA2_Stream2,ENABLE);//使能DMA2数据流2(相当于 CR |=(1<<0))
}

