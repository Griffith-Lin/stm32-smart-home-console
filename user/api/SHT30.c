#include "SHT30.h"

//不用硬件
////pb6 i2c1_scl af4   
////pb7 i2c1_sda af4
////都有上拉电阻
//void sht30_ini(void)
//{
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
//    
//     
//    GPIO_InitTypeDef gpio_InitTypeDef={0};
//    
//    gpio_InitTypeDef.GPIO_Mode=GPIO_Mode_AF;
//    gpio_InitTypeDef.GPIO_OType=GPIO_OType_OD;
//    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_6;
//    gpio_InitTypeDef.GPIO_PuPd=GPIO_PuPd_NOPULL;
//    gpio_InitTypeDef.GPIO_Speed=GPIO_Low_Speed;    
//    GPIO_Init(GPIOB,&gpio_InitTypeDef); 
//    
//    gpio_InitTypeDef.GPIO_Pin=GPIO_Pin_7;
//    gpio_InitTypeDef.GPIO_Speed=GPIO_High_Speed;
//    GPIO_Init(GPIOB,&gpio_InitTypeDef);  
//    
//}


volatile uint16_t crc_data=0;

//直接软件模拟
//外设地址0x44，地址规定是7位
//主机发0x2c06给从机，开启高重复精度测量模式
//写位为0，读位为1
uint32_t sht30_i2c_send(uint16_t command)
{
    uint8_t ack_flag=0;
    uint8_t try_count=3;
    uint32_t buf_data=0; 
    
       
    i2c_master_start();
  
    //主机等从机应答，加失败重试
    do
    {
        ack_flag=i2c_master_write(0x44<<1);//写位
    }
    while(!ack_flag && try_count--);//没收到就重试
    
    //发MSB
    i2c_master_write(command>>8);

    
    //发LSB   
    i2c_master_write(command);
   
    
    i2c_master_stop();
       
    I2C_SCL = 1;//主机放开时钟线
    
    i2c_master_start();
      
    
    ack_flag=i2c_master_write((0x44<<1)+1);//读位
    

    
    if(ack_flag)
    {
        //I2C_SCL=0;//手册上写的是从机sht30下拉时钟线，不是主机下拉时钟线！！！！！！！！！
        
        //一般来说，i2c通信从机控制不了scl线，这个传感器比较特殊
        //温湿度的读取异常，加了下面这两行,阻塞等待从机真正释放 SCL
        uint32_t timeout = 500000;  // 超时保护,防真卡死时主机陪葬死等。而高重复精度测量最长 ~16ms
        while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) && timeout--);//等从机真正释放 SCL（也就是测量结束的时候）,线上真的有上升沿
               
        //先读到的是高位
        buf_data=i2c_master_read(1);//tem msb
        buf_data<<=8;   
        
        buf_data|=i2c_master_read(1) ;//tem lsb
        buf_data<<=8; 
        
            crc_data=i2c_master_read(1);
            crc_data<<=8;
        
        //湿度读取异常，一直是高电平。什么时候可能会出现一直高电平（数据线一直空闲），可能主机的继续发的应答，从机没有收到 。             
        //看起来像中途失步了，可能是i2c延时的问题
        /*
        
        从机只有在收到 NACK 时才会停止输出并释放总线。所以第 3 字节(T_CRC)之后的那个 ACK,从机把它误判成了 NACK。
        怎么误判的:从机在 SCL 上升沿采样 SDA——如果 SCL 高电平时间太短、或 SDA 拉低时机不对,从机采样的那一刻 SDA 还是高 → 读成 NACK → 停。

        头号嫌疑:Delay_Us 的真实时长。I2C 标准模式要求 SCL 高、低电平各 ≥ 4.7μs,你现在标称 4μs,已经在临界线上——别忘了 WS2812 绿闪的教训就是 delay 校准问题,同一个 delay.c 现在还在被 I2C 用着。
        
        
        
        
        什么同一个延时，能读取温度，却读取不了湿度

        原因找到了,先给结论:SHT30 发完温度块之后、发湿度块之前,有一段内部停顿——传感器拉低 SCL(时钟拉伸),而你的主机从不检查 SCL 真实电平,直接按自己的节奏盲采样,把停顿期间的悬空高电平读成了 0xFFFF。

        为什么同一个延时,温度能读、湿度不能读
        传感器并不是把 6 个字节一口气连续发完的。SHT3x 先测温度、后测湿度(湿度转换还要拿刚测出的温度做补偿,天然比温度晚完成)。数据流是两段:


        T_MSB T_LSB T_CRC   [传感器停顿:拉低SCL,继续算湿度]   H_MSB H_LSB H_CRC
        温度块在停顿之前——主机什么时候来都读得到,所以 4μs 延时下温度一直完美(CRC 全对,0x9D/0xEF/0x77/0x82 一个不差)。

        湿度块在停顿之后——主机必须"到得比停顿结束晚"。4μs 时主机太快,读完温度块 + 发 ACK,总共才几十微秒,传感器的停顿还没结束。

        关键在这里:停顿期间从机把 SCL 拉低。开漏线上,i2c.c:191 的 I2C_SCL = 1 只是把 ODR 写 1("放手"),线仍然被从机按在地上——线上根本没有上升沿,你给从机喂的全是假时钟。从机没收到时钟就不会翻位,它还把 SDA 放了(悬空高),于是 i2c.c:193 采样到的全是 1 → 组装出 0xFFFF。这也正是之前 CRC 证据的结论:H_CRC=0xFF 不是传感器发的数据(它若真发 0xFFFF,CRC 必是 0xAC),是悬空电平。

        为什么改大到 10μs 就好了
        传感器的停顿时长是固定的真实时间(内部湿度计算,约几十微秒)。主机的延时决定它"到达湿度块的时间":

        4μs/位:主机太快,第 4 字节的时钟落在停顿窗口里 → 假时钟 + 悬空 SDA → 0xFF
        10μs/位:主机慢下来,等它读完温度块、发完 ACK,传感器的停顿已经结束 → 湿度块的真数据按时到达
        提醒:现在的修法是"踩对了节奏",不是"修好了机制"
        10μs 能工作是因为它恰好慢过了停顿时长——换个编译器优化等级、换块板子、换个批次的传感器(停顿时长有离散性),可能又坏。健壮的修法是让主机检查 SCL 真实电平,在 i2c_master_read 每个位里:


        I2C_SCL = 0;
        Delay_Us(4);
        I2C_SCL = 1;
        while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)); // 等从机真正释放 SCL,线上真的有上升沿
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
        {
            data |= 1;
        }
        Delay_Us(4);
        这样 4μs 也能用、任何速度都能用——因为主机不再"假装"时钟到了。另一个方案是换 0x2400(无时钟拉伸命令)+ 读之前 Delay_Ms(10),彻底绕开这个特性。

        核心一句话:软件 I2C 主机只要不检查 SCL 线的真实状态,它就永远不知道自己在给从机发假时钟——0x2C06 的 CS 位允许从机拉伸时钟,你就必须尊重这条线。
        
        
        等等！！
        真正原因是，时序图中，从机拉低scl线我看成了主机拉低时钟线，写错了时序。要等从机测量结束，也就是放开scl线的时候，主机才能读，不然会死锁
        
        */
        
        
        buf_data|=i2c_master_read(1) ;//hu msb
        buf_data<<=8;   
        
        buf_data|=i2c_master_read(1) ;//hu lsb
               
            crc_data|=i2c_master_read(0);
              
        i2c_master_stop();
             
        
    }
     return buf_data;
    
    
    //if(!ack_flag)
 
    
}

