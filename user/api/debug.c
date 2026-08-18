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

uint16_t adc_arr[2];

void adc_debug_auto(void)
{
    for(int i=0;i<2;i++)
    {
        while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);//转换完成退出阻塞
        adc_arr[i]=ADC_GetConversionValue(ADC1);
    }

    printf("光照强度：%.2f%%\r\n",(4095-adc_arr[1])/(float)4095 *100);//读了DR后会清EOC
    printf("火焰指数：%d\r\n",adc_arr[1]);
    TIM6_delay(500);

}

void W25Qxx_test(void)
{
    W25Qxx_write_enable();
    W25Qxx_Read_ID();
    W25Qxx_Read_SR1_busy();

    TIM6_delay(500);
}




volatile uint8_t mlx90614_buf[2]={0};
volatile uint16_t mlx90614_read_flag=0;

void mlx90614_test(void)
{
    
    mlx90614_i2c_read(0x07,mlx90614_buf);//读标志位
    
    mlx90614_read_flag=(mlx90614_buf[1]<<8) | mlx90614_buf[0];//由高8位（DataH）和低8位（DataL）两部分组成，
//    printf("%d\r\n",mlx90614_read_flag);
    
    
    float result=temperature_calculate(mlx90614_buf);
    printf("%.2f\r\n",result);
    
    Delay_Ms(1000);    
}


void rtc_test(void)
{
//      RTC_Show_Time();
//		Delay_Ms(1000);
        
        
        //是编译期宏，格式固定为 "Aug 11 2026"（月 日 年）
//		printf("%s\r\n",__DATE__);
        const char *week[] = {"日", "一", "二", "三", "四", "五", "六"};
        printf("编译日期: %s 星期%d\r\n", __DATE__, compile_date.weekday);
		printf("%s\r\n",__TIME__);
		Delay_Ms(1000);
}

void ws2812e_test(void)
{
    
        ws2812e_open_reset(green,red,blue,4);
        green+=50;
        red+=50;
        blue+=50;
        Delay_Ms(1000);
}


void sht31_test(void)
{
    uint32_t data = 0;

    data = sht30_i2c_send(0x2c06); // 有时钟拉伸的高重复精度测量模式
    //        data = sht30_i2c_send(0x2400);//无时钟拉伸的高重复精度测量模式
    get_tem_hu(data);

    printf("0x%x\r\n", data);
    printf("%.2f\r\n", tem_data);
    printf("%.2f\r\n", hu_data);
    printf("crc=0x%04x\r\n", crc_data); // 高8位=温度CRC,低8位=湿度CRC
}

