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
void mlx90614_i2c_read(uint8_t command,uint8_t *buf)
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


float temperature_calculate(uint8_t *buf)
{
    float tmp=(buf[1]<<8) | buf[0];//float类型数据有32bit
    tmp=tmp*0.02-273.15;
    return tmp;
}

//uint8_t mlx90614_buf[2]={0};
//uint16_t mlx90614_read_flag=0;
//char t_cmd[100]={0};

//static uint8_t  pub_busy = 0;        // 1=已发出，等应答
//static uint32_t last_pub = 0;

//void Tcloud_mlx90614_tem(void)
//{
//     uint32_t now = GetTim6Tick();

//    if (pub_busy)                    // 上一条还没回
//    {
//        if (idle_flag)               // 应答帧到了（OK/ERROR 都算），本轮结束
//        {
//            idle_flag = 0;
//            pub_busy  = 0;
//        }
//        else if (now - last_pub > 3000)  // 兜底：3 秒没应答就放弃
//        {
//            pub_busy = 0;
//        }
//        return;
//    }

//    if (now - last_pub < 10000)      // 10 秒门控
//        return;
//    last_pub = now;
//    
//    
//    
//    mlx90614_i2c_read(0x07,mlx90614_buf);//读标志位
//    
//    mlx90614_read_flag=(mlx90614_buf[1]<<8) | mlx90614_buf[0];//由高8位（DataH）和低8位（DataL）两部分组成，
////    printf("%d\r\n",mlx90614_read_flag);
//    
//    
//    float result=temperature_calculate(mlx90614_buf);
//    printf("%.2f\r\n",result);
//    
//    snprintf(t_cmd, sizeof(t_cmd),
//                 "AT+MQTTPUB=0,\"attributes\",\"{\\\"human_tem\\\":%.2f}\",0,0\r\n",
//                 result);  
//    
//    idle_flag = 0;                   // 清残留帧
//    usart2_send_string((uint8_t*)t_cmd);
//    pub_busy = 1;
//}


