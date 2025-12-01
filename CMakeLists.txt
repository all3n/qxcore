cmake_minimum_required(VERSION 3.16)

# 项目信息
project(QXCore
    VERSION 0.1.0
    DESCRIPTION "High-performance C++ quantitative computing framework"
    LANGUAGES CXX
)

# C++ 标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 选项配置
option(QXCORE_BUILD_TESTS "Build tests" ON)
option(QXCORE_BUILD_EXAMPLES "Build examples" ON)
option(QXCORE_ENABLE_LOG_SPDLOG "Enable spdlog backend" ON)
option(QXCORE_ENABLE_LOG_GLOG "Enable glog backend" OFF)

# CMake 模块路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 包含第三方依赖管理模块
include(ThirdParty)

# 配置交叉编译支持
configure_third_party_cross_compilation()

# 为 Zig 交叉编译强制设置 Abseil C++ 标准
if(CMAKE_TOOLCHAIN_FILE MATCHES "zig-toolchain")
    set(ABSL_CXX_STANDARD 17 CACHE STRING "C++ standard for Abseil")
    set(ABSL_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require C++17 for Abseil")
    set(ABSL_CXX_EXTENSIONS OFF CACHE BOOL "Disable C++ extensions for Abseil")
    
    # 跳过 Abseil 的 C++ 标准检测
    set(ABSL_INTERNAL_AT_LEAST_CXX17 1 CACHE INTERNAL "Skip C++17 check")
    set(ABSL_INTERNAL_AT_LEAST_CXX20 0 CACHE INTERNAL "Skip C++20 check")
endif()

# 包含目录
include_directories(include)

# 添加第三方依赖
add_third_party_dependency(absl third_party/abseil-cpp absl absl REQUIRED)

# 日志后端依赖
if(QXCORE_ENABLE_LOG_SPDLOG)
    add_third_party_dependency(spdlog third_party/spdlog spdlog spdlog)
endif()

if(QXCORE_ENABLE_LOG_GLOG)
    add_third_party_dependency(glog third_party/glog glog glog)
endif()

# 验证第三方依赖
validate_third_party_dependencies()

# 添加子目录
add_subdirectory(src)

if(QXCORE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(QXCORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 安装配置
include(GNUInstallDirs)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# 配置文件生成
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QXCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QXCoreConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QXCore
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QXCoreConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QXCoreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QXCoreConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QXCore
)
