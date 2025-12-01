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

#include "qxcore/log/spdlog_backend.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <absl/strings/str_format.h>

namespace qxcore {
namespace log {

SpdlogBackend::~SpdlogBackend() {
  if (logger_) {
    shutdown();
  }
}

absl::Status SpdlogBackend::init(const std::string& name, LogLevel level) {
  if (initialized_) {
    return absl::AlreadyExistsError("Logger already initialized");
  }

  if (name.empty()) {
    return absl::InvalidArgumentError("Logger name cannot be empty");
  }

  try {
    // 创建控制台和文件输出
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        name + ".log", true);

    // 创建多 sink 日志器
    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    logger_ = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    
    // 设置日志级别
    logger_->set_level(ToSpdlogLevel(level));
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
    
    // 注册到 spdlog
    spdlog::register_logger(logger_);
    
    current_level_ = level;
    initialized_ = true;
    
    return absl::OkStatus();
  } catch (const std::exception& e) {
    return absl::InternalError(absl::StrFormat("Failed to initialize spdlog: %s", e.what()));
  }
}

absl::Status SpdlogBackend::set_level(LogLevel level) {
  if (!initialized_) {
    return absl::FailedPreconditionError("Logger not initialized");
  }

  try {
    logger_->set_level(ToSpdlogLevel(level));
    current_level_ = level;
    return absl::OkStatus();
  } catch (const std::exception& e) {
    return absl::InternalError(absl::StrFormat("Failed to set log level: %s", e.what()));
  }
}

LogLevel SpdlogBackend::get_level() const {
  return current_level_;
}

bool SpdlogBackend::is_enabled(LogLevel level) const {
  if (!initialized_) {
    return false;
  }
  return IsLogLevelEnabled(current_level_, level);
}

void SpdlogBackend::log(LogLevel level, absl::string_view msg) {
  if (!initialized_ || !is_enabled(level)) {
    return;
  }

  try {
    logger_->log(ToSpdlogLevel(level), msg);
  } catch (...) {
    // 静默处理日志错误，避免异常传播
  }
}


void SpdlogBackend::flush() {
  if (!initialized_) {
    return;
  }

  try {
    logger_->flush();
  } catch (...) {
    // 静默处理日志错误，避免异常传播
  }
}

void SpdlogBackend::shutdown() {
  if (!initialized_) {
    return;
  }

  try {
    if (logger_) {
      logger_->flush();
      spdlog::drop(logger_->name());
    }
    initialized_ = false;
  } catch (...) {
    // 静默处理日志错误，避免异常传播
  }
}

spdlog::level::level_enum SpdlogBackend::ToSpdlogLevel(LogLevel level) {
  switch (level) {
    case LogLevel::kTrace:
      return spdlog::level::trace;
    case LogLevel::kDebug:
      return spdlog::level::debug;
    case LogLevel::kInfo:
      return spdlog::level::info;
    case LogLevel::kWarn:
      return spdlog::level::warn;
    case LogLevel::kError:
      return spdlog::level::err;
    case LogLevel::kCritical:
      return spdlog::level::critical;
    default:
      return spdlog::level::info;
  }
}

LogLevel SpdlogBackend::FromSpdlogLevel(spdlog::level::level_enum level) {
  switch (level) {
    case spdlog::level::trace:
      return LogLevel::kTrace;
    case spdlog::level::debug:
      return LogLevel::kDebug;
    case spdlog::level::info:
      return LogLevel::kInfo;
    case spdlog::level::warn:
      return LogLevel::kWarn;
    case spdlog::level::err:
      return LogLevel::kError;
    case spdlog::level::critical:
      return LogLevel::kCritical;
    case spdlog::level::off:
      return LogLevel::kCritical;
    default:
      return LogLevel::kInfo;
  }
}


}  // namespace log
}  // namespace qxcore
