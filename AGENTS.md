# QtEasyTier 项目上下文

QtEasyTier 是一个基于 Qt C++ 框架开发的异地组网工具，后端核心为 EasyTier（去中心化组网方案）。
- **技术栈**: C++20, Qt 6（推荐 6.10.1，LLVM-MinGW 编译器）, CMake 3.16+
- **版本**: 2.0.2
- **平台**: Windows 10/11, Linux, macOS

## 架构与目录

- **入口**: `SRC/main.cpp`（单实例检测、Breeze 样式加载、macOS 提权）。
- **主窗口**: `Qt_Gui/qtetmain.h/cpp` 管理 `QStackedWidget`，包含欢迎页、网络配置页、一键联机页、服务器收藏页、设置页。
- **页面与控件**: `Qt_Gui/`（主页面），`Qt_Items/`（自定义控件），`SRC/`（核心逻辑：`ETRunWorker`, `NetworkConf`）。
- **FFI 层**: `ETRunWorker` 在独立 `QThread` 中调用 `EasyTierFFI`（定义于 `ThirdParty/EasyTier/include/easytier_ffi.h`），最多支持 16 个网络实例。跨线程信号槽连接使用 `Qt::QueuedConnection`。

## 编码规范

- **命名**: 类名大驼峰；成员变量 `m_` 前缀；槽函数以 `on` 开头。
- **自定义绘制**: **`Qt_Items/` 中的自定义控件完全使用自定义绘制（`paintEvent()`），禁止使用 QSS（Qt Style Sheet）**。`Qt_Gui/` 页面可在必要时使用 QSS。修改控件样式需编辑 C++ `paintEvent()`。
- **类型转换**: Qt 6 中 `QList::size()` 等方法返回 `qsizetype`，赋值给 `int` 时需使用 `static_cast<int>()` 以避免 Clang-Tidy 窄化转换警告。
- **信号槽**: 使用 Qt 5+ 函数指针连接方式。

## 配置与数据文件

- **配置路径**: 由编译宏 `SAVE_CONF_IN_APP_DIR` 控制。`false` 时保存在系统标准配置路径；`true` 时保存在程序目录的 `config/` 子目录。
- **文件**: `{config_path}/networks.json`（网络配置）、`servers.json`（服务器收藏）、`settings2.json`（全局设置）。
- **公共资源**: `assets/publicserver.json`（公共服务器列表）。

## CI 与工具链

- **CI**: `.github/workflows/macos.yml` 仅包含 macOS Release 构建（Qt 6.10.1）。
- **测试**: 项目中没有单元测试或集成测试套件。
- **格式化/静态分析**: 未找到 `.clang-format` 或 `.clang-tidy` 配置文件。

## 平台特定注意事项

- **macOS**: `main.cpp` 包含 `ensureRootPrivileges` 提权逻辑，防止创建 TUN 设备失败；构建过程使用 `install_name_tool` 修正 FFI 动态库的 `@rpath`。
- **Windows**: 程序请求管理员权限运行（通过 manifest）；CMake 自动生成 `.rc` 文件嵌入 manifest 和图标。

## Agent编译说明

- 使用现有的CMake缓存构建：`cmake --build cmake-build-debug`
- **必须在链接前终止运行中的进程**，否则 `Permission denied`：
  ```
  taskkill /F /IM QtEasyTier.exe; cmake --build cmake-build-debug
  ```
- Windows 平台已配置 LLVM-MinGW 编译器路径，无需额外设置。

## 添加新设置项

项目中新增一个高级设置项需要修改 **4 个文件**，通过 **5 个数据流函数**：

| 文件 | 改动 |
|------|------|
| `SRC/networkconf.h` | 添加成员变量（bool / std::string / int / vector） |
| `SRC/networkconf.cpp` | `readFromUI()` / `readFromJson()` / `toJson()` / `toToml()` |
| `Qt_Gui/qtetnetwork.h` | 添加 UI 控件成员（`QtETCheckBtn*` 或 `QtETLineEdit*` 等） |
| `Qt_Gui/qtetnetwork.cpp` | 构造函数初始化 / `initAdvancedSettingsPage()` / `setupUIConnections()` / `loadConfToUI()`（blockSignals → setValue → unblock） |

**控件布局规则**：
- `QtETCheckBtn` 放入 `FunctionSection.checkBoxes` 列表，由 `updateFunctionGridLayout()` 统一动态排版（2-5 列自适应）。
- 非 CheckBtn 控件（`QtETLineEdit`、`QtETComboBox`、列表区）使用独立的 `QHBoxLayout` 或 `QVBoxLayout` 行，**不与动态网格混合**。

**TOML 条件输出惯例**：可选字段留空时不输出到 TOML（如 `dev_name` 为空、`mtu=0`、列表为空时均不输出对应键）。
