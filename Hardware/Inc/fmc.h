#ifndef __FMC_H__
#define __FMC_H__

#include "main.h"

/* FMC SDRAM 数据基地址(Bank1 映射) */
#define SDRAM_BANK_ADDR			((uint32_t)0xC0000000)

/* SDRAM 命令目标 bank */
#define FMC_COMMAND_TARGET_BANK		FMC_SDRAM_CMD_TARGET_BANK1

/* 命令超时(ms) */
#define SDRAM_TIMEOUT			((uint32_t)0x1000)

/* 模式寄存器位定义(W9825G6KH 手册 §10.4) */
#define SDRAM_MODEREG_BURST_LENGTH_1		((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2		((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4		((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8		((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL	((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED	((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2		((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3		((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD	((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE	((uint16_t)0x0200)

extern SDRAM_HandleTypeDef hsdram1;

void sdram_init(void);

#endif /* __FMC_H__ */
