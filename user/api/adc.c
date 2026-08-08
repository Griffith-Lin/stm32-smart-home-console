#include "adc.h"

void adc_ini(void)
{
    //adc通用配置
    ADC_CommonInitTypeDef adc_CommonInitTypeDef={0};
    
    adc_CommonInitTypeDef.ADC_Mode = ADC_Mode_Independent;          // 独立模式 (不用双ADC同步)
    adc_CommonInitTypeDef.ADC_Prescaler = ADC_Prescaler_Div4;       // ADC 时钟 = APB2(84MHz) / 4 = 21MHz (不能超过36MHz，根据数据手册adc特性)
    adc_CommonInitTypeDef.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; // 暂时不用 DMA
    adc_CommonInitTypeDef.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    //这句指的是在双重模式下，两个 ADC 的采样启动时间之间，插入 5 个 ADC 时钟周期的延迟，延迟越长，抗电源干扰能力越强，但整体吞吐率会略微下降。
    
    ADC_CommonInit(&adc_CommonInitTypeDef);
    
    
    //配置adc1参数
    ADC_InitTypeDef adc_InitTypeDef={0};
    
    adc_InitTypeDef.ADC_Resolution = ADC_Resolution_12b;// 12位分辨率 (0~4095)
    adc_InitTypeDef.ADC_ScanConvMode = DISABLE;// 关闭扫描模式 先一次只读一个通道
//    adc_InitTypeDef.ADC_ContinuousConvMode = DISABLE;// 关闭连续转换 (我们想要软件触发，读一次算一次)
    adc_InitTypeDef.ADC_ContinuousConvMode = ENABLE;//打开连续转   ！！！含义是：第一次触发后，自动持续转换！！！
    adc_InitTypeDef.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // 不用外部触发
    adc_InitTypeDef.ADC_DataAlign = ADC_DataAlign_Right;// 数据右对齐 (标准做法)
    adc_InitTypeDef.ADC_NbrOfConversion = 1;// 每次转换 1 个通道
    
    ADC_Init(ADC1, &adc_InitTypeDef);
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);
    
    ADC_Cmd(ADC1,ENABLE);
    
    ADC_SoftwareStartConv(ADC1);  //第一次触发，让它开始连续转换
}

