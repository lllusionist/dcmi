#include "led.h"

/* 板载 LED 初始化: PC13 推挽输出, 初始熄灭 */
void led_init(void)
{
	GPIO_InitTypeDef gpio_init = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();

	HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);

	gpio_init.Pin = LED_PIN;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(LED_GPIO_PORT, &gpio_init);
}
