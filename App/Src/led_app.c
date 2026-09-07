#include "led_app.h"
#include "led.h"	/* LED 引脚宏与 led_init(硬件层) */
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile int s_mode = LED_HEARTBEAT;

/* 唯一 LED 任务: 按模式驱动 GPIO */
void Task_LED(void *argument)
{
	(void)argument;

	for (;;) {
		switch (s_mode) {
		case LED_FAST_BLINK:
			HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
			vTaskDelay(100);
			break;
		case LED_ON:
			HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
			vTaskDelay(100);
			break;
		case LED_OFF:
			HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);
			vTaskDelay(100);
			break;
		case LED_HEARTBEAT:
		default:
			HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
			vTaskDelay(500);
			break;
		}
	}
}

/* 模块初始化(调度前): 底层 GPIO */
void led_app_init(void)
{
	led_init();
}

/* 对外 API: 业务代码只发命令, 不碰 GPIO */
void led_set_mode(int mode)
{
	s_mode = mode;
}
