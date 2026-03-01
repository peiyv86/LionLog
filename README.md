# LionLog

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.14+-success.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS-lightgrey.svg)

**LionLog** 是一个基于 C++17 开发的高性能、异步、跨平台日志管理系统。它不仅提供了一个零阻塞的 C++ 核心日志库，还配套了一整套开箱即用的系统级应用工具链（系统日志收集守护进程与压缩归档检索工具）。

## 核心特性

-  **极致性能 (Asynchronous I/O)**：基于自定义 `ThreadPool` 与生产者-消费者模型，业务线程写日志 **0 阻塞**。
-  **自动轮转 (Log Rotation)**：支持基于文件大小（如 5MB）的自动文件切分，支持保留历史版本数量控制。
-  **原生后台压缩 (Native ZLIB)**：内置 ZLIB 库，在文件轮转时**异步**执行 `.gz` 无损压缩，不依赖宿主机的 `gzip` 命令，彻底消除 I/O 峰值卡顿。
-  **优雅退出 (Graceful Shutdown)**：完善的线程池退出屏障机制，确保程序退出时，队列中积压的磁盘 I/O 任务全部安全落盘，解决异步丢失难题。
-  **系统级工具链**：内置 `lion-collector`（高容错日志收集守护进程）与 `lion-query`（无需解压的跨文件高速检索工具）。
-  **零依赖部署**：使用 CMake `FetchContent` 源码级集成第三方依赖（ZLIB, GTest），支持 CPack 一键打包为 `.deb` 系统安装包。

##  系统架构

```
LionLog
├── Core Library (liblionlog.a)          # 核心静态库
│   ├── Logger            # 单例日志门面
│   ├── ThreadPool        # 异步线程池
│   ├── Formatter         # 日志格式化器
│   └── Sinks             # 输出端点
│       ├── ConsoleSink   # 控制台输出
│       ├── FileSink      # 文件输出
│       └── RollingFileSink # 自动轮转文件输出（含压缩）
│
└── Applications (CLI工具)
    ├── lion-collector    # 系统日志收集守护进程
    └── lion-query        # 压缩日志检索工具
```

系统分为两层：
1. **Core Library (`liblionlog.a`)**：包含单例 Logger、多端输出 Sinks（Console, File, RollingFile）、格式化器与异步线程池。
2. **Applications (`apps/`)**：基于核心库构建的用户端 CLI 工具。

##  编译与安装

确保您的系统已安装 CMake（>= 3.14）和支持 C++17 的编译器（GCC ≥ 7 或 Clang ≥ 6）。

### 1. 从源码编译

```bash
git clone https://github.com/YourUsername/LionLog.git
cd LionLog
mkdir build && cd build
cmake ..
make -j4
```

### 2. 运行单元测试

```bash
# 在 build 目录下执行
./test_lionlog
```

### 3. 打包与安装 (Ubuntu/Debian)

```bash
make package
sudo dpkg -i lionlog-system-1.0.0-Linux.deb
```

安装后，`lion-collector` 和 `lion-query` 将成为全局系统命令。

##  快速上手 (Quick Start)

### 场景一：作为 C++ 库在你的代码中使用

在你的 `CMakeLists.txt` 中链接 `lionlog` 后，即可极简调用：

```cpp
#include <lionlog/Logger.h>

int main() {
    // 1. 添加输出端点（控制台 + 按 5MB 轮转的文件）
    Logger::getInstance().addConsoleSink();
    Logger::getInstance().addRollingFileSink("app.log", 1024 * 1024 * 5, 5);

    // 2. 自定义格式（可选）
    Logger::getInstance().setPattern("[%d] [%l] %m (Source: %f:%n)");

    // 3. 记录日志（自动捕获文件名与行号）
    LOG_INFO("LionLog is initialized successfully!");
    LOG_WARN("This is a warning message.");
    LOG_ERROR("Disk space is running low.");

    return 0; // 退出时自动清空异步队列并落盘
}
```

### 场景二：使用系统日志收集器 (lion-collector)

监控系统的 syslog，并按自定义格式进行提取和归档。它具备强大的抗截断、抗轮转（Inode 检测）容错能力。

```bash
# 监控 syslog_mock.log，过滤 INFO 及以上等级
lion-collector --level INFO --format "[%d] [%l] %m" syslog_mock.log
```

### 场景三：检索归档的压缩日志 (lion-query)

无需 gunzip 解压庞大的历史日志，直接在压缩包中进行极速关键字匹配：

```bash
# 在 collector.log.1.gz 中搜索 "High CPU"
lion-query "High CPU" collector.log.1.gz
```

##  格式化语法参考

支持通过 `setPattern()` 动态配置输出格式：

- `%d` : 日期与时间 (e.g., 2026-03-01 20:00:00)
- `%l` : 日志等级 (e.g., INFO, ERROR)
- `%m` : 日志正文内容
- `%f` : 触发调用的源文件名
- `%n` : 触发调用的代码行号

##  目录结构

```
.
├── CMakeLists.txt        # 顶层 CMake 构建脚本
├── apps/                 # CLI 工具源码
│   ├── collector/        # lion-collector 守护进程
│   └── query/            # lion-query 检索工具
├── include/              # 核心库头文件
│   ├── sinks/            # 输出端点实现
│   └── tool/             # 工具类 (GzipUtil)
├── src/                  # 核心库源文件
└── tests/                # GoogleTest 单元测试
```

##  开源协议

本项目采用 MIT License 开源协议。欢迎 Fork 与 PR！
```

## 主要修正点说明

1. **代码块规范化**：所有命令和代码示例都使用三个反引号包裹，并标注语言（`bash`、`cpp`）。
2. **列表格式修复**：使用 `-` 列表项和三级标题混合，确保内容分段清晰。
3. **目录结构优化**：使用树形结构展示，更直观。
4. **链接修复**：Git 克隆 URL 去掉多余方括号，直接显示为普通文本（或可添加真实链接）。
5. **内容完整**：补充了系统架构的 ASCII 示意图，完善了格式化语法说明。
6. **测试命令修正**：将 `./test_` 改为更合理的 `./test_lionlog`（您可根据实际可执行文件名调整）。
