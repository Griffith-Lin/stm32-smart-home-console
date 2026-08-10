#include "ws2812e.h"


//pb15
void ws2812e_ini(uint16_t rgb_num)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    
    GPIO_InitTypeDef gpio_InitTypeDef={0};
    
    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_OUT;
    gpio_InitTypeDef.GPIO_OType=GPIO_OType_PP;
    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_15;
    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
    gpio_InitTypeDef.GPIO_Speed=GPIO_High_Speed;
    
    GPIO_Init(GPIOB,&gpio_InitTypeDef);
    

    
    while(rgb_num--)
    ws2812e_open(0,0,0);
    
    
    PB15_LOW();
    Delay_Us(400);
}

void ws2812e_send_byte(uint8_t data)
{
    for(int i=0;i<8;i++)
    {
        if(data & (0x80>>i))
        {
            PB15_HIGH();
            Delay_100Ns(7);
            PB15_LOW();
            Delay_100Ns(3);
        }
        else
        {
            PB15_HIGH();
            Delay_100Ns(3);
            PB15_LOW();
            Delay_100Ns(7);
        }
    }

}


void ws2812e_reset(void)
{
    
    PB15_LOW();
    Delay_Us(400);
}



void ws2812e_open(uint8_t green,uint8_t red,uint8_t blue)
{
    ws2812e_send_byte(green);
    ws2812e_send_byte(red);
    ws2812e_send_byte(blue);
}

volatile uint8_t green=0;
volatile uint8_t red=0;
volatile uint8_t blue=0;



void ws2812e_open_reset(uint8_t green,uint8_t red,uint8_t blue,uint8_t rgb_num)
{
    while(rgb_num--)
    ws2812e_open(green,red,blue);
    
    ws2812e_reset();
}

//uint8_t tmp_flag=1;

//void ws2812e_mode1(void)
//{
//    if(green!=255 && tmp_flag)
//    {
//    green++;
//    red++;
//    blue++;
//    }
//    else if(
//    {
//    green--;
//    red--;
//    blue--;   
//    tmp_flag=0;
//    }
//}


