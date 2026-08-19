#ifndef __DMA_
#define __DMA_

#include "stm32f4xx.h"

void dma2_stream7_ini(uint8_t ndtr,uint32_t m0ar);

void dma2_stream0_ini(uint8_t ndtr,uint32_t m0ar);

void dma2_stream0_dbbuf_ini(uint8_t ndtr,uint32_t m0ar,uint32_t m1ar);
#endif





