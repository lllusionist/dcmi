#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

#define LED_PIN GPIO_PIN_13
#define LED_GPIO_PORT GPIOC

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
