#include "usart1.h"

//pa9 10
void Usart1_Config(uint32_t brr)
{
    RCC->AHB1ENR |= (1<<0);//使能gpioa的时钟
    RCC->APB2ENR |= (1<<4);//使能usart1的时钟
    
    GPIOA->MODER &=~(0xf<<18);//清零
    GPIOA->MODER |=(0xa<<18);//功能模式为复用
    GPIOA->OTYPER &=~(3<<9);//推挽
    
    GPIOA->OSPEEDR |= (1<<18);//引脚速度，默认为0（低速）
    GPIOA->PUPDR &=~(0xf<<18);//上拉下拉设置为无，和ODR、IDR有关
    
    GPIOA->AFR[1] |= (0x77<<4); //配置复用功能映射（就是在数据手册上的复用功能表上，找你要的功能对应的AF是几）,AFR 寄存器就是"引脚功能选择器"，决定每个引脚连接哪个外设。
    //AFR[0]是复用功能低位寄存器，AFR[1]是复用功能高位寄存器
    
    uint32_t numerator = 84000000;
    uint32_t div_m = numerator / (16 * brr);
    uint32_t remainder = numerator % (16 * brr);//提取余数
    uint32_t div_f = (remainder * 16 + (16 * brr) / 2) / (16 * brr);//四舍五入技巧：在纯整数除法中，没有round(）函数。要实现的四舍五入，标准做法是在分子上加上除数的一半，即x+(y/2) / y
    USART1->BRR = (div_m << 4) | div_f;

    
    USART1->CR1 |=(3<<2);//发送器 接收器使能
	USART1->CR1 &=~(1<<10);//禁止奇偶校验
	USART1->CR1 &=~(1<<12);//字长 8位
	USART1->CR1 &=~(1<<15);//16倍过采样
	USART1->CR2 &=~(3<<12);//1位停止位
	USART1->CR1 |=(1<<13);//使能USART1
    
//    NVIC_SetPriority(USART1_IRQn, 6);//111 0
//    NVIC_EnableIRQ(USART1_IRQn);
    
//    USART1->CR1 |= (1<<5);   // RXNEIE：接收中断使能,当 USART_SR 寄存器中 ORE=1 或 RXNE=1 时，生成 USART 中断
//                             //即串口接收到1字节数据触发中断
//                             
//    USART1->CR1 |= (1<<4);   // IDLEIE：空闲中断使能    
}


//单字节发送
void usart1_send_byte(uint8_t byte)
{
    while(! (USART1->SR & (1<<7)))//这一位叫 TXE（Transmit data register empty，发送数据寄存器空。空时txe值为1，跳出阻塞
    {
        //这里的 while 起到的正是典型的阻塞（Blocking） 作用，这种机制在嵌入式中称为阻塞式轮询（Polling）       
    }
    USART1->DR=byte;//将该字节送入TDR（发送数据寄存器）
}

//单字节接收
uint8_t usart1_rev_byte(void)
{
    uint8_t byte;
    while(!(USART1->SR & (1<<5)))//接收数据寄存器（DR）中已经有完整、有效的新数据，CPU 现在可以安全地去读取它了。
    {
        //这里的 while 起到的正是典型的阻塞（Blocking） 作用，这种机制在嵌入式中称为阻塞式轮询（Polling）     
    }
    byte=USART1->DR;//位5自动归0
    return byte;
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



