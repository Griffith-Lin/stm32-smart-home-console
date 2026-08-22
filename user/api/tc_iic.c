#include "tc_iic.h"
const u16 TPX[] = {0x03, 0x09}; // 触摸地址

void CTP_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    //    CTP_SDA_OUT();

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   // 无上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    //	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    //    GPIO_InitTypeDef GPIO_InitStructure={0};

    //	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能PORTB时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   // 上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOD, GPIO_Pin_5);
    GPIO_SetBits(GPIOA, GPIO_Pin_8);
    //	CTP_SDA_OUT();

    NVIC_InitTypeDef nvic_InitTypeDef = {0};

    nvic_InitTypeDef.NVIC_IRQChannel = EXTI1_IRQn;
    nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_InitTypeDef.NVIC_IRQChannelSubPriority = 0;
    nvic_InitTypeDef.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvic_InitTypeDef);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);

    EXTI_InitTypeDef exti_InitTypeDef = {0};

    exti_InitTypeDef.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_InitTypeDef.EXTI_Line = EXTI_Line1;
    exti_InitTypeDef.EXTI_Trigger = EXTI_Trigger_Falling;
    exti_InitTypeDef.EXTI_LineCmd = ENABLE;

    EXTI_Init(&exti_InitTypeDef);
}

void CTP_IIC_Start(void)
{
    CTP_SDA_H;
    CTP_SCL_H;
    Delay_Us(5);
    CTP_SDA_L;
}

void CTP_IIC_Stop(void)
{
    CTP_SDA_L;
    CTP_SCL_H;
    Delay_Us(5);
    CTP_SDA_H;
}

u8 CTP_WaitAck(void)
{
    u8 ms = 0;
    CTP_SDA_H;
    CTP_SCL_L;
    Delay_Us(5);
    CTP_SCL_H;
    if (CTP_SDA_IN)
    {
        CTP_IIC_Stop();
        return 1;
    }
    Delay_Us(5);
    CTP_SCL_L;
    Delay_Us(5);
    return 0;
}

void CTP_IICAck(void)
{
    CTP_SCL_L;
    Delay_Us(5);
    CTP_SDA_L;
    Delay_Us(5);
    CTP_SCL_H;
    Delay_Us(5);
    CTP_SCL_L;
    Delay_Us(5);
}

void CTP_IICNack(void)
{
    CTP_SDA_H;
    CTP_SCL_L;
    Delay_Us(5);
    CTP_SCL_H;
    Delay_Us(5);
    CTP_SCL_L;
    Delay_Us(5);
}

void CTP_SendByte(u8 dat)
{
    u8 i;

    for (i = 0; i < 8; i++)
    {
        CTP_SCL_L;
        Delay_Us(5);
        if (dat & (0x80 >> i))
        {
            CTP_SDA_H;
        }
        else
        {
            CTP_SDA_L;
        }
        Delay_Us(5);
        //		dat<<=1;
        CTP_SCL_H;
        Delay_Us(5);
    }
}

u8 CTP_ReadByte(u8 ack)
{
    u8 i, dat;
    //    CTP_SDA = 1;
    for (i = 0; i < 8; i++)
    {
        CTP_SCL_L;
        //        CTP_SDA_INPUT();
        CTP_SDA_H;
        Delay_Us(5);
        CTP_SCL_H;
        Delay_Us(5);
        dat <<= 1;
        if (CTP_SDA_IN)
            dat |= 1;
    }
    if (!ack)
    {
        CTP_IICNack();
    }
    else
    {
        CTP_IICAck();
    }
    return dat;
}

// 写数据
u8 CST816S_WR_DATA(u16 addr, u8 data)
{
    u8 reg;
    CTP_IIC_Start();    // IIC起始信号
    CTP_SendByte(0x2A); // 发送写指令
    CTP_WaitAck();
    CTP_SendByte(addr);
    CTP_WaitAck();
    CTP_SendByte(data); // 写入数据
    CTP_WaitAck();
    CTP_IIC_Stop(); // IIC结束信号
    reg = SUCCESS;
    return reg;
}

// 读数据
u8 CST816S_RD_DATA(u16 addr, u8 len, u8 *value)
{
    u8 reg = ERROR, i;
    CTP_IIC_Start();
    CTP_SendByte(0x2A); // 发送写指令
    CTP_WaitAck();
    CTP_SendByte(addr);
    CTP_WaitAck();
    CTP_IIC_Start();
    CTP_SendByte(0x2B);
    CTP_WaitAck();
    for (i = 0; i < len; i++)
    {
        if (i == (len - 1))
        {
            value[i] = CTP_ReadByte(0x00);
        }
        else
        {
            value[i] = CTP_ReadByte(0x01);
        }
    }
    CTP_IIC_Stop();
    reg = SUCCESS;
    return reg;
}

