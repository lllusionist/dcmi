#ifndef __LCD_APP_H__
#define __LCD_APP_H__

#include <stdint.h>

/*
 * LCD 显示服务: 初始化 + API + 常驻任务(生产者 -> 消费者)
 *  lcd_app_init()  调度前调用: 屏初始化 + 显示队列
 *  lcd_show()      任意任务可调(非阻塞): 投递一行文本
 *  Task_LCD()      唯一画屏任务, 由 main 统一创建
 */

void lcd_app_init(void);
void lcd_show(uint8_t line, const char *text);
void Task_LCD(void *argument);

#endif /* __LCD_APP_H__ */
