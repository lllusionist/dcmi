#include "sys.h"

/*
 * 系统时钟配置:
 * 25MHz HSE -> PLL1 x192 -> SYSCLK 480MHz
 * HCLK = 240MHz(同时是 FMC 内核时钟源 D1HCLK), APB2 = 120MHz
 * SDCLK = 240MHz / 3 = 80MHz(fmc.c 的 SDClockPeriod 决定)
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef rcc_osc = {0};
	RCC_ClkInitTypeDef rcc_clk = {0};

	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
		;

	rcc_osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	rcc_osc.HSEState = RCC_HSE_ON;
	rcc_osc.PLL.PLLState = RCC_PLL_ON;
	rcc_osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	rcc_osc.PLL.PLLM = 5;
	rcc_osc.PLL.PLLN = 192;
	rcc_osc.PLL.PLLP = 2;
	rcc_osc.PLL.PLLQ = 2;
	rcc_osc.PLL.PLLR = 2;
	rcc_osc.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	rcc_osc.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	rcc_osc.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&rcc_osc) != HAL_OK)
		Error_Handler();

	rcc_clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
			    RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
			    RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
	rcc_clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	rcc_clk.SYSCLKDivider = RCC_SYSCLK_DIV1;
	rcc_clk.AHBCLKDivider = RCC_HCLK_DIV2;
	rcc_clk.APB3CLKDivider = RCC_APB3_DIV2;
	rcc_clk.APB1CLKDivider = RCC_APB1_DIV2;
	rcc_clk.APB2CLKDivider = RCC_APB2_DIV2;
	rcc_clk.APB4CLKDivider = RCC_APB4_DIV2;

	if (HAL_RCC_ClockConfig(&rcc_clk, FLASH_LATENCY_4) != HAL_OK)
		Error_Handler();
}

/* 全局错误处理: 关中断后死循环(便于调试器定位) */
void Error_Handler(void)
{
	__disable_irq();
	while (1)
		;
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
	(void)file;
	(void)line;
}

#endif
