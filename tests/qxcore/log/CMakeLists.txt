# QXCore Log 模块测试配置

# 收集测试源文件
set(QXCORE_LOG_TEST_SOURCES
    log_level_test.cc
    spdlog_backend_test.cc
    glog_backend_test.cc
    log_test.cc
    consistency_test.cc
)

# 创建测试可执行文件
add_executable(qxcore_log_tests ${QXCORE_LOG_TEST_SOURCES})

# 链接依赖
target_link_libraries(qxcore_log_tests
    PRIVATE
        QXCore::log
        GTest::gtest
        GTest::gtest_main
        absl::strings
        absl::status
)

# 根据配置添加后端依赖
if(QXCORE_ENABLE_LOG_SPDLOG)
    target_link_libraries(qxcore_log_tests PRIVATE spdlog::spdlog)
endif()

if(QXCORE_ENABLE_LOG_GLOG)
    target_link_libraries(qxcore_log_tests PRIVATE glog::glog)
endif()

# 设置编译选项
target_compile_features(qxcore_log_tests PRIVATE cxx_std_17)

# 添加测试到 CTest
add_test(NAME QXCoreLogTests COMMAND qxcore_log_tests)

# 性能基准测试
find_package(benchmark QUIET)
if(benchmark_FOUND)
    set(QXCORE_LOG_BENCHMARK_SOURCES
        log_benchmark.cc
    )
    
    add_executable(qxcore_log_benchmarks ${QXCORE_LOG_BENCHMARK_SOURCES})
    
    target_link_libraries(qxcore_log_benchmarks
        PRIVATE
            QXCore::log
            benchmark::benchmark
            benchmark::benchmark_main
    )
    
    # 根据配置添加后端依赖
    if(QXCORE_ENABLE_LOG_SPDLOG)
        target_link_libraries(qxcore_log_benchmarks PRIVATE spdlog::spdlog)
    endif()
    
    if(QXCORE_ENABLE_LOG_GLOG)
        target_link_libraries(qxcore_log_benchmarks PRIVATE glog::glog)
    endif()
    
    target_compile_features(qxcore_log_benchmarks PRIVATE cxx_std_17)
    
    # 添加基准测试
    add_test(NAME QXCoreLogBenchmarks COMMAND qxcore_log_benchmarks --benchmark_min_time=0.1)
endif()
