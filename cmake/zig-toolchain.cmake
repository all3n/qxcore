# Zig CMake 工具链
cmake_minimum_required(VERSION 3.15)

# 检测 Zig 是否可用
find_program(ZIG_EXECUTABLE zig REQUIRED)
message(STATUS "Found Zig: ${ZIG_EXECUTABLE}")

# 设置目标（可从命令行覆盖）
if(NOT DEFINED ZIG_TARGET)
    set(ZIG_TARGET "native")
endif()

# 解析目标三元组
if(ZIG_TARGET STREQUAL "native")
    execute_process(
        COMMAND ${ZIG_EXECUTABLE} env ZIG_TARGET
        OUTPUT_VARIABLE ZIG_NATIVE_TARGET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(ZIG_TARGET ${ZIG_NATIVE_TARGET})
endif()

# 设置编译器
set(CMAKE_C_COMPILER ${ZIG_EXECUTABLE})
set(CMAKE_C_COMPILER_ARG1 "cc;-target;${ZIG_TARGET}")

set(CMAKE_CXX_COMPILER ${ZIG_EXECUTABLE})
set(CMAKE_CXX_COMPILER_ARG1 "c++;-target;${ZIG_TARGET}")

# 设置系统信息
if(ZIG_TARGET MATCHES "linux")
    set(CMAKE_SYSTEM_NAME Linux)
elseif(ZIG_TARGET MATCHES "windows")
    set(CMAKE_SYSTEM_NAME Windows)
elseif(ZIG_TARGET MATCHES "darwin|macos")
    set(CMAKE_SYSTEM_NAME Darwin)
else()
    set(CMAKE_SYSTEM_NAME Generic)
endif()

# 提取架构
if(ZIG_TARGET MATCHES "x86_64")
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
elseif(ZIG_TARGET MATCHES "aarch64|arm64")
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
elseif(ZIG_TARGET MATCHES "arm")
    set(CMAKE_SYSTEM_PROCESSOR arm)
else()
    set(CMAKE_SYSTEM_PROCESSOR ${ZIG_TARGET})
endif()

# 设置编译标志
set(CMAKE_C_FLAGS_INIT "")
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")

# 查找设置
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 跳过编译器测试（Zig 可能不通过标准测试）
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

message(STATUS "Using Zig toolchain for target: ${ZIG_TARGET}")
