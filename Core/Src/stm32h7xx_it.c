#include "main.h"
#include "stm32h7xx_it.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

void NMI_Handler(void)
{
	while (1)
		;
}

void HardFault_Handler(void)
{
	while (1)
		;
}

void MemManage_Handler(void)
{
	while (1)
		;
}

void BusFault_Handler(void)
{
	while (1)
		;
}

void UsageFault_Handler(void)
{
	while (1)
		;
}

/*
 * SVC_Handler / PendSV_Handler 由 FreeRTOS 接管:
 * FreeRTOSConfig.h 将 vPortSVCHandler / xPortPendSVHandler
 * 别名到这两个标准名, port.c 展开为强符号覆盖启动文件 weak 默认
 */

void DebugMon_Handler(void)
{
}

void SysTick_Handler(void)
{
	extern void xPortSysTickHandler(void);	/* FreeRTOS tick(port 层提供) */

	HAL_IncTick();				/* HAL 时基(支撑 HAL_Delay) */

	/* 仅调度器运行后喂 RTOS tick(调度前 SysTick 已每 1ms 中断) */
	if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
		xPortSysTickHandler();
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}
