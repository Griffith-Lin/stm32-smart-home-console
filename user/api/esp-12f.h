#ifndef __ESP_12F_
#define __ESP_12F_

#include "stm32f4xx.h"
#include "stdio.h"
#include "wavplay.h"
#include "beep.h"
#include "led.h"

#define OK 0
#define OUT 1
#define ERROR 2    



extern volatile uint8_t str2_buf[500];//用来接收串口数据

uint8_t wifi_send_command(char *cmd,char *rev,uint32_t timeout);

void usart2_ini(uint32_t brr);

void esp_analysis(void);

void esp_12f_ini(void);

void check(uint8_t back);
#endif


