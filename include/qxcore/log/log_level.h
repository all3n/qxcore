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

#ifndef QXCORE_LOG_LOG_LEVEL_H_
#define QXCORE_LOG_LOG_LEVEL_H_

#include <string>
#include <absl/strings/string_view.h>

namespace qxcore {
namespace log {

// 日志级别枚举
enum class LogLevel {
  kTrace = 0,
  kDebug = 1,
  kInfo = 2,
  kWarn = 3,
  kError = 4,
  kCritical = 5
};

// 将日志级别转换为字符串
absl::string_view LogLevelToString(LogLevel level);

// 将字符串转换为日志级别
bool StringToLogLevel(absl::string_view str, LogLevel& level);

// 获取日志级别的数值
inline int LogLevelToInt(LogLevel level) {
  return static_cast<int>(level);
}

// 检查日志级别是否启用
// 注意：数字越大级别越低（Trace=0 是最高级别，Critical=5 是最低级别）
// 当目标级别 >= 当前级别时，应该启用（即设置为 Debug 时，Debug/Info/Warn/Error/Critical 显示，Trace 不显示）
inline bool IsLogLevelEnabled(LogLevel current_level, LogLevel target_level) {
  return LogLevelToInt(target_level) >= LogLevelToInt(current_level);
}

}  // namespace log
}  // namespace qxcore

#endif  // QXCORE_LOG_LOG_LEVEL_H_
