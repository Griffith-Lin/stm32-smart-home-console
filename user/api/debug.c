#include "debug.h"


uint8_t tmp=0;

uint8_t gl5506_flag=0;

void adc_debug_manual(void)
{
        if(gl5506_flag==1)
    {
        gl5506_flag=0;    
        while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
        ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
        printf("%x\r\n",ADC_GetConversionValue(ADC1));
        TIM6_delay(500);
    }
}

/*
CH10 采样(480周期) → CH10 转换(12周期) → EOC=1, DR=CH10值
                                           ↓ 硬件自动
CH11 采样(480周期) → CH11 转换(12周期) → EOC=1, DR=CH11值(覆盖)
                                           ↓ 连续模式自动回到 rank1
CH10 采样 → ... 循环
*/

void adc_debug_auto(void)
{
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);//转换完成退出阻塞
    printf("光照强度：%.2f%%\r\n",(4095-ADC_GetConversionValue(ADC1))/(float)4095 *100);//读了DR后会清EOC
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
    printf("火焰指数：%d\r\n",ADC_GetConversionValue(ADC1));
    TIM6_delay(500);


}

