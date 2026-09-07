#ifndef __LED_APP_H__
#define __LED_APP_H__

/*
 * LED 模块: 初始化 + API + 常驻任务
 *  led_app_init()  调度前调用: GPIO 初始化
 *  led_set_mode()  任意任务可调: 切换 LED 行为
 *  Task_LED()      任务函数, 由 main 统一创建
 */

enum led_mode {
	LED_HEARTBEAT,	/* 心跳: 500ms 翻转 */
	LED_FAST_BLINK,	/* 快闪: 100ms 翻转(出错指示) */
	LED_ON,		/* 常亮 */
	LED_OFF,	/* 熄灭 */
};

void led_app_init(void);
void led_set_mode(int mode);
void Task_LED(void *argument);

#endif /* __LED_APP_H__ */
