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

#include "qxcore/log/glog_backend.h"
#include <glog/logging.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_format.h>

namespace qxcore {
namespace log {

absl::Status GlogBackend::init(const std::string& name, LogLevel level) {
  if (initialized_) {
    return absl::AlreadyExistsError("Logger already initialized");
  }

  try {
    // 初始化 glog（如果尚未初始化）
    if (!google::IsGoogleLoggingInitialized()) {
      google::InitGoogleLogging(name.c_str());
    }

    logger_name_ = name;
    current_level_ = level;
    initialized_ = true;

    // 设置 glog 日志级别
    absl::Status status = set_level(level);
    if (!status.ok()) {
      initialized_ = false;
      return status;
    }

    return absl::OkStatus();
  } catch (const std::exception& e) {
    return absl::InternalError(absl::StrFormat("Failed to initialize glog: %s", e.what()));
  }
}

absl::Status GlogBackend::set_level(LogLevel level) {
  if (!initialized_) {
    return absl::FailedPreconditionError("Logger not initialized");
  }

  try {
    // glog 使用 FLAGS_minloglevel 控制最小日志级别
    // 0=INFO, 1=WARNING, 2=ERROR, 3=FATAL
    int glog_level = ToGlogLevel(level);
    google::SetLogDestination(glog_level, 
        absl::StrCat(logger_name_, ".log").c_str());
    
    current_level_ = level;
    return absl::OkStatus();
  } catch (const std::exception& e) {
    return absl::InternalError(fmt::format("Failed to set log level: {}", e.what()));
  }
}

LogLevel GlogBackend::get_level() const {
  return current_level_;
}

bool GlogBackend::is_enabled(LogLevel level) const {
  if (!initialized_) {
    return false;
  }
  return IsLogLevelEnabled(current_level_, level);
}

void GlogBackend::log(LogLevel level, absl::string_view msg) {
  if (!initialized_ || !is_enabled(level)) {
    return;
  }

  try {
    std::string msg_str(msg);
    switch (level) {
      case LogLevel::kTrace:
      case LogLevel::kDebug:
      case LogLevel::kInfo:
        LOG(INFO) << msg_str;
        break;
      case LogLevel::kWarn:
        LOG(WARNING) << msg_str;
        break;
      case LogLevel::kError:
        LOG(ERROR) << msg_str;
        break;
      case LogLevel::kCritical:
        LOG(FATAL) << msg_str;
        break;
    }
  } catch (...) {
    // 静默处理日志错误，避免异常传播
  }
}


void GlogBackend::flush() {
  if (!initialized_) {
    return;
  }

  try {
    google::FlushLogFiles(google::GLOG_INFO);
  } catch (...) {
    // 静默处理日志错误，避免异常传播
  }
}

void GlogBackend::shutdown() {
  if (!initialized_) {
    return;
  }

  try {
    google::FlushLogFiles(google::GLOG_INFO);
    initialized_ = false;
  } catch (...) {
    // 静默处理日志错误，避免异常传播
  }
}

int GlogBackend::ToGlogLevel(LogLevel level) {
  switch (level) {
    case LogLevel::kTrace:
    case LogLevel::kDebug:
    case LogLevel::kInfo:
      return google::GLOG_INFO;
    case LogLevel::kWarn:
      return google::GLOG_WARNING;
    case LogLevel::kError:
      return google::GLOG_ERROR;
    case LogLevel::kCritical:
      return google::GLOG_FATAL;
    default:
      return google::GLOG_INFO;
  }
}

LogLevel GlogBackend::FromGlogLevel(int level) {
  switch (level) {
    case google::GLOG_INFO:
      return LogLevel::kInfo;
    case google::GLOG_WARNING:
      return LogLevel::kWarn;
    case google::GLOG_ERROR:
      return LogLevel::kError;
    case google::GLOG_FATAL:
      return LogLevel::kCritical;
    default:
      return LogLevel::kInfo;
  }
}

}  // namespace log
}  // namespace qxcore
