#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "sys.h"
#include "fmc.h"
#include "mpu.h"
#include "lcd_spi_200.h"

int main(void)
{
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  sdram_init();
  led_init();
  uart_init();
  SPI_LCD_Init();

  while (1)
  {
    HAL_Delay(50);
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
  }
}