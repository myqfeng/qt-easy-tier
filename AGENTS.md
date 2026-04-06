# QtEasyTier 项目上下文

## 项目概述

QtEasyTier 是一个基于 Qt C++ 框架开发的异地组网工具，提供直观的图形界面帮助用户配置和管理虚拟网络。其后端核心为 EasyTier —— 一个去中心化的组网方案，无需依赖中心服务器，每个节点平等独立，为用户提供安全、可靠、低成本的异地组网服务。

### 项目特点
- **快速**: 纯 Qt C++ 开发，无 Chromium/Webview，前端占用不超过 50MB
- **美观**: UI 样式移植自 KDE 的 Breeze 样式
- **简单**: 直观的网络配置管理界面
- **丰富**: 支持 EasyTier 大部分功能，按需定制虚拟网络
- **安全**: 基于 EasyTier 去中心化组网方案
- **高效**: 使用 FFI 直接调用核心库，无需子进程管理

### 技术栈
- **语言**: C++ 20
- **框架**: Qt 6（推荐 6.10.1），支持 Qt5 向后兼容
- **构建系统**: CMake 3.16+
- **UI 样式**: KDE Breeze Style（移植版）
- **核心绑定**: EasyTier FFI（Rust 库的 C 绑定）

### 当前版本
- 版本号: 1.4.2
- 平台支持: Windows 10/11、Linux

---

## 目录结构

```
QtEasyTier/
├── Qt_Gui/               # 主界面源码
│   ├── qtetmain.h/cpp/ui # 主窗口类
│   └── qtetnetwork.h/cpp/ui # 网络配置页面
├── Qt_Items/             # 自定义控件
│   ├── qtetcheckbtn.h/cpp    # 自定义开关按钮
│   ├── qtetlistwidget.h/cpp  # 自定义列表控件
│   └── qtetnodeinfo.h/cpp    # 节点信息显示控件
├── Qt_QRC/               # 资源文件（图标等）
│   ├── icons.qrc         # 资源文件定义
│   ├── icon.ico          # 应用程序图标
│   └── *.svg             # SVG 图标资源
├── SRC/                  # 核心源码
│   ├── main.cpp          # 程序入口、单实例检测、Breeze样式加载
│   ├── ETRunWorker.h/cpp # EasyTier FFI 运行工作类
│   └── networkconf.h/cpp # 网络配置数据结构
├── ThirdParty/           # 第三方依赖
│   ├── Breeze/           # Breeze 主题资源
│   │   ├── data/         # 配色方案、本地化
│   │   └── styles/       # 样式库
│   ├── EasyTier/         # EasyTier 核心库
│   │   ├── include/      # FFI 头文件 (easytier_ffi.h)
│   │   ├── library/      # Windows 静态库
│   │   └── share/        # 平台特定文件
│   │       ├── linux/    # Linux 可执行文件
│   │       └── windows/  # Windows 可执行文件
│   └── toml++/           # TOML 解析库
├── CDIRCalculator/       # CIDR 计算器子项目
├── assets/               # 项目资源（图片、manifest等）
│   ├── app.manifest      # Windows 应用程序清单
│   ├── publicserver.json # 公共服务器列表
│   └── *.png/*.webp      # 图片资源
├── cmake-build-debug/    # Debug 构建目录
└── cmake-build-release/  # Release 构建目录
```

---

## 核心类说明

### QtETMain
主窗口类，负责：
- 欢迎页面和功能导航
- 系统托盘管理
- 界面切换（欢迎页、网络配置页）
- 单实例运行检测
- 网络启动/停止通知处理

**关键成员：**
- `m_mainStackedWidget`: 主堆叠窗口
- `m_helloPage`: 欢迎页面
- `m_networkPage`: 网络配置页面
- `m_trayIcon`: 系统托盘图标
- `m_trayMenu`: 托盘右键菜单

**信号槽：**
- `onNetworkStartedNotify()`: 网络启动完成通知
- `onNetworkStoppedNotify()`: 网络停止完成通知
- `onTrayIconActivated()`: 托盘图标激活处理

### QtETNetwork
网络配置页面，核心功能包括：
- **网络列表管理**: 新建、删除、切换网络配置
- **配置导入导出**: 支持 JSON 格式的配置文件
- **基础设置**: 用户名、网络名称、密码、IP 地址、DHCP
- **服务器管理**: 添加/删除服务器节点
- **高级设置**: KCP/QUIC 代理、TUN 模式、P2P、IPv6、加密等
- **运行管理**: 启动/停止网络实例
- **节点监测**: 实时显示网络中的节点信息

**关键成员：**
- `m_networkConfs`: 网络配置列表
- `m_networksList`: 网络列表控件
- `m_runThread`: 运行网络的工作线程
- `m_runWorker`: ETRunWorker 工作对象
- `m_monitorTimer`: 节点监测定时器
- `m_nodeInfoWidgets`: 节点信息控件列表

**信号：**
- `networkStarted(QString, bool, QString)`: 网络启动信号
- `networkStopped(QString, bool, QString)`: 网络停止信号

