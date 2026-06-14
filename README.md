# TWS Tools

ESP32 TWS 蓝牙耳机修复工具，支持 BLE + SPP 双模蓝牙通信。

## 项目简介

本项目提供一套完整的 TWS（True Wireless Stereo）蓝牙耳机修复工具解决方案，包含：

- **firmware**: ESP32 固件，实现 BLE GATT 客户端和 SPP 串口通信
- **client**: Python GUI 客户端，提供设备扫描、修复工具、协议抓包等功能

## 功能特性

- 多板级支持（Bose Open Earbud Ultra、LE Bose QC Earbuds 等）
- BLE GATT 客户端（服务发现、特征值读写、通知订阅）
- SPP 蓝牙串口通信
- UART 命令接口
- 设备扫描与连接
- 修复工具（复位、固件升级、清除配对等）
- 协议抓包与分析

## 项目结构

```
tws-tools/
├── README.md                 # 项目概述（本文件）
├── docs/                     # 详细技术文档
│   ├── architecture.md       # 架构设计说明
│   ├── board-design.md       # 板级抽象设计
│   └── ble-spp-guide.md      # BLE/SPP 使用指南
│
├── client/                   # Python GUI 客户端
│   ├── main.py               # 主程序入口
│   ├── requirements.txt      # Python 依赖
│   └── README.md             # 客户端说明
│
└── firmware/                 # ESP32 固件
    ├── main/                 # 主程序源码
    │   ├── boards/           # 板级实现
    │   │   ├── common/       # 公共抽象层
    │   │   ├── bose-open-earbud-ultra/
    │   │   └── le-bose-qc-earbuds/
    │   ├── application.cc    # 应用入口
    │   └── main.cc           # ESP-IDF 入口
    ├── partitions/           # 分区表配置
    ├── scripts/              # 构建脚本
    ├── CMakeLists.txt        # CMake 配置
    └── README.md             # 固件说明
```

## 快速开始

### 固件编译与烧录

```bash
# 进入固件目录
cd firmware

# 选择目标板子（例如 bose-open-earbud-ultra）
idf.py set-target esp32

# 编译
idf.py build

# 烧录
idf.py -p COMx flash
```

### 客户端运行

```bash
# 进入客户端目录
cd client

# 安装依赖
pip install -r requirements.txt

# 运行
python main.py
```

## 支持的设备

| 设备 | 状态 | 说明 |
|------|------|------|
| Bose Open Earbud Ultra | ✅ 已实现 | 完整 UUID 定义和 BLE/SPP 实现 |
| LE Bose QC Earbuds | 🚧 占位 | 待补充 UUID 和具体实现 |

## 文档

详细技术文档请参阅 [docs/](docs/) 目录：

- [架构设计](docs/architecture.md) - 整体架构和模块关系
- [板级抽象设计](docs/board-design.md) - 如何新增板子和 UUID 管理
- [BLE/SPP 使用指南](docs/ble-spp-guide.md) - 蓝牙通信接口使用说明

## 许可证

MIT License