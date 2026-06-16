/**
 * @file board_bose_open_earbud_ultra.cc
 * @brief Bose Open Earbud Ultra 板级实现
 */

#include "board.h"
#include "ble_manager.h"
#include "spp_manager.h"
#include "ble_uuid.h"

#include <esp_log.h>
#include <esp_gap_ble_api.h>
#include <esp_gattc_api.h>
#include <esp_spp_api.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <cstring>
#include <inttypes.h>

#define TAG "BoseOpenEarbudUltra"

// =====================================================================
// BLE 管理器实现
// =====================================================================

class BoseBleManager : public BleManager {
private:
    bool m_connected;
    uint16_t m_gattcAppId;
    uint16_t m_connId;
    DataCallback m_dataCallback;
    ConnectionCallback m_connectionCallback;
    static esp_gatt_if_t s_gattc_if;

    // GATT 事件回调（静态）
    static void gattcEventHandler(esp_gattc_cb_event_t event,
                                  esp_gatt_if_t gattc_if,
                                  esp_ble_gattc_cb_param_t* param);

public:
    BoseBleManager() : m_connected(false), m_gattcAppId(0), m_connId(0) {}
    
    ~BoseBleManager() override = default;

    bool initialize() override {
        ESP_LOGI(TAG, "Initializing BLE manager...");
        
        // 注册 GATT 客户端应用
        esp_err_t ret = esp_ble_gattc_app_register(m_gattcAppId);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "GATT client register failed: %s", esp_err_to_name(ret));
            return false;
        }
        
        // 注册 GATT 事件回调
        esp_ble_gattc_register_callback(gattcEventHandler);
        
        ESP_LOGI(TAG, "BLE manager initialized");
        return true;
    }

    bool connect(const char* address = nullptr) override {
        ESP_LOGI(TAG, "BLE connecting...");
        // TODO: 实现扫描和连接逻辑
        m_connected = true;
        if (m_connectionCallback) {
            m_connectionCallback(true);
        }
        return true;
    }

    void disconnect() override {
        ESP_LOGI(TAG, "BLE disconnecting");
        if (m_connected) {
            esp_ble_gattc_close(s_gattc_if, m_connId);
        }
        m_connected = false;
        if (m_connectionCallback) {
            m_connectionCallback(false);
        }
    }

    bool isConnected() override { return m_connected; }
    
    bool discoverServices() override {
        ESP_LOGI(TAG, "Discovering services...");
        esp_ble_gattc_search_service(s_gattc_if, m_connId, nullptr);
        return true;
    }
    
    bool hasService(const char* serviceUUID) override {
        // TODO: 检查服务是否存在
        return false;
    }

    bool writeCharacteristic(const char* serviceUUID,
                             const char* charUUID,
                             const uint8_t* data,
                             size_t length) override {
        ESP_LOGI(TAG, "Writing to characteristic: %s", charUUID);
        // TODO: 实现写入逻辑
        return false;
    }

    bool readCharacteristic(const char* serviceUUID,
                            const char* charUUID,
                            uint8_t* buffer,
                            size_t* length) override {
        ESP_LOGI(TAG, "Reading from characteristic: %s", charUUID);
        // TODO: 实现读取逻辑
        return false;
    }

    bool enableNotify(const char* serviceUUID, const char* charUUID) override {
        ESP_LOGI(TAG, "Enabling notify: %s", charUUID);
        // TODO: 写入 CCCD 启用通知
        return false;
    }

    bool disableNotify(const char* serviceUUID, const char* charUUID) override {
        ESP_LOGI(TAG, "Disabling notify: %s", charUUID);
        // TODO: 写入 CCCD 禁用通知
        return false;
    }

    void setDataCallback(DataCallback callback) override {
        m_dataCallback = callback;
    }

    void setConnectionCallback(ConnectionCallback callback) override {
        m_connectionCallback = callback;
    }
};

// 静态成员定义
esp_gatt_if_t BoseBleManager::s_gattc_if = 0;

