#include "mpu.h"

/*
 * MPU 配置: 为 SDRAM 访问放行
 * Region0 = 全 4GB NO_ACCESS 兜底 + SubRegionDisable = 0xC7:
 * 摘出 SRD0(Flash)/SRD1(内部RAM)/SRD2(外设)/SRD6(SDRAM)/SRD7(系统区),
 * 使其落入背景区(特权模式默认放行)。
 */
void MPU_Config(void)
{
	MPU_Region_InitTypeDef mpu = {0};

	HAL_MPU_Disable();

	mpu.Enable = MPU_REGION_ENABLE;
	mpu.Number = MPU_REGION_NUMBER0;
	mpu.BaseAddress = 0x0;
	mpu.Size = MPU_REGION_SIZE_4GB;
	mpu.SubRegionDisable = 0xC7;
	mpu.TypeExtField = MPU_TEX_LEVEL0;
	mpu.AccessPermission = MPU_REGION_NO_ACCESS;
	mpu.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	mpu.IsShareable = MPU_ACCESS_SHAREABLE;
	mpu.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	mpu.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

	HAL_MPU_ConfigRegion(&mpu);

	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
