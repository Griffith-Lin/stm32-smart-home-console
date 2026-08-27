#ifndef __LED_
#define __LED_

#include "stm32f4xx.h"


#define LED1_ON  GPIO_ResetBits(GPIOD,GPIO_Pin_12)
#define LED1_OFF GPIO_SetBits(GPIOD,GPIO_Pin_12)

#define LED2_ON  GPIO_ResetBits(GPIOD,GPIO_Pin_13)
#define LED2_OFF GPIO_SetBits(GPIOD,GPIO_Pin_13)

#define LED3_ON  GPIO_ResetBits(GPIOD,GPIO_Pin_14)
#define LED3_OFF GPIO_SetBits(GPIOD,GPIO_Pin_14)

void led_ini(void);


#endif


