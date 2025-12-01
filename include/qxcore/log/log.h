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

#ifndef QXCORE_LOG_LOG_H_
#define QXCORE_LOG_LOG_H_

#include <string>
#include <memory>
#include <absl/strings/string_view.h>
#include <absl/status/status.h>
#include "qxcore/log/log_level.h"

namespace qxcore {
namespace log {

// 前向声明后端类
class SpdlogBackend;
class GlogBackend;

// 日志前端接口 - 模板化设计支持编译期多态
template<typename Backend>
class Log {
 public:
  // 构造函数
  Log() = default;
  
  // 禁用拷贝构造和赋值
  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;
  
  // 移动构造和赋值
  Log(Log&&) = default;
  Log& operator=(Log&&) = default;

  // 初始化日志系统
  absl::Status init(const std::string& name, LogLevel level = LogLevel::kInfo) {
    return backend_.init(name, level);
  }

  // 设置日志级别
  absl::Status set_level(LogLevel level) {
    return backend_.set_level(level);
  }

  // 获取当前日志级别
  LogLevel get_level() const {
    return backend_.get_level();
  }

  // 检查日志级别是否启用
  bool is_enabled(LogLevel level) const {
    return backend_.is_enabled(level);
  }

  // 基础日志接口
  void log(LogLevel level, absl::string_view msg) {
    if (is_enabled(level)) {
      backend_.log(level, msg);
    }
  }

  // 格式化日志接口
  template<typename... Args>
  void logf(LogLevel level, absl::string_view fmt_str, Args&&... args) {
    if (is_enabled(level)) {
      backend_.logf(level, fmt_str, std::forward<Args>(args)...);
    }
  }

  // 便捷接口
  template<typename... Args>
  void trace(absl::string_view fmt_str, Args&&... args) {
    logf(LogLevel::kTrace, fmt_str, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void debug(absl::string_view fmt_str, Args&&... args) {
    logf(LogLevel::kDebug, fmt_str, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void info(absl::string_view fmt_str, Args&&... args) {
    logf(LogLevel::kInfo, fmt_str, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void warn(absl::string_view fmt_str, Args&&... args) {
    logf(LogLevel::kWarn, fmt_str, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void error(absl::string_view fmt_str, Args&&... args) {
    logf(LogLevel::kError, fmt_str, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void critical(absl::string_view fmt_str, Args&&... args) {
    logf(LogLevel::kCritical, fmt_str, std::forward<Args>(args)...);
  }

  // 刷新日志缓冲区
  void flush() {
    backend_.flush();
  }

  // 关闭日志系统
  void shutdown() {
    backend_.shutdown();
  }

 private:
  Backend backend_;
};

}  // namespace log
}  // namespace qxcore

// 包含后端实现并定义默认类型
#ifdef QXCORE_ENABLE_LOG_SPDLOG
#include "qxcore/log/spdlog_backend.h"
namespace qxcore {
namespace log {
using DefaultLog = Log<SpdlogBackend>;
}
}
#elif defined(QXCORE_ENABLE_LOG_GLOG)
#include "qxcore/log/glog_backend.h"
namespace qxcore {
namespace log {
using DefaultLog = Log<GlogBackend>;
}
}
#else
#error "No log backend enabled. Please enable either spdlog or glog."
#endif

namespace qxcore {
namespace log {

// 全局日志实例访问
DefaultLog& GetDefaultLogger();

// 初始化全局日志器
absl::Status InitDefaultLogger(const std::string& name, LogLevel level = LogLevel::kInfo);

}  // namespace log
}  // namespace qxcore

// 日志宏定义
#define QXLOG_TRACE(logger, ...) logger.trace(__VA_ARGS__)
#define QXLOG_DEBUG(logger, ...) logger.debug(__VA_ARGS__)
#define QXLOG_INFO(logger, ...)  logger.info(__VA_ARGS__)
#define QXLOG_WARN(logger, ...)  logger.warn(__VA_ARGS__)
#define QXLOG_ERROR(logger, ...) logger.error(__VA_ARGS__)
#define QXLOG_CRITICAL(logger, ...) logger.critical(__VA_ARGS__)

// 全局日志宏
#define QXLOG_GLOBAL_TRACE(...) QXLOG_TRACE(qxcore::log::GetDefaultLogger(), __VA_ARGS__)
#define QXLOG_GLOBAL_DEBUG(...) QXLOG_DEBUG(qxcore::log::GetDefaultLogger(), __VA_ARGS__)
#define QXLOG_GLOBAL_INFO(...)  QXLOG_INFO(qxcore::log::GetDefaultLogger(), __VA_ARGS__)
#define QXLOG_GLOBAL_WARN(...)  QXLOG_WARN(qxcore::log::GetDefaultLogger(), __VA_ARGS__)
#define QXLOG_GLOBAL_ERROR(...) QXLOG_ERROR(qxcore::log::GetDefaultLogger(), __VA_ARGS__)
#define QXLOG_GLOBAL_CRITICAL(...) QXLOG_CRITICAL(qxcore::log::GetDefaultLogger(), __VA_ARGS__)

#endif  // QXCORE_LOG_LOG_H_
