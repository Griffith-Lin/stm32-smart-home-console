#include "rtc.h"



void RTC_Cal_Config(void)
{
	
    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//使能电源接口时钟
	PWR_BackupAccessCmd(ENABLE);//使能备份域访问
    
	RCC_LSICmd(ENABLE);//使能LSI时钟
    
    
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);//选择RTC时钟为LSI
    
	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
	
//	RTC_WriteProtectionCmd(DISABLE);//取消写保护
//	RTC_EnterInitMode();//进入初始化模式
//	while(RTC_GetFlagStatus(RTC_FLAG_INITF)==RESET);//等待标志位
//	
//	
//	RTC_ExitInitMode();//退出初始化模式
//	RTC_WriteProtectionCmd(ENABLE);//使能写保护
	
	//上述的流程在任意的写入库函数中都存在 可以省略不写
    
	RTC_InitTypeDef rtc_InitTypeDef={0};
	
	rtc_InitTypeDef.RTC_HourFormat=RTC_HourFormat_24;//时间格式
	rtc_InitTypeDef.RTC_AsynchPrediv=127;//异步分频 128分频
	rtc_InitTypeDef.RTC_SynchPrediv=249;//同步分频 250分频
    
	RTC_Init(&rtc_InitTypeDef);
    
    
    Compile_WeekDay();
    Compile_Time();
    
	//直接赋值初始时间
	RTC_Set_Date(compile_date.year,compile_date.month,compile_date.day,compile_date.weekday);
	RTC_Set_Time(RTC_H12_PM,compile_time.hour,compile_time.min,compile_time.second);
}

/*
RTC_TimeTypeDef是硬件寄存器的软件抽象,无需关心具体是寄存器哪一位

初始化函数的接口设计为接收结构体指针 (&rtc_TimeTypeDef)，避免了大结构体压栈拷贝的开销，同时允许库函数读取配置并写入硬件。
*/

//配置RTC_TimeTypeDef结构体
void RTC_Set_Time(uint8_t H12,uint8_t Hours,uint8_t Minutes,uint8_t Seconds)
{
	RTC_TimeTypeDef rtc_TimeTypeDef={0};
    
	rtc_TimeTypeDef.RTC_H12=H12;
	rtc_TimeTypeDef.RTC_Hours=Hours;
	rtc_TimeTypeDef.RTC_Minutes=Minutes;
	rtc_TimeTypeDef.RTC_Seconds=Seconds;
    
	RTC_SetTime(RTC_Format_BIN,&rtc_TimeTypeDef);
}

//配置RTC_DateTypeDef结构体
void RTC_Set_Date(uint8_t Year,uint8_t Month,uint8_t Date,uint8_t WeekDay)
{
	RTC_DateTypeDef rtc_DateTypeDef={0};
	
	rtc_DateTypeDef.RTC_Date=Date;
	rtc_DateTypeDef.RTC_Month=Month;
	rtc_DateTypeDef.RTC_WeekDay=WeekDay;
	rtc_DateTypeDef.RTC_Year=Year;
	
	RTC_SetDate(RTC_Format_BIN,&rtc_DateTypeDef);
}



RTC_TimeTypeDef RTC_Time={0};
RTC_DateTypeDef RTC_Date={0};

uint8_t rtc_date[50]={0};
uint8_t rtc_time[50]={0};

void RTC_GetTimeDate(void)
{
    //指定返回参数格式为BIN还是BCD
	RTC_GetTime(RTC_Format_BIN,&RTC_Time);
	RTC_GetDate(RTC_Format_BIN,&RTC_Date);
}

void RTC_Show_Time(void)
{

    RTC_GetTimeDate();

    
    sprintf((char*)rtc_date,"%2d年-%2d月-%2d日 星期%d",2000+RTC_Date.RTC_Year,RTC_Date.RTC_Month,RTC_Date.RTC_Date,RTC_Date.RTC_WeekDay);
    sprintf((char*)rtc_time,"%02d:%02d:%02d",RTC_Time.RTC_Hours,RTC_Time.RTC_Minutes,RTC_Time.RTC_Seconds); 
	//用%02d时为了固定显示宽度，防止位数变化时，末尾的区域刷新不到
    
//	printf("%d年-%d月-%d日 星期%d\t",2000+RTC_Date.RTC_Year,RTC_Date.RTC_Month,RTC_Date.RTC_Date,RTC_Date.RTC_WeekDay);
//	printf("%d:%d:%d\r\n",RTC_Time.RTC_Hours,RTC_Time.RTC_Minutes,RTC_Time.RTC_Seconds);
    
    
}



