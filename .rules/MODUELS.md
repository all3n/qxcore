## 1. Log 模块 (spdlog 封装)

### 接口设计
`include/qxcore/log/logger.hpp`
```cpp
#pragma once
#include <memory>
#include <string>
#include <string_view>

namespace qxcore {
namespace log {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL,
    OFF
};

class Logger {
public:
    virtual ~Logger() = default;
    
    virtual void log(LogLevel level, std::string_view message) = 0;
    virtual void flush() = 0;
    virtual void set_level(LogLevel level) = 0;
    virtual LogLevel get_level() const = 0;
    
    // 工厂函数
    static std::shared_ptr<Logger> create_console_logger(std::string_view name);
    static std::shared_ptr<Logger> create_file_logger(std::string_view name, std::string_view filename);
    static std::shared_ptr<Logger> create_rotating_file_logger(
        std::string_view name, std::string_view filename, size_t max_size, size_t max_files);
};

// 全局日志器
void init_global_logger(std::shared_ptr<Logger> logger);
std::shared_ptr<Logger> get_global_logger();

} // namespace log
} // namespace qxcore

// 日志宏
#define QXLOG_TRACE(...)    ::qxcore::log::get_global_logger()->log(::qxcore::log::LogLevel::TRACE, fmt::format(__VA_ARGS__))
#define QXLOG_DEBUG(...)    ::qxcore::log::get_global_logger()->log(::qxcore::log::LogLevel::DEBUG, fmt::format(__VA_ARGS__))
#define QXLOG_INFO(...)     ::qxcore::log::get_global_logger()->log(::qxcore::log::LogLevel::INFO, fmt::format(__VA_ARGS__))
#define QXLOG_WARN(...)     ::qxcore::log::get_global_logger()->log(::qxcore::log::LogLevel::WARN, fmt::format(__VA_ARGS__))
#define QXLOG_ERROR(...)    ::qxcore::log::get_global_logger()->log(::qxcore::log::LogLevel::ERROR, fmt::format(__VA_ARGS__))
#define QXLOG_CRITICAL(...) ::qxcore::log::get_global_logger()->log(::qxcore::log::LogLevel::CRITICAL, fmt::format(__VA_ARGS__))
```

### Spdlog 后端实现
`src/qxcore/backends/spdlog/spdlog_backend.cpp`
```cpp
#include "qxcore/log/logger.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace qxcore {
namespace log {

class SpdLogger : public Logger {
    std::shared_ptr<spdlog::logger> logger_;
    
public:
    explicit SpdLogger(std::shared_ptr<spdlog::logger> logger) : logger_(std::move(logger)) {}
    
    void log(LogLevel level, std::string_view message) override {
        switch(level) {
            case LogLevel::TRACE: logger_->trace(message); break;
            case LogLevel::DEBUG: logger_->debug(message); break;
            case LogLevel::INFO: logger_->info(message); break;
            case LogLevel::WARN: logger_->warn(message); break;
            case LogLevel::ERROR: logger_->error(message); break;
            case LogLevel::CRITICAL: logger_->critical(message); break;
            default: break;
        }
    }
    
    void flush() override { logger_->flush(); }
    
    void set_level(LogLevel level) override {
        logger_->set_level(static_cast<spdlog::level::level_enum>(level));
    }
    
    LogLevel get_level() const override {
        return static_cast<LogLevel>(logger_->level());
    }
};

std::shared_ptr<Logger> Logger::create_console_logger(std::string_view name) {
    auto spd_logger = spdlog::stdout_color_mt(std::string(name));
    return std::make_shared<SpdLogger>(spd_logger);
}

std::shared_ptr<Logger> Logger::create_file_logger(std::string_view name, std::string_view filename) {
    auto spd_logger = spdlog::basic_logger_mt(std::string(name), std::string(filename));
    return std::make_shared<SpdLogger>(spd_logger);
}

std::shared_ptr<Logger> Logger::create_rotating_file_logger(
    std::string_view name, std::string_view filename, size_t max_size, size_t max_files) {
    auto spd_logger = spdlog::rotating_logger_mt(
        std::string(name), std::string(filename), max_size, max_files);
    return std::make_shared<SpdLogger>(spd_logger);
}

} // namespace log
} // namespace qxcore
```

