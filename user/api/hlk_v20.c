#include "hlk_v20.h"

//pd8 u3_tx   pd9 u3_rx
void Usart3_hlk_ini(uint32_t brr)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AF;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Speed=GPIO_Low_Speed;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_8 | GPIO_Pin_9;
    

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
    GPIO_Init(GPIOD,&gpio_InitTypeDef);
    
    
    USART_InitTypeDef usart_InitTypeDef;
    
    usart_InitTypeDef.USART_BaudRate = brr;            // 库函数内部自动计算 div_m 和 div_f
    usart_InitTypeDef.USART_WordLength = USART_WordLength_8b; // CR1 &= ~(1<<12) -> 8位
    usart_InitTypeDef.USART_StopBits = USART_StopBits_1;      // CR2 &= ~(3<<12) -> 1位停止位
    usart_InitTypeDef.USART_Parity = USART_Parity_No;         // CR1 &= ~(1<<10) -> 无校验
    usart_InitTypeDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无流控
    usart_InitTypeDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // CR1 |= (3<<2) -> 收发使能

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级
    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//接收中断使能
    USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);//空闲中断使能

    // 初始化 USART3
    USART_Init(USART3, &usart_InitTypeDef);
    
    //使能
    USART_Cmd(USART3, ENABLE);
}

void usart3_send_byte(uint8_t byte)
{
    while(!USART_GetFlagStatus(USART3,USART_FLAG_TXE));//等待接收标志位

    USART_SendData(USART3,byte);
}



uint8_t usart3_rev_byte(void)
{
    uint8_t byte;
    while(!USART_GetFlagStatus(USART3,USART_FLAG_RXNE));
    byte=USART_ReceiveData(USART3);
    return byte;
}


volatile uint8_t usart3_flag=0;
volatile uint8_t str3_buf[100];//用来接收串口数据

void USART3_IRQHandler(void) 
{
    static uint16_t i=0;
    if(USART_GetITStatus(USART3,USART_IT_RXNE))//接收中断
    {
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清除标志位
        str3_buf[i]=USART_ReceiveData(USART3);
        //命令词小科小科，接收串口返回数据00 35 0a
        printf("%0x ",str3_buf[i]);
        i++;
    }
    
    
    if(USART_GetITStatus(USART3,USART_IT_IDLE))//空闲中断
    {
        volatile uint32_t temp;
        temp=USART3->SR;
        temp=USART3->DR;
        (void)temp;
        
         str3_buf[i]='\0';
        i=0;
        usart3_flag=1;
    }
}





//串口从上位机接收不规范字符串(....\r\n)
//调用者传的 buffer 可能是 uint8_t buff[50]，但函数不知道，发 200 字节就溢出了,最好再接收一个长度max_len
void usart3_rev_string(uint8_t * buffer)
{
    while(1)
    {
        *buffer = usart3_rev_byte();
        
        if(*buffer == '\n')
        {
            break;
        }
        
        buffer++;
    }
    
    *(buffer+1)='\0';
}

//串口3发送字符串
void usart3_send_string(volatile uint8_t *str)//刚刚没有写volatile修饰 uint8_t *str，编译器警告了。传参时 volatile uint8_t * → uint8_t *，编译器认为你丢掉了 volatile 修饰，这是在提醒你："这个变量我本来每次都要从内存读的，传进去之后函数可能把它优化成寄存器缓存，你不怕吗？"
{
    while(*str!='\0')
	{
		usart3_send_byte(*str);
		str++;
	}
    usart3_send_byte('\0');//再发送一个'\0'规范发送出的字符串
}


//串口3接收规范字符串(....\0)
void usart3_rev_string_norm(uint8_t * buffer)
{
    while(1)
    {
        *buffer = usart3_rev_byte();
        
        if(*buffer == '\0')
        {
            break;
        }
        
        buffer++;
    }
    
    
}
