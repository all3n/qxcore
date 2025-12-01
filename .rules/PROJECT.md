# 🚀 **QXCore 项目总规范（完整提示词）**

## **1. 核心原则**

* 专注于 **C++量化计算** 和 **高性能金融数据处理**
* 使用现代 C++（C++17/20/23）
* **禁止 RTTI，禁止 C++ 异常（throw/try/catch）**
* 全项目采用 **absl::Status / absl::StatusOr** 做错误处理
* 追求零拷贝、优化缓存局部性、SOA 布局
* 资源管理严格使用 RAII
* 模块化、可扩展、后端可插拔
* 针对量化计算场景设计优化（SIMD、GPU、多线程）

---

## **2. 响应风格**

* 简洁、直接、工程化
* 代码优先，质量优先，性能优先
* 采用现代 C++（RAII、智能指针、移动语义）
* 避免任何不必要拷贝，优先使用引用、span、view
* 注释少而精确
* 全局使用 Google 风格命名规范

---

## **3. 第三方库策略**

### **3.1 首选库（必须支持）**
不改动代码 third_party

* **absl**

  * absl::Status / StatusOr
  * absl::Span
  * absl::flat_hash_map / flat_hash_set
  * absl::Time
  * absl::Cord 等

### **3.2 可选库（可按模块启用）**

* **spdlog**（默认日志后端）
* **glog**（可选日志后端）
* **boost::asio**（可选网络模块）
* CUDA、OpenCL 等加速后端

所有第三方库均通过 CMake `option()` 控制启用。

---

## **4. 错误处理规范（严格 Google 风格，无异常）**

### **4.1 全局禁用异常**

* 禁止使用 `throw`, `try`, `catch`
* API 必须返回：

  * `absl::Status`
  * `absl::StatusOr<T>`

### **4.2 状态使用规范**

* 错误必须明确返回，例如：

  ```cpp
  return absl::InvalidArgumentError("size mismatch");
  ```
* 绝不使用 errno / int error_code

### **4.3 接口命名规范**

* `Status load_config(...)`
* `StatusOr<Data> parse_ohlcv(...)`

### **4.4 资源安全**

* RAII 自动释放资源
* Status 返回不会影响资源管理

---

## **5. 代码风格（Google 风格）**

### **5.1 命名**

* 类名：`OhlcvProcessor`
* 函数：`calculate_sma`
* 变量：`close_prices`
* 常量：`kMaxBufferSize`
* 枚举值：`DataSourceType::kMarketData`
* 宏：`QX_CORE_API`

### **5.2 文件组织**

* 头文件 `.h`，实现 `.cc`
* 文件名小写加下划线：`ohlcv_processor.h`
* 每个类一个头文件
* 包含顺序：相关 → C头 → C++头 → 第三方 → 项目头

---

## **6. 数据结构规范（OHLCV）**

* 使用 **SOA**（结构化数组）提高缓存友好性
* 统一前端接口
* 后端可定义特定内存布局
* 支持零拷贝数据视图（span/view）
* 数据在后端之间保持兼容

---

## **7. 后端抽象层设计**

### **7.1 编译期多态（首选）**

* 模板参数注入后端实现
* 避免虚函数和运行时开销

### **7.2 统一前端接口**

```cpp
template<typename Backend = AutoBackend>
class OhlcvProcessor {
 public:
  absl::Status process(const OhlcvData& data)
  {
    return backend_.process(data);
  }

 private:
  Backend backend_;
};
```

### **7.3 支持后端矩阵**

* CPU（基础）
* SIMD（AVX/AVX2/AVX-512、NEON/SVE）
* CUDA
* OpenCL
* 多线程后端

---

## **8. 日志组件（Log）规范**

### 1. 核心目标

* 统一日志接口，隐藏具体后端。
* 支持模板静态多态，可切换后端（默认 spdlog，可选 glog）。
* 支持 **fmt 风格格式化**。
* 提供宏接口（`QXLOG_INFO` 等）。
* 高性能、异步、安全。

---

### 2. 日志接口

