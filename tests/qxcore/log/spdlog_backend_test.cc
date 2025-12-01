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

#ifdef QXCORE_ENABLE_LOG_SPDLOG

#include "qxcore/log/spdlog_backend.h"
#include <gtest/gtest.h>
#include <absl/status/status.h>

namespace qxcore {
namespace log {

class SpdlogBackendTest : public ::testing::Test {
 protected:
  void SetUp() override {
    backend_ = std::make_unique<SpdlogBackend>();
  }
  
  void TearDown() override {
    if (backend_) {
      backend_->shutdown();
    }
  }

  std::unique_ptr<SpdlogBackend> backend_;
};

TEST_F(SpdlogBackendTest, Initialization) {
  // 测试初始化
  absl::Status status = backend_->init("test_spdlog", LogLevel::kDebug);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(backend_->get_level(), LogLevel::kDebug);
  EXPECT_TRUE(backend_->is_enabled(LogLevel::kDebug));
  EXPECT_TRUE(backend_->is_enabled(LogLevel::kInfo));
  EXPECT_FALSE(backend_->is_enabled(LogLevel::kTrace));
}

TEST_F(SpdlogBackendTest, DoubleInitialization) {
  // 测试重复初始化
  absl::Status status1 = backend_->init("test_spdlog", LogLevel::kInfo);
  EXPECT_TRUE(status1.ok());
  
  absl::Status status2 = backend_->init("test_spdlog2", LogLevel::kDebug);
  EXPECT_FALSE(status2.ok());
  EXPECT_EQ(status2.code(), absl::StatusCode::kAlreadyExists);
}

TEST_F(SpdlogBackendTest, SetLevel) {
  absl::Status status = backend_->init("test_spdlog", LogLevel::kInfo);
  EXPECT_TRUE(status.ok());
  
  // 测试设置级别
  status = backend_->set_level(LogLevel::kError);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(backend_->get_level(), LogLevel::kError);
  
  // 测试级别启用状态
  EXPECT_TRUE(backend_->is_enabled(LogLevel::kError));
  EXPECT_TRUE(backend_->is_enabled(LogLevel::kCritical));
  EXPECT_FALSE(backend_->is_enabled(LogLevel::kWarn));
  EXPECT_FALSE(backend_->is_enabled(LogLevel::kInfo));
}

TEST_F(SpdlogBackendTest, SetLevelWithoutInitialization) {
  // 测试未初始化时设置级别
  absl::Status status = backend_->set_level(LogLevel::kError);
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.code(), absl::StatusCode::kFailedPrecondition);
}

TEST_F(SpdlogBackendTest, BasicLogging) {
  absl::Status status = backend_->init("test_spdlog", LogLevel::kDebug);
  EXPECT_TRUE(status.ok());
  
  // 测试基础日志接口（这些调用不应该崩溃）
  EXPECT_NO_THROW(backend_->log(LogLevel::kInfo, "Test message"));
  EXPECT_NO_THROW(backend_->log(LogLevel::kWarn, "Warning message"));
  EXPECT_NO_THROW(backend_->log(LogLevel::kError, "Error message"));
}

TEST_F(SpdlogBackendTest, FormattedLogging) {
  absl::Status status = backend_->init("test_spdlog", LogLevel::kDebug);
  EXPECT_TRUE(status.ok());
  
  // 测试格式化日志接口
  EXPECT_NO_THROW(backend_->logf(LogLevel::kInfo, "Formatted message: {}", 42));
  EXPECT_NO_THROW(backend_->logf(LogLevel::kWarn, "Warning: {} - {}", "test", 123));
  EXPECT_NO_THROW(backend_->logf(LogLevel::kError, "Error code: {}", 404));
}

TEST_F(SpdlogBackendTest, LogLevelFiltering) {
  absl::Status status = backend_->init("test_spdlog", LogLevel::kWarn);
  EXPECT_TRUE(status.ok());
  
  // 只有 WARN 及以上级别应该被启用
  EXPECT_FALSE(backend_->is_enabled(LogLevel::kTrace));
  EXPECT_FALSE(backend_->is_enabled(LogLevel::kDebug));
  EXPECT_FALSE(backend_->is_enabled(LogLevel::kInfo));
  EXPECT_TRUE(backend_->is_enabled(LogLevel::kWarn));
  EXPECT_TRUE(backend_->is_enabled(LogLevel::kError));
  EXPECT_TRUE(backend_->is_enabled(LogLevel::kCritical));
}

TEST_F(SpdlogBackendTest, FlushAndShutdown) {
  absl::Status status = backend_->init("test_spdlog", LogLevel::kInfo);
  EXPECT_TRUE(status.ok());
  
  // 测试刷新和关闭
  EXPECT_NO_THROW(backend_->flush());
  EXPECT_NO_THROW(backend_->shutdown());
  
  // 关闭后应该无法记录日志
  EXPECT_FALSE(backend_->is_enabled(LogLevel::kInfo));
}

TEST_F(SpdlogBackendTest, LoggingWithoutInitialization) {
  // 测试未初始化时的日志记录
  EXPECT_NO_THROW(backend_->log(LogLevel::kInfo, "Should not crash"));
  EXPECT_NO_THROW(backend_->logf(LogLevel::kInfo, "Should not crash: {}", 42));
  EXPECT_NO_THROW(backend_->flush());
  EXPECT_NO_THROW(backend_->shutdown());
}

}  // namespace log
}  // namespace qxcore

#endif  // QXCORE_ENABLE_LOG_SPDLOG
