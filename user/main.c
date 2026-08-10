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
    
    spi_ini();
    
   
    ws2812e_ini(4);//灯是有缓存的，所以芯片复位时灯不会灭，要手动加上逻辑
    
    
    while(1)
    {   

        ws2812e_open_reset(green,red,blue,4);
        
    }   
    
}

