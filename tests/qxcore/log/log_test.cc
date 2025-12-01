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
#include <gtest/gtest.h>
#include <absl/status/status.h>

namespace qxcore {
namespace log {

class LogTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // 每个测试前重置全局日志器
    logger_ = std::make_unique<DefaultLog>();
  }
  
  void TearDown() override {
    if (logger_) {
      logger_->shutdown();
    }
  }

  std::unique_ptr<DefaultLog> logger_;
};

TEST_F(LogTest, Initialization) {
  // 测试初始化
  absl::Status status = logger_->init("test_logger", LogLevel::kDebug);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(logger_->get_level(), LogLevel::kDebug);
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kDebug));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kInfo));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kTrace));
}

TEST_F(LogTest, SetLevel) {
  absl::Status status = logger_->init("test_logger", LogLevel::kInfo);
  EXPECT_TRUE(status.ok());
  
  // 测试设置级别
  status = logger_->set_level(LogLevel::kError);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(logger_->get_level(), LogLevel::kError);
  
  // 测试级别启用状态
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kError));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kCritical));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kWarn));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kInfo));
}

TEST_F(LogTest, BasicLogging) {
  absl::Status status = logger_->init("test_logger", LogLevel::kDebug);
  EXPECT_TRUE(status.ok());
  
  // 测试基础日志接口（这些调用不应该崩溃）
  EXPECT_NO_THROW(logger_->log(LogLevel::kInfo, "Test message"));
  EXPECT_NO_THROW(logger_->log(LogLevel::kWarn, "Warning message"));
  EXPECT_NO_THROW(logger_->log(LogLevel::kError, "Error message"));
}

TEST_F(LogTest, FormattedLogging) {
  absl::Status status = logger_->init("test_logger", LogLevel::kDebug);
  EXPECT_TRUE(status.ok());
  
  // 测试格式化日志接口
  EXPECT_NO_THROW(logger_->logf(LogLevel::kInfo, "Formatted message: {}", 42));
  EXPECT_NO_THROW(logger_->logf(LogLevel::kWarn, "Warning: {} - {}", "test", 123));
  EXPECT_NO_THROW(logger_->logf(LogLevel::kError, "Error code: {}", 404));
}

TEST_F(LogTest, ConvenienceMethods) {
  absl::Status status = logger_->init("test_logger", LogLevel::kDebug);
  EXPECT_TRUE(status.ok());
  
  // 测试便捷方法
  EXPECT_NO_THROW(logger_->trace("Trace message"));
  EXPECT_NO_THROW(logger_->debug("Debug message"));
  EXPECT_NO_THROW(logger_->info("Info message"));
  EXPECT_NO_THROW(logger_->warn("Warning message"));
  EXPECT_NO_THROW(logger_->error("Error message"));
  EXPECT_NO_THROW(logger_->critical("Critical message"));
  
  // 测试格式化便捷方法
  EXPECT_NO_THROW(logger_->info("Formatted: {}", "test"));
  EXPECT_NO_THROW(logger_->error("Error: {} - {}", "code", 500));
}

TEST_F(LogTest, FlushAndShutdown) {
  absl::Status status = logger_->init("test_logger", LogLevel::kInfo);
  EXPECT_TRUE(status.ok());
  
  // 测试刷新和关闭
  EXPECT_NO_THROW(logger_->flush());
  EXPECT_NO_THROW(logger_->shutdown());
  
  // 关闭后应该无法记录日志
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kInfo));
}

TEST_F(LogTest, LogLevelFiltering) {
  absl::Status status = logger_->init("test_logger", LogLevel::kWarn);
  EXPECT_TRUE(status.ok());
  
  // 只有 WARN 及以上级别应该被启用
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kTrace));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kDebug));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kInfo));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kWarn));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kError));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kCritical));
}

TEST_F(LogTest, GlobalLogger) {
  // 测试全局日志器
  DefaultLog& global_logger = GetDefaultLogger();
  EXPECT_NO_THROW(global_logger.info("Global logger test"));
  
  // 测试全局日志器初始化
  absl::Status status = InitDefaultLogger("global_test", LogLevel::kDebug);
  EXPECT_TRUE(status.ok());
  
  DefaultLog& new_global_logger = GetDefaultLogger();
  EXPECT_EQ(new_global_logger.get_level(), LogLevel::kDebug);
}

}  // namespace log
}  // namespace qxcore
