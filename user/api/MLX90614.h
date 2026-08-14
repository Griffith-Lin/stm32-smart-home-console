#ifndef __MLX90614_
#define __MLX90614_

#include "stm32f4xx.h"

#include "i2c.h"

void mlx90614_i2c_send(uint8_t data);

void mlx90614_i2c_read(uint8_t command,volatile uint8_t *buf);

float temperature_calculate(volatile uint8_t *buf);
#endif

