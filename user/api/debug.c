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
//       RTC_Show_Time();//在唤醒时钟里面调用
//		Delay_Ms(1000);
        
        
        //__DATE__是编译期宏，格式固定为 "Aug 11 2026"（月 日 年）
//		printf("%s\r\n",__DATE__);
//        const char *week[] = {"日", "一", "二", "三", "四", "五", "六"};
//        printf("编译日期: %s 星期%d\r\n", __DATE__, compile_date.weekday);
//		printf("%s\r\n",__TIME__);
//		Delay_Ms(1000);
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
    //data = sht30_i2c_send(0x2400);//无时钟拉伸的高重复精度测量模式
    get_tem_hu(data);

    printf("0x%x\r\n", data);
    printf("%.2f\r\n", tem_data);
    printf("%.2f\r\n", hu_data);
    printf("crc=0x%04x\r\n", crc_data); // 高8位=温度CRC,低8位=湿度CRC
}


void lcd_test(void)
{
        LCD_Dis_Pic(0,0,gImage_gif0);
        Sys_delay_ms(5);
        LCD_Dis_Pic(0,0,gImage_gif1);
        Sys_delay_ms(5);
        LCD_Dis_Pic(0,0,gImage_gif2);
        Sys_delay_ms(5);
}



uint8_t tem_dis[20]={0};
uint8_t hu_dis[20]={0};

void lcd_test2(void)
{
     rtc_test();         
    LCD_Font_Dis(50,30,16,RED,WHITE,rtc_date);
    LCD_Font_Dis(100,100,24,RED,WHITE,rtc_time);
        
        
        
    sht31_test();                 
    sprintf((char *)tem_dis, "温度：%.2f", tem_data);
    sprintf((char *)hu_dis,  "湿度：%.2f", hu_data);     

        
    LCD_Font_Dis(150,100,24,RED,WHITE,tem_dis);
    LCD_Font_Dis(200,100,24,RED,WHITE,hu_dis);  
}

uint8_t sd_buff[20];

void sd_test(void)
{
    uint8_t ret = SD_GetCID(sd_buff);
    printf("ret=%d  ", ret);        // 0=成功, 1=失败
    //for(int i=0;i<16;i++) printf("%x",sd_buff[i]);
    //printf("\r\n");
    //为什么输出的是 1b534d554420202010d160ba6808627，总共31位   
    //因为这串 31 个字符其实是， 1b 53 4d 55 44 20 20 20 10 d1 60 ba 68 0 86 27 ，中间有个0只显示了一位，改成定宽补零 + 空格就清楚了:
    for(int i=0;i<16;i++) printf("%02x ", sd_buff[i]);
    printf("\r\n");
    Delay_Ms(1000);
}



/*
常见返回码(每步都打出来,哪步失败一目了然)
返回值	含义	常见原因
0	FR_OK 成功	—
1	FR_DISK_ERR 底层读写错	SPI 问题、卡坏
3	FR_NOT_READY 磁盘没就绪	没插卡、SD_Initialize 失败
4	FR_NO_FILE	文件不存在
8	FR_EXIST	文件已存在(FA_CREATE_NEW 时)
13	FR_NO_FILESYSTEM	卡不是 FAT 格式
跑之前确认:卡在电脑上格式化成 FAT32(你 ffconf.h:234 里 FF_FS_EXFAT 0,不支持 exFAT——大容量卡 Windows 默认格成 exFAT,就会报 13)。跑完后把卡插回电脑,应该能看到 hello.txt,内容是 hello FatFs!。

检查你 diskio.c 时发现的两个隐患
disk_write 的无限重试:while(res != 0) 死循环重试,卡一旦写失败(写保护/坏卡)MCU 就永远卡死在这。建议改成计数重试,比如重试 3 次后返回错误。
GET_SECTOR_COUNT 写死了 1989632(≈1GB)。你已经有 SD_GetSectorCount()(从 CSD 读真实容量),建议换成它,否则卡容量大于 1GB 时 f_getfree 之类算空间会错。
两个使用提示
你的 FF_USE_STRFUNC 0,所以 f_puts/f_printf 没编译进去——测试里我用 f_write/f_read。以后想用字符串函数,把它改成 1 就行。
用了 f_mount 之后,main.c:81-82 的 SD_WaitReady()/SD_Initialize() 就多余了(f_mount 内部会自动初始化磁盘),留着无害,但可以删掉。
跑通这个之后,下一步可以试 f_opendir/f_readdir 列目录、f_unlink 删文件、f_lseek 随机读写——底层用法都是同一套"检查 FRESULT"模式。
*/

/* ================= FatFs 简易测试 ================= */
/* 流程:挂载 -> 创建写 -> 关闭 -> 重开读 -> 打印 -> 关闭 */

FATFS fs;      /* 文件系统对象(约 560B) */
FIL   fil;     /* 文件对象(约 544B)    */
/* 这两个加起来超过 Keil 默认 1KB 栈,必须放全局或 static,不能放函数栈上 */

void ff_test(void)
{
    FRESULT fr;
    UINT    bw, br;
    char    rbuf[64];
    char    wbuf[] = "hello FatFs!\r\n";

    /* 1. 挂载:把 SD 卡绑定为逻辑盘 "0:",opt=1 表示立即挂载
          首次挂载时 FatFs 会自动调用 disk_initialize -> SD_Initialize */
    fr = f_mount(&fs, "0:", 1);
    printf("f_mount : %d\r\n", fr);       /* 0=FR_OK; 13=卡没格式化成FAT */

    /* 2. 打开文件:FA_CREATE_ALWAYS = 不存在就创建,存在就清空重写 */
    fr = f_open(&fil, "0:/hello.txt", FA_CREATE_ALWAYS | FA_WRITE);
    printf("f_open W: %d\r\n", fr);

    /* 3. 写文件,sizeof-1 是不把结尾 \0 写进去 */
    fr = f_write(&fil, wbuf, sizeof(wbuf)-1, &bw);//要1是因为文件结束符是EOF，而不是'\0'
    printf("f_write : %d, wrote=%d\r\n", fr, bw);

    /* 4. 关闭:必须调,不然 FAT 表和数据还停在内部缓冲,没真正落盘 */
    fr = f_close(&fil);
    printf("f_close : %d\r\n", fr);

    /* 5. 重新打开读 */
    fr = f_open(&fil, "0:/hello.txt", FA_READ);
    printf("f_open R: %d\r\n", fr);

    /* 6. 读文件 */
    fr = f_read(&fil, rbuf, sizeof(rbuf)-1, &br);
    rbuf[br] = '\0';                     /* f_read 不补 \0,手动补再打印 */
    printf("f_read  : %d, read=%d -> %s", fr, br, rbuf);

    /* 7. 关闭 */
    f_close(&fil);
}

