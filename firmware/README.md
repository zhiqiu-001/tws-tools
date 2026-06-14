# Firmware - ESP32 TWS 蓝牙耳机修复工具固件

基于 ESP-IDF 的 ESP32 固件，实现 BLE + SPP 双模蓝牙通信。

## 功能特性

- **BLE GATT 客户端**: 服务发现、特征值读写、通知订阅
- **SPP 蓝牙串口**: 经典蓝牙串口通信
- **UART 命令接口**: 串口命令解析和处理
- **多板级支持**: 支持不同型号的 TWS 耳机

## 目录结构

```
firmware/
├── main/
│   ├── boards/                   # 板级实现
│   │   ├── common/               # 公共抽象层
│   │   │   ├── board.h           # 板级抽象基类
│   │   │   ├── board.cc          # 基类实现（蓝牙控制器初始化）
│   │   │   ├── ble_manager.h     # BLE 管理器接口
│   │   │   ├── spp_manager.h     # SPP 管理器接口
│   │   │   └── uuid_defs.h       # UUID 定义宏
│   │   │
│   │   ├── bose-open-earbud-ultra/
│   │   │   ├── board_*.cc        # 板级实现
│   │   │   ├── ble_uuid.h        # UUID 定义
│   │   │   └── config.json       # 板子配置
│   │   │
│   │   └── le-bose-qc-earbuds/
│   │       ├── board_*.cc
│   │       └── config.json
│   │
│   ├── application.cc            # 应用初始化
│   ├── main.cc                   # ESP-IDF 入口
│   └── CMakeLists.txt
│
├── partitions/                   # 分区表
│   └── v1/
│       ├── 8m.csv                # 8MB Flash 分区
│       └── 16m.csv               # 16MB Flash 分区
│
├── scripts/
│   └── release.py                # 发布构建脚本
│
├── CMakeLists.txt                # 项目 CMake
├── sdkconfig.defaults            # 默认配置
└── Kconfig.projbuild             # 项目配置选项
```

## 环境要求

- ESP-IDF v5.0+
- ESP32 / ESP32-S3 开发板

## 编译与烧录

### 1. 设置目标芯片

```bash
idf.py set-target esp32
# 或 ESP32-S3
idf.py set-target esp32s3
```

### 2. 选择板子配置

通过 menuconfig 选择目标板子：

```bash
idf.py menuconfig
# 导航到: TWS Tools Configuration -> Board Selection
```

### 3. 编译

```bash
idf.py build
```

### 4. 烧录

```bash
idf.py -p COMx flash monitor
```

## UART 命令

固件支持以下 UART 命令（以换行符结束）：

| 命令 | 说明 |
|------|------|
| `status` | 查询设备状态 |
| `reset` | 重启设备 |
| `ble_connect` | 连接 BLE 设备 |
| `ble_disconnect` | 断开 BLE 连接 |
| `spp_connect` | 连接 SPP 设备 |
| `spp_disconnect` | 断开 SPP 连接 |

## 架构设计

详见 [../docs/architecture.md](../docs/architecture.md)

### 初始化流程

```
Board::initialize()
    │
    ├─ initializeBt()          // 蓝牙控制器初始化（基类）
    │   ├─ esp_bt_controller_init()
    │   ├─ esp_bt_controller_enable(BTDM)
    │   ├─ esp_bluedroid_init()
    │   └─ esp_bluedroid_enable()
    │
    ├─ m_bleManager->initialize()  // BLE 管理器初始化
    │   └─ esp_ble_gattc_app_register()
    │
    ├─ m_sppManager->initialize()  // SPP 管理器初始化
    │   └─ esp_spp_init()
    │
    └─ UartCommandTask()           // UART 任务启动
```

## 新增板子

详见 [../docs/board-design.md](../docs/board-design.md)

### 步骤概览

1. 在 `boards/` 下创建新目录
2. 创建 `config.json` 配置文件
3. 创建 `ble_uuid.h` 定义 UUID
4. 创建 `board_*.cc` 实现板级类
5. 更新 CMakeLists.txt