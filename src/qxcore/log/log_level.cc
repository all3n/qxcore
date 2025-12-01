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

#include "qxcore/log/log_level.h"

#include <absl/strings/ascii.h>

namespace qxcore {
namespace log {

absl::string_view LogLevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::kTrace:
      return "TRACE";
    case LogLevel::kDebug:
      return "DEBUG";
    case LogLevel::kInfo:
      return "INFO";
    case LogLevel::kWarn:
      return "WARN";
    case LogLevel::kError:
      return "ERROR";
    case LogLevel::kCritical:
      return "CRITICAL";
    default:
      return "UNKNOWN";
  }
}

bool StringToLogLevel(absl::string_view str, LogLevel& level) {
  std::string lower_str = absl::AsciiStrToLower(str);
  
  if (lower_str == "trace") {
    level = LogLevel::kTrace;
    return true;
  } else if (lower_str == "debug") {
    level = LogLevel::kDebug;
    return true;
  } else if (lower_str == "info") {
    level = LogLevel::kInfo;
    return true;
  } else if (lower_str == "warn" || lower_str == "warning") {
    level = LogLevel::kWarn;
    return true;
  } else if (lower_str == "error") {
    level = LogLevel::kError;
    return true;
  } else if (lower_str == "critical" || lower_str == "fatal") {
    level = LogLevel::kCritical;
    return true;
  }
  
  return false;
}

}  // namespace log
}  // namespace qxcore
