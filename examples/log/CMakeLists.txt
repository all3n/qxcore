# QXCore Log 模块示例配置

# 收集示例源文件
set(QXCORE_LOG_EXAMPLE_SOURCES
    log_example.cc
)

# 创建示例可执行文件
add_executable(qxcore_log_example ${QXCORE_LOG_EXAMPLE_SOURCES})

# 链接依赖
target_link_libraries(qxcore_log_example
    PRIVATE
        QXCore::log
        absl::strings
        absl::status
)

# 根据配置添加后端依赖
if(QXCORE_ENABLE_LOG_SPDLOG)
    target_link_libraries(qxcore_log_example PRIVATE spdlog::spdlog)
endif()

if(QXCORE_ENABLE_LOG_GLOG)
    target_link_libraries(qxcore_log_example PRIVATE glog::glog)
endif()

# 设置编译选项
target_compile_features(qxcore_log_example PRIVATE cxx_std_17)

# 设置输出目录
set_target_properties(qxcore_log_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# 添加线程支持（多线程示例需要）
find_package(Threads REQUIRED)
target_link_libraries(qxcore_log_example PRIVATE Threads::Threads)
