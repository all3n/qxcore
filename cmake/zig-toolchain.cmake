# Zig CMake 工具链
cmake_minimum_required(VERSION 3.15)

if (USE_ZIG)
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
    
    # 创建编译器包装器
    set(COMPILER_WRAPPER_DIR ${CMAKE_BINARY_DIR}/compiler_wrappers)
    file(MAKE_DIRECTORY ${COMPILER_WRAPPER_DIR})
    
    # C 编译器包装器
    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/zig-cc-wrapper.in
        ${COMPILER_WRAPPER_DIR}/zig-cc
        @ONLY
    )
    
    # C++ 编译器包装器
    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/zig-cxx-wrapper.in
        ${COMPILER_WRAPPER_DIR}/zig-cxx
        @ONLY
    )
    
    # 设置可执行权限
    execute_process(
        COMMAND chmod +x ${COMPILER_WRAPPER_DIR}/zig-cc ${COMPILER_WRAPPER_DIR}/zig-cxx
    )
    
    # 设置编译器
    set(CMAKE_C_COMPILER ${COMPILER_WRAPPER_DIR}/zig-cc)
    set(CMAKE_CXX_COMPILER ${COMPILER_WRAPPER_DIR}/zig-cxx)
    
    # 设置 ar 工具
    set(CMAKE_AR "${ZIG_EXECUTABLE} ar")
    set(CMAKE_RANLIB "${ZIG_EXECUTABLE} ranlib")
    
    # 设置静态库创建命令
    set(CMAKE_C_CREATE_STATIC_LIBRARY "${CMAKE_AR} rcs <TARGET> <OBJECTS>")
    set(CMAKE_CXX_CREATE_STATIC_LIBRARY "${CMAKE_AR} rcs <TARGET> <OBJECTS>")
    
    # 设置编译标志
    set(CMAKE_C_FLAGS_INIT "")
    set(CMAKE_CXX_FLAGS_INIT "-std=c++17")
    
    # 强制设置 C++ 标准
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    
    # 平台特定的编译标志
    if(ZIG_TARGET MATCHES "windows")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
        # Windows 特定的宏定义
        add_definitions(-DHAS_STRPTIME=0)
        # 抑制未使用参数警告
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
    elseif(ZIG_TARGET MATCHES "linux")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
        # Linux 特定的宏定义
        add_definitions(-DHAS_STRPTIME=1)
    elseif(ZIG_TARGET MATCHES "darwin|macos")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        # macOS 特定的宏定义
        add_definitions(-DHAS_STRPTIME=1)
    endif()
    
    # 添加通用警告抑制
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gcc-compat -Wno-unknown-pragmas -Wno-error")
    
    # 设置库查找路径
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    
    # 跳过编译器测试（Zig 可能不通过标准测试）
    set(CMAKE_C_COMPILER_FORCED TRUE)
    set(CMAKE_CXX_COMPILER_FORCED TRUE)
    
    # 强制设置 C++ 标准相关变量，跳过检测
    set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT 17)
    set(CMAKE_CXX_STANDARD_DEFAULT 17)
    set(CMAKE_CXX_EXTENSIONS_DEFAULT OFF)
    
    # 设置编译器特性映射（解决 target_compile_features 问题）
    set(CMAKE_CXX_COMPILE_FEATURES cxx_std_11;cxx_std_14;cxx_std_17;cxx_std_20)
    set(CMAKE_CXX_KNOWN_FEATURES cxx_std_11;cxx_std_14;cxx_std_17;cxx_std_20)
    
    # 为 Abseil 设置特殊变量
    set(ABSL_CXX_STANDARD 17)
    set(ABSL_CXX_STANDARD_REQUIRED ON)
    set(ABSL_CXX_EXTENSIONS OFF)
    
    # 设置交叉编译的根路径
    if(ZIG_TARGET MATCHES "windows")
        set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32")
    elseif(ZIG_TARGET MATCHES "linux")
        set(CMAKE_FIND_ROOT_PATH "/usr/aarch64-linux-gnu")
    endif()
    
    # 为第三方依赖设置特殊变量
    set(ZIG_CROSS_COMPILING TRUE)
    set(CMAKE_CROSSCOMPILING TRUE)
    
    # 禁用一些不兼容的功能
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    
    # 为 Zig 交叉编译设置线程支持
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    set(CMAKE_USE_PTHREADS_INIT TRUE)
    
    # 强制设置线程相关变量
    set(CMAKE_HAVE_LIBC_PTHREAD 1)
    set(Threads_FOUND TRUE)
    set(CMAKE_THREAD_LIBS_INIT "-pthread")
    set(CMAKE_THREAD_LIBS_INIT "-pthread")
    
    # 设置 RPATH
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    
    message(STATUS "Using Zig toolchain for target: ${ZIG_TARGET}")
    message(STATUS "Cross-compilation flags: ${CMAKE_CXX_FLAGS}")
endif()
