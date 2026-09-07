#include "led_app.h"
#include "gpio.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

/* 当前模式(单字节写,任务间无竞争; volatile 防编译器缓存) */
static volatile LedMode_t s_mode = LED_HEARTBEAT;

/* 唯一 LED 任务: 读状态决定行为(标准三层中的"层3"), 由 main 统一创建 */
void Task_LED(void *argument)
{
  (void)argument;
  for (;;)
  {
    switch (s_mode)
    {
      case LED_FAST_BLINK:
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
        vTaskDelay(100);
        break;

      case LED_ON:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
        vTaskDelay(100);          /* 保持住即可, 无需急循环 */
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

/* 层1: 模块初始化(调度前调用) —— 只做 GPIO, 任务由 main 统一装配 */
void led_app_init(void)
{
  led_init();   /* 底层 GPIO 配置(原 gpio.c) */
}

/* 层2: 对外 API —— 业务代码只管发命令, 不碰 GPIO */
void led_set_mode(LedMode_t mode)
{
  s_mode = mode;
}