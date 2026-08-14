#include "MLX90614.h"



//主机发给从机数据
void mlx90614_i2c_send(uint8_t data)
{
    i2c_master_start();
    
    uint8_t id_w=0x5a & ~(1<<7);
    
    
    
    uint8_t ack_flag=0;
    uint8_t try_count=3;
    //主机等从机应答，加失败重试
    do
    {
        ack_flag=i2c_master_write(id_w); 
    }
    while((!ack_flag) && try_count--);//没收到就重试
    
    i2c_master_write(data);//命令
    
    i2c_master_stop();
}

//主机读从机数据
void mlx90614_i2c_read(uint8_t command,volatile uint8_t *buf)
{
    i2c_master_start();
    
    uint8_t id_w=(0x5A << 1) | 0;//从高位到低位传输，第0位是第8个传输的位
    uint8_t id_r=(0x5A << 1) | 1;
    
        
    uint8_t ack_flag=0;
    uint8_t try_count=3;
    //主机等从机应答，加失败重试
    do
    {
        ack_flag=i2c_master_write(id_w);
    }
    while(!ack_flag && try_count--);//没收到就重试
    
    i2c_master_write(command);//命令
    
    i2c_master_start();
    
    i2c_master_write(id_r); 
    
    //MLX90614 的 16 位数据分两次发——第一次吐低字节，第二次吐高字节。这是 MLX90614（SMBus）协议的规定
    buf[0] = i2c_master_read(1);   // 第一字节，回应 ACK
    buf[1] = i2c_master_read(0);   // 最后一字节，回应 NACK
    
    i2c_master_stop();
}


float temperature_calculate(volatile uint8_t *buf)
{
    float tmp=(buf[1]<<8) | buf[0];//float类型数据有32bit
    tmp=tmp*0.02-273.15;
    return tmp;
}