### ETRunWorker
EasyTier FFI 运行工作类，设计用于在独立线程中运行，负责：
- **网络实例管理**: 启动、停止网络实例
- **FFI 调用**: 封装 EasyTier Rust 库的 C 绑定接口
- **信息收集**: 收集运行中网络实例的状态信息

**关键成员：**
- 内部使用 `EasyTierFFI` 命名空间的函数

**信号：**
- `etRunStarted(string, bool, string)`: 网络启动完成
- `etRunStopped(string, bool, string)`: 网络停止完成
- `infosCollected(vector<KVPair>)`: 网络信息收集完成

**常量配置：**
- `MAX_WAIT_TIME = 30000`: 最大等待时间
- `MAX_NETWORK_INSTANCES = 16`: 最大网络实例数

### NetworkConf
网络配置数据类，包含：
- **基础配置**: hostname、network_name、network_secret、ipv4、dhcp
- **服务器列表**: peers 列表
- **高级设置**: 各类功能开关（KCP、QUIC、P2P、IPv6 等）
- **实例标识**: instance_name（用于 FFI 管理）
- **运行状态**: is_running 标志

**关键方法：**
- `readFromUI()`: 从界面读取配置
- `readFromJson()`: 从 JSON 读取配置
- `toJson()`: 输出为 JSON
- `toToml()`: 输出为 TOML 配置字符串
- `generateInstanceName()`: 生成随机实例名称

### QtETNodeInfo
节点信息显示控件，提供：
- **连接类型显示**: 直连（绿）、中转（橙）、服务器（蓝）
- **节点信息**: 主机名、虚拟 IP、延迟、协议、连接方式
- **本机标识**: 紫色标签标识本机节点
- **悬停效果**: 边框高亮动画

**关键成员：**
- `m_nodeInfo`: 节点信息结构体
- `m_connTypeLabel`: 连接类型标签
- `m_localLabel`: 本机标签

### QtETCheckBtn
自定义开关按钮控件，特点：
- **左边文字，右边开关**: 现代化开关样式
- **平滑动画**: 滑块过渡动效
- **悬停高亮**: 边框渐变效果
- **提示文字**: 支持显示提示信息

### QtETListWidget
自定义列表控件，特点：
- **圆角高亮**: 选中项圆角高亮效果
- **渐显动画**: 选中/悬停渐变动画
- **自定义委托**: QtETListWidgetDelegate 绘制

---

## EasyTier FFI 接口

### 命名空间 EasyTierFFI

**核心函数：**

| 函数 | 说明 |
|------|------|
| `parseConfig(cfgStr)` | 解析 TOML 配置字符串，验证语法 |
| `runNetworkInstance(cfgStr)` | 创建并启动网络实例 |
| `retainNetworkInstance(instNames, length)` | 批量管理实例，终止不在列表中的实例 |
| `collectNetworkInfos(infos, maxLength)` | 收集所有运行实例的状态信息 |
| `getErrorMsg()` | 获取最后的错误信息 |

**数据结构：**
```cpp
struct KVPair {
    std::string key;    // 实例名称
    std::string value;  // JSON 格式的实例信息
};
```

---

## 构建与运行

### 环境要求
- Qt 6（推荐 6.10.1），支持 Qt5 向后兼容
- CMake 3.16+
- C++ 20 标准
- llvm-mingw 编译器（Windows）

### 构建命令

