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

#ifndef QXCORE_LOG_SPDLOG_BACKEND_H_
#define QXCORE_LOG_SPDLOG_BACKEND_H_

#include <string>
#include <memory>
#include <absl/strings/string_view.h>
#include <absl/status/status.h>
#include <absl/strings/str_format.h>
#include "qxcore/log/log_level.h"

// 包含完整的 spdlog 头文件以支持模板函数
#include <spdlog/spdlog.h>

namespace qxcore {
namespace log {

// Spdlog 后端实现
class SpdlogBackend {
 public:
  SpdlogBackend() = default;
  ~SpdlogBackend();

  // 禁用拷贝构造和赋值
  SpdlogBackend(const SpdlogBackend&) = delete;
  SpdlogBackend& operator=(const SpdlogBackend&) = delete;
  
  // 移动构造和赋值
  SpdlogBackend(SpdlogBackend&&) = default;
  SpdlogBackend& operator=(SpdlogBackend&&) = default;

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
      // 直接使用 spdlog 的格式化功能
      logger_->log(ToSpdlogLevel(level), fmt_str, std::forward<Args>(args)...);
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
  static spdlog::level::level_enum ToSpdlogLevel(LogLevel level);
  static LogLevel FromSpdlogLevel(spdlog::level::level_enum level);

  std::shared_ptr<spdlog::logger> logger_;
  LogLevel current_level_ = LogLevel::kInfo;
  bool initialized_ = false;
};

}  // namespace log
}  // namespace qxcore

#endif  // QXCORE_LOG_SPDLOG_BACKEND_H_