## 2. Time 模块

### 接口设计
`include/qxcore/time/datetime.hpp`
```cpp
#pragma once
#include <chrono>
#include <string>
#include <string_view>

namespace qxcore {
namespace time {

using Timestamp = std::chrono::system_clock::time_point;
using Duration = std::chrono::nanoseconds;

class DateTime {
    Timestamp timestamp_;
    
public:
    DateTime() = default;
    explicit DateTime(Timestamp timestamp) : timestamp_(timestamp) {}
    explicit DateTime(std::string_view iso_string);
    
    static DateTime now();
    static DateTime from_ymd(int year, int month, int day);
    static DateTime from_ymdhms(int year, int month, int day, int hour, int minute, int second);
    
    // 格式转换
    std::string to_iso_string() const;
    std::string to_string(std::string_view format) const;
    
    // 时间运算
    DateTime add_days(int days) const;
    DateTime add_hours(int hours) const;
    DateTime add_minutes(int minutes) const;
    DateTime add_seconds(int seconds) const;
    
    // 获取组件
    int year() const;
    int month() const;
    int day() const;
    int hour() const;
    int minute() const;
    int second() const;
    int millisecond() const;
    
    // 交易日相关
    bool is_trading_day() const;
    bool is_weekend() const;
    DateTime next_trading_day() const;
    DateTime previous_trading_day() const;
    
    Timestamp timestamp() const { return timestamp_; }
    
    // 比较操作符
    bool operator==(const DateTime& other) const { return timestamp_ == other.timestamp_; }
    bool operator<(const DateTime& other) const { return timestamp_ < other.timestamp_; }
    // ... 其他操作符
};

// 时间区间
class TimeRange {
    DateTime start_;
    DateTime end_;
    
public:
    TimeRange(DateTime start, DateTime end) : start_(start), end_(end) {}
    
    bool contains(const DateTime& dt) const { return dt >= start_ && dt <= end_; }
    Duration duration() const { return end_.timestamp() - start_.timestamp(); }
    
    DateTime start() const { return start_; }
    DateTime end() const { return end_; }
};

// 交易日历
class TradingCalendar {
public:
    virtual ~TradingCalendar() = default;
    
    virtual bool is_trading_day(const DateTime& dt) const = 0;
    virtual DateTime next_trading_day(const DateTime& dt) const = 0;
    virtual DateTime previous_trading_day(const DateTime& dt) const = 0;
    virtual std::vector<DateTime> get_trading_days(const TimeRange& range) const = 0;
    
    static std::shared_ptr<TradingCalendar> create_china_calendar();
    static std::shared_ptr<TradingCalendar> create_us_calendar();
};

} // namespace time
} // namespace qxcore
```

## 3. Format 模块

### JSON 接口 (rapidjson)
`include/qxcore/format/json.hpp`
```cpp
#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <memory>

namespace qxcore {
namespace format {

class JsonValue {
    class Impl;
    std::unique_ptr<Impl> impl_;
    
public:
    JsonValue();
    JsonValue(std::nullptr_t);
    JsonValue(bool value);
    JsonValue(int value);
    JsonValue(double value);
    JsonValue(std::string_view value);
    JsonValue(const JsonValue& other);
    JsonValue(JsonValue&& other) noexcept;
    ~JsonValue();
    
    JsonValue& operator=(const JsonValue& other);
    JsonValue& operator=(JsonValue&& other) noexcept;
    
    // 类型检查
    bool is_null() const;
    bool is_bool() const;
    bool is_int() const;
    bool is_double() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;
    
    // 值获取
    bool as_bool() const;
    int as_int() const;
    double as_double() const;
    std::string as_string() const;
    
    // 数组操作
    size_t size() const;
    JsonValue& operator[](size_t index);
    const JsonValue& operator[](size_t index) const;
    void push_back(const JsonValue& value);
    
    // 对象操作
    bool has_key(std::string_view key) const;
    JsonValue& operator[](std::string_view key);
    const JsonValue& operator[](std::string_view key) const;
    void set(std::string_view key, const JsonValue& value);
    std::vector<std::string> keys() const;
    
    // 序列化/反序列化
    static JsonValue parse(std::string_view json_str);
    std::string stringify(bool pretty = false) const;
};

class JsonDocument {
    JsonValue root_;
    
public:
    static JsonDocument parse(std::string_view json_str);
    static JsonDocument create_object();
    static JsonDocument create_array();
    
    JsonValue& root() { return root_; }
    const JsonValue& root() const { return root_; }
    
    std::string stringify(bool pretty = false) const { return root_.stringify(pretty); }
};

} // namespace format
} // namespace qxcore
```

