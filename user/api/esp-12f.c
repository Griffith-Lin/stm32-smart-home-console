#include "esp-12f.h"

volatile uint8_t esp_analysis_flag=0;
volatile uint8_t usart2_flag=0;
volatile uint8_t str2_buf[500]={0};//用来接收串口数据

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
                            
	uint16_t cnt=0;
	usart2_send_string((uint8_t*)cmd);
	Delay_Ms(100);
	while(1)
	{
		while(!idle_flag)//未接收完成
		{
			cnt++;
			Delay_Ms(1);
			if(cnt>timeout)
			{
				return OUT;
			}
		}
		idle_flag=0;
		if(strstr((const char *)str2_buf,(const char *)rev)!=NULL)
		{
			return OK;//接收到正确码
		}
        if(strstr((const char *)str2_buf,(const char *)"ERROR")!=NULL)
		{
			return ERROR;//接收到错误码
		}
	}
}

uint8_t check_cnt=0;

void check(uint8_t back)
{
    if(back==0)
    {
        printf("%d OK\r\n",check_cnt++);
    }
    else if(back==1)
    {    
        printf("%d OUT\r\n",check_cnt++);
    }
    else if(back==2)
    {    
        printf("%d ERROR\r\n",check_cnt++);
    }
}


void esp_12f_ini(void)
{
//    usart2_send_string((uint8_t*)"+++");//退出透传,退出透传要求 +++ 前后有静默，
//    Delay_Ms(1000);
//    //+++ 不带 CRLF，不是一条完整的 AT 命令。模块（普通 AT 模式下）的解析器收到 +++ 后，只会把它当作"未完成的行"暂存在行缓冲里，一直等结束符。
//    //然后无论隔多久，你下一条命令到达时，会和它拼成一行：
//    check(wifi_send_command("AT\r\n","OK",500)); //这里因为前面的+++脏数据，变成了+++AT\r\n,模块识别错误，返回ERROR   
    /*
    不发 +++：把模块的 RST/CH_PD 引脚接到 STM32 的一个 GPIO，初始化时先硬件复位模块——模块必然从普通 AT 模式启动，永远不需要 +++。
    这是最干净的工程做法，也彻底解决了"模块残留透传状态"这类脏状态问题。
    */
    
    
    //0
    check(wifi_send_command("AT\r\n","OK",500));                                // 先探活，同时验证波特率对不对
    
    //1
    check(wifi_send_command("AT+CWQAP\r\n","OK",500));                          // 断开旧连接
    
    //2
    check(wifi_send_command("AT+CWMODE=1\r\n","OK",500));                       // Station 模式
    
    //3
    check(wifi_send_command("AT+CWJAP=\"DESKTOP-HTLNPUV 4127\",\"88888888\"\r\n","OK",5000)); // 连 WiFi

//    测试用
//    wifi_send_command("AT+CIPSTART=\"TCP\",\"192.168.11.140\",8086\r\n","OK",5000);

    //4
    check(wifi_send_command("AT+CIPMODE=1\r\n","OK",500));                      // 透传模式（必须先于 CIPSEND）
      
//    后面有mqtt指令，要先注释
//    check(wifi_send_command("AT+CIPSEND\r\n","OK",500));                        // 进入透传，之后发的都是数据 Delay_Ms(1000);
    
    //这三条只有第一次设置会返回ok
    //5 6 7
    check(wifi_send_command("AT+MQTTUSERCFG=0,1,\"9203454aeb1e4a4f94fd9423dac71221\",\"dwy5lftp43w54p7e\",\"nE2s7UByXC\",0,0,\"\"\r\n","OK",10000));     //设置三元组
    check(wifi_send_command("AT+MQTTCONN=0,\"sh-3-mqtt.iot-api.com\",1883,1\r\n","OK",10000)); //连接broker主机
    check(wifi_send_command("AT+MQTTSUB=0,\"attributes/push\",0\r\n","OK",5000));   //订阅主题
    
    
   

}




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

        idle_flag=1;
        
        
        
        i=0;
        

    }
}



//用返回的信息做判断
void esp_analysis(void)
{
    if(idle_flag==0)
        return;
        
    idle_flag=0;
    printf("str2_buf=%s\r\n",str2_buf);
    
    if(strstr((const char *)str2_buf,"{\"led\":0}"))
    {
        printf("LED1_OFF\r\n");
        LED1_OFF;
    }
    if(strstr((const char *)str2_buf,"{\"led\":1}"))
    {
        printf("LED1_ON\r\n");
        LED1_ON;
    }
}


