#include "usart1.h"

////pa9 10
//void Usart1_Config(uint32_t brr)
//{
//    RCC->AHB1ENR |= (1<<0);//使能gpioa的时钟
//    RCC->APB2ENR |= (1<<4);//使能usart1的时钟
//    
//    GPIOA->MODER &=~(0xf<<18);//清零
//    GPIOA->MODER |=(0xa<<18);//功能模式为复用
//    GPIOA->OTYPER &=~(3<<9);//推挽
//    
//    GPIOA->OSPEEDR |= (1<<18);//引脚速度，默认为0（低速）
//    GPIOA->PUPDR &=~(0xf<<18);//上拉下拉设置为无，和ODR、IDR有关
//    
//    GPIOA->AFR[1] |= (0x77<<4); //配置复用功能映射（就是在数据手册上的复用功能表上，找你要的功能对应的AF是几）,AFR 寄存器就是"引脚功能选择器"，决定每个引脚连接哪个外设。
//    //AFR[0]是复用功能低位寄存器，AFR[1]是复用功能高位寄存器
//    
//    uint32_t numerator = 84000000;
//    uint32_t div_m = numerator / (16 * brr);
//    uint32_t remainder = numerator % (16 * brr);//提取余数
//    uint32_t div_f = (remainder * 16 + (16 * brr) / 2) / (16 * brr);//四舍五入技巧：在纯整数除法中，没有round(）函数。要实现的四舍五入，标准做法是在分子上加上除数的一半，即x+(y/2) / y
//    USART1->BRR = (div_m << 4) | div_f;

//    
//    USART1->CR1 |=(3<<2);//发送器 接收器使能
//	USART1->CR1 &=~(1<<10);//禁止奇偶校验
//	USART1->CR1 &=~(1<<12);//字长 8位
//	USART1->CR1 &=~(1<<15);//16倍过采样
//	USART1->CR2 &=~(3<<12);//1位停止位
//	USART1->CR1 |=(1<<13);//使能USART1
//    
////    NVIC_SetPriority(USART1_IRQn, 6);//111 0
////    NVIC_EnableIRQ(USART1_IRQn);
//    
////    USART1->CR1 |= (1<<5);   // RXNEIE：接收中断使能,当 USART_SR 寄存器中 ORE=1 或 RXNE=1 时，生成 USART 中断
////                             //即串口接收到1字节数据触发中断
////                             
////    USART1->CR1 |= (1<<4);   // IDLEIE：空闲中断使能    
//}




void Usart1_Config(uint32_t brr)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AF;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Speed=GPIO_Low_Speed;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10;
    

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_Init(GPIOA,&gpio_InitTypeDef);
    
    
    USART_InitTypeDef usart_InitTypeDef;
    
    usart_InitTypeDef.USART_BaudRate = brr;            // 库函数内部自动计算 div_m 和 div_f
    usart_InitTypeDef.USART_WordLength = USART_WordLength_8b; // CR1 &= ~(1<<12) -> 8位
    usart_InitTypeDef.USART_StopBits = USART_StopBits_1;      // CR2 &= ~(3<<12) -> 1位停止位
    usart_InitTypeDef.USART_Parity = USART_Parity_No;         // CR1 &= ~(1<<10) -> 无校验
    usart_InitTypeDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无流控
    usart_InitTypeDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // CR1 |= (3<<2) -> 收发使能

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级
    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//接收中断使能
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//空闲中断使能

    // 初始化 USART1
    USART_Init(USART1, &usart_InitTypeDef);
    
    //使能
    USART_Cmd(USART1, ENABLE);
    
}

////单字节发送
//void usart1_send_byte(uint8_t byte)
//{
//    while(! (USART1->SR & (1<<7)))
//    {
//        //这里的 while 起到的正是典型的阻塞（Blocking） 作用，这种机制在嵌入式中称为阻塞式轮询（Polling）
//        //位 7 (TXE) 为 1，表示的是 “发送数据寄存器 (TDR) 为空”，可以接收 CPU 写入的新数据。而不是“数据已到达移位寄存器”（虽然 TXE 置 1 的前提确实是上一个数据已经转移到了移位寄存器，但 TXE 的直接含义是 TDR 空）。
//    }
//    USART1->DR=byte;//TDR（发送数据寄存器）
//}

void usart1_send_byte(uint8_t byte)
{
    while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));//等待接收标志位

    USART_SendData(USART1,byte);
}


////单字节接收
//uint8_t usart1_rev_byte(void)
//{
//    uint8_t byte;
//    while(!(USART1->SR & (1<<5)))//接收数据寄存器（DR）中已经有完整、有效的新数据，CPU 现在可以安全地去读取它了。
//    {
//        //这里的 while 起到的正是典型的阻塞（Blocking） 作用，这种机制在嵌入式中称为阻塞式轮询（Polling）     
//    }
//    byte=USART1->DR;//位5自动归0,RDR（接收数据寄存器）
//    return byte;
//}

uint8_t usart1_rev_byte(void)
{
    uint8_t byte;
    while(!USART_GetFlagStatus(USART1,USART_FLAG_RXNE));
    byte=USART_ReceiveData(USART1);
    return byte;
}


volatile uint8_t usart_flag=0;
volatile uint8_t str_buf[100];//用来接收串口数据

void USART1_IRQHandler(void) 
{
    static uint16_t i=0;
    if(USART_GetITStatus(USART1,USART_IT_RXNE))//接收中断
    {
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除标志位
        str_buf[i++]=USART_ReceiveData(USART1);
    }
    
    
    if(USART_GetITStatus(USART1,USART_IT_IDLE))//空闲中断
    {
        volatile uint32_t temp;
        temp=USART1->SR;
        temp=USART1->DR;
        (void)temp;
        
         str_buf[i]='\0';
        i=0;
        usart_flag=1;
    }
}





//串口从上位机接收不规范字符串(....\r\n)
//调用者传的 buffer 可能是 uint8_t buff[50]，但函数不知道，发 200 字节就溢出了,最好再接收一个长度max_len
void usart1_rev_string(uint8_t * buffer)
{
    while(1)
    {
        *buffer = usart1_rev_byte();
        
        if(*buffer == '\n')
        {
            break;
        }
        
        buffer++;
    }
    
    *(buffer+1)='\0';
}

//串口1发送字符串
void usart1_send_string(volatile uint8_t *str)//刚刚没有写volatile修饰 uint8_t *str，编译器警告了。传参时 volatile uint8_t * → uint8_t *，编译器认为你丢掉了 volatile 修饰，这是在提醒你："这个变量我本来每次都要从内存读的，传进去之后函数可能把它优化成寄存器缓存，你不怕吗？"
{
    while(*str!='\0')
	{
		usart1_send_byte(*str);
		str++;
	}
    usart1_send_byte('\0');//再发送一个'\0'规范发送出的字符串
}


//串口1接收规范字符串(....\0)
void usart1_rev_string_norm(uint8_t * buffer)
{
    while(1)
    {
        *buffer = usart1_rev_byte();
        
        if(*buffer == '\0')
        {
            break;
        }
        
        buffer++;
    }
    
    
}

//验证接收的字符串
void rev_process(const volatile char* buff)
{
    if(strcmp((const char *)buff,"led1_on")== 0)
    {
        
    }
    if(strcmp((const char *)buff,"led1_off")== 0)
    {
        
    }
}



