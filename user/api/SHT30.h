#ifndef __SHT30_
#define __SHT30_

#include "stm32f4xx.h"
#include "i2c.h"
#include "stdio.h"

extern volatile float tem_data;
extern volatile float hu_data;
extern volatile uint16_t crc_data;

uint32_t sht30_i2c_send(uint16_t command);
void get_tem_hu(uint32_t data);
#endif


