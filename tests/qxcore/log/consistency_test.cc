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

/**
 * @file consistency_test.cc
 * @brief 日志后端一致性测试
 */

#include "qxcore/log/log.h"
#ifdef QXCORE_ENABLE_LOG_SPDLOG
#include "qxcore/log/spdlog_backend.h"
#endif
#ifdef QXCORE_ENABLE_LOG_GLOG
#include "qxcore/log/glog_backend.h"
#endif
#include "gtest/gtest.h"
#include <sstream>
#include <string>

using namespace qxcore::log;

// 测试用的后端一致性检查类
template<typename BackendType>
class BackendConsistencyTest : public ::testing::Test {
 protected:
  void SetUp() override {
    logger_ = std::make_unique<Log<BackendType>>();
    status_ = logger_->init("test_logger", LogLevel::kDebug);
    ASSERT_TRUE(status_.ok()) << "Failed to initialize logger: " << status_.message();
  }

  void TearDown() override {
    if (logger_) {
      logger_->shutdown();
    }
  }

  std::unique_ptr<Log<BackendType>> logger_;
  absl::Status status_;
};

// 类型定义 - 只在启用相应后端时定义
#ifdef QXCORE_ENABLE_LOG_SPDLOG
using SpdlogConsistencyTest = BackendConsistencyTest<SpdlogBackend>;
#endif

#ifdef QXCORE_ENABLE_LOG_GLOG
using GlogConsistencyTest = BackendConsistencyTest<GlogBackend>;
#endif

// 测试初始化一致性
#ifdef QXCORE_ENABLE_LOG_SPDLOG
TEST_F(SpdlogConsistencyTest, InitializationConsistency) {
  EXPECT_TRUE(status_.ok());
  EXPECT_EQ(logger_->get_level(), LogLevel::kDebug);
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kDebug));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kInfo));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kTrace));
}
#endif

#ifdef QXCORE_ENABLE_LOG_GLOG
TEST_F(GlogConsistencyTest, InitializationConsistency) {
  EXPECT_TRUE(status_.ok());
  EXPECT_EQ(logger_->get_level(), LogLevel::kDebug);
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kDebug));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kInfo));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kTrace));
}
#endif

// 测试设置级别一致性
#ifdef QXCORE_ENABLE_LOG_SPDLOG
TEST_F(SpdlogConsistencyTest, SetLevelConsistency) {
  // 设置为 WARN 级别
  absl::Status status = logger_->set_level(LogLevel::kWarn);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(logger_->get_level(), LogLevel::kWarn);
  
  // 检查级别启用状态
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kDebug));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kInfo));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kWarn));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kError));
  
  // 设置为 TRACE 级别
  status = logger_->set_level(LogLevel::kTrace);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(logger_->get_level(), LogLevel::kTrace);
  
  // 所有级别都应该启用
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kTrace));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kDebug));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kInfo));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kWarn));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kError));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kCritical));
}
#endif

#ifdef QXCORE_ENABLE_LOG_GLOG
TEST_F(GlogConsistencyTest, SetLevelConsistency) {
  // 设置为 WARN 级别
  absl::Status status = logger_->set_level(LogLevel::kWarn);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(logger_->get_level(), LogLevel::kWarn);
  
  // 检查级别启用状态
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kDebug));
  EXPECT_FALSE(logger_->is_enabled(LogLevel::kInfo));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kWarn));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kError));
  
  // 设置为 TRACE 级别
  status = logger_->set_level(LogLevel::kTrace);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(logger_->get_level(), LogLevel::kTrace);
  
  // 所有级别都应该启用
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kTrace));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kDebug));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kInfo));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kWarn));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kError));
  EXPECT_TRUE(logger_->is_enabled(LogLevel::kCritical));
}
#endif

// 测试错误处理一致性
#ifdef QXCORE_ENABLE_LOG_SPDLOG
TEST_F(SpdlogConsistencyTest, ErrorHandlingConsistency) {
  // 测试无效的日志器名称（空字符串）
  Log<SpdlogBackend> invalid_logger;
  absl::Status status = invalid_logger.init("", LogLevel::kInfo);
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.code(), absl::StatusCode::kInvalidArgument);
  
  // 测试重复初始化
  status = logger_->init("another_name", LogLevel::kWarn);
  // 这个行为可能因后端而异，我们只检查不会崩溃
  // 具体的返回值取决于后端实现
}
#endif

#ifdef QXCORE_ENABLE_LOG_GLOG
TEST_F(GlogConsistencyTest, ErrorHandlingConsistency) {
  // 测试无效的日志器名称（空字符串）
  Log<GlogBackend> invalid_logger;
  absl::Status status = invalid_logger.init("", LogLevel::kInfo);
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.code(), absl::StatusCode::kInvalidArgument);
  
  // 测试重复初始化
  status = logger_->init("another_name", LogLevel::kWarn);
  // 这个行为可能因后端而异，我们只检查不会崩溃
  // 具体的返回值取决于后端实现
}
#endif

