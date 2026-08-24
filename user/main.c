#include "main.h"

int fputc(int c, FILE *stream)

{
    while ((USART1->SR & (1 << 7)) == 0)
        ; // 当发送数据缓冲区不为空时，循环发送,直到发送完毕
    USART1->DR = c;
    return c;
}

/*    WS2812E闪绿灯排查
上电复位
  → 硬件启动（电压爬坡、振荡器起振）        ← PB15 浮空
  → Reset_Handler（汇编启动代码）            ← PB15 浮空
  → SystemInit()（配置 HSE/PLL，耗时数ms）   ← PB15 浮空
  → .data/.bss 初始化                        ← PB15 浮空
  → main() 第14行  ← 你的寄存器操作，PB15 才变 LOW
  → ...大量初始化（TIM6/LED/USART/...）...
  → ws2812e_ini()  ← 发全零数据，灯熄灭

问题出在浮空区间——在 main() 执行之前，PB15 已经浮空了很长时间（SystemInit 配置 PLL 就要好几毫秒）。这段时间足够 WS2812E 锁存到噪声数据，绿灯就亮了。

在 main() 里写的拉低pb15操作只能缩短闪绿灯的窗口，但覆盖不到 SystemInit 和启动代码。

我在SystemInit()就下拉,至此软件能做的已经做完了，再不行只能硬件加下拉电阻

关键问题是 WS2812E 比 STM32 醒得快。WS2812E 是纯硬件，上电后几微秒就开始工作。STM32 要等电压稳定、振荡器起振、PLL 锁定……整个过程几十毫秒。
在这段时间里，MCU 完全无法控制 PB15，WS2812E 早已锁存完噪声数据。
*/
int main(void)
{

    // NVIC_SetPriorityGrouping(3);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); // 不配置默认为4位占先

    TIM6_Task_Init(84, 1000);
    led_ini();
    Usart1_Config(115200);

    key_ini();
    Exti_key_ini();
    sg90_PWM(84, 20000);
    beep_ini();

//    motor_pwm_ini(84, 1000);
//    mortor_minspeed_open();

    adc_ini();
    adc_GL5506_ini(); // 光敏电阻

//    spi1_w25_ini();
    spi1_sd_Init();

    i2c_master_ini();

    ws2812e_ini(4); // 灯是有缓存的，所以芯片复位时灯不会灭，要手动加上灯的复位逻辑
    // 后面的 i2c_master_ini 会操作 GPIOB（PB6/PB7），虽然没有直接动 PB15，但同一组 GPIO 的寄存器读写可能产生微妙影响。加上前导复位丢失，噪声数据就一直在第一个灯里锁着。
    // 复位时第一个灯闪绿灯，原因是硬件浮空，加下拉电阻解决
    // WS2812 的 DIN 脚内部有弱上拉（~100kΩ），会把线往上拽。但 PCB 走线本身是天线，会耦合周围的电磁噪声。结果就是 DIN 上的电压随机波动——可能刚好跨过 WS2812 的高低电平阈值，被当成数据吞进去。
    // 多调用一次变色函数，解决复位时第一个灯常亮绿灯。原因可能是复位时有脏数据进入灯带

    RTC_Cal_Config();                   // 时间初始化
    alarm_ini(RTC_H12_PM, 0, 0, 30, 3); // 闹钟初始化
    rtc_wakeup_ini();                   // 唤醒初始化

    in_cap_ini(84, 1000); // 输入捕获初始化      会让风扇停止转动（因为同时占用了TIM3_CH3）    会让彩灯时序乱掉（关闭全局中断保护时序）

    irm_3638T_ini(168, 30000);

    LCD_Init();
    CST816S_Init();

//	DMA_Font_Config();
//	Font_Load();//专门用于字库下载的函数，死循环判断标志位

    
    uint8_t arr[20]="hello你好";
//    LCD_Roll_Dis(100,24,RED,WHITE,arr,1);//死循环

//    SD_WaitReady();SD_Initialize()中已有等待帧
//    while(SD_Initialize())printf("初始化失败");
    
    ff_test();
    
	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(0, 0);		//耳机音量设置
	WM8978_SPKvol_Set(0);		//喇叭音量设置
	WM8978_ADDA_Cfg(1, 0);		//开启DAC
	WM8978_Input_Cfg(0, 0, 0);	//关闭输入通道
	WM8978_Output_Cfg(1, 0);	//开启DAC输出   
	WM8978_I2S_Cfg(2, 0);		//飞利浦标准,16位数据长度
	I2S2_Init(I2S_Standard_Phillips, I2S_Mode_MasterTx, I2S_CPOL_Low, I2S_DataFormat_16bextended);	//飞利浦标准,主机发送,时钟低电平有效,16位扩展帧长度
	I2S2_SampleRate_Set(44100);	//设置采样率
	I2S2_TX_DMA_Init(NULL, NULL, WAV_I2S_TX_DMA_BUFSIZE/2); 				//配置TX DMA
	status_dev.volume = 50;	//初始保存音量 0~63
	WM8978_SPKvol_Set(status_dev.volume);
     
    
    
//    Audio_MusicPlay();//循环播放 按键切换歌曲

	while(1)
	{
    lcd_test2();
        
        
//		Wav_PlaySong((u8 *)"0:MUSIC/许嵩、何曼婷 - 素颜.wav");//单曲播放
	}
    
    
}
