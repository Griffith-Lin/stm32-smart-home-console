#include "main.h"

int fputc(int c,FILE *stream)
    
{
	while((USART1->SR & (1<<7))==0);//当发送数据缓冲区不为空时，循环发送,直到发送完毕 
	USART1->DR=c;
	return c;
}

int main(void)
{
    NVIC_SetPriorityGrouping(3);//配置优先级组 3位占先位数 1位次级位数
    
    TIM6_Delay_Init(84, 1000);
    led_ini();
    Usart1_Config(115200);
    key_ini();
    Exti_key_ini();
    

    while(1)
    {
        printf("111\r\n");
        GPIO_SetBits(GPIOD,GPIO_Pin_12);
        TIM6_delay(1000);
        GPIO_ResetBits(GPIOD,GPIO_Pin_12);
        TIM6_delay(1000);
    }   
    
}

