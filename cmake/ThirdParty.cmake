# QXCore 第三方依赖管理模块
# 支持 system 和 bundled 两种模式

include(GNUInstallDirs)

# 宏：添加第三方依赖
# 参数：
#   NAME: 依赖名称（如 absl, spdlog）
#   SOURCE_DIR: 源代码目录路径
#   SYSTEM_NAME: find_package 使用的名称
#   BUNDLED_TARGET: 构建后的目标名称
#   REQUIRED: 是否必需
macro(add_third_party_dependency NAME SOURCE_DIR SYSTEM_NAME BUNDLED_TARGET)
    set(ARG_REQUIRED OFF)
    set(ARG_OPTIONAL OFF)
    
    # 解析参数
    cmake_parse_arguments(ARG "" "" "REQUIRED;OPTIONAL" ${ARGN})
    
    # 创建选项控制使用系统库还是 bundled
    option(USE_SYSTEM_${NAME} "Use system ${NAME} instead of bundled" OFF)
    
    if(USE_SYSTEM_${NAME})
        # 使用系统库
        find_package(${SYSTEM_NAME} ${ARG_REQUIRED} ${ARG_OPTIONAL})
        if(${SYSTEM_NAME}_FOUND)
            message(STATUS "Using system ${NAME}")
            add_library(${NAME} INTERFACE)
            target_link_libraries(${NAME} INTERFACE ${SYSTEM_NAME}::${SYSTEM_NAME})
        else()
            if(ARG_REQUIRED)
                message(FATAL_ERROR "System ${NAME} not found and bundled not enabled")
            else()
                message(WARNING "System ${NAME} not found, ${NAME} support disabled")
            endif()
        endif()
    else()
        # 使用 bundled 源代码
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_DIR}/CMakeLists.txt)
            message(STATUS "Using bundled ${NAME} from ${SOURCE_DIR}")
            
            # 添加子目录构建
            add_subdirectory(${SOURCE_DIR})
            
            # 创建别名目标以便统一使用
            if(NOT TARGET ${NAME})
                add_library(${NAME} INTERFACE)
                target_link_libraries(${NAME} INTERFACE ${BUNDLED_TARGET})
            endif()
            
            # 设置包含目录
            target_include_directories(${NAME} INTERFACE
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>
            )
        else()
            message(FATAL_ERROR "Bundled ${NAME} source not found at ${SOURCE_DIR}")
        endif()
    endif()
    
    # 导出目标
    set(${NAME}_TARGET ${NAME} PARENT_SCOPE)
endmacro()

# 宏：配置第三方依赖的交叉编译支持
macro(configure_third_party_cross_compilation)
    # 为 Zig 交叉编译设置特殊配置
    if(CMAKE_TOOLCHAIN_FILE MATCHES "zig-toolchain")
        
        # 设置 C++ 标准库
        if(ZIG_TARGET MATCHES "windows")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
        endif()
        
        # 禁用一些不兼容的选项
        set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "Disable absl install for cross compilation")
        set(SPDLOG_INSTALL OFF CACHE BOOL "Disable spdlog install for cross compilation")
        
        # 强制设置 Abseil C++ 标准
        set(ABSL_CXX_STANDARD 17 CACHE STRING "C++ standard for Abseil")
        set(ABSL_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require C++17 for Abseil")
        set(ABSL_CXX_EXTENSIONS OFF CACHE BOOL "Disable C++ extensions for Abseil")
        
        # 设置编译选项
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
        
        message(STATUS "Configured third party dependencies for Zig cross compilation: ${ZIG_TARGET}")
    endif()
endmacro()

# 宏：验证第三方依赖
macro(validate_third_party_dependencies)
    message(STATUS "Validating third party dependencies...")
    
    # 检查必需的依赖
    if(QXCORE_ENABLE_LOG_SPDLOG AND NOT TARGET spdlog)
        message(FATAL_ERROR "spdlog target not found")
    endif()
    
    if(QXCORE_ENABLE_LOG_GLOG AND NOT TARGET glog)
        message(FATAL_ERROR "glog target not found") 
    endif()
    
    if(NOT TARGET absl)
        message(FATAL_ERROR "absl target not found")
    endif()
    
    message(STATUS "All third party dependencies validated successfully")
endmacro()
