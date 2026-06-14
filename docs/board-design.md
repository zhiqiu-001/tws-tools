# 板级抽象设计

本文档描述如何新增板子和 UUID 管理。

## 设计理念

### 核心原则

1. **抽象基类定义公共接口** - Board、BleManager、SppManager
2. **公共功能在基类实现** - 蓝牙控制器初始化只需调用一次
3. **差异化功能在子类实现** - UUID、具体 BLE/SPP 实现

### 避免重复

蓝牙控制器初始化在 `Board::initializeBt()` 中实现，所有板子只需调用此方法：

```cpp
void BoseOpenEarbudUltra::initialize() override {
    // 1. 调用基类方法（公共蓝牙控制器初始化）
    initializeBt();
    
    // 2. 初始化 BLE/SPP 管理器（板子特有）
    m_bleManager->initialize();
    m_sppManager->initialize();
}
```

## 文件结构

新增板子需要以下文件：

```
boards/
└── new-device/
    ├── board_new_device.cc      # 板级实现
    ├── ble_uuid.h               # UUID 定义
    └── config.json              # 板子配置
```

## 新增板子步骤

### 1. 创建目录和配置文件

```json
// config.json
{
    "manufacturer": "bose",
    "name": "new-device",
    "version": "1.0.0",
    "target": "esp32",
    "builds": [
        {
            "name": "new-device",
            "sdkconfig_append": []
        }
    ]
}
```

### 2. 定义 UUID

```cpp
// ble_uuid.h
#pragma once
#include "uuid_defs.h"

namespace NewDevice {

// 标准蓝牙服务
namespace GAP {
    DEFINE_UUID(SERVICE,      "00001800-0000-1000-8000-00805f9b34fb");
    DEFINE_UUID(DEVICE_NAME, "00002a00-0000-1000-8000-00805f9b34fb");
}

// 私有服务
namespace PRIVATE {
    DEFINE_UUID(SERVICE,       "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx");
    DEFINE_UUID(MAIN_CONTROL, "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx");
}

} // namespace NewDevice
```

### 3. 实现板级类

```cpp
// board_new_device.cc
#include "board.h"
#include "ble_manager.h"
#include "spp_manager.h"
#include "ble_uuid.h"

#include <esp_log.h>
#include <memory>

#define TAG "NewDevice"

// BLE 管理器实现
class NewDeviceBleManager : public BleManager {
public:
    bool initialize() override {
        ESP_LOGI(TAG, "Initializing BLE manager...");
        // 注册 GATT 客户端
        esp_ble_gattc_app_register(0);
        return true;
    }
    
    // 实现其他方法...
};

// SPP 管理器实现
class NewDeviceSppManager : public SppManager {
public:
    bool initialize() override {
        ESP_LOGI(TAG, "Initializing SPP manager...");
        esp_spp_init(ESP_SPP_MODE_CB);
        return true;
    }
    
    // 实现其他方法...
};

// 板级实现
class NewDevice : public Board {
private:
    std::unique_ptr<NewDeviceBleManager> m_bleManager;
    std::unique_ptr<NewDeviceSppManager> m_sppManager;

public:
    NewDevice() 
        : m_bleManager(std::make_unique<NewDeviceBleManager>())
        , m_sppManager(std::make_unique<NewDeviceSppManager>())
    {}

    void initialize() override {
        ESP_LOGI(TAG, "Initializing NewDevice board...");
        
        // 1. 蓝牙控制器初始化（基类方法）
        initializeBt();
        
        // 2. BLE/SPP 初始化
        m_bleManager->initialize();
        m_sppManager->initialize();
        
        // 3. UART 任务
        xTaskCreate([](void* param) {
            static_cast<Board*>(param)->UartCommandTask();
        }, "UartTask", 4096, this, tskIDLE_PRIORITY + 1, nullptr);
    }

    BleManager* getBleManager() override { return m_bleManager.get(); }
    SppManager* getSppManager() override { return m_sppManager.get(); }

protected:
    void handleCommand(const char* command) override {
        // 处理命令...
    }
};

DECLARE_BOARD(NewDevice);
```

### 4. 更新 CMakeLists.txt

在 `boards/CMakeLists.txt` 中添加新板子：

```cmake
if(CONFIG_BOARD_NEW_DEVICE)
    set(BOARD_SRC "new-device/board_new_device.cc")
endif()
```

## UUID 管理

### UUID 定义方式

使用 `DEFINE_UUID` 宏：

```cpp
// uuid_defs.h
#define DEFINE_UUID(name, value) static constexpr const char* name = value
```

### UUID 组织方式

按服务分组，使用 namespace：

```cpp
namespace BoseOpenEarbudUltra {

// 标准蓝牙SIG服务
namespace GAP { ... }
namespace GATT { ... }
namespace DID { ... }

// Bose 私有服务
namespace FEBE { ... }
namespace FE2C { ... }
namespace EB10 { ... }
namespace FD92 { ... }

} // namespace BoseOpenEarbudUltra
```

### 使用 UUID

```cpp
// 连接服务
m_bleManager->writeCharacteristic(
    BoseOpenEarbudUltra::FEBE::SERVICE,
    BoseOpenEarbudUltra::FEBE::MAIN_CONTROL,
    data, length
);
```

## 已实现的板子

| 板子 | 文件 | UUID 定义 | 状态 |
|------|------|-----------|------|
| Bose Open Earbud Ultra | `board_bose_open_earbud_ultra.cc` | `ble_uuid.h` | ✅ 完整实现 |
| LE Bose QC Earbuds | `board_le_bose_qc_earbuds.cc` | 待添加 | 🚧 占位 |

## Bose Open Earbud Ultra UUID 参考

详见 [ble_uuid.h](../firmware/main/boards/bose-open-earbud-ultra/ble_uuid.h)

### 服务列表

| 服务 | UUID | 功能 |
|------|------|------|
| GAP | 00001800-... | 设备名称、外观 |
| GATT | 00001801-... | 服务变更通知 |
| DID | 0000180a-... | 设备信息 |
| FEBE | 0000febe-... | 主控制（电量、固件） |
| FE2C | 0000fe2c-... | 副控制（降噪、EQ） |
| EB10 | 0000eb10-... | 左右耳通讯 |
| FD92 | 0000fd92-... | TWS 链路状态 |