#ifndef __BEEP_
#define __BEEP_

#include "stm32f4xx.h"
#include "delay.h"

void beep_ini(void);

void beep_one(void);

//#define BEEP_ON()   do { GPIO_SetBits(GPIOD, GPIO_Pin_15); } while(0)
//#define BEEP_OFF()  do { GPIO_ResetBits(GPIOD, GPIO_Pin_15); } while(0)

#define BEEP_ON()   (GPIOD->BSRR = GPIO_Pin_15)      // ÖÃÎ»
#define BEEP_OFF()  (GPIOD->BSRR = (GPIO_Pin_15 << 16)) // ¸´Î»


#endif
