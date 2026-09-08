#include "vofa.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"
#include <stdio.h>

#define VOFA_RX_BUF_SIZE	128	/* RX 流缓冲容量 */
#define VOFA_TX_BUF_SIZE	256	/* TX 流缓冲容量 */

static StreamBufferHandle_t vofa_rx_sb;
static StreamBufferHandle_t vofa_tx_sb;

/* JustFloat 帧尾(小端) */
static const uint8_t vofa_frame_tail[4] = { 0x00, 0x00, 0x80, 0x7F };

static uint8_t send_buf[MAX_BUFFER_SIZE];
static uint16_t cnt;

static void vofa_rx_byte(uint8_t byte)
{
	BaseType_t woken = pdFALSE;

	xStreamBufferSendFromISR(vofa_rx_sb, &byte, 1, &woken);
	portYIELD_FROM_ISR(woken);
}

static void task_vofa_rx(void *arg)
{
	uint8_t byte;
	uint8_t line[32];
	uint8_t reply[40];
	uint32_t idx = 0;
	int n;

	(void)arg;
	for (;;) {
		xStreamBufferReceive(vofa_rx_sb, &byte, 1, portMAX_DELAY);

		if (byte == '\r' || byte == '\n') {	/* 一行结束 */
			if (idx > 0) {
				line[idx] = '\0';

				n = snprintf((char *)reply, sizeof(reply),
					     "echo:%s\r\n", line);
				xStreamBufferSend(vofa_tx_sb, reply, (size_t)n, 0);	/* 回显: 入 TX 缓冲 */

				idx = 0;
			}
		} else if (idx < sizeof(line) - 1) {
			line[idx++] = byte;	/* 攒行 */
		}
	}
}

static void task_vofa_tx(void *arg)
{
	uint8_t chunk[64];
	size_t got;

	(void)arg;
	for (;;) {
		got = xStreamBufferReceive(vofa_tx_sb, chunk, sizeof(chunk),
					   portMAX_DELAY);
		if (got > 0)
			vofa_transmit(chunk, (uint16_t)got);
	}
}

void vofa_thread_init(void)
{
	vofa_rx_sb = xStreamBufferCreate(VOFA_RX_BUF_SIZE, 1);
	vofa_tx_sb = xStreamBufferCreate(VOFA_TX_BUF_SIZE, 1);

	uart_set_rx_sink(vofa_rx_byte);
}

void vofa_thread_start(void)
{
	if (xTaskCreate(task_vofa_rx, "vofa_rx", 256, NULL, 2, NULL) != pdPASS)
		Error_Handler();
	if (xTaskCreate(task_vofa_tx, "vofa_tx", 256, NULL, 1, NULL) != pdPASS)
		Error_Handler();
}

void vofa_transmit(uint8_t *buf, uint16_t len)
{
#if VOFA_TX_USB
	
#else
	uart_tx_bytes(buf, len);
#endif
}

void vofa_send_data(uint8_t num, float data)
{
	(void)num;

	if (cnt + 4 > MAX_BUFFER_SIZE)
		return;

	send_buf[cnt++] = byte0(data);
	send_buf[cnt++] = byte1(data);
	send_buf[cnt++] = byte2(data);
	send_buf[cnt++] = byte3(data);
}

void vofa_sendframetail(void)
{
	if (cnt + 4 > MAX_BUFFER_SIZE) {
		cnt = 0;
		return;
	}

	send_buf[cnt++] = vofa_frame_tail[0];
	send_buf[cnt++] = vofa_frame_tail[1];
	send_buf[cnt++] = vofa_frame_tail[2];
	send_buf[cnt++] = vofa_frame_tail[3];

	xStreamBufferSend(vofa_tx_sb, send_buf, cnt, 0);
	cnt = 0;
}

void task_vofa(void *arg)
{
	for(;;)
	{
		// vofa_send_data(0, test);
		// vofa_send_data(1, test*2);
		// vofa_sendframetail();
		vTaskDelay(10);
	}
}