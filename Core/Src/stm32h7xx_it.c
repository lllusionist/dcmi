#include "main.h"
#include "stm32h7xx_it.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

void NMI_Handler(void)
{

   while (1)
  {
  }

}

void HardFault_Handler(void)
{

  while (1)
  {

  }
}

void MemManage_Handler(void)
{

  while (1)
  {

  }
}

void BusFault_Handler(void)
{

  while (1)
  {

  }
}

void UsageFault_Handler(void)
{

  while (1)
  {

  }
}

/* SVC_Handler / PendSV_Handler 由 FreeRTOS 接管:
 * FreeRTOSConfig.h 里将 vPortSVCHandler/xPortPendSVHandler 别名到这两个标准名,
 * port.c 编译时展开为同名强符号,自动覆盖启动文件的 weak 默认,无需在此定义 */

void DebugMon_Handler(void)
{

}

void SysTick_Handler(void)
{
  extern void xPortSysTickHandler(void);   /* FreeRTOS tick(port 层提供) */

  HAL_IncTick();          /* HAL 时基(支撑 HAL_Delay) */

  /* 仅在调度器运行后喂 RTOS tick(启动调度前 SysTick 已每 1ms 中断) */
  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
  {
    xPortSysTickHandler();
  }
}

void USART1_IRQHandler()
{
  HAL_UART_IRQHandler(&huart1);
}
