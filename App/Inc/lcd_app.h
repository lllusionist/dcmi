#ifndef __LCD_APP_H__
#define __LCD_APP_H__

#include <stdint.h>

void lcd_app_init(void);
void lcd_show(uint8_t line, const char *text);
void Task_LCD(void *argument);

#endif /* __LCD_APP_H__ */
