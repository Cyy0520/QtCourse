# 天气数据展示与趋势分析系统

基于 Qt 6 开发的天气数据展示与趋势分析桌面应用程序。

## 功能特性

- 🏠 实时天气展示
- 📅 天气预报（逐小时/多日）
- 📊 数据可视化分析
- 📜 历史数据查询
- 🏙️ 城市管理
- ⚙️ 个性化设置

## 技术栈

- Qt 6.9.2
- C++17
- SQLite 数据库
- Qt Charts 图表库
- Qt Network 网络模块

## 编译运行

### 环境要求

- Qt 6.9.2 或更高版本
- MinGW 64-bit 编译器
- CMake 3.16+ (可选)

### 编译步骤

```bash
# 使用 qmake
qmake WeatherAnalysis.pro
make

# 或使用 Qt Creator 直接打开 .pro 文件
```

## 项目结构

```
WeatherAnalysis/
├── src/                    # 源代码目录
│   ├── main.cpp           # 程序入口
│   ├── mainwindow.cpp     # 主窗口实现
│   ├── mainwindow.h       # 主窗口声明
│   └── mainwindow.ui      # 主窗口UI文件
├── WeatherAnalysis.pro    # Qt项目文件
├── .gitignore
└── README.md
```

## 开发进度

- [x] Task 1: 项目初始化 + 基础窗口框架
- [ ] Task 2: SQLite数据库初始化
- [ ] Task 3: 网络请求基础封装
- [ ] ...

## 许可证

MIT License