//几个全局量没有中断/硬件共享,加 volatile 反而误导读者以为有。普通全局即可。
volatile float tem_data=0;
volatile float hu_data=0;
//char t_cmd[100]={0};
//提取buf_data中的温湿度
void get_tem_hu(uint32_t data)
{
    tem_data=(data>>16)  & (0xffff);
    tem_data=(175*tem_data)/(0xffff)-45;
    
    
    hu_data=data & (0xffff);
    hu_data=(100*hu_data)/(0xffff);
       

}


/*
两个定时器从 0 同时起跑：同一轮主循环里两个都会触发，温度和湿度是连着发的，不是间隔 10 秒。
两个状态机抢同一个 idle_flag/str2_buf：tem 的应答帧会把 hu 的 busy 提前结束，反之亦然——应答对不上号，行为不可预测。
*/

//static uint8_t  pub_tem_busy = 0;          // 1=已发出，等应答
//static uint32_t last_tem_pub = 0;

//void Tcloud_tem(void)
//{
//    uint32_t now = GetTim6Tick();

//    if (pub_tem_busy)                      // 上一条只发不等，现在这里判断有没有应答
//    {
//        if (idle_flag)                 // 应答帧到了（无论 OK/ERROR），本轮结束
//        {
//            idle_flag = 0;
//            pub_tem_busy  = 0;
//        }
//        else if (now - last_tem_pub > 3000) // 兜底：3 秒没任何应答就放弃，防卡死
//        {
//            pub_tem_busy = 0;
//        }
//        return;
//    }

