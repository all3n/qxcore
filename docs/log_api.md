# QXCore 日志模块 API 文档

## 概述

QXCore 日志模块是一个高性能、可扩展的 C++ 日志库，专为量化计算场景设计。它提供了统一的日志接口，支持多种后端实现，并遵循 QXCore 的核心设计原则：

- **无异常设计**：使用 `absl::Status` 进行错误处理
- **高性能**：支持零拷贝、异步日志、SIMD 优化
- **模块化**：支持多种后端（spdlog、glog 等）
- **线程安全**：支持多线程并发日志记录
- **格式化支持**：支持 fmt 风格的格式化字符串

## 核心组件

### 1. 日志级别 (LogLevel)

```cpp
enum class LogLevel {
  kTrace = 0,     // 跟踪级别
  kDebug = 1,     // 调试级别  
  kInfo = 2,      // 信息级别
  kWarn = 3,      // 警告级别
  kError = 4,     // 错误级别
  kCritical = 5   // 严重错误级别
};
```

#### 级别工具函数

```cpp
// 将日志级别转换为字符串
absl::string_view LogLevelToString(LogLevel level);

// 将字符串转换为日志级别
bool StringToLogLevel(absl::string_view str, LogLevel& level);

// 检查日志级别是否启用
bool IsLogLevelEnabled(LogLevel current_level, LogLevel check_level);
```

### 2. 日志后端接口

#### SpdlogBackend

基于 spdlog 的高性能日志后端，支持：
- 多 sink 输出（控制台 + 文件）
- 异步日志
- 彩色输出
- 高性能格式化

```cpp
class SpdlogBackend {
public:
  // 初始化日志系统
  absl::Status init(const std::string& name, LogLevel level = LogLevel::kInfo);
  
  // 设置日志级别
  absl::Status set_level(LogLevel level);
  
  // 获取当前日志级别
  LogLevel get_level() const;
  
  // 检查日志级别是否启用
  bool is_enabled(LogLevel level) const;
  
  // 基础日志接口
  void log(LogLevel level, absl::string_view msg);
  
  // 格式化日志接口
  template<typename... Args>
  void logf(LogLevel level, absl::string_view fmt_str, Args&&... args);
  
  // 刷新日志缓冲区
  void flush();
  
  // 关闭日志系统
  void shutdown();
};
```

#### GlogBackend

基于 Google Glog 的日志后端，支持：
- Google 风格的日志格式
- 自动日志轮转
- 崩溃处理
- 性能分析集成

```cpp
class GlogBackend {
public:
  // 接口与 SpdlogBackend 完全一致
  // ...
};
```

### 3. 统一日志接口

```cpp
template<typename Backend>
class Log {
public:
  // 初始化日志系统
  absl::Status init(const std::string& name, LogLevel level = LogLevel::kInfo);
  
  // 设置日志级别
  absl::Status set_level(LogLevel level);
  
  // 获取当前日志级别
  LogLevel get_level() const;
  
  // 检查日志级别是否启用
  bool is_enabled(LogLevel level) const;
  
  // 基础日志接口
  void log(LogLevel level, absl::string_view msg);
  
  // 格式化日志接口
  template<typename... Args>
  void logf(LogLevel level, absl::string_view fmt_str, Args&&... args);
  
  // 便捷方法
  template<typename... Args>
  void trace(absl::string_view fmt_str, Args&&... args);
  
  template<typename... Args>
  void debug(absl::string_view fmt_str, Args&&... args);
  
  template<typename... Args>
  void info(absl::string_view fmt_str, Args&&... args);
  
  template<typename... Args>
  void warn(absl::string_view fmt_str, Args&&... args);
  
  template<typename... Args>
  void error(absl::string_view fmt_str, Args&&... args);
  
  template<typename... Args>
  void critical(absl::string_view fmt_str, Args&&... args);
  
  // 刷新和关闭
  void flush();
  void shutdown();
};
```

