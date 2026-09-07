#include "usart.h"
#include "stm32h7xx_hal_uart.h"

uint8_t rx_tmp_buf[2];
uint8_t rx_buf[256];
uint8_t rx_cnt;

UART_HandleTypeDef huart1;

void uart_init(void)
{
	GPIO_InitTypeDef gpio_init = {0};

	/* IO: PA9(TX) / PA10(RX) 复用 USART1 */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	gpio_init.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_init.Alternate = GPIO_AF7_USART1;

	HAL_GPIO_Init(GPIOA, &gpio_init);

	/* USART1: 115200, 8N1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	__HAL_RCC_USART1_CLK_ENABLE();

	HAL_UART_Init(&huart1);

	/* NVIC */
	HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	/* 启动 1 字节中断接收 */
	HAL_UART_Receive_IT(&huart1, rx_tmp_buf, 1);

	/* printf 关闭行缓冲, 立即输出 */
	setvbuf(stdout, NULL, _IONBF, 0);
}

/* HAL 接收完成回调: 收 1 字节入环形缓冲后重新挂接收 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) {
		rx_buf[rx_cnt++] = rx_tmp_buf[0];
		HAL_UART_Receive_IT(&huart1, rx_tmp_buf, 1);
	}
}

/* printf 底层重定向: newlib _write -> USART1 */
int _write(int fd, char *buf, int size)
{
	(void)fd;
	if (HAL_UART_Transmit(&huart1, (uint8_t *)buf, (uint16_t)size,
			      1000) != HAL_OK)
		return -1;
	return size;
}