//    
//    
//    
//    if (now - last_tem_pub < 10000)        // 10 秒周期门控
//        return;
//    
//    
//    last_tem_pub = now;

//    get_tem_hu(sht30_i2c_send(0x2c06)); 

//    snprintf(t_cmd, sizeof(t_cmd),
//             "AT+MQTTPUB=0,\"attributes\",\"{\\\"tem\\\":%.2f}\",0,0\r\n",
//             (float)tem_data);
//    
//   
//    
//    idle_flag = 0;                     // 清残留帧，防止把旧应答当成这次的
//    usart2_send_string((uint8_t*)t_cmd); // 只发不等
//    pub_tem_busy  = 1;
//    
//}

//static uint8_t  pub_hu_busy = 0;          // 1=已发出，等应答
//static uint32_t last_hu_pub = 0;

//void Tcloud_hu(void)
//{
//    uint32_t now = GetTim6Tick();

//    if (pub_hu_busy)                      // 上一条还没回
//    {
//        if (idle_flag)                 // 应答帧到了（无论 OK/ERROR），本轮结束
//        {
//            idle_flag = 0;
//            pub_hu_busy  = 0;
//        }
//        else if (now - last_hu_pub > 3000) // 兜底：3 秒没任何应答就放弃，防卡死
//        {
//            pub_hu_busy = 0;
//        }
//        return;
//    }

//    if (now - last_hu_pub < 10000)        // 10 秒周期门控
//        return;
//    
//    
//    last_hu_pub = now;

//    get_tem_hu(sht30_i2c_send(0x2c06)); 
//   
//    
//    snprintf(t_cmd, sizeof(t_cmd),
//             "AT+MQTTPUB=0,\"attributes\",\"{\\\"hu\\\":%.2f}\",0,0\r\n",
//             (float)hu_data);

//    
//    idle_flag = 0;                     // 清残留帧，防止把旧应答当成这次的
//    usart2_send_string((uint8_t*)t_cmd); // 只发不等
//    pub_hu_busy  = 1;
//    
//}




//static uint8_t  pub_busy = 0;        // 1=已发出，等应答
//static uint8_t  pub_sel  = 0;        // 0=发温度 1=发湿度
//static uint32_t last_pub = 0;

//void Tcloud_tem_hu(void)             // 主循环每轮调用（替换原来的两个函数）
//{
//    uint32_t now = GetTim6Tick();

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

//    get_tem_hu(sht30_i2c_send(0x2c06));  

//    if (pub_sel == 0)
//        snprintf(t_cmd, sizeof(t_cmd),
//                 "AT+MQTTPUB=0,\"attributes\",\"{\\\"tem\\\":%.2f}\",0,0\r\n",
//                 (float)tem_data);
//    else
//        snprintf(t_cmd, sizeof(t_cmd),
//                 "AT+MQTTPUB=0,\"attributes\",\"{\\\"hu\\\":%.2f}\",0,0\r\n",
//                 (float)hu_data);
//    pub_sel ^= 1;                    // 下次发另一个

//    idle_flag = 0;                   // 清残留帧
//    usart2_send_string((uint8_t*)t_cmd);
//    pub_busy = 1;
//}

