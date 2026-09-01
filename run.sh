#!/bin/bash
# ============================================================
#  STM32H743XIH6 一键构建 + 烧录脚本
#  用法:
#    ./run.sh           # 编译并烧录 (默认 CMSIS-DAP)
#    ./run.sh stlink    # 编译并烧录 (ST-Link)
#    ./run.sh build     # 只编译，不烧录
#  要求: arm-none-eabi-gcc / cmake / make / openocd 在 PATH 中
# ============================================================
set -e

# ---------- 定位项目根目录 ----------
SCRIPT_DIR=$(dirname "$(realpath "$0")")
PROJECT_DIR=$SCRIPT_DIR
while [ ! -f "$PROJECT_DIR/CMakeLists.txt" ]; do
  PROJECT_DIR=$(dirname "$PROJECT_DIR")
done
PROJECT_NAME=$(basename "$PROJECT_DIR")
echo "Project: $PROJECT_DIR"

# ---------- 烧录器选择 ----------
MODE="${1:-flash}"
PROBE_CFG="download.cfg"
if [ "$MODE" = "stlink" ]; then
  PROBE_CFG="download-stlink.cfg"
fi

# ---------- 工具链检查 ----------
MISSING=0
for TOOL in arm-none-eabi-gcc cmake make openocd; do
  if ! command -v $TOOL >/dev/null 2>&1; then
    echo "[错误] 缺少工具: $TOOL"
    MISSING=1
  fi
done
if [ $MISSING -eq 1 ]; then
  echo "请先安装缺失工具并加入 PATH，然后重新运行。"
  echo "  - 工具链: https://developer.arm.com/downloads/-/gnu-rm (安装时勾选 Add to PATH)"
  echo "  - cmake/make: MSYS2 中 pacman -S cmake make"
  echo "  - openocd: MSYS2 中 pacman -S mingw-w64-x86_64-openocd"
  exit 1
fi

# ---------- 构建 ----------
BUILD_DIR="$PROJECT_DIR/build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
echo ""
echo "== cmake =="
cmake "$PROJECT_DIR"
echo ""
echo "== make =="
make -j 16

# ---------- 生成 bin/hex ----------
ELF_FILE="$BUILD_DIR/${PROJECT_NAME}.elf"
BIN_FILE="$BUILD_DIR/${PROJECT_NAME}.bin"
HEX_FILE="$BUILD_DIR/${PROJECT_NAME}.hex"
if [ -f "$ELF_FILE" ]; then
  arm-none-eabi-objcopy -O binary "$ELF_FILE" "$BIN_FILE"
  arm-none-eabi-objcopy -O ihex "$ELF_FILE" "$HEX_FILE"
  echo "ELF/BIN/HEX 生成完成: $ELF_FILE"
else
  echo "[错误] ELF 文件未生成，编译可能失败"
  exit 1
fi

# ---------- 烧录 ----------
if [ "$MODE" = "build" ]; then
  echo "已跳过烧录 (build 模式)"
  exit 0
fi
echo ""
echo "== 烧录 ($PROBE_CFG) =="
openocd -f "$PROJECT_DIR/$PROBE_CFG"
echo "烧录完成。"
