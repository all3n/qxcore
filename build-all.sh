#!/bin/bash
# 一键构建所有目标
APP_NAME=qxcore

TARGETS=(
   #"x86_64-linux-gnu"
   #"aarch64-linux-gnu"
   #"x86_64-linux-musl"
   #"x86_64-windows-gnu"
   #"x86_64-macos"
    "aarch64-macos"
)

for target in "${TARGETS[@]}"; do
    echo "Building for $target"
    
    build_dir="build/build-$target"
    cmake -B $build_dir \
          -DZIG_TARGET=$target \
          -DCMAKE_TOOLCHAIN_FILE=cmake/zig-toolchain.cmake \
          -DCMAKE_BUILD_TYPE=Release
    
    cmake --build $build_dir --config Release
    
    # 移动输出文件
    if [[ $target == *"windows"* ]]; then
        ext=".exe"
    else
        ext=""
    fi
    
    if [ -f "$build_dir/${APP_NAME}$ext" ]; then
        mkdir -p dist
        cp "$build_dir/${APP_NAME}$ext" "dist/${APP_NAME}-$target$ext"
    fi
done

echo "All builds completed. See dist/ directory."
