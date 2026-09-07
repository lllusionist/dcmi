#ifndef __LED_APP_H__
#define __LED_APP_H__

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
