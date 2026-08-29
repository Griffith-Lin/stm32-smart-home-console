#include "touch_ui.h"
#include "wavplay.h"    /* status_dev / PLAY_* */
#include "audioplay.h"  /* audiodev */

/* --- 三个图标的示例业务（与红外 deal_if 语义一致，替换成你自己的业务） --- */
static void on_last_press(void)  { status_dev.PlayState = PLAY_PREVIOUS; }
static void on_next_press(void)  { status_dev.PlayState = PLAY_NEXT; }

static void on_play_press(void)
{
    if (status_dev.PlayState == PLAY_CLEAR)      /* 播放空闲时才响应 */
    {
        if (audiodev.status & 0X01)              /* 播放中 -> 暂停 */
            status_dev.PlayState = PLAY_PAUSE;
        else                                     /* 暂停 -> 继续 */
            status_dev.PlayState = PLAY_PLAY;
    }
}

static TouchRegion_t touch_regions[] =
{
    {50, 150, 44, 44, on_last_press},    /* LCD_Dis_Pic(150,50, gImage_last)  */
    {100, 150, 46, 46, on_play_press},   /* LCD_Dis_Pic(150,100,gImage_play)  */
    {150, 150, 44, 44, on_next_press},   /* LCD_Dis_Pic(150,150,gImage_next)  */
    /* 在此追加：{x, y, w, h, 回调}；区域坐标 = LCD_Dis_Pic 的 (col, row)，宽高取位图 pic[2..5] */
};

void touch_ui_scan(void)
{
    uint8_t i;

//因为歌曲播放时是while死循环，只能通过中断的方式实现触摸事件
//    if (TP_tint_flag == 0)                       /* 无新触摸事件 */
//        return;
//    TP_tint_flag = 0;                            /* 消费事件 */

    for (i = 0; i < sizeof(touch_regions) / sizeof(TouchRegion_t); i++)
    {
        TouchRegion_t *r = &touch_regions[i];
        if (tp_dev.x[0] >= r->x && tp_dev.x[0] < r->x + r->w &&
            tp_dev.y[0] >= r->y && tp_dev.y[0] < r->y + r->h)
        {
            if (r->on_press)
                r->on_press();                   /* 回调在主循环执行，不阻塞中断 */
            break;
        }
    }
}
