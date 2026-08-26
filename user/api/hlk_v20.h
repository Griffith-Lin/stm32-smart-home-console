#ifndef __HLK_V20
#define __HLK_V20

#include "stm32f4xx.h"
#include "stdio.h"
#include "wavplay.h"
#include "motor.h"

extern volatile uint8_t hlk_getcommand_flag;

void Usart3_hlk_ini(uint32_t brr);

void hlk_send_frame(uint16_t cmd, uint8_t *data, uint8_t len);

uint8_t hlk_getcommand(void);

void HLK_Control(u8 cmd);
#endif

