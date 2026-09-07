#include "led_app.h"
#include "led.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile int s_mode = LED_HEARTBEAT;

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

void led_app_init(void)
{
	led_init();
}

void led_set_mode(int mode)
{
	s_mode = mode;
}
