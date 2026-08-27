#ifndef __ESP_12F_
#define __ESP_12F_

#include "stm32f4xx.h"
#include "stdio.h"
#include "wavplay.h"
#include "beep.h"

#define OK 0
#define OUT 1
#define ERROR 2    



extern volatile uint8_t str2_buf[500];//用来接收串口数据

void usart2_ini(uint32_t brr);

void esp_analysis(volatile uint8_t *buf);

void esp_12f_ini(void);
#endif


