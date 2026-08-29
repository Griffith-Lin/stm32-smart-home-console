#ifndef __TOUCH_UI_
#define __TOUCH_UI_

#include "stm32f4xx.h"
#include "tc_iic.h"     /* tp_dev / TP_tint_flag */



typedef struct
{
    uint16_t x, y;              /* 区域左上角（LCD 显示坐标） */
    uint16_t w, h;              /* 区域宽高 */
    void (*on_press)(void);     /* 点击回调：主循环执行 */
} TouchRegion_t;

void touch_ui_scan(void);       /* 主循环每圈调用，无事件立即返回，不阻塞 */

#endif