// GATT 事件回调实现
void BoseBleManager::gattcEventHandler(esp_gattc_cb_event_t event,
                                       esp_gatt_if_t gattc_if,
                                       esp_ble_gattc_cb_param_t* param) {
    switch (event) {
        case ESP_GATTC_REG_EVT:
            ESP_LOGI(TAG, "GATT client registered");
            s_gattc_if = gattc_if;
            break;
        case ESP_GATTC_OPEN_EVT:
            ESP_LOGI(TAG, "BLE connection opened");
            break;
        case ESP_GATTC_CLOSE_EVT:
            ESP_LOGI(TAG, "BLE connection closed");
            break;
        case ESP_GATTC_SEARCH_RES_EVT: {
            esp_bt_uuid_t* uuid = &param->search_res.srvc_id.uuid;
            if (uuid->len == ESP_UUID_LEN_16) {
                ESP_LOGI(TAG, "Service found: UUID16: 0x%04x", uuid->uuid.uuid16);
            } else if (uuid->len == ESP_UUID_LEN_32) {
                ESP_LOGI(TAG, "Service found: UUID32: 0x%08" PRIx32, uuid->uuid.uuid32);
            } else if (uuid->len == ESP_UUID_LEN_128) {
                ESP_LOGI(TAG, "Service found: UUID128: %02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x",
                         uuid->uuid.uuid128[0], uuid->uuid.uuid128[1], uuid->uuid.uuid128[2], uuid->uuid.uuid128[3],
                         uuid->uuid.uuid128[4], uuid->uuid.uuid128[5], uuid->uuid.uuid128[6], uuid->uuid.uuid128[7],
                         uuid->uuid.uuid128[8], uuid->uuid.uuid128[9], uuid->uuid.uuid128[10], uuid->uuid.uuid128[11],
                         uuid->uuid.uuid128[12], uuid->uuid.uuid128[13], uuid->uuid.uuid128[14], uuid->uuid.uuid128[15]);
            }
            break;
        }
        case ESP_GATTC_READ_CHAR_EVT:
            ESP_LOGI(TAG, "Characteristic read complete");
            break;
        case ESP_GATTC_WRITE_CHAR_EVT:
            ESP_LOGI(TAG, "Characteristic write complete");
            break;
        case ESP_GATTC_NOTIFY_EVT:
            ESP_LOGI(TAG, "Received notification");
            break;
        default:
            break;
    }
}

// =====================================================================
// SPP 管理器实现
// =====================================================================

class BoseSppManager : public SppManager {
private:
    bool m_connected;
    uint32_t m_sppHandle;
    DataCallback m_dataCallback;
    ConnectionCallback m_connectionCallback;

    // SPP 事件回调（静态）
    static void sppEventHandler(esp_spp_cb_event_t event, esp_spp_cb_param_t* param);

public:
    BoseSppManager() : m_connected(false), m_sppHandle(0) {}
    
    ~BoseSppManager() override = default;

    bool initialize() override {
        ESP_LOGI(TAG, "Initializing SPP manager...");
        
        // 初始化 SPP（回调模式）
        esp_spp_cfg_t cfg = {
            .mode = ESP_SPP_MODE_CB,
            .enable_l2cap_ertm = true,
            .tx_buffer_size = 0,
        };
        esp_err_t ret = esp_spp_enhanced_init(&cfg);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPP init failed: %s", esp_err_to_name(ret));
            return false;
        }
        
        // 注册 SPP 事件回调
        esp_spp_register_callback(sppEventHandler);
        
        // 设置设备名称
        esp_ble_gap_set_device_name("Bose Open Earbud Ultra");
        