volatile struct COMPILE_data compile_date;

void Compile_WeekDay(void)
{
    const char *date = __DATE__;   // "Aug 11 2026"
    const char *mon_str[] = {"Jan","Feb","Mar","Apr","May","Jun",
                             "Jul","Aug","Sep","Oct","Nov","Dec"};
    
    
    for (int i = 0; i < 12; i++) {
        if (date[0] == mon_str[i][0] && date[1] == mon_str[i][1] && date[2] == mon_str[i][2]) {
            compile_date.month = i + 1;
            break;
        }
    }
    
    compile_date.day  = (date[4] == ' ' ? 0 : (date[4] - '0') * 10) + (date[5] - '0');
    
    uint16_t year = (date[7] - '0') * 1000 + (date[8] - '0') * 100 
                  + (date[9] - '0') * 10  + (date[10] - '0');
    compile_date.year=year-2000;
    
    // Sakamoto 算法
    static const uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= compile_date.month < 3;
    compile_date.weekday= (year + year / 4 - year / 100 + year / 400 + t[compile_date.month - 1] + compile_date.day) % 7;
}

volatile struct COMPILE_time compile_time;

void Compile_Time(void)
{
    const char *time = __TIME__;   // "16:42:30"
    compile_time.hour   = (time[0] - '0') * 10 + (time[1] - '0');
    compile_time.min    = (time[3] - '0') * 10 + (time[4] - '0');
    compile_time.second = (time[6] - '0') * 10 + (time[7] - '0');
}


void alarm_ini(uint8_t H12,uint8_t Hours,uint8_t Minutes,uint8_t Seconds,uint8_t Weekday)
{
    RTC_AlarmTypeDef rtc_AlarmTypeDef={0};  //闹钟配置结构体
	RTC_TimeTypeDef rtc_TimeTypeDef={0};    //时间配置结构体
	EXTI_InitTypeDef exti_InitTypeDef={0};  //EXTI配置结构体
	NVIC_InitTypeDef nvic_InitTypeDef={0};  //嵌套向量中断控制器配置结构体
    
    
//    RTC->CR &=~(1<<8);//禁能闹钟A
    RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
    
//    轮询 RTC_ISR 寄存器中的ALRAWF或ALRBWF位，直到其中一个置1，以确保闹钟寄存器可以访问。大约需要2个RTCCLK时钟周期（由于时钟同步）。
//    while(!(RTC->ISR & (1<<0)));
    while(RTC_GetFlagStatus(RTC_FLAG_ALRAWF)!=SET);
    
//    设置亚秒掩码，这里省略
//    RTC->ALRMASSR;
//    RTC->ALRMAR;
//    RTC_AlarmSubSecondConfig(RTC_Alarm_A,RTC_AlarmSubSecondMask_SS14_13,
    
    rtc_TimeTypeDef.RTC_H12=H12;
	rtc_TimeTypeDef.RTC_Hours=Hours;
	rtc_TimeTypeDef.RTC_Minutes=Minutes;
	rtc_TimeTypeDef.RTC_Seconds=Seconds;

	rtc_AlarmTypeDef.RTC_AlarmTime=rtc_TimeTypeDef;//闹钟时间配置
	rtc_AlarmTypeDef.RTC_AlarmDateWeekDay=Weekday;//选择的是星期而不是日期，所以这里配置的是星期
	rtc_AlarmTypeDef.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//星期/日期选择
	rtc_AlarmTypeDef.RTC_AlarmMask=RTC_AlarmMask_Minutes | RTC_AlarmMask_Hours;
    //掩码，RTC_AlarmMask 的工作机制是：被掩码屏蔽的字段不参与闹钟比较，相当于“通配符/任意值”。
	//闹钟产生中断事件，需要满足星期 时 分 秒与配置的相同，用掩码屏蔽分钟，意思就是满足星期 时 秒相同就可以触发中断
    
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&rtc_AlarmTypeDef);
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//使能闹钟中断
	
	exti_InitTypeDef.EXTI_Line=EXTI_Line17;//线17，从参考手册的唤醒事件管理中得知
	exti_InitTypeDef.EXTI_LineCmd=ENABLE;
	exti_InitTypeDef.EXTI_Mode=EXTI_Mode_Interrupt;//中断服务模式,触发后进入NVIC中断服务程序
	exti_InitTypeDef.EXTI_Trigger=EXTI_Trigger_Rising_Falling;//上下沿触发
    
	EXTI_Init(&exti_InitTypeDef);
	
    
	nvic_InitTypeDef.NVIC_IRQChannel=RTC_Alarm_IRQn;//闹钟通道
	nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=3;//占先
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority=0;//次级
    
	NVIC_Init(&nvic_InitTypeDef);
	
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//使能闹钟
    
    
}


