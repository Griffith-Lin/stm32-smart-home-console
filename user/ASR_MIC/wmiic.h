#ifndef _IIC_H_
#define _IIC_H_

#include "io_bit.h"  
#include "delay.h"

#define WM_IIC_SCL       PBout(10)     //SCL时钟引脚
#define WM_IIC_SDA    	 PBout(11)     //输出SDA	 
#define WM_IIC_SDAIN     PBin(11)      //输入SDA 
#define WM_IIC_SDA_IN()  {GPIOB->MODER &= ~(3<<2*11);} //SDA设置为输入
#define WM_IIC_SDA_OUT() {GPIOB->MODER &= ~(3<<2*11);  GPIOB->MODER |= (1<<2*11);} //SDA设置为通用输出

/*IIC驱动*/
void WM_IIC_Init(void);
void WM_IIC_Start(void);
void WM_IIC_Stop(void);
unsigned char WM_IIC_Wait_ACK(void);
void WM_IIC_WriteByte(unsigned char data);
unsigned char WM_IIC_ReadByte(unsigned char ack);

#endif