### YAML 接口 (yaml-cpp)
`include/qxcore/format/yaml.hpp`
```cpp
#pragma once
#include <string>
#include <string_view>

namespace qxcore {
namespace format {

class YamlValue {
    class Impl;
    std::unique_ptr<Impl> impl_;
    
public:
    YamlValue();
    YamlValue(std::string_view value);
    YamlValue(const YamlValue& other);
    YamlValue(YamlValue&& other) noexcept;
    ~YamlValue();
    
    // 类似JsonValue的接口，保持一致性
    // ...
};

class YamlDocument {
public:
    static YamlDocument parse(std::string_view yaml_str);
    static YamlDocument parse_file(std::string_view filename);
    
    YamlValue& root();
    const YamlValue& root() const;
    
    std::string stringify() const;
    void save_to_file(std::string_view filename) const;
};

} // namespace format
} // namespace qxcore
```

### INI 接口
`include/qxcore/format/ini.hpp`
```cpp
#pragma once
#include <string>
#include <string_view>
#include <map>
#include <vector>

namespace qxcore {
namespace format {

class IniDocument {
    std::map<std::string, std::map<std::string, std::string>> data_;
    
public:
    static IniDocument parse(std::string_view ini_str);
    static IniDocument parse_file(std::string_view filename);
    
    // 节操作
    bool has_section(std::string_view section) const;
    std::vector<std::string> sections() const;
    
    // 键值操作
    bool has_key(std::string_view section, std::string_view key) const;
    std::string get(std::string_view section, std::string_view key, std::string_view default_val = "") const;
    void set(std::string_view section, std::string_view key, std::string_view value);
    
    // 类型安全获取
    int get_int(std::string_view section, std::string_view key, int default_val = 0) const;
    double get_double(std::string_view section, std::string_view key, double default_val = 0.0) const;
    bool get_bool(std::string_view section, std::string_view key, bool default_val = false) const;
    
    // 序列化
    std::string stringify() const;
    void save_to_file(std::string_view filename) const;
};

} // namespace format
} // namespace qxcore
```

## 4. IO 模块

`include/qxcore/io/file.hpp`
```cpp
#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <fstream>

namespace qxcore {
namespace io {

class File {
public:
    static bool exists(std::string_view path);
    static bool is_file(std::string_view path);
    static bool is_directory(std::string_view path);
    static uint64_t file_size(std::string_view path);
    
    static std::string read_all_text(std::string_view path);
    static std::vector<uint8_t> read_all_bytes(std::string_view path);
    static std::vector<std::string> read_all_lines(std::string_view path);
    
    static void write_all_text(std::string_view path, std::string_view content);
    static void write_all_bytes(std::string_view path, const std::vector<uint8_t>& data);
    static void append_all_text(std::string_view path, std::string_view content);
    
    static bool create_directory(std::string_view path);
    static bool remove(std::string_view path);
    static bool rename(std::string_view old_path, std::string_view new_path);
    
    static std::vector<std::string> list_files(std::string_view directory);
    static std::vector<std::string> list_directories(std::string_view directory);
};

// 内存映射文件
class MemoryMappedFile {
    void* data_;
    size_t size_;
    
public:
    explicit MemoryMappedFile(std::string_view path);
    ~MemoryMappedFile();
    
    const void* data() const { return data_; }
    void* data() { return data_; }
    size_t size() const { return size_; }
    
    // 禁止拷贝
    MemoryMappedFile(const MemoryMappedFile&) = delete;
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
};

} // namespace io
} // namespace qxcore
```

## 5. Requests 模块 (HTTP客户端)

