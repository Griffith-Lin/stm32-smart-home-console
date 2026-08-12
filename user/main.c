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
    
   
    i2c_master_ini();
    

    ws2812e_ini(4);//灯是有缓存的，所以芯片复位时灯不会灭，要手动加上灯的复位逻辑
    //后面的 i2c_master_ini 会操作 GPIOB（PB6/PB7），虽然没有直接动 PB15，但同一组 GPIO 的寄存器读写可能产生微妙影响。加上前导复位丢失，噪声数据就一直在第一个灯里锁着。
    //复位时第一个灯闪绿灯，原因是硬件浮空，加下拉电阻解决
    //WS2812 的 DIN 脚内部有弱上拉（~100kΩ），会把线往上拽。但 PCB 走线本身是天线，会耦合周围的电磁噪声。结果就是 DIN 上的电压随机波动——可能刚好跨过 WS2812 的高低电平阈值，被当成数据吞进去。
    //多调用一次变色函数，解决复位时第一个灯常亮绿灯。原因是复位时有脏数据进入灯带
    
   
    RTC_Cal_Config();
    
    while(1)
    {         
        RTC_Show_Time();
        Delay_Ms(1000);

    }   
    
}

