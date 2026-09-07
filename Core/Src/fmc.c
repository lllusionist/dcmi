#include "fmc.h"

SDRAM_HandleTypeDef hsdram1;

static void sdram_controller_config(void);
static void sdram_powerup_sequence(void);

/* sdram_init: SDRAM 初始化唯一入口(两阶段在内部完成,调用方无需关心细节):
 *   阶段1 sdram_controller_config() 配置 FMC 控制器(时钟/引脚/SDCR1+SDTR1)
 *   阶段2 sdram_powerup_sequence()   向芯片发上电协议命令序列
 * 完成后 0xC0000000 起 32MB 即可当普通内存访问 */
void sdram_init(void)
{
    sdram_controller_config();
    sdram_powerup_sequence();
}

/* 阶段1: 配置 FMC 控制器 (W9825G6KH-6I) */
static void sdram_controller_config(void)
{
    FMC_SDRAM_TimingTypeDef SdramTiming = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* FMC 内核时钟源 = D1HCLK(240MHz),SDCLK 再经 SDClockPeriod 分频 = 80MHz */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    __HAL_RCC_FMC_CLK_ENABLE();

    /** FMC GPIO Configuration
    PE1   ------> FMC_NBL1
    PE0   ------> FMC_NBL0
    PG15   ------> FMC_SDNCAS
    PD0   ------> FMC_D2
    PD1   ------> FMC_D3
    PG8   ------> FMC_SDCLK
    PF2   ------> FMC_A2
    PF1   ------> FMC_A1
    PF0   ------> FMC_A0
    PG5   ------> FMC_BA1
    PF3   ------> FMC_A3
    PG4   ------> FMC_BA0
    PG2   ------> FMC_A12
    PF5   ------> FMC_A5
    PF4   ------> FMC_A4
    PC0   ------> FMC_SDNWE
    PH2   ------> FMC_SDCKE0
    PE10   ------> FMC_D7
    PH3   ------> FMC_SDNE0
    PF13   ------> FMC_A7
    PF14   ------> FMC_A8
    PE9   ------> FMC_D6
    PE11   ------> FMC_D8
    PD15   ------> FMC_D1
    PD14   ------> FMC_D0
    PF12   ------> FMC_A6
    PF15   ------> FMC_A9
    PE12   ------> FMC_D9
    PE15   ------> FMC_D12
    PF11   ------> FMC_SDNRAS
    PG0   ------> FMC_A10
    PE8   ------> FMC_D5
    PE13   ------> FMC_D10
    PD10   ------> FMC_D15
    PD9   ------> FMC_D14
    PG1   ------> FMC_A11
    PE7   ------> FMC_D4
    PE14   ------> FMC_D11
    PD8   ------> FMC_D13
    */
    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_9
                            |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_8
                            |GPIO_PIN_13|GPIO_PIN_7|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_5|GPIO_PIN_4
                            |GPIO_PIN_2|GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_14
                            |GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_3
                            |GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_13|GPIO_PIN_14
                            |GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* GPIO_InitStruct */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    
    /* 芯片几何配置(FMC_SDCR1): 13行×9列×4bank×16bit = 32MB,与 W9825G6KH 手册一致 */
    hsdram1.Instance = FMC_SDRAM_DEVICE;
    hsdram1.Init.SDBank = FMC_SDRAM_BANK1;                  /* SDNE0 -> 0xC0000000 */
    hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;   /* 列 9 位 */
    hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;        /* 行 13 位 */
    hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;     /* 16 位总线 */
    hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;/* 4 bank */
    hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;   /* CL3,须与 LMR 一致 */
    hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_3;  /* 240MHz/3=80MHz */
    hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;

    /* 时序(FMC_SDTR1): 7 个命令间隔,单位 SDCLK 周期,
       由手册 AC 参数 ns ÷ 12.5ns(80MHz) 换算,填小了会出错,留余量无害 */
    SdramTiming.LoadToActiveDelay = 2;   /* tMRD */
    SdramTiming.ExitSelfRefreshDelay = 7;/* tXSR 72ns */
    SdramTiming.SelfRefreshTime = 4;     /* tRAS 42ns */
    SdramTiming.RowCycleDelay = 7;       /* tRC 60ns */
    SdramTiming.WriteRecoveryTime = 3;   /* tWR */
    SdramTiming.RPDelay = 2;             /* tRP 15ns */
    SdramTiming.RCDDelay = 2 ;           /* tRCD 15ns */

    HAL_SDRAM_Init(&hsdram1, &SdramTiming);
}

/* 阶段2: SDRAM 芯片上电协议序列(W9825G6KH 手册 §7.1/§7.2)
 * 顺序: CLK_ENABLE(拉高CKE) -> PALL -> AREF×8 -> LMR(模式寄存器) -> 刷新率 */
static void sdram_powerup_sequence(void)
{
	__IO uint32_t tmpmrd = 0;
	FMC_SDRAM_CommandTypeDef Command = {0};

	/* 开启 SDRAM 时钟(CKE 拉高,芯片离开掉电态) */
	Command.CommandMode 				= FMC_SDRAM_CMD_CLK_ENABLE;
	Command.CommandTarget 				= FMC_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber 		= 1;
	Command.ModeRegisterDefinition 	= 0;

	HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);
	HAL_Delay(1);

	/* PALL: 全预充电(全部行关门,回到已知状态) */
	Command.CommandMode 				= FMC_SDRAM_CMD_PALL;
	Command.CommandTarget 				= FMC_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber 		= 1;
	Command.ModeRegisterDefinition 	= 0;

	HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

	/* AREF ×8: 上电协议要求的连续自动刷新 */
	Command.CommandMode 				= FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.CommandTarget 				= FMC_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber			= 8;
	Command.ModeRegisterDefinition 	= 0;

	HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

	/* LMR: 写模式寄存器(BL=2/顺序/CL3/标准模式/单点写) */
	tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2         |
							SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
							SDRAM_MODEREG_CAS_LATENCY_3           |
							SDRAM_MODEREG_OPERATING_MODE_STANDARD |
							SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	Command.CommandMode					= FMC_SDRAM_CMD_LOAD_MODE;
	Command.CommandTarget 				= FMC_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber 		= 1;
	Command.ModeRegisterDefinition 	= tmpmrd;

	HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);
	
	/* 刷新率计算: 605 = (64ms ÷ 8192行) × 80MHz − 20
	 *   W9825 手册 §9.5: tREF = 64ms,即 64ms 内须刷完全部 8192 行
	 *   -> 每行最大间隔 = 64ms/8192 = 7.8125µs
	 *   SDCLK = 240MHz/3 = 80MHz -> 一拍 12.5ns
	 *   7.8125µs ÷ 12.5ns = 625 拍,即每 625 拍须插一次 Auto Refresh
	 *   减 20 拍余量(官方惯例): 625 − 20 = 605
	 *   注意: 刷新率必须随 SDCLK 重算;填大会漏刷丢数据,填小仅浪费带宽 */
	HAL_SDRAM_ProgramRefreshRate(&hsdram1, 605);

}