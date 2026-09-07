#ifndef __LCD_APP_H__
#define __LCD_APP_H__

#include <stdint.h>

/* LCD 显示服务: 初始化 + API + 常驻任务(生产者→消费者)
 *   lcd_app_init()  调度前调用: 屏初始化 + 创建显示队列 + 创建 Task_LCD
 *   lcd_show()      任何任务可调(非阻塞): 提交一行文本, 由唯一 LCD 任务绘制
 * 好处: 多个任务想显示也不会打架(屏只有 Task_LCD 在碰) */

void lcd_app_init(void);
void lcd_show(uint8_t line, const char *text);
void Task_LCD(void *argument);   /* 任务函数: 由 main 在全部外设初始化后统一创建 */

#endif /* __LCD_APP_H__ */