#include "esp-12f.h"



//用usart2来接收esp返回的信息
//usart2 apb1   u3_tx pa2   u3_rx  pa3
void usart2_ini(uint32_t brr)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AF;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Speed=GPIO_Low_Speed;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_2 | GPIO_Pin_3;
    

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    GPIO_Init(GPIOA,&gpio_InitTypeDef);
    
    
    USART_InitTypeDef usart_InitTypeDef;
    
    usart_InitTypeDef.USART_BaudRate = brr;            // 库函数内部自动计算 div_m 和 div_f
    usart_InitTypeDef.USART_WordLength = USART_WordLength_8b; 
    usart_InitTypeDef.USART_StopBits = USART_StopBits_1;      //1位停止位
    usart_InitTypeDef.USART_Parity = USART_Parity_No;         //无校验
    usart_InitTypeDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无流控
    usart_InitTypeDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发中断使能

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级
    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//接收中断使能
    USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//空闲中断使能

    // 初始化 USART2
    USART_Init(USART2, &usart_InitTypeDef);
    
    //使能
    USART_Cmd(USART2, ENABLE);
}



void usart2_send_byte(uint8_t byte)
{
    while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE));//等待接收标志位

    USART_SendData(USART2,byte);
}



uint8_t usart2_rev_byte(void)
{
    uint8_t byte;
    while(!USART_GetFlagStatus(USART2,USART_FLAG_RXNE));
    byte=USART_ReceiveData(USART2);
    return byte;
}

//串口2发送字符串
void usart2_send_string(volatile uint8_t *str)//刚刚没有写volatile修饰 uint8_t *str，编译器警告了。传参时 volatile uint8_t * → uint8_t *，编译器认为你丢掉了 volatile 修饰，这是在提醒你："这个变量我本来每次都要从内存读的，传进去之后函数可能把它优化成寄存器缓存，你不怕吗？"
{
    while(*str!='\0')
	{
		usart2_send_byte(*str);
		str++;
	}
    
}


//串口2接收规范字符串(....\0)
void usart2_rev_string_norm(uint8_t * buffer)
{
    while(1)
    {
        *buffer = usart2_rev_byte();
        
        if(*buffer == '\0')
        {
            break;
        }
        
        buffer++;
    }
    
    
}

uint8_t idle_flag=0;
//发送后，接收返回码，判断返回码
uint8_t wifi_send_command(char *cmd,char *rev,uint32_t timeout)
{
     idle_flag = 0;                        // 关键：清掉上一帧残留标志
    usart2_send_string((uint8_t*)cmd);    // 只发一次

    uint32_t count = timeout;
    while(!idle_flag && count > 0)        // 带超时的等待
    {
        Delay_Ms(1);
        count--;
    }
    if(count == 0) return OUT;              // 超时：模块没回

    if(strstr((const char *)str2_buf, rev) != NULL)
     return OK;                         // 应答匹配
    
    return ERROR;                             // 有应答但不匹配（比如 ERROR）
}


void esp_12f_ini(void)
{
    //usart2_send_string("+++");//退出透传
    
    wifi_send_command("AT\r\n","OK",500);                                // 先探活，同时验证波特率对不对
    
    wifi_send_command("AT+CWQAP\r\n","OK",500);                          // 断开旧连接
    
    wifi_send_command("AT+CWMODE=1\r\n","OK",500);                       // Station 模式
    
    wifi_send_command("AT+CWJAP=\"DESKTOP-HTLNPUV 4127\",\"88888888\"\r\n","OK",5000); // 连 WiFi

    wifi_send_command("AT+CIPSTART=\"TCP\",\"192.168.11.140\",8086\r\n","OK",5000);

    wifi_send_command("AT+CIPMODE=1\r\n","OK",500);                      // 透传模式（必须先于 CIPSEND）
   
    wifi_send_command("AT+CIPSEND\r\n","OK",500);                        // 进入透传，之后发的都是数据 Delay_Ms(1000);
    
    
    

    
    usart2_send_string((uint8_t*)"666666\r\n"); 
    usart2_send_string((uint8_t*)"666666\r\n");
    usart2_send_string((uint8_t*)"666666\r\n");
    usart2_send_string((uint8_t*)"666666\r\n");
}


volatile uint8_t esp_analysis_flag=0;
volatile uint8_t usart2_flag=0;
volatile uint8_t str2_buf[500]={0};//用来接收串口数据

void USART2_IRQHandler(void) 
{
    static uint16_t i=0;
    if(USART_GetITStatus(USART2,USART_IT_RXNE))//接收中断
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清除标志位
        str2_buf[i++]=USART_ReceiveData(USART2);
        
    }
    
    //注意是RX引脚，在 RX 引脚上先收到过数据，然后线路保持高电平（空闲电平）持续一个完整字符帧的时间（1 起始位 + 8 数据位 + 1 停止位 = 10 个位时间）。即可进入空闲中断
    if(USART_GetITStatus(USART2,USART_IT_IDLE))//空闲中断
    {
        volatile uint32_t temp;
        temp=USART2->SR;
        temp=USART2->DR;
        (void)temp;
        str2_buf[i] = '\0';     // 补上，strstr 才能正确工作

        esp_analysis_flag=1;
        
        i=0;
        

    }
}



//用返回的信息做判断
void esp_analysis(volatile uint8_t *buf)
{
    if(esp_analysis_flag==0)
        return;
        
//    if(str2_buf[0]==1)
    
    Wav_PlayRevert((uint8_t*)"0:prompt/嗨我在呢.wav");
    esp_analysis_flag=0;
}


