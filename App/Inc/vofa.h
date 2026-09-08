#ifndef __VOFA_H__
#define __VOFA_H__

#include <stdint.h>

/* float 拆字节(小端), 与官方 bz-foc/printf 一致 */
#define byte0(dw_temp)     (*(char *)(&dw_temp))
#define byte1(dw_temp)     (*((char *)(&dw_temp) + 1))
#define byte2(dw_temp)     (*((char *)(&dw_temp) + 2))
#define byte3(dw_temp)     (*((char *)(&dw_temp) + 3))

#define MAX_BUFFER_SIZE	1024	/* 帧组装缓冲 */

/* 传输后端: 改宏切换 UART / USB(需接入 usbd_cdc_if) */
#define VOFA_TX_UART	1
#define VOFA_TX_USB	0

void vofa_thread_init(void);				/* 阶段1: RX 缓冲 + 钩子(本工程) */
void vofa_thread_start(void);				/* 阶段2: 建 RX 任务(本工程) */
void vofa_transmit(uint8_t *buf, uint16_t len);		/* 通信工具: USART 或 USB */
void vofa_send_data(uint8_t num, float data);		/* 发送接口 */
void vofa_sendframetail(void);				/* 发送帧尾 */

#endif /* __VOFA_H__ */