### 4. 默认日志器

```cpp
// 默认日志器类型别名
using DefaultLog = Log<SpdlogBackend>;

// 获取全局默认日志器
DefaultLog& GetDefaultLogger();

// 初始化全局默认日志器
absl::Status InitDefaultLogger(const std::string& name, LogLevel level);
```

### 5. 日志宏

```cpp
// 便捷的日志宏
#define QXLOG_TRACE(logger, ...) logger.trace(__VA_ARGS__)
#define QXLOG_DEBUG(logger, ...) logger.debug(__VA_ARGS__)
#define QXLOG_INFO(logger, ...)  logger.info(__VA_ARGS__)
#define QXLOG_WARN(logger, ...)  logger.warn(__VA_ARGS__)
#define QXLOG_ERROR(logger, ...) logger.error(__VA_ARGS__)
#define QXLOG_CRITICAL(logger, ...) logger.critical(__VA_ARGS__)
```

## 使用指南

### 基本使用

```cpp
#include "qxcore/log/log.h"

using namespace qxcore::log;

int main() {
  // 初始化默认日志器
  absl::Status status = InitDefaultLogger("my_app", LogLevel::kInfo);
  if (!status.ok()) {
    std::cerr << "初始化失败: " << status.message() << std::endl;
    return 1;
  }
  
  // 获取默认日志器
  DefaultLog& logger = GetDefaultLogger();
  
  // 记录不同级别的日志
  logger.trace("调试信息");
  logger.debug("调试详情");
  logger.info("一般信息");
  logger.warn("警告信息");
  logger.error("错误信息");
  logger.critical("严重错误");
  
  // 格式化日志
  logger.info("用户 {} 执行了操作 {}", user_id, action);
  
  // 使用日志宏
  QXLOG_INFO(logger, "使用宏记录日志: {}", "测试消息");
  
  // 刷新并关闭
  logger.flush();
  logger.shutdown();
  
  return 0;
}
```

### 自定义日志器

```cpp
// 使用 Spdlog 后端创建自定义日志器
Log<SpdlogBackend> custom_logger;

// 初始化
absl::Status status = custom_logger.init("custom", LogLevel::kDebug);
if (!status.ok()) {
  // 处理错误
}

// 使用自定义日志器
custom_logger.info("自定义日志器消息");

// 动态调整日志级别
custom_logger.set_level(LogLevel::kWarn);

// 清理
custom_logger.shutdown();
```

### 多线程日志

```cpp
#include <thread>
#include <vector>

void worker_thread(int thread_id) {
  DefaultLog& logger = GetDefaultLogger();
  
  for (int i = 0; i < 100; ++i) {
    logger.info("线程 {} 消息 {}", thread_id, i);
  }
}

int main() {
  InitDefaultLogger("multithread", LogLevel::kInfo);
  
  std::vector<std::thread> threads;
  for (int i = 0; i < 4; ++i) {
    threads.emplace_back(worker_thread, i);
  }
  
  for (auto& t : threads) {
    t.join();
  }
  
  GetDefaultLogger().shutdown();
  return 0;
}
```

### 性能优化建议

1. **日志级别过滤**：在生产环境中设置合适的日志级别
2. **异步日志**：使用支持异步的后端（如 spdlog）
3. **批量日志**：避免频繁的单条日志记录
4. **格式化优化**：减少复杂的格式化操作

```cpp
// 好的做法：级别检查
if (logger.is_enabled(LogLevel::kDebug)) {
  logger.debug("复杂的调试信息: {}", expensive_computation());
}

// 好的做法：批量日志
for (const auto& item : items) {
  logger.info("处理项目: {}", item.id);
}
logger.flush();  // 批量刷新
```

## 配置选项

### CMake 配置

```cmake
# 启用 spdlog 后端（默认开启）
option(QXCORE_ENABLE_LOG_SPDLOG "Enable spdlog backend" ON)

# 启用 glog 后端（默认关闭）
option(QXCORE_ENABLE_LOG_GLOG "Enable glog backend" OFF)
```