void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA)==SET)
	{
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		
//		printf("时间到了\r\n");
	}
	EXTI_ClearITPendingBit(EXTI_Line17);
}


void rtc_wakeup_ini(void)
{
    RTC_WriteProtectionCmd(DISABLE);//RTC 寄存器默认开启写保护。需要手动关闭
    
//    RTC->CR &=~(1<<10);//唤醒定时器禁能
    RTC_WakeUpCmd(DISABLE);
    
    while(RTC_GetFlagStatus(RTC_FLAG_WUTWF)!=SET);//唤醒定时器 它的写标志。在 RTC_CR 寄存器中的WUTE位置0后，当唤醒定时器值可更改时，由硬件将该位置1。

    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//选择时钟源     前 4 个用的是 RTCCLK 直接分频（高频，精度高，但最大只能到几十秒）；后 2 个用的是经过同步和异步通道的 CK_SPRE（RTC 预分频器输出的 1Hz 信号，低频，精度低，但能到1s到18小时，18小时到36小时）。
    
    RTC_ClearFlag(RTC_FLAG_WUTF);//根据手册，这里要清标志位。  
    //没有硬件自动清零机制：与某些外设标志位在读寄存器或写操作时自动清零不同，WUTF 只能通过软件写 RTC_ClearFlag() 来清除。不存在任何配置步骤能隐式完成这件事。因此，显式清除不是“建议”，而是填补硬件设计空白的必要操作。
    
    RTC_SetWakeUpCounter(0);//WUTR  配置要计数次数（唤醒周期）  STM32 的 Wakeup 计数器是递减到 0 后再额外计一个周期才触发中断，硬件设计上是 (WUTR + 1) 个时钟周期。
    
    
    
    RTC_ITConfig(RTC_IT_WUT,ENABLE);//使能唤醒定时器中断
    
    
    //中断向量表（Interrupt Vector Table, IVT）本质上是一个存储中断服务程序（ISR）入口地址的数组。
    //在实际开发中，你不需要手动维护这张表。
    //启动文件（如 startup_stm32f4xx.s）和链接脚本已经定义了它的结构，你只需要在 C 代码中按照规范命名中断处理函数（如 EXTI0_IRQHandler），编译器会自动将其地址填入向量表的对应位置。
    
    //嵌套向量中断控制器配置
    NVIC_InitTypeDef nvic_InitTypeDef={0};
    
    nvic_InitTypeDef.NVIC_IRQChannel=RTC_WKUP_IRQn;
    nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=3;
    nvic_InitTypeDef.NVIC_IRQChannelSubPriority=0;    
    nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;
    
    NVIC_Init(&nvic_InitTypeDef);
    
    //EXTI配置
    EXTI_InitTypeDef exti_InitTypeDef={0};
    
    exti_InitTypeDef.EXTI_Mode=EXTI_Mode_Interrupt;
    exti_InitTypeDef.EXTI_Line=EXTI_Line22;
    exti_InitTypeDef.EXTI_Trigger=EXTI_Trigger_Rising;//从参考手册rtc中断章节得知
    exti_InitTypeDef.EXTI_LineCmd=ENABLE;
    
    EXTI_Init(&exti_InitTypeDef);
    
    
    RTC_WakeUpCmd(ENABLE);
    
    RTC_WriteProtectionCmd(ENABLE);//恢复写保护
}

void RTC_WKUP_IRQHandler (void)
{
    if(RTC_GetITStatus(RTC_IT_WUT)==SET)
    {
        RTC_ClearITPendingBit(RTC_IT_WUT);
 
        EXTI_ClearITPendingBit(EXTI_Line22);
        //RTC Wakeup 的中断链路是：RTC → EXTI Line 22 → NVIC。
        //如果你只清除了 RTC 内部的标志位，但 EXTI Line 22 的 Pending Bit 仍然置位。这会导致：
        //退出 ISR 后，NVIC 立即再次触发中断（因为 EXTI 挂起位还在）
        
        
        RTC_Show_Time();
        
    }
}


