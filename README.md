# QXCore 日志模块

QXCore 日志模块是一个高性能、可扩展的 C++ 日志库，专为量化计算场景设计。它提供了统一的日志接口，支持多种后端实现，并遵循 QXCore 的核心设计原则。

## 特性

- 🚀 **高性能**：零拷贝设计、异步日志、SIMD 优化
- 🔧 **模块化**：支持多种后端（spdlog、glog 等）
- 🛡️ **无异常**：使用 `absl::Status` 进行错误处理
- 🧵 **线程安全**：支持多线程并发日志记录
- 📝 **格式化**：支持 fmt 风格的格式化字符串
- 🎯 **量化优化**：专为量化计算场景设计

## 快速开始

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
  
  // 记录日志
  logger.info("应用启动成功");
  logger.warn("这是一个警告消息");
  logger.error("这是一个错误消息");
  
  // 格式化日志
  logger.info("用户 {} 执行了操作 {}", user_id, action);
  
  // 使用日志宏
  QXLOG_INFO(logger, "使用宏记录日志: {}", "测试消息");
  
  // 清理
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
custom_logger.shutdown();
```

## 项目结构

```
qxcore/
├── include/qxcore/log/          # 头文件
│   ├── log_level.h             # 日志级别定义
│   ├── log.h                   # 统一日志接口
│   ├── spdlog_backend.h        # Spdlog 后端
│   └── glog_backend.h          # Glog 后端
├── src/qxcore/log/            # 源文件
│   ├── log_level.cc            # 日志级别实现
│   ├── log.cc                  # 统一日志接口实现
│   ├── spdlog_backend.cc       # Spdlog 后端实现
│   └── glog_backend.cc        # Glog 后端实现
├── tests/qxcore/log/          # 测试文件
│   ├── log_level_test.cc       # 日志级别测试
│   ├── log_test.cc            # 核心接口测试
│   ├── spdlog_backend_test.cc  # Spdlog 后端测试
│   ├── glog_backend_test.cc    # Glog 后端测试
│   ├── consistency_test.cc     # 后端一致性测试
│   └── log_benchmark.cc       # 性能基准测试
├── examples/                   # 示例代码
│   └── log_example.cc         # 使用示例
└── docs/                      # 文档
    └── log_api.md             # API 文档
```

## 构建和安装

### 依赖要求

- **C++17** 或更高版本
- **CMake** 3.12 或更高版本
- **Abseil** (必需)
- **spdlog** (可选，默认启用)
- **glog** (可选，默认关闭)
- **GoogleTest** (测试需要)
- **Google Benchmark** (基准测试需要)

### 构建步骤

```bash
# 克隆仓库
git clone https://github.com/all3n/qxcore.git
cd qxcore

# 创建构建目录
mkdir build && cd build

# 配置构建
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
make -j$(nproc)

# 运行测试
ctest

# 安装
make install
```

### CMake 配置选项

```cmake
# 启用 spdlog 后端（默认开启）
option(QXCORE_ENABLE_LOG_SPDLOG "Enable spdlog backend" ON)

# 启用 glog 后端（默认关闭）
option(QXCORE_ENABLE_LOG_GLOG "Enable glog backend" OFF)

# 构建测试（默认开启）
option(QXCORE_BUILD_TESTS "Build tests" ON)

# 构建示例（默认开启）
option(QXCORE_BUILD_EXAMPLES "Build examples" ON)
```

## 使用指南

### 日志级别

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

### 后端选择

#### SpdlogBackend (推荐)
- 高性能异步日志
- 多 sink 输出
- 彩色控制台输出
- 自动日志轮转

#### GlogBackend
- Google 风格日志格式
- 崩溃处理
- 性能分析集成
- 自动日志轮转

### 性能优化

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

## 测试

### 运行单元测试

```bash
# 构建测试
make qxcore_log_tests

# 运行测试
./tests/qxcore_log_tests

# 或使用 CTest
ctest --output-on-failure
```

### 运行性能基准测试

```bash
# 构建基准测试
make qxcore_log_benchmarks

# 运行基准测试
./tests/qxcore_log_benchmarks
```

### 测试覆盖率

```bash
# 启用覆盖率
cmake .. -DCMAKE_BUILD_TYPE=Debug -DQXCORE_ENABLE_COVERAGE=ON

# 生成覆盖率报告
make coverage
```

## 性能基准

基于基准测试的性能数据（参考值）：

| 操作 | SpdlogBackend | GlogBackend |
|------|---------------|-------------|
| 简单日志 | ~50 ns/op | ~80 ns/op |
| 格式化日志 | ~100 ns/op | ~150 ns/op |
| 级别过滤 | ~5 ns/op | ~8 ns/op |

*注：实际性能取决于硬件配置、编译选项和具体使用场景。*

## 文档

- [API 文档](docs/log_api.md) - 详细的 API 使用说明
- [示例代码](examples/log_example.cc) - 完整的使用示例
- [设计文档](CLAUDE.md) - 项目设计原则和规范

## 贡献指南

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建 Pull Request

### 代码规范

- 遵循 Google C++ 风格指南
- 使用 clang-format 格式化代码
- 所有公共接口必须有文档注释
- 新功能必须包含单元测试
- 性能关键代码需要基准测试

## 许可证

本项目采用 Apache License 2.0 许可证。详见 [LICENSE](LICENSE) 文件。

## 更新日志

### v1.0.0 (2024-12-01)

- ✨ 初始版本发布
- 🚀 实现 SpdlogBackend 和 GlogBackend
- 🧵 线程安全的日志接口
- 📝 fmt 风格的格式化支持
- 🧪 完整的单元测试和基准测试
- 📚 详细的 API 文档

## 支持

如果您遇到问题或有建议，请：

1. 查看 [API 文档](docs/log_api.md)
2. 搜索 [Issues](https://github.com/all3n/qxcore/issues)
3. 创建新的 Issue
4. 联系维护者

## 致谢

感谢以下开源项目的支持：

- [Abseil](https://abseil.io/) - C++ 库集合
- [spdlog](https://github.com/gabime/spdlog) - 快速 C++ 日志库
- [glog](https://github.com/google/glog) - Google C++ 日志库
- [GoogleTest](https://github.com/google/googletest) - C++ 测试框架
- [Google Benchmark](https://github.com/google/benchmark) - C++ 基准测试框架
