# QXCore Log 模块构建配置

# 收集头文件
set(QXCORE_LOG_HEADERS
    ${CMAKE_SOURCE_DIR}/include/qxcore/log/log_level.h
    ${CMAKE_SOURCE_DIR}/include/qxcore/log/log.h
    ${CMAKE_SOURCE_DIR}/include/qxcore/log/spdlog_backend.h
    ${CMAKE_SOURCE_DIR}/include/qxcore/log/glog_backend.h
)

# 收集源文件
set(QXCORE_LOG_SOURCES
    log_level.cc
    log.cc
    spdlog_backend.cc
    glog_backend.cc
)

# 创建日志库
add_library(qxcore_log STATIC ${QXCORE_LOG_SOURCES})

# 设置目标属性
target_include_directories(qxcore_log
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接基础依赖
target_link_libraries(qxcore_log
    PUBLIC
        absl::base
        absl::strings
        absl::status
        fmt::fmt
)

# 根据配置添加后端依赖
if(QXCORE_ENABLE_LOG_SPDLOG)
    target_link_libraries(qxcore_log PUBLIC spdlog::spdlog)
    target_compile_definitions(qxcore_log PUBLIC QXCORE_ENABLE_LOG_SPDLOG)
endif()

if(QXCORE_ENABLE_LOG_GLOG)
    target_link_libraries(qxcore_log PUBLIC glog::glog)
    target_compile_definitions(qxcore_log PUBLIC QXCORE_ENABLE_LOG_GLOG)
endif()

# 设置编译选项
target_compile_features(qxcore_log PUBLIC cxx_std_17)

# 创建别名目标以保持兼容性
add_library(QXCore::log ALIAS qxcore_log)

# 导出目标
install(TARGETS qxcore_log
    EXPORT QXCoreTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装头文件
install(FILES ${QXCORE_LOG_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qxcore/log
)
