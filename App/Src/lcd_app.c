#include "lcd_app.h"
#include "lcd_spi_200.h"
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdio.h>
#include <string.h>

/* 屏分辨率(来自 lcd_spi_200.h 宏) */
#define LCD_LINE_Y_START	20
#define LCD_LINE_H		24
#define LCD_MSG_MAX_TEXT		32

/* 一行文本消息 */
struct lcd_msg {
	uint8_t line;
	char text[LCD_MSG_MAX_TEXT];
};

static QueueHandle_t s_q;	/* 显示指令队列(模块私有) */

/* 唯一 LCD 任务: 屏只被它碰(模块私有) */
static void Task_LCD(void *argument)
{
	struct lcd_msg m;
	uint16_t y;

	(void)argument;

	for (;;) {
		if (xQueueReceive(s_q, &m, portMAX_DELAY) == pdTRUE) {
			y = LCD_LINE_Y_START + m.line * LCD_LINE_H;
			LCD_ClearRect(0, y, LCD_Width, LCD_LINE_H);
			LCD_SetColor(LCD_GREEN);
			LCD_DisplayString(10, y, m.text);
		}
	}
}

void lcd_app_init(void)
{
	SPI_LCD_Init();

	LCD_SetBackColor(LCD_BLACK);
	LCD_Clear();
	LCD_SetColor(LCD_GREEN);
	LCD_DisplayString(10, 2, (char *)"FreeRTOS LCD App");

	s_q = xQueueCreate(8, sizeof(struct lcd_msg));
}

/* 阶段2(所有慢速外设初始化完成后调用): 创建 LCD 任务 */
void lcd_app_start(void)
{
	if (xTaskCreate(Task_LCD, "LCD", 256, NULL, 2, NULL) != pdPASS)
		Error_Handler();
}

void lcd_show(uint8_t line, const char *text)
{
	struct lcd_msg m;

	if (s_q == NULL)
		return;
	if (line > (LCD_Height / LCD_LINE_H))
		line = 0;

	m.line = line;
	snprintf(m.text, sizeof(m.text), "%s", text);

	xQueueSend(s_q, &m, 0);
}
