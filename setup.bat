@echo off
chcp 65001 >nul
setlocal EnableDelayedExpansion

echo ============================================
echo  STM32H743XIH6 工程环境配置
echo ============================================
echo.

REM ---------- 1. 探测 arm-none-eabi-gcc ----------
set "GCC_DIR="

where arm-none-eabi-gcc >nul 2>&1
if %errorlevel%==0 (
    for /f "delims=" %%i in ('where arm-none-eabi-gcc') do (
        set "GCC_FULL=%%i"
        goto :found
    )
)

REM ---------- 2. 常见安装位置搜索 ----------
for %%P in (
    "%USERPROFILE%\arm-gnu-toolchain*\bin"
    "D:\arm-gnu-toolchain*\bin"
    "C:\arm-gnu-toolchain*\bin"
    "C:\ST\STM32CubeIDE*\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*\tools\bin"
    "C:\Program Files*\STMicroelectronics\STM32CubeIDE*\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*\tools\bin"
    "%ProgramFiles%\STMicroelectronics\STM32CubeIDE*\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*\tools\bin"
) do (
    if exist "%%P\arm-none-eabi-gcc.exe" (
        set "GCC_FULL=%%P\arm-none-eabi-gcc.exe"
        goto :found
    )
)

echo [错误] 未找到 arm-none-eabi-gcc 工具链！
echo.
echo 请先安装 ARM GNU 工具链，任选一种方式：
echo   1. 官方安装包:  https://developer.arm.com/downloads/-/gnu-rm
echo      安装时勾选 "Add to PATH"；或安装后运行本脚本。
echo   2. MSYS2:       pacman -S mingw-w64-x86_64-arm-none-eabi-gcc
echo   3. STM32CubeIDE 自带的工具链（安装本工程同款 IDE 即可）。
echo.
echo 安装完成后重新运行本脚本。
pause
exit /b 1

:found
set "GCC_DIR=%~dp0"
for %%i in ("!GCC_FULL!") do set "GCC_DIR=%%~dpi"

echo [1/3] 找到 ARM GCC: !GCC_FULL!
echo.

REM ---------- 3. 写入用户环境变量 ARM_GCC_BIN ----------
set "GCC_BIN=!GCC_DIR:~0,-1!"
if /i "!GCC_BIN!"=="" goto :bad

setx ARM_GCC_BIN "!GCC_BIN!" >nul
if errorlevel 1 (
    echo [警告] 环境变量写入失败，请手动设置:
    echo        setx ARM_GCC_BIN "!GCC_BIN!"
) else (
    echo [2/3] 环境变量 ARM_GCC_BIN 已设置: !GCC_BIN!
)
echo.

REM ---------- 4. 工具检查 ----------
echo [3/3] 检查其他工具...
where cmake >nul 2>&1 && echo     - cmake   OK || echo     - cmake   缺失(MSYS2: pacman -S cmake)
where make >nul 2>&1 && echo     - make    OK || echo     - make    缺失(MSYS2: pacman -S make)
where openocd >nul 2>&1 && echo     - openocd OK || echo     - openocd 缺失(可选，烧录用)
echo.

echo ============================================
echo  配置完成!
echo  * 请【重新打开】VS Code，IntelliSense 将自动生效
echo  * 在 MSYS2/Git Bash 中执行 ./run.sh 一键编译+烧录
echo ============================================
pause
exit /b 0

:bad
echo [错误] 无法解析工具链路径。
pause
exit /b 1
