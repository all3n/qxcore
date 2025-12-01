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

#ifndef QXCORE_LOG_GLOG_BACKEND_H_
#define QXCORE_LOG_GLOG_BACKEND_H_

#include <string>
#include <absl/strings/string_view.h>
#include <absl/status/status.h>
#include <absl/strings/str_format.h>
#include "qxcore/log/log_level.h"

namespace qxcore {
namespace log {

// Glog 后端实现
class GlogBackend {
 public:
  GlogBackend() = default;
  ~GlogBackend() = default;

  // 禁用拷贝构造和赋值
  GlogBackend(const GlogBackend&) = delete;
  GlogBackend& operator=(const GlogBackend&) = delete;
  
  // 移动构造和赋值
  GlogBackend(GlogBackend&&) = default;
  GlogBackend& operator=(GlogBackend&&) = default;

  // 初始化日志系统
  absl::Status init(const std::string& name, LogLevel level = LogLevel::kInfo);

  // 设置日志级别
  absl::Status set_level(LogLevel level);

  // 获取当前日志级别
  LogLevel get_level() const;

  // 检查日志级别是否启用
  bool is_enabled(LogLevel level) const;

  // 基础日志接口
  void log(LogLevel level, absl::string_view msg);

  // 格式化日志接口
  template<typename... Args>
  void logf(LogLevel level, absl::string_view fmt_str, Args&&... args) {
    if (!initialized_ || !is_enabled(level)) {
      return;
    }

    try {
      // 对于没有参数的情况，直接使用原始字符串
      if constexpr (sizeof...(args) == 0) {
        log(level, fmt_str);
      } else {
        // 对于有参数的情况，使用 absl 格式化
        std::string formatted = absl::StrFormat(fmt_str, std::forward<Args>(args)...);
        log(level, formatted);
      }
    } catch (...) {
      // 静默处理日志错误，避免异常传播
    }
  }

  // 刷新日志缓冲区
  void flush();

  // 关闭日志系统
  void shutdown();

 private:
  // 转换日志级别
  static int ToGlogLevel(LogLevel level);
  static LogLevel FromGlogLevel(int level);

  std::string logger_name_;
  LogLevel current_level_ = LogLevel::kInfo;
  bool initialized_ = false;
};

}  // namespace log
}  // namespace qxcore

#endif  // QXCORE_LOG_GLOG_BACKEND_H_
