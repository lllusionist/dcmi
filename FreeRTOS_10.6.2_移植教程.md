# STM32H743 + FreeRTOS V10.6.2 移植教程

> 适用：STM32H743（Cortex-M7，arm-none-eabi-gcc，CMake 构建）
> 本教程以本工程（dcmi）实际移植过程为准，讲的是 **FreeRTOS Kernel V10.6.2**
> 如果你抄的代码来自 V11 内核资料，先对照文末"版本差异"一节。

---

## 0. 移植到底要做什么

FreeRTOS 内核（tasks.c / queue.c / list.c ...）与硬件无关，你需要做 4 件事：

| # | 任务 | 产出 |
|---|---|---|
| 1 | 把内核 + 匹配编译器的 port 层放进工程 | `FreeRTOS-Kernel/` 目录 |
| 2 | 写一份针对你芯片的配置 | `Core/Inc/FreeRTOSConfig.h` |
| 3 | 把内核源文件加进构建 | CMakeLists 条目 |
| 4 | 接好 3 个中断 + 1 个时基 | `stm32h7xx_it.c` |

---

## 1. 获取内核并精简

### 1.1 目录结构（V10.6.2 官方）

```
FreeRTOS-Kernel/
├── include/                        ← 内核头文件(FreeRTOS.h 等)
├── croutine.c event_groups.c list.c queue.c
├── stream_buffer.c tasks.c timers.c
└── portable/
    ├── GCC/ARM_CM7/r0p1/           ← ⚠️ GCC 编译器的 CM7 移植层
    │     ├── port.c                (汇编:上下文切换/SysTick 配置)
    │     └── portmacro.h
    └── MemMang/heap_4.c            ← 内存分配器(选 heap_4)
```

**铁律：port 目录必须和编译器匹配**
- GCC 工具链 → `portable/GCC/...`
- Keil(ARMCC) → `portable/RVDS/...`
- IAR → `portable/IAR/...`

用错编译器的 port，`__asm` 语法全会报错。官方包里有 70+ 个编译器/CPU 目录，**只留上面这些，其余全删**（本工程做法）。

## 2. FreeRTOSConfig.h（最关键的配置文件）

新建 `Core/Inc/FreeRTOSConfig.h`。逐个讲必须正确的配置：

```c
#include <stdint.h>
extern uint32_t SystemCoreClock;          /* 来自 system_stm32h7xx.c */
#ifndef CMSIS_device_header
#define CMSIS_device_header "stm32h7xx.h" /* 让内核包含你的 CMSIS 头 */
#endif

/* ── 基础调度 ── */
#define configUSE_PREEMPTION                     1   /* 抢占式 */
#define configSUPPORT_DYNAMIC_ALLOCATION         1   /* 允许 xTaskCreate */
#define configTICK_RATE_HZ                       ((TickType_t)1000)  /* 1ms 节拍 */

/* ── 时钟(必须等于 SysTick 时钟源频率) ── */
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )  /* = 480MHz */
```

### ⚠️ 优先级必须写"8 位寄存器格式"

CM7 的 NVIC 只实现高 4 位优先级(0x00~0xF0)。这个 port 启动时会做检查：
写 0xFF 读回 0xF0 → 断言 `configMAX_SYSCALL & 0xF0 != 0`。
**直接写 15 / 5 会让断言失败死循环**，必须左移：

```c
#ifdef __NVIC_PRIO_BITS
 #define configPRIO_BITS  __NVIC_PRIO_BITS        /* H7 = 4 */
#else
 #define configPRIO_BITS  4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* 15<<4 = 0xF0(最低), 5<<4 = 0x50 */
#define configKERNEL_INTERRUPT_PRIORITY   ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
```

含义：0xF0 = 内核自己(SysTick/PendSV/SVC)用最低优先级；
0x50 = "能用 FromISR API 的中断"的最高优先级（数值比它小的中断不能调 RTOS API）。

### 其余建议值

```c
#define configENABLE_FPU                         1   /* hard-float 编译必须开 */
#define configMAX_PRIORITIES                     ( 8 )
#define configMINIMAL_STACK_SIZE                 ((uint16_t)128)   /* 单位: 字 */
#define configTOTAL_HEAP_SIZE                    ((size_t)32 * 1024)  /* 任务TCB+栈都从这切 */
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              1
#define configUSE_COUNTING_SEMAPHORES            1
#define configCHECK_FOR_STACK_OVERFLOW           2   /* 栈溢出检测(配钩子) */
#define configUSE_TIMERS                         1   /* 软件定时器 */
#define configTIMER_TASK_STACK_DEPTH             256

/* API 裁剪开关(V10 大部分函数仍无条件存在, 按需开) */
#define INCLUDE_vTaskDelay                   1
#define INCLUDE_xTaskDelayUntil              1
#define INCLUDE_vTaskSuspend                 1
#define INCLUDE_xTaskGetSchedulerState       1

/* 断言失败 = 关中断死循环(立刻暴露, 别带病运行) */
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}
```

