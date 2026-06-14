# 架构设计

本文档描述 TWS Tools 固件的整体架构设计。

## 整体架构

```
┌─────────────────────────────────────────────────────────────────┐
│                         Application                              │
│                      (application.cc)                            │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                           Board                                  │
│                         (board.h)                                │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
│  │ initializeBt()  │  │ UartCommandTask │  │ handleCommand() │ │
│  │ 蓝牙控制器初始化 │  │ UART命令处理    │  │ 命令分发        │ │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘ │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                    getBleManager() / getSppManager()        ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┴───────────────┐
              ▼                               ▼
┌─────────────────────────┐     ┌─────────────────────────┐
│      BleManager         │     │      SppManager         │
│    (ble_manager.h)      │     │    (spp_manager.h)      │
│                         │     │                         │
│  - initialize()         │     │  - initialize()         │
│  - connect()            │     │  - connect()            │
│  - discoverServices()   │     │  - send()               │
│  - writeCharacteristic()│     │  - receive()            │
│  - enableNotify()       │     │                         │
└─────────────────────────┘     └─────────────────────────┘
              │                               │
              ▼                               ▼
┌─────────────────────────┐     ┌─────────────────────────┐
│   BoseBleManager        │     │   BoseSppManager        │
│  (具体板子实现)          │     │  (具体板子实现)          │
│                         │     │                         │
│  - GATT 事件回调        │     │  - SPP 事件回调         │
│  - UUID 引用            │     │                         │
└─────────────────────────┘     └─────────────────────────┘
```

## 模块说明

### 1. Board 抽象层

**职责**：
- 蓝牙控制器初始化（公共部分）
- UART 命令处理
- BLE/SPP 管理器访问接口

**设计原则**：
- 抽象基类定义稳定接口
- 公共功能在基类实现（避免重复）
- 可变部分通过虚函数委托给子类

### 2. BleManager 抽象层

**职责**：
- BLE 连接管理
- GATT 服务发现
- 特征值读写
- 通知订阅

**接口定义**：

```cpp
class BleManager {
public:
    virtual bool initialize() = 0;
    virtual bool connect(const char* address) = 0;
    virtual bool discoverServices() = 0;
    virtual bool writeCharacteristic(...) = 0;
    virtual bool readCharacteristic(...) = 0;
    virtual bool enableNotify(...) = 0;
};
```

### 3. SppManager 抽象层

**职责**：
- SPP 连接管理
- 数据收发

**接口定义**：

```cpp
class SppManager {
public:
    virtual bool initialize() = 0;
    virtual bool connect(const char* address) = 0;
    virtual bool send(const uint8_t* data, size_t length) = 0;
    virtual int receive(uint8_t* buffer, size_t maxLength) = 0;
};
```

## 初始化流程

### 时序图

```
main.cc                Board              BleManager          SppManager
   │                     │                    │                  │
   │  app_main()         │                    │                  │
   │────────────────────►│                    │                  │
   │                     │                    │                  │
   │                     │  initialize()      │                  │
   │                     │───────────────────►│                  │
   │                     │                    │                  │
   │                     │  initializeBt()    │                  │
   │                     │──┐                 │                  │
   │                     │  │ BT Controller   │                  │
   │                     │  │ Bluedroid       │                  │
   │                     │◄─┘                 │                  │
   │                     │                    │                  │
   │                     │                    │ initialize()     │
   │                     │                    │─────────────────►│
   │                     │                    │                  │
   │                     │                    │                  │ initialize()
   │                     │                    │                  │──┐
   │                     │                    │                  │◄─┘
   │                     │                    │                  │
   │                     │  UartCommandTask() │                  │
   │                     │──┐                 │                  │
   │                     │◄─┘                 │                  │
   │                     │                    │                  │
```

### 初始化步骤

| 步骤 | 方法 | 说明 |
|------|------|------|
| 1 | `esp_bt_controller_init()` | 蓝牙控制器初始化 |
| 2 | `esp_bt_controller_enable(BTDM)` | 启用双模蓝牙 |
| 3 | `esp_bluedroid_init()` | Bluedroid 协议栈初始化 |
| 4 | `esp_bluedroid_enable()` | 启用 Bluedroid |
| 5 | `esp_ble_gattc_app_register()` | 注册 GATT 客户端 |
| 6 | `esp_spp_init()` | 初始化 SPP |

## 事件处理

### BLE GATT 事件

```cpp
void gattcEventHandler(esp_gattc_cb_event_t event, ...) {
    switch (event) {
        case ESP_GATTC_REG_EVT:       // 注册完成
        case ESP_GATTC_OPEN_EVT:      // 连接打开
        case ESP_GATTC_SEARCH_RES_EVT: // 服务发现
        case ESP_GATTC_READ_CHAR_EVT:  // 读取完成
        case ESP_GATTC_WRITE_CHAR_EVT: // 写入完成
        case ESP_GATTC_NOTIFY_EVT:     // 收到通知
    }
}
```

### SPP 事件

```cpp
void sppEventHandler(esp_spp_cb_event_t event, ...) {
    switch (event) {
        case ESP_SPP_INIT_EVT:       // SPP 初始化完成
        case ESP_SPP_OPEN_EVT:       // 连接打开
        case ESP_SPP_CLOSE_EVT:      // 连接关闭
        case ESP_SPP_DATA_IND_EVT:   // 收到数据
        case ESP_SPP_WRITE_EVT:      // 发送完成
    }
}
```

## 设计模式

### 单例模式

Board 采用单例模式，确保全局只有一个实例：

```cpp
static Board& GetInstance() {
    static Board* instance = static_cast<Board*>(create_board());
    return *instance;
}
```

### 工厂模式

通过 `DECLARE_BOARD` 宏注册具体板类：

```cpp
#define DECLARE_BOARD(BOARD_CLASS_NAME) \
void* create_board() { \
    return new BOARD_CLASS_NAME(); \
}
```

### 组合模式

Board 组合 BleManager 和 SppManager：

```cpp
class BoseOpenEarbudUltra : public Board {
private:
    std::unique_ptr<BoseBleManager> m_bleManager;
    std::unique_ptr<BoseSppManager> m_sppManager;
};
```