// 测试日志记录一致性
#ifdef QXCORE_ENABLE_LOG_SPDLOG
TEST_F(SpdlogConsistencyTest, LoggingConsistency) {
  // 设置为 INFO 级别
  absl::Status status = logger_->set_level(LogLevel::kInfo);
  EXPECT_TRUE(status.ok());
  
  // 这些调用不应该崩溃
  logger_->trace("Trace message");
  logger_->debug("Debug message");
  logger_->info("Info message");
  logger_->warn("Warning message");
  logger_->error("Error message");
  logger_->critical("Critical message");
  
  // 格式化日志
  logger_->info("Formatted message: {} {}", 42, "test");
  logger_->error("Error code: {}", 404);
  
  // 刷新
  logger_->flush();
}
#endif

#ifdef QXCORE_ENABLE_LOG_GLOG
TEST_F(GlogConsistencyTest, LoggingConsistency) {
  // 设置为 INFO 级别
  absl::Status status = logger_->set_level(LogLevel::kInfo);
  EXPECT_TRUE(status.ok());
  
  // 这些调用不应该崩溃
  logger_->trace("Trace message");
  logger_->debug("Debug message");
  logger_->info("Info message");
  logger_->warn("Warning message");
  logger_->error("Error message");
  logger_->critical("Critical message");
  
  // 格式化日志
  logger_->info("Formatted message: {} {}", 42, "test");
  logger_->error("Error code: {}", 404);
  
  // 刷新
  logger_->flush();
}
#endif

// 跨后端一致性测试 - 只在同时启用两个后端时编译
#if defined(QXCORE_ENABLE_LOG_SPDLOG) && defined(QXCORE_ENABLE_LOG_GLOG)
class CrossBackendConsistencyTest : public ::testing::Test {
 protected:
  void SetUp() override {
    spdlog_logger_ = std::make_unique<Log<SpdlogBackend>>();
    glog_logger_ = std::make_unique<Log<GlogBackend>>();
    
    absl::Status status1 = spdlog_logger_->init("spdlog_test", LogLevel::kInfo);
    absl::Status status2 = glog_logger_->init("glog_test", LogLevel::kInfo);
    
    ASSERT_TRUE(status1.ok()) << "Failed to initialize spdlog logger: " << status1.message();
    ASSERT_TRUE(status2.ok()) << "Failed to initialize glog logger: " << status2.message();
  }

  void TearDown() override {
    if (spdlog_logger_) {
      spdlog_logger_->shutdown();
    }
    if (glog_logger_) {
      glog_logger_->shutdown();
    }
  }

  std::unique_ptr<Log<SpdlogBackend>> spdlog_logger_;
  std::unique_ptr<Log<GlogBackend>> glog_logger_;
};

TEST_F(CrossBackendConsistencyTest, LevelSettingConsistency) {
  // 测试所有级别的设置是否一致
  std::vector<LogLevel> levels = {
    LogLevel::kTrace, LogLevel::kDebug, LogLevel::kInfo,
    LogLevel::kWarn, LogLevel::kError, LogLevel::kCritical
  };
  
  for (LogLevel level : levels) {
    absl::Status status1 = spdlog_logger_->set_level(level);
    absl::Status status2 = glog_logger_->set_level(level);
    
    EXPECT_TRUE(status1.ok()) << "Spdlog failed to set level " << static_cast<int>(level);
    EXPECT_TRUE(status2.ok()) << "Glog failed to set level " << static_cast<int>(level);
    
    EXPECT_EQ(spdlog_logger_->get_level(), level);
    EXPECT_EQ(glog_logger_->get_level(), level);
    
    // 检查级别启用状态的一致性
    EXPECT_EQ(spdlog_logger_->is_enabled(level), glog_logger_->is_enabled(level));
    EXPECT_EQ(spdlog_logger_->is_enabled(LogLevel::kInfo), glog_logger_->is_enabled(LogLevel::kInfo));
    EXPECT_EQ(spdlog_logger_->is_enabled(LogLevel::kError), glog_logger_->is_enabled(LogLevel::kError));
  }
}

TEST_F(CrossBackendConsistencyTest, BasicOperationsConsistency) {
  // 测试基本操作的一致性
  EXPECT_EQ(spdlog_logger_->get_level(), glog_logger_->get_level());
  EXPECT_EQ(spdlog_logger_->is_enabled(LogLevel::kInfo), glog_logger_->is_enabled(LogLevel::kInfo));
  EXPECT_EQ(spdlog_logger_->is_enabled(LogLevel::kError), glog_logger_->is_enabled(LogLevel::kError));
  
  // 测试级别设置
  absl::Status status1 = spdlog_logger_->set_level(LogLevel::kWarn);
  absl::Status status2 = glog_logger_->set_level(LogLevel::kWarn);
  
  EXPECT_TRUE(status1.ok());
  EXPECT_TRUE(status2.ok());
  EXPECT_EQ(spdlog_logger_->get_level(), glog_logger_->get_level());
}
#endif