### ⭐ V10 老式集成的精髓：异常函数别名宏

```c
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
```

**原理**：port.c 编译时这两个名字被预处理展开成 CMSIS 标准中断名（强符号），
自动覆盖启动文件里 `.weak SVC_Handler / PendSV_Handler` 的默认实现。
→ **启动文件向量表一行都不用改**，也不会有"向量指向谁"的纠纷。

## 3. 构建：CMakeLists 加条目

`cmake/stm32cubemx/CMakeLists.txt`（CubeMX 的 CMake 是手工维护的！）：

```cmake
# include 路径(加在 MX_Include_Dirs)
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/portable/GCC/ARM_CM7/r0p1

# 源文件(加在 MX_Application_Src)
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS-Kernel/portable/GCC/ARM_CM7/r0p1/port.c
```

漏一个文件 → 链接 `undefined reference`（常见于忘加 heap_4.c 或 port.c）。

## 4. 中断接线（stm32h7xx_it.c）

### 4.1 删掉空 SVC_Handler / PendSV_Handler

```c
/* 原来的空函数必须删除! 否则与 port.c 展开出的强符号重复定义 */
```

### 4.2 SysTick_Handler 双服务

```c
#include "FreeRTOS.h"
#include "task.h"

void SysTick_Handler(void)
{
  extern void xPortSysTickHandler(void);

  HAL_IncTick();   /* HAL_Delay 依赖的时基 */

  /* 只有调度器运行后才喂 RTOS tick:
   * HAL_Init 之后 SysTick 就开始每 1ms 中断, 但那时内核还没初始化 */
  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
  {
    xPortSysTickHandler();
  }
}
```

## 5. main.c 接入

```c
#include "FreeRTOS.h"
#include "task.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{                                   /* configCHECK_FOR_STACK_OVERFLOW=2 需要 */
  (void)xTask; (void)pcTaskName;
  Error_Handler();
}

static void Task_LED(void *arg)     /* 任务函数: 永不 return */
{
  (void)arg;
  for (;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
    vTaskDelay(500);                /* 让出 CPU */
  }
}

int main(void)
{
  MPU_Config();
  HAL_Init();
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);  /* ⚠️ 必须: 4位抢占分组 */
  SystemClock_Config();
  MX_GPIO_Init();

  /* 一次性硬件初始化全部放调度前 */
  uart_init();
  ...

  xTaskCreate(Task_LED, "Task_LED", 256, NULL, 1, NULL);  /* 栈256字=1KB, 优先级1 */
  vTaskStartScheduler();            /* 启动调度器, 永不返回 */

  while (1) {}                      /* 兜底 */
}
```

### 启动顺序 = 契约

1. `HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4)` —— 不设分组，
   0xF0/0x50 这套 8 位格式全部错乱
2. 外设初始化全部在调度器启动前（SDRAM/串口/屏等，别放任务里做一次性的活）
3. `vTaskStartScheduler()` 后永不返回；若返回 = 堆不够建 idle/timer 任务

---

## 6. 验证

烧录后应该看到：LED 500ms 翻转（任务在跑）。
加上串口心跳（printf + vTaskDelay(500)）能直观确认 tick 频率：
**若心跳节奏不是 0.5s → `configCPU_CLOCK_HZ` 与真实 SysTick 时钟不符**，改它即可。

## 7. 常见故障速查（本工程实战踩过的）

| 现象 | 原因 | 处理 |
|---|---|---|
| 复位后立刻 HardFault(BusFault) | SysTick 在调度前喂了 RTOS tick | 用 4.2 的"调度状态判断"写法 |
| 死在 configASSERT 死循环 | 优先级写了 15/5 裸值 | 改成 `<< (8-configPRIO_BITS)` 8 位格式 |
| 链接 undefined reference | CMake 漏加源文件 | 对照第 3 节清单 |
| 任务栈溢出 | 任务函数栈不够 / 递归 | 开 stack overflow 检查 + 钩子 |
| tick 快慢不对 | configCPU_CLOCK_HZ 错 | 心跳校准 |

## 8. 版本差异速查（V10.6.2 vs V11.x）

你搜到的很多教程/代码是 V10 时代（本教程适用）；V11 内核有这些差别：

| 项 | V10.6.2 | V11.x |
|---|---|---|
| 异常接线 | FreeRTOSConfig 别名宏(本教程) | 改 startup 向量 + 关 `configCHECK_HANDLER_INSTALLATION` |
| `INCLUDE_vTaskDelay` | 默认在 | 变成可选, 不定义链接报错 |
| `INCLUDE_vTaskDelayUntil` | 有效 | 已废弃, 与 xTaskDelayUntil 同定义即报错 |
| 优先级格式 | 同样要 8 位格式 | 同样要 8 位格式 |

> 一句话：**本教程的配置写法在 V10.6.2 上是官方推荐路径（ST CubeMX 集成同款）**。
