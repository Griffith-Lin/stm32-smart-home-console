#ifndef __HLK_V20
#define __HLK_V20

#include "stm32f4xx.h"
#include "stdio.h"

void Usart3_hlk_ini(uint32_t brr);

void hlk_send_frame(uint16_t cmd, uint8_t *data, uint8_t len);
#endif

