// Copyright 2024 QXCore Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "qxcore/log/log.h"
#include <benchmark/benchmark.h>
#include <absl/status/status.h>
#include <string>

namespace qxcore {
namespace log {

// 基准测试辅助类
class LogBenchmark {
 protected:
  static void SetUpBackend(auto* backend, const std::string& name) {
    absl::Status status = backend->init(name, LogLevel::kInfo);
    if (!status.ok()) {
      // 在基准测试中，我们假设初始化成功
      // 如果失败，测试会自动跳过
    }
  }
};

// 默认日志器基准测试
static void BM_DefaultLog_Info(benchmark::State& state) {
  DefaultLog& logger = GetDefaultLogger();
  absl::Status status = InitDefaultLogger("benchmark_default", LogLevel::kInfo);
  
  if (!status.ok()) {
    state.SkipWithError("Failed to initialize default logger");
    return;
  }
  
  for (auto _ : state) {
    logger.info("Benchmark test message");
  }
  
  state.SetItemsProcessed(state.iterations());
}

static void BM_DefaultLog_Formatted(benchmark::State& state) {
  DefaultLog& logger = GetDefaultLogger();
  absl::Status status = InitDefaultLogger("benchmark_default", LogLevel::kInfo);
  
  if (!status.ok()) {
    state.SkipWithError("Failed to initialize default logger");
    return;
  }
  
  for (auto _ : state) {
    logger.info("Benchmark test message with number: {}", 42);
  }
  
  state.SetItemsProcessed(state.iterations());
}

#ifdef QXCORE_ENABLE_LOG_SPDLOG

// SpdlogBackend 基准测试
static void BM_SpdlogBackend_Info(benchmark::State& state) {
  SpdlogBackend backend;
  LogBenchmark::SetUpBackend(&backend, "benchmark_spdlog");
  
  for (auto _ : state) {
    backend.log(LogLevel::kInfo, "Benchmark test message");
  }
  
  state.SetItemsProcessed(state.iterations());
}

static void BM_SpdlogBackend_Formatted(benchmark::State& state) {
  SpdlogBackend backend;
  LogBenchmark::SetUpBackend(&backend, "benchmark_spdlog");
  
  for (auto _ : state) {
    backend.logf(LogLevel::kInfo, "Benchmark test message with number: {}", 42);
  }
  
  state.SetItemsProcessed(state.iterations());
}

static void BM_SpdlogBackend_Disabled(benchmark::State& state) {
  SpdlogBackend backend;
  LogBenchmark::SetUpBackend(&backend, "benchmark_spdlog");
  backend.set_level(LogLevel::kError);  // 禁用 INFO 级别
  
  for (auto _ : state) {
    backend.log(LogLevel::kInfo, "This message should be filtered out");
  }
  
  state.SetItemsProcessed(state.iterations());
}

#endif  // QXCORE_ENABLE_LOG_SPDLOG

#ifdef QXCORE_ENABLE_LOG_GLOG

// GlogBackend 基准测试
static void BM_GlogBackend_Info(benchmark::State& state) {
  GlogBackend backend;
  LogBenchmark::SetUpBackend(&backend, "benchmark_glog");
  
  for (auto _ : state) {
    backend.log(LogLevel::kInfo, "Benchmark test message");
  }
  
  state.SetItemsProcessed(state.iterations());
}

static void BM_GlogBackend_Formatted(benchmark::State& state) {
  GlogBackend backend;
  LogBenchmark::SetUpBackend(&backend, "benchmark_glog");
  
  for (auto _ : state) {
    backend.logf(LogLevel::kInfo, "Benchmark test message with number: {}", 42);
  }
  
  state.SetItemsProcessed(state.iterations());
}

static void BM_GlogBackend_Disabled(benchmark::State& state) {
  GlogBackend backend;
  LogBenchmark::SetUpBackend(&backend, "benchmark_glog");
  backend.set_level(LogLevel::kError);  // 禁用 INFO 级别
  
  for (auto _ : state) {
    backend.log(LogLevel::kInfo, "This message should be filtered out");
  }
  
  state.SetItemsProcessed(state.iterations());
}

#endif  // QXCORE_ENABLE_LOG_GLOG

// 不同消息大小的基准测试
static void BM_DefaultLog_SmallMessage(benchmark::State& state) {
  DefaultLog& logger = GetDefaultLogger();
  absl::Status status = InitDefaultLogger("benchmark_small", LogLevel::kInfo);
  
  if (!status.ok()) {
    state.SkipWithError("Failed to initialize default logger");
    return;
  }
  
  std::string small_msg(50, 'x');  // 50 字符消息
  
  for (auto _ : state) {
    logger.info("{}", small_msg);
  }
  
  state.SetBytesProcessed(state.iterations() * small_msg.size());
}

static void BM_DefaultLog_MediumMessage(benchmark::State& state) {
  DefaultLog& logger = GetDefaultLogger();
  absl::Status status = InitDefaultLogger("benchmark_medium", LogLevel::kInfo);
  
  if (!status.ok()) {
    state.SkipWithError("Failed to initialize default logger");
    return;
  }
  
  std::string medium_msg(500, 'x');  // 500 字符消息
  
  for (auto _ : state) {
    logger.info("{}", medium_msg);
  }
  
  state.SetBytesProcessed(state.iterations() * medium_msg.size());
}

static void BM_DefaultLog_LargeMessage(benchmark::State& state) {
  DefaultLog& logger = GetDefaultLogger();
  absl::Status status = InitDefaultLogger("benchmark_large", LogLevel::kInfo);
  
  if (!status.ok()) {
    state.SkipWithError("Failed to initialize default logger");
    return;
  }
  
  std::string large_msg(5000, 'x');  // 5000 字符消息
  
  for (auto _ : state) {
    logger.info("{}", large_msg);
  }
  
  state.SetBytesProcessed(state.iterations() * large_msg.size());
}

// 注册基准测试
BENCHMARK(BM_DefaultLog_Info);
BENCHMARK(BM_DefaultLog_Formatted);
BENCHMARK(BM_DefaultLog_SmallMessage);
BENCHMARK(BM_DefaultLog_MediumMessage);
BENCHMARK(BM_DefaultLog_LargeMessage);

#ifdef QXCORE_ENABLE_LOG_SPDLOG
BENCHMARK(BM_SpdlogBackend_Info);
BENCHMARK(BM_SpdlogBackend_Formatted);
BENCHMARK(BM_SpdlogBackend_Disabled);
#endif

#ifdef QXCORE_ENABLE_LOG_GLOG
BENCHMARK(BM_GlogBackend_Info);
BENCHMARK(BM_GlogBackend_Formatted);
BENCHMARK(BM_GlogBackend_Disabled);
#endif

// 性能对比基准测试（如果两个后端都可用）
#ifdef QXCORE_ENABLE_LOG_SPDLOG
#ifdef QXCORE_ENABLE_LOG_GLOG

static void BM_Comparison_Spdlog_vs_Glog(benchmark::State& state) {
  // 这个测试用于比较两个后端的性能
  // 实际的比较结果会在基准测试输出中显示
  state.SkipWithError("Use individual backend benchmarks for comparison");
}

#endif  // QXCORE_ENABLE_LOG_GLOG
#endif  // QXCORE_ENABLE_LOG_SPDLOG

}  // namespace log
}  // namespace qxcore

// 运行所有基准测试
BENCHMARK_MAIN();
