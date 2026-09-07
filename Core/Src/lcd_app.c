#include "lcd_app.h"
#include "lcd_spi_200.h"
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdio.h>
#include <string.h>

/* 屏分辨率(来自 lcd_spi_200.h 宏) */
#define LCD_W  LCD_Width
#define LCD_H  LCD_Height

/* 一行文本消息 */
#define LCD_MSG_MAX_TEXT  32
#define LCD_LINE_Y_START  20     /* 第一行文字 Y */
#define LCD_LINE_H        24     /* 行高 */

typedef struct
{
  uint8_t line;
  char    text[LCD_MSG_MAX_TEXT];
} LcdMsg_t;

static QueueHandle_t s_q;              /* 显示指令队列(模块私有) */

/* 层3: 唯一 LCD 任务 —— 屏只被它碰(由 main 统一创建) */
void Task_LCD(void *argument)
{
  (void)argument;
  LcdMsg_t m;

  for (;;)
  {
    if (xQueueReceive(s_q, &m, portMAX_DELAY) == pdTRUE)
    {
      uint16_t y = LCD_LINE_Y_START + m.line * LCD_LINE_H;
      LCD_ClearRect(0, y, LCD_W, LCD_LINE_H);       /* 先清整行, 防叠字 */
      LCD_SetColor(LCD_GREEN);
      LCD_DisplayString(10, y, m.text);
    }
  }
}

/* 层1: 模块初始化(调度前) —— 屏硬件 + 队列 + 创建任务 */
void lcd_app_init(void)
{
  SPI_LCD_Init();               /* SPI5 + 屏幕初始化序列(现有驱动) */

  LCD_SetBackColor(LCD_BLACK);
  LCD_Clear();
  LCD_SetColor(LCD_GREEN);
  LCD_DisplayString(10, 2, (char *)"FreeRTOS LCD App");

  s_q = xQueueCreate(8, sizeof(LcdMsg_t));   /* 容量 8 条指令 */
}

/* 层2: 对外 API —— 非阻塞投递, 队列满则丢弃(显示不该阻塞业务) */
void lcd_show(uint8_t line, const char *text)
{
  LcdMsg_t m;

  if (s_q == NULL)
  {
    return;
  }
  if (line > (LCD_H / LCD_LINE_H))
  {
    line = 0;
  }

  m.line = line;
  snprintf(m.text, sizeof(m.text), "%s", text);

  xQueueSend(s_q, &m, 0);
}