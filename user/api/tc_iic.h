#ifndef _TC_IIC_H
#define _TC_IIC_H


#include "stm32f4xx.h"
#include "stdio.h"
#include "delay.h"
#include "motor.h"
#include "touch_ui.h"


#define CTP_SCL_H (GPIOA->ODR |= (1 << 8))
#define CTP_SCL_L (GPIOA->ODR &=~ (1 << 8))


#define CTP_SDA_H (GPIOC->ODR |= (1 << 9))
#define CTP_SDA_L (GPIOC->ODR &=~ (1 << 9))

#define CTP_RES_H (GPIOD->ODR |= (1 << 5))
#define CTP_RES_L (GPIOD->ODR &=~ (1 << 5))

#define CTP_INT (GPIOD->IDR & (1 << 1))// ´¥Ãþ±ÊÖÐ¶Ï

#define CTP_SDA_IN   GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)


extern volatile uint32_t tim6_tick_ms;
extern volatile uint32_t last_touch_ms;    


typedef struct
{
	u16 x[2]; 
	u16 y[2];
	u8  sta;                //´¥Ãþ×´Ì¬  
}mt_tp_dev;

extern mt_tp_dev tp_dev;

extern volatile uint8_t TP_tint_flag;

void CTP_GPIOInit(void);
void CTP_IIC_Start(void);
void CTP_IIC_Stop(void);
u8 CTP_WaitAck(void);
void CTP_IICAck(void);
void CTP_IICNack(void);
void CTP_SendByte(u8 dat);
u8 CTP_ReadByte(u8 ack);
void  CST816S_Init(void);
u8 CST816S_Scan(void);

#endif