```cpp
enum class LogLevel { kTrace, kDebug, kInfo, kWarn, kError, kCritical };

template <typename Backend>
class Log {
public:
    absl::Status init(const std::string& name, LogLevel level) { return backend_.init(name, level); }

    void log(LogLevel l, absl::string_view msg) { backend_.log(l, msg); }

    template <typename... Args>
    void logf(LogLevel l, absl::string_view fmt_str, Args&&... args) {
        backend_.logf(l, fmt_str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void trace(absl::string_view fmt_str, Args&&... args) { logf(LogLevel::kTrace, fmt_str, args...); }
    template <typename... Args>
    void debug(absl::string_view fmt_str, Args&&... args) { logf(LogLevel::kDebug, fmt_str, args...); }
    template <typename... Args>
    void info(absl::string_view fmt_str, Args&&... args)  { logf(LogLevel::kInfo, fmt_str, args...); }
    template <typename... Args>
    void warn(absl::string_view fmt_str, Args&&... args)  { logf(LogLevel::kWarn, fmt_str, args...); }
    template <typename... Args>
    void error(absl::string_view fmt_str, Args&&... args) { logf(LogLevel::kError, fmt_str, args...); }
    template <typename... Args>
    void critical(absl::string_view fmt_str, Args&&... args) { logf(LogLevel::kCritical, fmt_str, args...); }

private:
    Backend backend_;
};
```

---

### 3. 日志 后端设计

```cpp
struct SpdlogBackend {
    absl::Status init(const std::string& name, LogLevel level);
    void log(LogLevel l, absl::string_view msg);
    template <typename... Args>
    void logf(LogLevel l, absl::string_view fmt_str, Args&&... args);
};

struct GlogBackend {
    absl::Status init(const std::string& name, LogLevel level);
    void log(LogLevel l, absl::string_view msg);
    template <typename... Args>
    void logf(LogLevel l, absl::string_view fmt_str, Args&&... args);
};
```

---

### 4. 日志宏接口

```cpp
#define QXLOG_TRACE(logger, ...) logger.trace(__VA_ARGS__)
#define QXLOG_DEBUG(logger, ...) logger.debug(__VA_ARGS__)
#define QXLOG_INFO(logger, ...)  logger.info(__VA_ARGS__)
#define QXLOG_WARN(logger, ...)  logger.warn(__VA_ARGS__)
#define QXLOG_ERROR(logger, ...) logger.error(__VA_ARGS__)
#define QXLOG_CRITICAL(logger, ...) logger.critical(__VA_ARGS__)
```

---

### 5. 日志CMake 配置

```cmake
option(QXCORE_ENABLE_LOG_SPDLOG "Enable spdlog backend" ON)
option(QXCORE_ENABLE_LOG_GLOG "Enable glog backend" OFF)
```

---


## **9. 目录结构**

```
include/qxcore/common/
include/qxcore/log/
include/qxcore/ohlcv/
src/qxcore/backends/cpu/
src/qxcore/backends/simd/
src/qxcore/backends/cuda/
src/qxcore/backends/opencl/

tests/
examples/
benchmarks/
```

---

## **10. 性能优化重点**

* SOA 内存布局优化
* SIMD 加速自动向量化
* CPU/GPU 多后端统一接口
* 内存池、对象池、无锁结构
* 避免堆分配
* 批量处理接口优于逐元素处理
* 支持数据预分配

---

## **11. 模块开发流程**

1. 设计统一接口（含 Status 错误协议）
2. CPU 参考后端实现
3. SIMD/GPU/多线程等后端逐一实现
4. 后端自动检测逻辑（AutoBackend）
5. 接口一致性测试
6. 性能基准测试
7. 回归测试

---

## **12. 文件操作规范**

* 接口稳定性优先
* 后端实现可独立编译与测试
* CMake 自动检测能力
* 数据结构跨后端必须兼容

---

下面是在你已有规范基础上，**保持风格一致、工程化加强、流程更可执行** 的优化版本。
重点强化了：**模块开发流程、测试验证流程、架构一致性、后端一致性检查、性能基准规范**。

