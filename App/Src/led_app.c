#include "led_app.h"
#include "led.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile int s_mode = LED_HEARTBEAT;

/* 唯一 LED 任务: 按模式驱动 GPIO(模块私有) */
static void Task_LED(void *argument)
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

void led_app_init(void)
{
	led_init();
}

/* 阶段2(所有慢速外设初始化完成后调用): 创建 LED 任务 */
void led_app_start(void)
{
	if (xTaskCreate(Task_LED, "LED", 256, NULL, 1, NULL) != pdPASS)
		Error_Handler();
}

void led_set_mode(int mode)
{
	s_mode = mode;
}
