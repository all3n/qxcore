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
#include <gtest/gtest.h>
#include <absl/strings/string_view.h>

namespace qxcore {
namespace log {

class LogLevelTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(LogLevelTest, LogLevelToString) {
  EXPECT_EQ(LogLevelToString(LogLevel::kTrace), "TRACE");
  EXPECT_EQ(LogLevelToString(LogLevel::kDebug), "DEBUG");
  EXPECT_EQ(LogLevelToString(LogLevel::kInfo), "INFO");
  EXPECT_EQ(LogLevelToString(LogLevel::kWarn), "WARN");
  EXPECT_EQ(LogLevelToString(LogLevel::kError), "ERROR");
  EXPECT_EQ(LogLevelToString(LogLevel::kCritical), "CRITICAL");
}

TEST_F(LogLevelTest, StringToLogLevel) {
  LogLevel level;
  
  // 测试小写
  EXPECT_TRUE(StringToLogLevel("trace", level));
  EXPECT_EQ(level, LogLevel::kTrace);
  
  EXPECT_TRUE(StringToLogLevel("debug", level));
  EXPECT_EQ(level, LogLevel::kDebug);
  
  EXPECT_TRUE(StringToLogLevel("info", level));
  EXPECT_EQ(level, LogLevel::kInfo);
  
  EXPECT_TRUE(StringToLogLevel("warn", level));
  EXPECT_EQ(level, LogLevel::kWarn);
  
  EXPECT_TRUE(StringToLogLevel("warning", level));
  EXPECT_EQ(level, LogLevel::kWarn);
  
  EXPECT_TRUE(StringToLogLevel("error", level));
  EXPECT_EQ(level, LogLevel::kError);
  
  EXPECT_TRUE(StringToLogLevel("critical", level));
  EXPECT_EQ(level, LogLevel::kCritical);
  
  EXPECT_TRUE(StringToLogLevel("fatal", level));
  EXPECT_EQ(level, LogLevel::kCritical);
  
  // 测试大小写混合
  EXPECT_TRUE(StringToLogLevel("TRACE", level));
  EXPECT_EQ(level, LogLevel::kTrace);
  
  EXPECT_TRUE(StringToLogLevel("Debug", level));
  EXPECT_EQ(level, LogLevel::kDebug);
  
  // 测试无效字符串
  EXPECT_FALSE(StringToLogLevel("invalid", level));
  EXPECT_FALSE(StringToLogLevel("", level));
  EXPECT_FALSE(StringToLogLevel("unknown", level));
}

TEST_F(LogLevelTest, LogLevelToInt) {
  EXPECT_EQ(LogLevelToInt(LogLevel::kTrace), 0);
  EXPECT_EQ(LogLevelToInt(LogLevel::kDebug), 1);
  EXPECT_EQ(LogLevelToInt(LogLevel::kInfo), 2);
  EXPECT_EQ(LogLevelToInt(LogLevel::kWarn), 3);
  EXPECT_EQ(LogLevelToInt(LogLevel::kError), 4);
  EXPECT_EQ(LogLevelToInt(LogLevel::kCritical), 5);
}

TEST_F(LogLevelTest, IsLogLevelEnabled) {
  // 测试相同级别
  EXPECT_TRUE(IsLogLevelEnabled(LogLevel::kInfo, LogLevel::kInfo));
  
  // 测试更高级别（数字更大的级别）
  EXPECT_TRUE(IsLogLevelEnabled(LogLevel::kInfo, LogLevel::kError));
  EXPECT_TRUE(IsLogLevelEnabled(LogLevel::kDebug, LogLevel::kCritical));
  
  // 测试更低级别（数字更小的级别）
  EXPECT_FALSE(IsLogLevelEnabled(LogLevel::kError, LogLevel::kInfo));
  EXPECT_FALSE(IsLogLevelEnabled(LogLevel::kCritical, LogLevel::kDebug));
}

}  // namespace log
}  // namespace qxcore
