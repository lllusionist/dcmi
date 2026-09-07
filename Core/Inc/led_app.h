#ifndef __LED_APP_H__
#define __LED_APP_H__

/* LED 模块: 初始化 + API + 常驻任务(标准三层)
 *   led_app_init()  调度前调用: GPIO 初始化 + 创建 Task_LED
 *   led_set_mode()  任何任务可调: 切换 LED 行为 */

typedef enum
{
  LED_HEARTBEAT,   /* 心跳: 500ms 翻转 */
  LED_FAST_BLINK,  /* 快闪: 100ms 翻转(如出错指示) */
  LED_ON,          /* 常亮 */
  LED_OFF          /* 熄灭 */
} LedMode_t;

void led_app_init(void);
void led_set_mode(LedMode_t mode);
void Task_LED(void *argument);   /* 任务函数: 由 main 在全部外设初始化后统一创建 */

#endif /* __LED_APP_H__ */