        ESP_LOGI(TAG, "SPP manager initialized");
        return true;
    }

    bool connect(const char* address = nullptr) override {
        ESP_LOGI(TAG, "SPP connecting...");
        // TODO: 实现连接逻辑
        m_connected = true;
        if (m_connectionCallback) {
            m_connectionCallback(true);
        }
        return true;
    }

    void disconnect() override {
        ESP_LOGI(TAG, "SPP disconnecting");
        if (m_connected && m_sppHandle != 0) {
            esp_spp_disconnect(m_sppHandle);
        }
        m_connected = false;
        if (m_connectionCallback) {
            m_connectionCallback(false);
        }
    }

    bool isConnected() override { return m_connected; }

    bool send(const uint8_t* data, size_t length) override {
        if (!m_connected || m_sppHandle == 0) {
            return false;
        }
        return esp_spp_write(m_sppHandle, length, const_cast<uint8_t*>(data)) == ESP_OK;
    }

    int receive(uint8_t* buffer, size_t maxLength) override {
        // SPP 使用回调模式，数据通过回调接收
        return -1;
    }

    void setDataCallback(DataCallback callback) override {
        m_dataCallback = callback;
    }

    void setConnectionCallback(ConnectionCallback callback) override {
        m_connectionCallback = callback;
    }
};

// SPP 事件回调实现
void BoseSppManager::sppEventHandler(esp_spp_cb_event_t event, esp_spp_cb_param_t* param) {
    switch (event) {
        case ESP_SPP_INIT_EVT:
            ESP_LOGI(TAG, "SPP initialized");
            break;
        case ESP_SPP_OPEN_EVT:
            ESP_LOGI(TAG, "SPP connection opened, handle=%" PRIu32, param->open.handle);
            break;
        case ESP_SPP_CLOSE_EVT:
            ESP_LOGI(TAG, "SPP connection closed");
            break;
        case ESP_SPP_DATA_IND_EVT:
            ESP_LOGI(TAG, "SPP data received, len=%d", param->data_ind.len);
            // 数据通过回调传递
            break;
        case ESP_SPP_WRITE_EVT:
            ESP_LOGI(TAG, "SPP write complete");
            break;
        default:
            break;
    }
}

// =====================================================================
// 板级实现
// =====================================================================

class BoseOpenEarbudUltraBoard : public Board {
private:
    std::unique_ptr<BoseBleManager> m_bleManager;
    std::unique_ptr<BoseSppManager> m_sppManager;

    

public:
    BoseOpenEarbudUltraBoard() 
        : m_bleManager(std::make_unique<BoseBleManager>())
        , m_sppManager(std::make_unique<BoseSppManager>())
    {}

    void initialize() override {
        ESP_LOGI(TAG, "Initializing Bose Open Earbud Ultra board...");
        
        // 1. 初始化蓝牙控制器（基类方法，只需调用一次）
        if (!initializeBt()) {
            ESP_LOGE(TAG, "Failed to initialize Bluetooth");
            return;
        }
        
        // 2. 初始化 BLE 管理器
        if (m_bleManager && !m_bleManager->initialize()) {
            ESP_LOGE(TAG, "Failed to initialize BLE manager");
        }
        
        // 3. 初始化 SPP 管理器
        if (m_sppManager && !m_sppManager->initialize()) {
            ESP_LOGE(TAG, "Failed to initialize SPP manager");
        }
        
        // 4. 启动 UART 命令任务
        xTaskCreate([](void* param) {
            static_cast<Board*>(param)->UartCommandTask();
        }, "UartTask", 4096, this, tskIDLE_PRIORITY + 1, nullptr);
        
        ESP_LOGI(TAG, "Board initialized successfully");
    }

    BleManager* getBleManager() override {
        return m_bleManager.get();
    }

    SppManager* getSppManager() override {
        return m_sppManager.get();
    }

protected:
    void handleCommand(const char* command) override {
        ESP_LOGI(TAG, "Command received: %s", command);
        
        if (strcmp(command, "status") == 0) {
            ESP_LOGI(TAG, "Device status: OK");
        } else if (strcmp(command, "reset") == 0) {
            ESP_LOGI(TAG, "Resetting device...");
        } else if (strcmp(command, "ble_connect") == 0) {
            if (m_bleManager) m_bleManager->connect();
        } else if (strcmp(command, "ble_disconnect") == 0) {
            if (m_bleManager) m_bleManager->disconnect();
        } else if (strcmp(command, "spp_connect") == 0) {
            if (m_sppManager) m_sppManager->connect();
        } else if (strcmp(command, "spp_disconnect") == 0) {
            if (m_sppManager) m_sppManager->disconnect();
        }
    }
};

DECLARE_BOARD(BoseOpenEarbudUltraBoard);