#### 正常模式（配置保存在系统路径）
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
cmake --install . --config Release
```

#### 便携模式（配置保存在程序目录）
```bash
mkdir build
cd build
cmake .. -DSAVE_CONF_IN_APP_DIR=true
cmake --build . --config Release
cmake --install . --config Release
```

> 注意：便携模式构建的程序禁用开机自启功能，避免污染系统环境。

### 构建输出
- 可执行文件输出到 `build/bin/`
- 安装目录为 `Install/`

### 运行参数
- `--auto-start`: 开机自启模式启动（不显示主窗口）

### 构建依赖复制
CMake 构建过程会自动复制以下内容到输出目录：
- ThirdParty/Breeze 目录（主题资源）
- ThirdParty/EasyTier/share/{platform} 目录（平台特定文件）
- assets/publicserver.json（公共服务器列表）
- CIDRCalculator（CIDR 计算器）
- Windows: 自动调用 windeployqt 部署 Qt 运行时
- Linux: 可选使用 linuxdeployqt 部署

---

## 编码规范

### 文件组织
- 主界面源码放在 `Qt_Gui/` 目录
- 自定义控件放在 `Qt_Items/` 目录
- 资源文件放在 `Qt_QRC/` 目录
- 核心工具类放在 `SRC/` 目录
- 第三方依赖放在 `ThirdParty/` 目录
- 项目资源放在 `assets/` 目录

### 命名约定
- 类名: 大驼峰命名（如 `QtETMain`, `QtETNetwork`）
- 成员变量: `m_` 前缀 + 小驼峰（如 `m_networkConfs`, `m_runWorker`）
- 私有方法: 无特殊前缀
- 信号/槽: 小驼峰命名，槽函数以 `on` 开头

### Qt 信号槽
使用 Qt 5+ 的函数指针连接方式：
```cpp
connect(button, &QPushButton::clicked, this, &ClassName::onButtonClicked);
```

### 线程安全
- 工作类（如 `ETRunWorker`）继承 `QObject`，可在独立线程中运行
- 信号槽连接使用 `Qt::QueuedConnection` 确保线程安全
- 遵循 Qt 线程绑定工作对象的规范

### 注释规范
使用 Doxygen 风格注释：
```cpp
/// @brief 简要描述
/// @param name 参数说明
/// @return 返回值说明
```

### 类型转换注意事项
Qt 6 中 `QList::size()` 等方法返回 `qsizetype`（即 `long long`），在赋值给 `int` 变量时需使用 `static_cast<int>()` 显式转换，避免 Clang-Tidy 窄化转换警告：
```cpp
for (int i = 0; i < static_cast<int>(list.size()); ++i) {
    // ...
}
```

---

## 关键实现细节

### 单实例运行
使用 `QLocalServer`/`QLocalSocket` 实现单实例检测，服务名称为 `QtEasyTier2-by-Viahuang`。

### 配置保存路径
通过编译宏 `SAVE_CONF_IN_APP_DIR` 控制：
- `true`: 配置保存在程序目录的 `config/` 子目录（便携模式）
- `false`: 配置保存在系统标准配置路径（`QStandardPaths::AppConfigLocation`）

### EasyTier FFI 集成
- 使用 `easytier_ffi.h` 头文件定义的 C 接口
- 链接 `easytier_ffi` 静态库（Windows）或动态库（Linux）
- 通过 `ETRunWorker` 类在独立线程中执行 FFI 调用
- 支持最多 16 个网络实例同时运行

### 网络配置管理
- 配置以 JSON 格式存储
- 支持配置导入/导出
- 每个网络配置有唯一的 `instance_name` 标识

### 节点监测
- 使用定时器（`m_monitorTimer`）定期调用 `collectNetworkInfos()`
- 解析 JSON 格式的节点信息并更新 UI
- 显示节点连接类型、延迟、协议等信息

### 系统托盘
- 支持最小化到系统托盘
- 托盘菜单：显示/隐藏窗口、退出程序
- 网络启动/停止时发送系统通知

### 主题色
- 高亮主题色: `#66ccff`
- 支持深色/浅色模式自动切换

### Windows 资源嵌入
- 自动生成 RC 文件嵌入 manifest 和图标
- manifest 文件位于 `assets/app.manifest`
- 图标文件位于 `Qt_QRC/icon.ico`
- 程序请求管理员权限运行

---

## 依赖说明

### Qt 组件
- `Qt::Widgets` - GUI 组件
- `Qt::Network` - 网络功能

### EasyTier FFI
- 头文件: `ThirdParty/EasyTier/include/easytier_ffi.h`
- Windows 库: `ThirdParty/EasyTier/library/` 和 `ThirdParty/EasyTier/share/windows/`
- Linux 库: `ThirdParty/EasyTier/share/linux/`

### 第三方库
- **Breeze 样式**: 从 KDE 移植的样式库，放置在 `ThirdParty/Breeze/` 目录
- **toml++**: TOML 解析库，放置在 `ThirdParty/toml++/` 目录

---

## 常见任务

### 添加新的网络配置选项
1. 在 `networkconf.h` 中添加成员变量
2. 在 `NetworkConf::readFromUI()` 和 `NetworkConf::toJson()` 中添加读写逻辑
3. 在 `QtETNetwork` 的初始化函数中创建对应的 UI 控件
4. 在 `NetworkConf::toToml()` 中添加 TOML 配置生成逻辑

### 修改 UI 布局
直接编辑 `Qt_Gui/` 或 `Qt_Items/` 目录下的 `.ui` 文件（Qt Designer 格式）。

### 添加新的自定义控件
1. 在 `Qt_Items/` 目录下创建 `.h` 和 `.cpp` 文件
2. 继承适当的 Qt 控件基类
3. 重写 `paintEvent()` 实现自定义绘制
4. 在 `CMakeLists.txt` 中已配置自动包含

### 调试构建问题
- 检查 CMake 配置输出
- 确认 Qt 路径正确设置
- 验证 `ThirdParty/EasyTier/` 目录结构正确
- 检查 `assets/app.manifest` 文件是否存在

### 更新公共服务器列表
编辑 `assets/publicserver.json` 文件，格式为 JSON 数组，每项包含 `url` 和 `contributor` 字段。

---

## Git 提交规范

- 提交信息使用中文描述
- 保持原子提交，每个 commit 只做一件事
- 不要提交 `build*/`、`Install/`、`*.user` 等生成文件
- `ThirdParty/EasyTier/` 目录已添加到 `.gitignore`（仅保留目录结构）

---

## 联系方式

- 项目地址: https://gitee.com/viagrahuang/qt-easy-tier
- 问题反馈: 欢迎提交 Issue 和 PR
- 交流群: EasyTier支持3群 (957189589)
- 许可证: GPLv3.0