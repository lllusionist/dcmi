#ifndef __USART_H__
#define __USART_H__

#include "main.h"
#include <stdio.h>

/*
 * USART1 驱动(无 FreeRTOS 依赖)
 * 收: 中断收到字节 -> 调注册的钩子(uart_set_rx_sink)
 * 发: uart_tx_bytes() 块发送(阻塞轮询, 供任务上下文调用)
 */

typedef void (*uart_rx_sink_t)(uint8_t byte);	/* 收字节钩子类型(中断上下文执行) */

void uart_init(void);
void uart_tx_bytes(const uint8_t *buf, uint32_t len);	/* 块发送原语 */
void uart_set_rx_sink(uart_rx_sink_t sink);		/* 注册收字节钩子 */

extern UART_HandleTypeDef huart1;

#endif /* __USART_H__ */