`include/qxcore/network/requests.hpp`
```cpp
#pragma once
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <memory>

namespace qxcore {
namespace network {

class Response {
    int status_code_;
    std::string status_text_;
    std::vector<uint8_t> content_;
    std::map<std::string, std::string> headers_;
    
public:
    Response(int status_code, std::string_view status_text, 
             std::vector<uint8_t> content, 
             std::map<std::string, std::string> headers)
        : status_code_(status_code), status_text_(status_text),
          content_(std::move(content)), headers_(std::move(headers)) {}
    
    int status_code() const { return status_code_; }
    const std::string& status_text() const { return status_text_; }
    const std::vector<uint8_t>& content() const { return content_; }
    const std::map<std::string, std::string>& headers() const { return headers_; }
    
    std::string text() const;
    bool ok() const { return status_code_ >= 200 && status_code_ < 300; }
    
    std::string header(std::string_view key) const {
        auto it = headers_.find(std::string(key));
        return it != headers_.end() ? it->second : "";
    }
};

class Session {
    class Impl;
    std::unique_ptr<Impl> impl_;
    
public:
    Session();
    ~Session();
    
    // 请求方法
    Response get(std::string_view url, 
                 const std::map<std::string, std::string>& headers = {});
    
    Response post(std::string_view url, 
                  const std::vector<uint8_t>& data,
                  const std::map<std::string, std::string>& headers = {});
    
    Response put(std::string_view url,
                 const std::vector<uint8_t>& data,
                 const std::map<std::string, std::string>& headers = {});
    
    Response delete_(std::string_view url,
                     const std::map<std::string, std::string>& headers = {});
    
    // 便捷方法
    Response post_json(std::string_view url, 
                       const format::JsonValue& json,
                       const std::map<std::string, std::string>& headers = {});
    
    Response post_form(std::string_view url,
                       const std::map<std::string, std::string>& form_data,
                       const std::map<std::string, std::string>& headers = {});
    
    // 会话配置
    void set_timeout(int milliseconds);
    void set_headers(const std::map<std::string, std::string>& headers);
    void set_auth(std::string_view username, std::string_view password);
    void set_proxy(std::string_view proxy_url);
};

// 便捷全局函数
Response get(std::string_view url, 
             const std::map<std::string, std::string>& headers = {});
Response post(std::string_view url, 
              const std::vector<uint8_t>& data,
              const std::map<std::string, std::string>& headers = {});
Response post_json(std::string_view url, 
                   const format::JsonValue& json,
                   const std::map<std::string, std::string>& headers = {});

} // namespace network
} // namespace qxcore
```

## CMake 配置更新

`CMakeLists.txt` 需要添加相应的依赖检测：

```cmake
# 检测和配置新模块依赖
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.11.0
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# 类似处理 rapidjson, yaml-cpp, libcurl 等依赖
```

## 使用示例

```cpp
#include "qxcore/log/logger.hpp"
#include "qxcore/time/datetime.hpp"
#include "qxcore/format/json.hpp"
#include "qxcore/network/requests.hpp"

int main() {
    // 初始化日志
    auto logger = qxcore::log::Logger::create_console_logger("quant");
    logger->set_level(qxcore::log::LogLevel::INFO);
    qxcore::log::init_global_logger(logger);
    
    QXLOG_INFO("启动量化计算服务");
    
    // 时间处理
    auto now = qxcore::time::DateTime::now();
    QXLOG_INFO("当前时间: {}", now.to_iso_string());
    
    // JSON 配置
    auto config = qxcore::format::JsonDocument::parse(R"({
        "strategy": "momentum",
        "parameters": {"lookback": 20, "threshold": 0.05}
    })");
    
    // HTTP 请求获取市场数据
    try {
        auto response = qxcore::network::get("https://api.marketdata.com/quotes/AAPL");
        if (response.ok()) {
            auto data = qxcore::format::JsonDocument::parse(response.text());
            QXLOG_INFO("AAPL 价格: {}", data.root()["price"].as_double());
        }
    } catch (const std::exception& e) {
        QXLOG_ERROR("获取市场数据失败: {}", e.what());
    }
    
    return 0;
}
```

这些新模块的设计遵循了：
- **零拷贝原则**: 使用 `string_view` 避免不必要的字符串拷贝
- **RAII 原则**: 所有资源自动管理
- **接口一致性**: 相似的模块提供相似的API
- **性能优先**: 内存映射文件、连接复用等优化
- **异常安全**: 所有操作保证异常安全

每个模块都可以独立使用，同时又能很好地集成到量化计算框架中。