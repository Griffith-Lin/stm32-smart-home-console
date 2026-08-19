#include "sys.h"

/*
Function name:Sys_delay_us
Description:us延时函数
param:ustime--延时时长
retval:None
Remarks:
内核时钟:2^24 *(1/168 us)= 99864us  
外部参考时钟:2^24 *(1/21 us)=798915us  
*/
void Sys_delay_us(uint32_t ustime)
{
	SysTick->CTRL &=~(1<<2);//外部参考时钟
	SysTick->LOAD=21*ustime-1;//重装载值
	SysTick->VAL=0;//清除当前值寄存器
	SysTick->CTRL |=(1<<0);//使能定时器
	while(!(SysTick->CTRL & (1<<16)));
	SysTick->CTRL &=~(1<<0);//禁止使能定时器
}

void Sys_delay_ms(uint32_t mstime)
{
	SysTick->CTRL &=~(1<<2);//外部参考时钟
	SysTick->LOAD=21000*mstime-1;//重装载值
	SysTick->VAL=0;//清除当前值寄存器
	SysTick->CTRL |=(1<<0);//使能定时器
	while(!(SysTick->CTRL & (1<<16)));
	SysTick->CTRL &=~(1<<0);//禁止使能定时器
}



//SysTick的中断服务函数 1ms
uint32_t time=0;
uint32_t temp=0;
uint32_t time1[2]={0,1000};
uint32_t time2[2]={0,500};
//void SysTick_Handler(void)
//{
//  if(SysTick->CTRL & (1<<16))
//	{
//		SysTick->CTRL &=~ (1<<16);
//		time++;
//		time1[0]++;
//		time2[0]++;
//		temp--;
//	}
//}

void sysnvic_delay(uint32_t mstime)
{
	uint32_t timeout;
	timeout=time;
	while(time-timeout<=mstime);
}


void sysnvic_delay1(uint32_t mstime)
{
	temp=mstime;
	while(temp);
}

