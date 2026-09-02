#include "usart.h"
#include "stm32h7xx_hal_uart.h"

uint8_t rx_tmp_buf[2];
uint8_t rx_buf[256];
uint8_t rx_cnt = 0;

UART_HandleTypeDef huart1;

void uart_init(void)
{
  // IO
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // USART
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

  __HAL_RCC_USART1_CLK_ENABLE();  // RCC
  
  HAL_UART_Init(&huart1);

  // NVIC
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);

  HAL_UART_Receive_IT(&huart1, rx_tmp_buf, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    rx_buf[rx_cnt++] = rx_tmp_buf[0];
    HAL_UART_Receive_IT(&huart1, rx_tmp_buf, 1);
  }
}