void CST816S_Init(void)
{
    u8 ID;
    CTP_GPIOInit();
    // 复位设备
    CTP_RES_L;
    Delay_Ms(50);
    CTP_RES_H;
    Delay_Ms(50);
    // 初始化设置
    CST816S_WR_DATA(0xfd, 0x0);
    // 读取器件ID
    CST816S_RD_DATA(0xA7, 1, &ID);

    printf("ID:0x%x\r\n", ID);
}

mt_tp_dev tp_dev;
uint16_t motor_speed = 0;

u8 CST816S_Scan()
{
    u8 buf[4];
    u8 i = 0;
    u8 res = 0;
    u8 temp;
    u8 tempsta;
    u8 mode;

    CST816S_RD_DATA(0x02, 1, &mode);       // 读取手指个数
    if (mode & 0X80 && ((mode & 0XF) < 6)) // mode & 0x80;芯片数据更新标志,置 1 代表有新触摸数据;
    {
        temp = 0;
        CST816S_WR_DATA(0x02, temp); // 如果有新数据,向 0x02 写 0,清除芯片的刷新标志位,否则芯片会持续上报旧数据
    }
    if ((mode & 0XF) && ((mode & 0XF) < 6))
    {
        temp = 0XFF << (mode & 0XF);        // 将点的个数转换为1的位数,匹配tp_dev.sta定义
        tempsta = tp_dev.sta;               // 保存原本的tp_dev.sta值(状态)
        tp_dev.sta = (~temp) | 0x81 | 0x40; // 固定置位基础触摸标志（按下标记、触摸中断标志）
        tp_dev.x[1] = tp_dev.x[0];          // 保存触点0的数据
        tp_dev.y[1] = tp_dev.y[0];
        for (i = 0; i < 2; i++)
        {
            if (tp_dev.sta & (1 << i)) // 触摸有效
            {
                CST816S_RD_DATA(TPX[i], 4, buf); // 读取坐标值
                tp_dev.x[i] = ((buf[0] & 0x0F) << 8) | buf[1];
                tp_dev.y[i] = ((buf[2] & 0x0F) << 8) | buf[3];

                printf("x[%d]:%d,y[%d]:%d\r\n", i, tp_dev.x[i], i, tp_dev.y[i]);
                TP_tint_flag = 0;
            }
        }
        res = 1;
        if (tp_dev.x[0] > 240 || tp_dev.y[0] > 280) // 非法数据(坐标超出了)
        {
            if ((mode & 0XF) > 1) // 有其他点有数据,则复第二个触点的数据到第一个触点.
            {
                tp_dev.x[0] = tp_dev.x[1];
                tp_dev.y[0] = tp_dev.y[1];
            }
            else // 非法数据,则忽略此次数据(还原原来的)
            {
                tp_dev.x[0] = tp_dev.x[1];
                tp_dev.y[0] = tp_dev.y[1];
                mode = 0X80;
                tp_dev.sta = tempsta; // 恢复tp_dev.sta
            }
        }
    }

    //		else //无触摸点按下
    //		{
    //        if(tp_dev.sta&0x81) //之前是被按下的
    //				{
    //            tp_dev.sta&=~(1<<7);	//标记按键松开
    //            tp_dev.x[0]=0;
    //            tp_dev.y[0]=0;
    //            tp_dev.sta&=0XE0;	//清除点有效标记
    //        }
    //				else //之前就没有被按下
    //				{
    //            tp_dev.x[0]=0;
    //            tp_dev.y[0]=0;
    //            tp_dev.sta&=0XE0;	//清除点有效标记
    //        }
    //    }

    return res;
}

volatile uint8_t TP_tint_flag = 0;

// 用tim6记录上次按下的时间刻，忽略按下后的200ms之内的按下并清标志把 INT 拉高
// 距上次有效按下 <200ms 的上报只写 0x02 清芯片 INT 标志然后 return,否则芯片无法产生下一次下降沿,后续按下就检测不到了
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1))
    {
        EXTI_ClearITPendingBit(EXTI_Line1);

        if (tim6_tick_ms - last_touch_ms < 200) /* 减慢按住期间的重报 */
        {
            CST816S_WR_DATA(0x02, 0); /* 只清芯片标志,让INT回高——否则下次按下没有边沿 */
            return;
        }
        last_touch_ms = tim6_tick_ms; /* 记录本次有效按下 */

        if (CST816S_Scan() == 1) /* 读到有效触点才动作 */
        {
            if (tp_dev.x[0] < 100)
            {
                if (motor_speed <= 700)
                    motor_speed += 300;
                else
                    motor_speed = 1000;
            }
            else
            {
                if (motor_speed > 300)
                    motor_speed -= 300;
                else
                    motor_speed = 0;
            }
            Motor_Control(motor_speed);
        }
    }
}
