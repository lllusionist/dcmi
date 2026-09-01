# STM32H743XIH6 - DCMI 工程

STM32H743 摄像头(DCMI)项目，CubeMX 生成 + CMake 构建，全工程相对路径，可自由拷贝到任意位置。

## 环境要求（Windows）

| 工具 | 说明 | 安装方式 |
|---|---|---|
| MSYS2 | 提供终端与包管理 | 官网安装 [msys2.org](https://www.msys2.org) |
| cmake + make | 构建工具 | MSYS2 中：`pacman -S cmake make` |
| arm-none-eabi-gcc | ARM 交叉编译器 | 官方 [gnu-rm](https://developer.arm.com/downloads/-/gnu-rm)（勾选 Add to PATH），或 `pacman -S mingw-w64-x86_64-arm-none-eabi-gcc` |
| openocd | 烧录/调试（可选） | `pacman -S mingw-w64-x86_64-openocd` |

> 也可以用 **Git Bash** 代替 MSYS2 终端，但建议将工具统一装到 MSYS2（原生版优先）。
> **重要**：请在 **MSYS2 自己的终端**（MSYS2 MINGW64/UCRT64）里构建，不要在 Git Bash 里调用 MSYS2 的 cmake/make。

## 快速开始

```bash
# 1. 首次使用：探测工具链并配置 VS Code IntelliSense（双击运行）
setup.bat

# 2. 一键构建 + 烧录（默认 CMSIS-DAP）
./run.sh

# 3. ST-Link 烧录
./run.sh stlink

# 4. 只编译不烧录
./run.sh build
```

## 手动构建

```bash
cd <工程目录>
rm -rf build
cmake -S . -B build
cmake --build build          # 或: cd build && make -j16
```

产物：`build/dcmi.elf` / `.bin` / `.hex`

## 烧录（手动方式）

```bash
cd build
openocd -f ../download.cfg          # CMSIS-DAP
openocd -f ../download-stlink.cfg   # ST-Link
```

## VS Code 调试

1. 安装扩展：`Cortex-Debug`
2. 首次运行 `setup.bat`（自动配置编译器路径）
3. 按 `F5` 启动 "Debug with OpenOCD"
4. 若 IntelliSense 报错，确认环境变量 `ARM_GCC_BIN` 已设置并重开 VS Code

## 常见问题

- **make 报 `compiler_depend.make: multiple targets match`**
  项目已在 `CMakeLists.txt` 中内置 `set(CMAKE_DEPENDS_USE_COMPILER FALSE)` 修复，请删除 build 目录后重新构建。
- **openocd 找不到接口配置**
  检查烧录器驱动已安装（CMSIS-DAP 免驱 / ST-Link 需驱动）。
- **换电脑后 VS Code 红色波浪线**
  运行 `setup.bat` 后重开 VS Code。

## 目录结构

```
dcmi/
├── Core/                  # 用户代码（main、中断、外设初始化）
├── Drivers/               # HAL 库 + CMSIS
├── cmake/                 # 工具链与 CubeMX 构建文件
├── CMakeLists.txt         # 顶层构建配置（含依赖扫描修复）
├── CMakePresets.json
├── run.sh                 # 一键构建+烧录
├── setup.bat              # 环境配置脚本
├── download*.cfg          # OpenOCD 烧录配置
├── startup_stm32h743xx.s  # 启动文件
├── STM32H743XX_FLASH.ld   # 链接脚本
└── STM32H743.svd          # 调试用外设描述
```
