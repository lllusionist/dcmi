#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "sys.h"
#include "fmc.h"
#include "mpu.h"
#include "led_app.h"
#include "lcd_app.h"
#include "vofa.h"
#include "FreeRTOS.h"
#include "task.h"

/* 栈溢出钩子(由 configCHECK_FOR_STACK_OVERFLOW=2 启用) */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	(void)xTask;
	(void)pcTaskName;
	Error_Handler();
}

int main(void)
{
	MPU_Config();
	HAL_Init();
	/* FreeRTOS 前提: 4 位抢占优先级分组 */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	SystemClock_Config();
	MX_GPIO_Init();

	sdram_init();
	uart_init();
	led_app_init();
	lcd_app_init();
	vofa_thread_init();
	led_app_start();
	lcd_app_start();
	vofa_thread_start();

	vTaskStartScheduler();

	while (1)
		;
}
