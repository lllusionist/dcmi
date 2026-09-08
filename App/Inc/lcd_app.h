#ifndef __LCD_APP_H__
#define __LCD_APP_H__

#include <stdint.h>

/*
 * LCD 显示服务: 两阶段装配
 *  lcd_app_init()   阶段1(调度前): 屏初始化(慢!) + 显示队列
 *  lcd_app_start()  阶段2(所有慢速外设初始化后): 创建 Task_LCD
 *  lcd_show()       任意任务可调(非阻塞): 投递一行文本
 */

void lcd_app_init(void);
void lcd_app_start(void);
void lcd_show(uint8_t line, const char *text);

#endif /* __LCD_APP_H__ */