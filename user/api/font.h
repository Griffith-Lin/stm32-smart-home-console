#ifndef __FONT_
#define __FONT_

#include "stm32f4xx.h"

#define BUFFSIZE 4096

enum
{
	Free_IT,
	DMA_IT,
	Usart_IT
};
	

typedef struct
{
	u8 buff[2][BUFFSIZE];
	u8 sta;
	u32 addr;
	u32 len;
	u8 flag;
}Font;

extern Font font;

extern const uint16_t arr_font111[256];

extern const unsigned char gImage_111[128158];

extern const unsigned char gImage_1[106408];

extern const unsigned char gImage_gif0[60008];
extern const unsigned char gImage_gif1[60008];
extern const unsigned char gImage_gif2[60008];

extern const unsigned char gImage_fan_add[6088];
extern const unsigned char gImage_fan_subtract[6088];
#endif
