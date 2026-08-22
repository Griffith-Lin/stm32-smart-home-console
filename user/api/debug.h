#ifndef __DEBUG_
#define __DEBUG_

#include "main.h"

#include <stdio.h>
#include <stdint.h> 


extern uint8_t sd_buff[20];

extern uint8_t tem_dis[20];
extern uint8_t hu_dis[20];

extern uint8_t tmp;
extern uint8_t gl5506_flag;

extern volatile uint8_t mlx90614_buf[2];
extern volatile uint16_t mlx90614_read_flag;


void adc_debug_manual(void);
void adc_debug_auto(void);
void W25Qxx_test(void);
void rtc_test(void);
void mlx90614_test(void);
void ws2812e_test(void);
void sht31_test(void);
#endif
