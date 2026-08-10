#include "main.h"

int fputc(int c,FILE *stream)
    
{
	while((USART1->SR & (1<<7))==0);//当发送数据缓冲区不为空时，循环发送,直到发送完毕 
	USART1->DR=c;
	return c;
}

int main(void)
{
    //NVIC_SetPriorityGrouping(3);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//不配置默认为4位占先
    
    TIM6_Task_Init(84, 1000);   
    led_ini();
    Usart1_Config(115200);
    
    key_ini();
    Exti_key_ini();
    sg90_PWM(84,20000);  
    beep_ini();
    
    motor_pwm_ini(84,1000);
    mortor_minspeed_open();
    
    
    adc_ini();
    adc_GL5506_ini();//光敏电阻
    
    while(1)
    {   
        W25Qxx_test();
        
    }   
    
}

