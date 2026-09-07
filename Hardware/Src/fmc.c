#include "fmc.h"

SDRAM_HandleTypeDef hsdram1;

static void sdram_controller_config(void);
static void sdram_powerup_sequence(void);

/*
 * sdram_init: SDRAM 初始化唯一入口(两阶段在内部完成)
 *  阶段1 sdram_controller_config() 配置 FMC 控制器(时钟/引脚/SDCR1+SDTR1)
 *  阶段2 sdram_powerup_sequence()   向芯片发上电协议命令序列
 * 完成后 0xC0000000 起 32MB 即可当普通内存访问
 */
void sdram_init(void)
{
	sdram_controller_config();
	sdram_powerup_sequence();
}

/* 阶段1: 配置 FMC 控制器(W9825G6KH-6I) */
static void sdram_controller_config(void)
{
	FMC_SDRAM_TimingTypeDef sdram_timing = {0};
	GPIO_InitTypeDef gpio_init = {0};
	RCC_PeriphCLKInitTypeDef periph_clk = {0};

	/* FMC 内核时钟源 = D1HCLK(240MHz), SDCLK 再经 SDClockPeriod 分频 = 80MHz */
	periph_clk.PeriphClockSelection = RCC_PERIPHCLK_FMC;
	periph_clk.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
	HAL_RCCEx_PeriphCLKConfig(&periph_clk);

	__HAL_RCC_FMC_CLK_ENABLE();

	/*
	 * FMC GPIO 配置(全部 AF12, VeryHigh):
	 * PE1->NBL1 PE0->NBL0 PG15->SDNCAS PD0->D2 PD1->D3 PG8->SDCLK
	 * PF0..5->A0..5 PG4->BA0 PG5->BA1 PG2->A12 PC0->SDNWE PH2->SDCKE0
	 * PE7..15->D4..12 PH3->SDNE0 PF11->SDNRAS PF12..15->A6..9
	 * PG0->A10 PG1->A11 PD8..10->D13..15 PD14/15->D0/D1
	 */
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_init.Alternate = GPIO_AF12_FMC;

	gpio_init.Pin = GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_10 | GPIO_PIN_9 |
			GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15 | GPIO_PIN_8 |
			GPIO_PIN_13 | GPIO_PIN_7 | GPIO_PIN_14;
	HAL_GPIO_Init(GPIOE, &gpio_init);

	gpio_init.Pin = GPIO_PIN_15 | GPIO_PIN_8 | GPIO_PIN_5 | GPIO_PIN_4 |
			GPIO_PIN_2 | GPIO_PIN_0 | GPIO_PIN_1;
	HAL_GPIO_Init(GPIOG, &gpio_init);

	gpio_init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_15 | GPIO_PIN_14 |
			GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_8;
	HAL_GPIO_Init(GPIOD, &gpio_init);

	gpio_init.Pin = GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_3 |
			GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_13 | GPIO_PIN_14 |
			GPIO_PIN_12 | GPIO_PIN_15 | GPIO_PIN_11;
	HAL_GPIO_Init(GPIOF, &gpio_init);

	gpio_init.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOC, &gpio_init);

	gpio_init.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	HAL_GPIO_Init(GPIOH, &gpio_init);

	/* 芯片几何配置(FMC_SDCR1): 13行 x 9列 x 4bank x 16bit = 32MB */
	hsdram1.Instance = FMC_SDRAM_DEVICE;
	hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
	hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
	hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
	hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
	hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
	hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;	/* 须与 LMR 一致 */
	hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_3;	/* 240MHz/3 = 80MHz */
	hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
	hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;

	/* 时序(FMC_SDTR1): 7 个命令间隔, 单位 SDCLK 周期,
	 * 由手册 AC 参数 ns / 12.5ns(80MHz) 换算; 填小出错, 留余量无害 */
	sdram_timing.LoadToActiveDelay = 2;	/* tMRD */
	sdram_timing.ExitSelfRefreshDelay = 7;	/* tXSR 72ns */
	sdram_timing.SelfRefreshTime = 4;	/* tRAS 42ns */
	sdram_timing.RowCycleDelay = 7;		/* tRC 60ns */
	sdram_timing.WriteRecoveryTime = 3;	/* tWR */
	sdram_timing.RPDelay = 2;		/* tRP 15ns */
	sdram_timing.RCDDelay = 2;		/* tRCD 15ns */

	HAL_SDRAM_Init(&hsdram1, &sdram_timing);
}

/*
 * 阶段2: SDRAM 芯片上电协议序列(W9825G6KH 手册 §7.1/§7.2)
 * 顺序: CLK_ENABLE(拉高CKE) -> PALL -> AREF x8 -> LMR -> 刷新率
 */
static void sdram_powerup_sequence(void)
{
	FMC_SDRAM_CommandTypeDef cmd = {0};
	uint32_t tmpmrd;

	/* 开启 SDRAM 时钟(CKE 拉高, 芯片离开掉电态) */
	cmd.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
	cmd.CommandTarget = FMC_COMMAND_TARGET_BANK;
	cmd.AutoRefreshNumber = 1;
	cmd.ModeRegisterDefinition = 0;

	HAL_SDRAM_SendCommand(&hsdram1, &cmd, SDRAM_TIMEOUT);
	HAL_Delay(1);

	/* PALL: 全预充电(全部行关门, 回到已知状态) */
	cmd.CommandMode = FMC_SDRAM_CMD_PALL;
	cmd.CommandTarget = FMC_COMMAND_TARGET_BANK;
	cmd.AutoRefreshNumber = 1;
	cmd.ModeRegisterDefinition = 0;

	HAL_SDRAM_SendCommand(&hsdram1, &cmd, SDRAM_TIMEOUT);

	/* AREF x8: 上电协议要求的连续自动刷新 */
	cmd.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	cmd.CommandTarget = FMC_COMMAND_TARGET_BANK;
	cmd.AutoRefreshNumber = 8;
	cmd.ModeRegisterDefinition = 0;

	HAL_SDRAM_SendCommand(&hsdram1, &cmd, SDRAM_TIMEOUT);

	/* LMR: 写模式寄存器(BL=2/顺序/CL3/标准模式/单点写) */
	tmpmrd = SDRAM_MODEREG_BURST_LENGTH_2 |
		 SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
		 SDRAM_MODEREG_CAS_LATENCY_3 |
		 SDRAM_MODEREG_OPERATING_MODE_STANDARD |
		 SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	cmd.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	cmd.CommandTarget = FMC_COMMAND_TARGET_BANK;
	cmd.AutoRefreshNumber = 1;
	cmd.ModeRegisterDefinition = tmpmrd;

	HAL_SDRAM_SendCommand(&hsdram1, &cmd, SDRAM_TIMEOUT);

	/*
	 * 刷新率: 605 = (64ms / 8192行) x 80MHz - 20
	 *  W9825 手册 §9.5: tREF = 64ms(64ms 内刷完全部 8192 行)
	 *  -> 每行最大间隔 = 64ms/8192 = 7.8125us
	 *  SDCLK = 240MHz/3 = 80MHz -> 一拍 12.5ns
	 *  7.8125us / 12.5ns = 625 拍, 每 625 拍须插一次 Auto Refresh
	 *  减 20 拍余量(官方惯例): 625 - 20 = 605
	 *  刷新率必须随 SDCLK 重算; 填大漏刷丢数据, 填小仅浪费带宽
	 */
	HAL_SDRAM_ProgramRefreshRate(&hsdram1, 605);
}
