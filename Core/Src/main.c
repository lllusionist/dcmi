#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "sys.h"
#include "fmc.h"
#include "mpu.h"
#include "led_app.h"
#include "lcd_app.h"
#include "FreeRTOS.h"
#include "task.h"

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

  if (xTaskCreate(Task_LED, "LED", 256, NULL, 1, NULL) != pdPASS)
  {
    Error_Handler();
  }
  if (xTaskCreate(Task_LCD, "LCD", 256, NULL, 2, NULL) != pdPASS)
  {
    Error_Handler();
  }

  vTaskStartScheduler();

  while (1)
  {
  }
}

/* 栈溢出钩子(由 configCHECK_FOR_STACK_OVERFLOW=2 启用) */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  (void)xTask;
  (void)pcTaskName;
  Error_Handler();
}