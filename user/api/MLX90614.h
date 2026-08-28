#ifndef __MLX90614_
#define __MLX90614_

#include "stm32f4xx.h"
#include "stdio.h"

#include "i2c.h"
#include "esp-12f.h"

void mlx90614_i2c_send(uint8_t data);

void mlx90614_i2c_read(uint8_t command,uint8_t *buf);

float temperature_calculate(uint8_t *buf);

void Tcloud_mlx90614_tem(void);
#endif

