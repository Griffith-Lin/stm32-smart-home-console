#ifndef _USART1_H
#define _USART1_H

#include "stm32f4xx.h"
#include "led.h"
#include "debug.h"

#include <string.h>
#include <stddef.h>
#include <stdio.h>


void Usart1_Config(u32 brr);

void usart1_send_byte(u8 btye);
u8 usart1_rev_byte(void);

void usart1_send_string(volatile uint8_t *str);
void usart1_rev_string(uint8_t * buffer);

void usart1_rev_string_norm(uint8_t * buffer);

void rev_process(const volatile char* buff);
    
#endif
