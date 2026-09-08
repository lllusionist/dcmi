#ifndef __LED_APP_H__
#define __LED_APP_H__

/*
 * LED 模块: 两阶段装配
 *  led_app_init()   阶段1(调度前): GPIO 初始化(快)
 *  led_app_start()  阶段2(所有慢外设初始化后): 创建 Task_LED
 *  led_set_mode()   任意任务可调: 切换 LED 行为
 */

enum led_mode {
	LED_HEARTBEAT,	/* 心跳: 500ms 翻转 */
	LED_FAST_BLINK,	/* 快闪: 100ms 翻转(出错指示) */
	LED_ON,		/* 常亮 */
	LED_OFF,	/* 熄灭 */
};

void led_app_init(void);
void led_app_start(void);
void led_set_mode(int mode);

#endif /* __LED_APP_H__ */