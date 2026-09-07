#ifndef __LED_H__
#define __LED_H__

#include "main.h"

/* 板载 LED 引脚定义 */
#define LED_PIN		GPIO_PIN_13
#define LED_GPIO_PORT	GPIOC

void led_init(void);	/* 配置 LED 引脚为推挽输出(初始灭) */

#endif /* __LED_H__ */