你可以直接把下面这段加入到你的提示词文档中。

---

# ✅ **13. 模块开发流程 **

每个模块必须遵循 **统一接口 → CPU baseline → 多后端实现 → 自动化验证 → 性能基准 → 文档输出** 的流程。

## **13.1 需求与接口定义阶段**

* 明确模块类型：

  * 数据结构模块
  * 算法模块（指标/统计/回测等）
  * I/O 模块（行情、存储）
  * 系统模块（线程池、日志、内存池等）

### 必须完成：

1. **定义统一前端接口（无异常、无RTTI）**
2. 所有接口返回 `absl::Status` / `absl::StatusOr<T>`
3. 提供最简可编译 header（包含文档）

### 示例：

```cpp
class XyzProcessor {
 public:
  Status init(const XyzConfig& cfg);
  StatusOr<Output> compute(absl::Span<const float> input) const;
};
```

---

## **13.2 CPU Baseline 实现（强制第一步）**

所有模块必须先实现**可读性最强、性能可接受的 CPU baseline**。

要求：

* 仅使用标准 C++ 与 absl
* 代码尽量直白、可验证
* 覆盖所有边界条件和错误路径
* 必须通过 full unit test 才能继续后端开发

目的：

✔ 作为 correctness baseline
✔ 为 SIMD/GPU 后端提供一致行为
✔ 便于自动化 diff/对比测试

---

## **13.3 多后端实现流程**

### 后端类型（按需启用）

* CPU baseline（强制）
* SIMD（AVX2 / AVX-512 / NEON / SVE）
* GPU（CUDA / OpenCL）
* 多线程（TBB / std::thread pool）
* 自定义高性能后端（内存池、零拷贝结构）

### 每个后端必须遵循：

1. **接口完全一致**
2. **返回值语义完全一致**
3. 针对 baseline 进行逐元素 / 批量一致性验证
4. 性能基准对比 baseline ≥ 1.5× 性能增益，否则不启用

---

# ✅ **14. 自动化测试验证流程（新增重点）**

QXCore 的测试系统分为：**单元测试 → 行为一致性测试 → 回归测试 → 性能基准测试**。

以下为强制流程：

---

## **14.1 单元测试（Unit Test）**

* 使用 GoogleTest
* 覆盖率要求：

  * 核心模块 ≥ 90%
  * 算法模块 ≥ 95%
* 所有错误路径必须测试：

```cpp
EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
```

---

## **14.2 行为一致性测试（Cross-backend Consistency Test）**

对所有实现后端：

```
CPU baseline 输出  ←→  SIMD 输出
CPU baseline 输出  ←→  CUDA 输出
CPU baseline 输出  ←→  MT 输出
```

要求：

* 对 input batch 进行逐元素比较
* 数值误差 ≤ 1e–7
* 状态/错误返回必须完全一致

示例自动化测试接口：

```cpp
Status TestBackendConsistency(const BackendA& a, const BackendB& b);
```

---

## **14.3 回归测试（Regression Test）**

用于避免未来修改导致算法偏移。

* 每个模块提供固定数据集（小、中、大）
* CPU baseline 输出作为金标准（golden results）
* CI 每次变更必须：

  * 与 golden results 做 diff
  * 不允许偏移（除非正式更新版本）

---

## **14.4 性能基准测试（Benchmark）**

* 使用 Google Benchmark
* 三种规模：small (10³) / medium (10⁵) / large (10⁷)
* 不同批次大小对性能影响需要记录
* 每个后端必须附上基准报告：

```
CPU_Baseline     120 ns/op
SIMD_AVX2         25 ns/op
CUDA_H100          0.8 ns/op
```

### 性能要求：

* SIMD ≥ 2× CPU baseline（必须）
* CUDA ≥ 10× CPU baseline（目标要求）
* 多线程应随 core 数线性提升

---

# ✅ **15. 模块文档与发布规范**

每个模块发布前必须包含：

### **15.1 用户文档（User API Doc）**

* 使用方式
* 参数与返回值语义
* 错误状态说明（包括所有 Status 返回）
* 示例代码

