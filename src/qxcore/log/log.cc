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
#include <mutex>

// 确保包含完整的后端定义
#ifdef QXCORE_ENABLE_LOG_SPDLOG
#include "qxcore/log/spdlog_backend.h"
#elif defined(QXCORE_ENABLE_LOG_GLOG)
#include "qxcore/log/glog_backend.h"
#endif

namespace qxcore {
namespace log {

namespace {

// 全局日志器实例和互斥锁
std::mutex g_logger_mutex;
DefaultLog* g_logger = nullptr;

// 确保全局日志器在程序结束时正确清理
class LoggerGuard {
 public:
  ~LoggerGuard() {
    std::lock_guard<std::mutex> lock(g_logger_mutex);
    if (g_logger != nullptr) {
      g_logger->shutdown();
      delete g_logger;
      g_logger = nullptr;
    }
  }
};

static LoggerGuard g_logger_guard;

}  // anonymous namespace

DefaultLog& GetDefaultLogger() {
  std::lock_guard<std::mutex> lock(g_logger_mutex);
  
  if (g_logger == nullptr) {
    // 创建默认日志器
    g_logger = new DefaultLog();
    
    // 使用默认配置初始化
    absl::Status status = g_logger->init("qxcore_default", LogLevel::kInfo);
    if (!status.ok()) {
      // 如果初始化失败，我们无法使用日志系统记录错误
      // 但至少确保对象存在
    }
  }
  
  return *g_logger;
}

absl::Status InitDefaultLogger(const std::string& name, LogLevel level) {
  std::lock_guard<std::mutex> lock(g_logger_mutex);
  
  if (g_logger != nullptr) {
    // 如果已经存在，先关闭旧的
    g_logger->shutdown();
    delete g_logger;
  }
  
  // 创建新的日志器
  g_logger = new DefaultLog();
  return g_logger->init(name, level);
}

}  // namespace log
}  // namespace qxcore

// 显式实例化模板类
#ifdef QXCORE_ENABLE_LOG_SPDLOG
template class qxcore::log::Log<qxcore::log::SpdlogBackend>;
#elif defined(QXCORE_ENABLE_LOG_GLOG)
template class qxcore::log::Log<qxcore::log::GlogBackend>;
#endif
