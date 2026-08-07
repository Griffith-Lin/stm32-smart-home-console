#include "main.h"

int fputc(int c,FILE *stream)
    
{
	while((USART1->SR & (1<<7))==0);//当发送数据缓冲区不为空时，循环发送,直到发送完毕 
	USART1->DR=c;
	return c;
}

int main(void)
{
    
//    NVIC_PriorityGroupConfig(3);
    
    TIM6_Delay_Init(84, 1000);
    led_ini();
    Usart1_Config(115200);
    key_ini();
    Exti_key_ini();
    

    while(1)
    {
        
        char send='a';
        usart1_send_byte(send);
        char rec;
        rec=usart1_rev_byte();//收一个字节
        usart1_send_byte(rec);
        
        
        
        
        

        
        TIM6_delay(1000);
    }   
    
}