### **15.2 工程文档（Engineering Doc）**

* 内存布局（SOA / AOS / 特殊结构）
* 算法复杂度
* SIMD/GPU 优化要点
* 性能基准截图

### **15.3 限制说明**

明确列出：

* 输入要求
* 数据规模限制
* 线程安全语义
* 后端可用性（CPU/SIMD/CUDA/etc）

---

# ✅ **16. 模块验收 Checklist（新增）**

发布前必须全部满足：

### ✔ 1. 接口稳定性

* 接口完成度 ≥ 95%
* 全部 Status 语义明确

### ✔ 2. 行为一致性

* 所有后端通过一致性测试

### ✔ 3. 性能达标

* SIMD ≥ 2× CPU baseline
* CUDA ≥ 10× CPU baseline（如适用）

### ✔ 4. 覆盖率符合要求

* 单元测试覆盖率达标
* 所有错误路径均测试

### ✔ 5. 跨平台编译（CI）

* Linux（GCC/Clang）
* macOS（Clang）
* Windows（MSVC）可选

### ✔ 6. 文档完整

* User API
* Engineering Doc
* Benchmark 报告

明白，我们可以在 CMake 依赖管理中明确 **优先使用最新稳定版本库**，并保持系统检测 + FetchContent 拉取的机制。以下是优化建议和实现方式：

---
# ** 17. CMake 依赖管理与自动化安装

## **核心原则**

1. **默认使用最新稳定版本**，通过 FetchContent 拉取最新 release/tag。
2. **系统库检测仅作为加速**（如果系统库版本较旧，可强制升级或使用 FetchContent）。
3. **允许开发者覆盖版本**，通过 `-D<LIB>_TAG=<tag>` 控制。
4. **cmake** 依赖定义放到cmake 目录下
5. **module** 每个module cmake相关配置 需要 放到自己 module 对应目录中，父级别 cmake 配置 只include 即可
6. 保证模块可选性和跨平台一致性。

---

## **CMake 实现示例**

```cmake
# 默认版本（最新稳定 release）
set(ABSL_DEFAULT_TAG "20231223.0")        # 最新 Abseil release
set(SPDLOG_DEFAULT_TAG "v1.12.0")         # 最新 spdlog release
set(GLOG_DEFAULT_TAG "v0.6.1")            # 最新 glog release

macro(find_or_fetch_latest name repo default_tag)
    # 可通过 -D<LIB>_TAG 指定特定版本
    if(NOT DEFINED ${name}_TAG)
        set(${name}_TAG ${default_tag})
    endif()

    find_package(${name} QUIET)
    if(NOT ${name}_FOUND)
        message(STATUS "${name} not found or outdated, fetching ${${name}_TAG}...")
        FetchContent_Declare(
            ${name}
            GIT_REPOSITORY ${repo}
            GIT_TAG ${${name}_TAG}
        )
        FetchContent_MakeAvailable(${name})
    else()
        message(STATUS "${name} found system-wide (version may be older)")
    endif()
endmacro()

# 使用示例
if(QXCORE_ENABLE_ABSL)
    find_or_fetch_latest(absl "https://github.com/abseil/abseil-cpp.git" ${ABSL_DEFAULT_TAG})
endif()

if(QXCORE_ENABLE_SPDLOG)
    find_or_fetch_latest(spdlog "https://github.com/gabime/spdlog.git" ${SPDLOG_DEFAULT_TAG})
endif()

if(QXCORE_ENABLE_GLOG)
    find_or_fetch_latest(glog "https://github.com/google/glog.git" ${GLOG_DEFAULT_TAG})
endif()
```

---

## **策略说明**

1. 系统已安装库优先使用，但如果版本低于最新稳定版，可通过 `-D<LIB>_TAG=<tag>` 强制拉取最新。
2. CI 构建始终可以保证最新稳定版本。
3. 避免依赖冲突，同时保证性能优化模块与最新库兼容。

---


## 默认debug测试
```
cmake --preset default && cmake --build --preset default
```

## summary
1. 每个工具完成功能 放到summary 中 