### 编译时配置

```cpp
// 检查后端可用性
#ifdef QXCORE_ENABLE_LOG_SPDLOG
  // spdlog 相关代码
#endif

#ifdef QXCORE_ENABLE_LOG_GLOG
  // glog 相关代码
#endif
```

## 错误处理

所有日志操作都返回 `absl::Status` 或 `absl::StatusOr<T>`：

```cpp
absl::Status status = logger.init("test", LogLevel::kInfo);
if (!status.ok()) {
  switch (status.code()) {
    case absl::StatusCode::kAlreadyExists:
      // 处理重复初始化
      break;
    case absl::StatusCode::kFailedPrecondition:
      // 处理前置条件失败
      break;
    case absl::StatusCode::kInternal:
      // 处理内部错误
      break;
    default:
      // 处理其他错误
      break;
  }
}
```

## 性能基准

基于基准测试的性能数据（参考值）：

| 操作 | SpdlogBackend | GlogBackend |
|------|---------------|-------------|
| 简单日志 | ~50 ns/op | ~80 ns/op |
| 格式化日志 | ~100 ns/op | ~150 ns/op |
| 级别过滤 | ~5 ns/op | ~8 ns/op |

*注：实际性能取决于硬件配置、编译选项和具体使用场景。*

## 最佳实践

1. **初始化**：在程序启动时初始化日志器
2. **级别管理**：根据环境设置合适的日志级别
3. **资源管理**：程序结束时正确关闭日志器
4. **线程安全**：日志接口是线程安全的，无需额外同步
5. **错误处理**：始终检查初始化和配置操作的返回状态

## 故障排除

### 常见问题

1. **日志不显示**
   - 检查日志级别设置
   - 确认日志器已正确初始化
   - 验证输出权限

2. **性能问题**
   - 降低日志级别
   - 使用异步后端
   - 减少格式化复杂度

3. **编译错误**
   - 确认启用了所需的后端
   - 检查依赖库是否正确安装
   - 验证 C++17 支持

### 调试技巧

```cpp
// 启用详细日志进行调试
logger.set_level(LogLevel::kTrace);

// 检查后端状态
if (!logger.is_enabled(LogLevel::kInfo)) {
  std::cerr << "INFO 级别被禁用" << std::endl;
}

// 验证初始化状态
absl::Status status = logger.init("debug", LogLevel::kDebug);
if (!status.ok()) {
  std::cerr << "初始化失败: " << status.message() << std::endl;
}
```

## 扩展开发

### 自定义后端

实现自定义日志后端需要满足以下接口：

```cpp
class CustomBackend {
public:
  absl::Status init(const std::string& name, LogLevel level);
  absl::Status set_level(LogLevel level);
  LogLevel get_level() const;
  bool is_enabled(LogLevel level) const;
  void log(LogLevel level, absl::string_view msg);
  
  template<typename... Args>
  void logf(LogLevel level, absl::string_view fmt_str, Args&&... args);
  
  void flush();
  void shutdown();
};
```

### 集成到现有项目

1. 添加 QXCore 依赖
2. 包含必要的头文件
3. 初始化日志系统
4. 使用统一的日志接口

```cpp
// 在现有项目中集成
#include "qxcore/log/log.h"

class ExistingClass {
private:
  qxcore::log::DefaultLog& logger_ = qxcore::log::GetDefaultLogger();
  
public:
  void doSomething() {
    QXLOG_INFO(logger_, "执行操作: {}", __func__);
    // 业务逻辑
  }
};
```

## 版本兼容性

- **C++ 标准**：C++17 及以上
- **编译器**：GCC 7+, Clang 5+, MSVC 2017+
- **平台**：Linux, macOS, Windows
- **依赖**：Abseil, spdlog/glog（可选）

