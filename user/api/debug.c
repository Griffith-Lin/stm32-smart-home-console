#include "debug.h"


uint8_t tmp=0;

uint8_t gl5506_flag=0;

void gl5506_debug_manual(void)
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


void gl5506_debug_auto(void)
{
    while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
    printf("%.2f%%\r\n",(4095-ADC_GetConversionValue(ADC1))/(float)4095 *100);
    TIM6_delay(500);
}
