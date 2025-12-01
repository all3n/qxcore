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
 * @file log_example.cc
 * @brief QXCore 日志模块使用示例
 * 
 * 本示例展示了如何使用 QXCore 日志模块的各种功能：
 * - 基本日志记录
 * - 格式化日志
 * - 日志级别控制
 * - 不同后端的使用
 * - 自定义日志器
 */

#include "qxcore/log/log.h"
#include <iostream>
#include <thread>
#include <vector>

using namespace qxcore::log;

// 示例 1: 基本日志使用
void BasicLoggingExample() {
  std::cout << "\n=== 基本日志使用示例 ===\n";
  
  // 使用全局默认日志器
  DefaultLog& logger = GetDefaultLogger();
  
  // 基本日志记录
  logger.trace("这是一条 TRACE 级别的日志");
  logger.debug("这是一条 DEBUG 级别的日志");
  logger.info("这是一条 INFO 级别的日志");
  logger.warn("这是一条 WARN 级别的日志");
  logger.error("这是一条 ERROR 级别的日志");
  logger.critical("这是一条 CRITICAL 级别的日志");
  
  // 刷新确保日志输出
  logger.flush();
}

// 示例 2: 格式化日志
void FormattedLoggingExample() {
  std::cout << "\n=== 格式化日志示例 ===\n";
  
  DefaultLog& logger = GetDefaultLogger();
  
  // 格式化日志记录
  int user_id = 12345;
  std::string action = "login";
  double success_rate = 98.5;
  
  logger.info("用户 {} 执行了 {} 操作", user_id, action);
  logger.warn("操作成功率: {:.1f}%", success_rate);
  logger.error("处理失败，错误码: {}", 404);
  
  logger.flush();
}

// 示例 3: 日志级别控制
void LogLevelControlExample() {
  std::cout << "\n=== 日志级别控制示例 ===\n";
  
  DefaultLog& logger = GetDefaultLogger();
  
  // 设置为 WARN 级别
  absl::Status status = logger.set_level(LogLevel::kWarn);
  if (!status.ok()) {
    std::cerr << "设置日志级别失败: " << status.message() << std::endl;
  }
  std::cout << "设置日志级别为 WARN\n";
  
  // 这些日志会被过滤掉
  logger.trace("这条 TRACE 日志不会显示");
  logger.debug("这条 DEBUG 日志不会显示");
  logger.info("这条 INFO 日志不会显示");
  
  // 这些日志会显示
  logger.warn("这条 WARN 日志会显示");
  logger.error("这条 ERROR 日志会显示");
  logger.critical("这条 CRITICAL 日志会显示");
  
  // 恢复到 DEBUG 级别
  status = logger.set_level(LogLevel::kDebug);
  if (!status.ok()) {
    std::cerr << "恢复日志级别失败: " << status.message() << std::endl;
  }
  std::cout << "恢复日志级别为 DEBUG\n";
  
  logger.debug("现在 DEBUG 日志又可以显示了");
  
  logger.flush();
}

// 示例 4: 使用日志宏
void LogMacroExample() {
  std::cout << "\n=== 日志宏使用示例 ===\n";
  
  // 使用便捷的日志宏
  QXLOG_TRACE(GetDefaultLogger(), "使用宏记录 TRACE 日志");
  QXLOG_DEBUG(GetDefaultLogger(), "使用宏记录 DEBUG 日志");
  QXLOG_INFO(GetDefaultLogger(), "使用宏记录 INFO 日志");
  QXLOG_WARN(GetDefaultLogger(), "使用宏记录 WARN 日志");
  QXLOG_ERROR(GetDefaultLogger(), "使用宏记录 ERROR 日志");
  QXLOG_CRITICAL(GetDefaultLogger(), "使用宏记录 CRITICAL 日志");
  
  // 格式化日志宏
  QXLOG_INFO(GetDefaultLogger(), "使用宏格式化: {} + {} = {}", 10, 20, 30);
  
  GetDefaultLogger().flush();
}

// 示例 5: 自定义日志器
void CustomLoggerExample() {
  std::cout << "\n=== 自定义日志器示例 ===\n";
  
  // 创建自定义日志器
  Log<SpdlogBackend> custom_logger;
  
  // 初始化自定义日志器
  absl::Status status = custom_logger.init("custom_logger", LogLevel::kDebug);
  if (!status.ok()) {
    std::cerr << "初始化自定义日志器失败: " << status.message() << std::endl;
    return;
  }
  
  std::cout << "自定义日志器初始化成功\n";
  
  // 使用自定义日志器
  custom_logger.info("这是来自自定义日志器的消息");
  custom_logger.warn("自定义日志器警告: {}", "测试警告");
  custom_logger.error("自定义日志器错误码: {}", 500);
  
  custom_logger.flush();
  custom_logger.shutdown();
}

// 示例 6: 多线程日志
void MultiThreadLoggingExample() {
  std::cout << "\n=== 多线程日志示例 ===\n";
  
  DefaultLog& logger = GetDefaultLogger();
  absl::Status status = logger.set_level(LogLevel::kInfo);
  if (!status.ok()) {
    std::cerr << "设置日志级别失败: " << status.message() << std::endl;
  }
  
  const int num_threads = 4;
  const int messages_per_thread = 10;
  std::vector<std::thread> threads;
  
  auto worker = [&](int thread_id) {
    for (int i = 0; i < messages_per_thread; ++i) {
      logger.info("线程 {} 消息 {}", thread_id, i);
      
      // 模拟一些工作
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  };
  
  // 启动多个线程
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(worker, i);
  }
  
  // 等待所有线程完成
  for (auto& t : threads) {
    t.join();
  }
  
  logger.info("多线程日志测试完成");
  logger.flush();
}

// 示例 7: 性能测试
void PerformanceExample() {
  std::cout << "\n=== 性能测试示例 ===\n";
  
  DefaultLog& logger = GetDefaultLogger();
  absl::Status status = logger.set_level(LogLevel::kInfo);
  if (!status.ok()) {
    std::cerr << "设置日志级别失败: " << status.message() << std::endl;
  }
  
  const int num_messages = 10000;
  auto start = std::chrono::high_resolution_clock::now();
  
  for (int i = 0; i < num_messages; ++i) {
    logger.info("性能测试消息 {}", i);
  }
  
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  
  double avg_time = static_cast<double>(duration.count()) / num_messages;
  double throughput = num_messages * 1000000.0 / duration.count();
  
  std::cout << "记录 " << num_messages << " 条日志耗时: " 
            << duration.count() << " 微秒\n";
  std::cout << "平均每条日志耗时: " << avg_time << " 微秒\n";
  std::cout << "日志吞吐量: " << throughput << " 条/秒\n";
  
  logger.flush();
}

int main() {
  std::cout << "QXCore 日志模块使用示例\n";
  std::cout << "========================\n";
  
  try {
    // 初始化默认日志器
    absl::Status status = InitDefaultLogger("qxcore_example", LogLevel::kDebug);
    if (!status.ok()) {
      std::cerr << "初始化默认日志器失败: " << status.message() << std::endl;
      return 1;
    }
    
    std::cout << "默认日志器初始化成功\n";
    
    // 运行各种示例
    BasicLoggingExample();
    FormattedLoggingExample();
    LogLevelControlExample();
    LogMacroExample();
    CustomLoggerExample();
    MultiThreadLoggingExample();
    PerformanceExample();
    
    std::cout << "\n=== 所有示例运行完成 ===\n";
    
    // 清理
    GetDefaultLogger().shutdown();
    
  } catch (const std::exception& e) {
    std::cerr << "示例运行出错: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
