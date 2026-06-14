/**
 * @file board_le_bose_qc_earbuds.cc
 * @brief LE Bose QC Earbuds 板级实现
 * 
 * 基于 BLE 调试信息系统整理，实现完整的 BLE/SPP 管理器
 */

#include "board.h"
#include "ble_manager.h"
#include "spp_manager.h"
#include "ble_uuid.h"

#include <esp_log.h>
#include <esp_gap_ble_api.h>
#include <esp_gattc_api.h>
#include <esp_bt_main.h>
#include <esp_spp_api.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory>
#include <cstring>

#define TAG "LeBoseQCEarbuds"

// =====================================================================
// BLE 管理器实现
// =====================================================================

class LeBoseQcBleManager : public BleManager {
private:
    bool m_connected;
    uint16_t m_connId;
    esp_gatt_if_t m_gattcIf;
    
    // 服务和特征值句柄缓存
    struct ServiceHandle {
        uint16_t startHandle;
        uint16_t endHandle;
    };
    
    struct CharHandle {
        uint16_t handle;
        uint16_t properties;
    };
    
    ServiceHandle m_febeService;
    ServiceHandle m_fe03Service;
    
    CharHandle m_febeMainControl;
    CharHandle m_febeBackupControl;
    CharHandle m_febeStatusNotify;
    
    CharHandle m_fe03DuplexChannel;
    CharHandle m_fe03CmdChannel1;
    CharHandle m_fe03CmdChannel2;
    CharHandle m_fe03CmdChannel3;
    CharHandle m_fe03CmdChannel4;
    CharHandle m_fe03StatusChannel;
    
    DataCallback m_dataCallback;
    ConnectionCallback m_connCallback;

public:
    LeBoseQcBleManager() 
        : m_connected(false)
        , m_connId(0)
        , m_gattcIf(ESP_GATT_IF_NONE)
        , m_dataCallback(nullptr)
        , m_connCallback(nullptr)
    {
        memset(&m_febeService, 0, sizeof(m_febeService));
        memset(&m_fe03Service, 0, sizeof(m_fe03Service));
    }
    
    bool initialize() override {
        ESP_LOGI(TAG, "Initializing BLE manager...");
        
        // 注册 GATT 客户端应用
        esp_err_t ret = esp_ble_gattc_app_register(0);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "GATT client app register failed: %s", esp_err_to_name(ret));
            return false;
        }
        
        ESP_LOGI(TAG, "BLE manager initialized successfully");
        return true;
    }
    
    bool connect(const char* address) override {
        if (!address) {
            ESP_LOGE(TAG, "Invalid address");
            return false;
        }
        
        ESP_LOGI(TAG, "Connecting to %s", address);
        
        // 解析地址
        esp_bd_addr_t bd_addr;
        for (int i = 0; i < ESP_BD_ADDR_LEN; i++) {
            bd_addr[i] = 0; // TODO: 解析地址字符串
        }
        
        // 打开连接
        esp_err_t ret = esp_ble_gattc_open(m_gattcIf, bd_addr, BLE_ADDR_TYPE_PUBLIC, true);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Connect failed: %s", esp_err_to_name(ret));
            return false;
        }
        
        return true;
    }
    
    void disconnect() override {
        if (m_connected) {
            esp_ble_gattc_close(m_gattcIf, m_connId);
            m_connected = false;
            ESP_LOGI(TAG, "Disconnected");
        }
    }
    
    bool isConnected() override { 
        return m_connected; 
    }
    
    bool discoverServices() override {
        if (!m_connected) {
            ESP_LOGE(TAG, "Not connected");
            return false;
        }
        
        ESP_LOGI(TAG, "Discovering services...");
        esp_err_t ret = esp_ble_gattc_search_service(m_gattcIf, m_connId, nullptr);
        return ret == ESP_OK;
    }
    
    bool hasService(const char* serviceUUID) override {
        // TODO: 实现服务检查
        return false;
    }
    
    bool writeCharacteristic(const char* serviceUUID, 
                             const char* charUUID,
                             const uint8_t* data, 
                             size_t length) override {
        if (!m_connected) {
            ESP_LOGE(TAG, "Not connected");
            return false;
        }
        
        // 根据服务UUID和特征值UUID查找句柄
        uint16_t handle = getCharHandle(serviceUUID, charUUID);
        if (handle == 0) {
            ESP_LOGE(TAG, "Characteristic not found");
            return false;
        }
        
        ESP_LOGI(TAG, "Writing to characteristic handle %d, length %d", handle, length);
        
        esp_err_t ret = esp_ble_gattc_write_char(
            m_gattcIf,
            m_connId,
            handle,
            length,
            data,
            ESP_GATT_WRITE_TYPE_RSP,
            ESP_GATT_AUTH_REQ_NONE
        );
        
        return ret == ESP_OK;
    }
    
    bool readCharacteristic(const char* serviceUUID,
                            const char* charUUID,
                            uint8_t* buffer,
                            size_t* length) override {
        if (!m_connected) {
            ESP_LOGE(TAG, "Not connected");
            return false;
        }
        
        uint16_t handle = getCharHandle(serviceUUID, charUUID);
        if (handle == 0) {
            ESP_LOGE(TAG, "Characteristic not found");
            return false;
        }
        
        ESP_LOGI(TAG, "Reading from characteristic handle %d", handle);
        
        esp_err_t ret = esp_ble_gattc_read_char(
            m_gattcIf,
            m_connId,
            handle,
            ESP_GATT_AUTH_REQ_NONE
        );
        
        // TODO: 等待读取结果回调
        return ret == ESP_OK;
    }
    
    bool enableNotify(const char* serviceUUID, const char* charUUID) override {
        if (!m_connected) {
            ESP_LOGE(TAG, "Not connected");
            return false;
        }
        
        uint16_t handle = getCharHandle(serviceUUID, charUUID);
        if (handle == 0) {
            ESP_LOGE(TAG, "Characteristic not found");
            return false;
        }
        
        ESP_LOGI(TAG, "Enabling notify for characteristic handle %d", handle);
        
        // 写入 CCCD 启用通知
        uint8_t notifyEnable[] = {0x01, 0x00};
        
        // 获取 CCCD 描述符句柄（通常是特征值句柄+1）
        uint16_t cccdHandle = handle + 1;
        
        esp_err_t ret = esp_ble_gattc_write_char_descr(
            m_gattcIf,
            m_connId,
            cccdHandle,
            sizeof(notifyEnable),
            notifyEnable,
            ESP_GATT_WRITE_TYPE_RSP,
            ESP_GATT_AUTH_REQ_NONE
        );
        
        return ret == ESP_OK;
    }
    
    bool disableNotify(const char* serviceUUID, const char* charUUID) override {
        if (!m_connected) {
            ESP_LOGE(TAG, "Not connected");
            return false;
        }
        
        uint16_t handle = getCharHandle(serviceUUID, charUUID);
        if (handle == 0) {
            ESP_LOGE(TAG, "Characteristic not found");
            return false;
        }
        
        ESP_LOGI(TAG, "Disabling notify for characteristic handle %d", handle);
        
        uint8_t notifyDisable[] = {0x00, 0x00};
        uint16_t cccdHandle = handle + 1;
        
        esp_err_t ret = esp_ble_gattc_write_char_descr(
            m_gattcIf,
            m_connId,
            cccdHandle,
            sizeof(notifyDisable),
            notifyDisable,
            ESP_GATT_WRITE_TYPE_RSP,
            ESP_GATT_AUTH_REQ_NONE
        );
        
        return ret == ESP_OK;
    }
    
    void setDataCallback(DataCallback callback) override {
        m_dataCallback = callback;
    }
    
    void setConnectionCallback(ConnectionCallback callback) override {
        m_connCallback = callback;
    }
    
    // GATT 事件回调处理
    void handleGattcEvent(esp_gattc_cb_event_t event, 
                          esp_gatt_if_t gattc_if,
                          esp_ble_gattc_cb_param_t* param) {
        m_gattcIf = gattc_if;
        
        switch (event) {
            case ESP_GATTC_REG_EVT:
                ESP_LOGI(TAG, "GATT client registered, app_id=%d", param->reg.app_id);
                break;
                
            case ESP_GATTC_OPEN_EVT:
                m_connId = param->open.conn_id;
                m_connected = true;
                ESP_LOGI(TAG, "Connection opened, conn_id=%d", m_connId);
                if (m_connCallback) {
                    m_connCallback(true);
                }
                // 自动开始服务发现
                discoverServices();
                break;
                
            case ESP_GATTC_CLOSE_EVT:
                m_connected = false;
                ESP_LOGI(TAG, "Connection closed");
                if (m_connCallback) {
                    m_connCallback(false);
                }
                break;
                
            case ESP_GATTC_SEARCH_RES_EVT:
                // 服务发现结果
                handleServiceDiscovery(param);
                break;
                
            case ESP_GATTC_SEARCH_CMPL_EVT:
                ESP_LOGI(TAG, "Service discovery completed");
                // 订阅关键特征的通知
                subscribeNotifications();
                break;
                
            case ESP_GATTC_READ_CHAR_EVT:
                // 读取结果
                if (m_dataCallback && param->read.status == ESP_GATT_OK) {
                    m_dataCallback(param->read.value, param->read.value_len);
                }
                break;
                
            case ESP_GATTC_WRITE_CHAR_EVT:
                ESP_LOGI(TAG, "Write completed, status=%d", param->write.status);
                break;
                
            case ESP_GATTC_NOTIFY_EVT:
                // 收到通知
                ESP_LOGI(TAG, "Received notification, handle=%d, len=%d", 
                         param->notify.handle, param->notify.value_len);
                if (m_dataCallback) {
                    m_dataCallback(param->notify.value, param->notify.value_len);
                }
                break;
                
            default:
                break;
        }
    }

private:
    uint16_t getCharHandle(const char* serviceUUID, const char* charUUID) {
        // FEBE 服务
        if (strcmp(serviceUUID, LeBoseQCEarbuds::FEBE::SERVICE) == 0) {
            if (strcmp(charUUID, LeBoseQCEarbuds::FEBE::MAIN_CONTROL) == 0) {
                return m_febeMainControl.handle;
            }
            if (strcmp(charUUID, LeBoseQCEarbuds::FEBE::BACKUP_CONTROL) == 0) {
                return m_febeBackupControl.handle;
            }
            if (strcmp(charUUID, LeBoseQCEarbuds::FEBE::STATUS_NOTIFY) == 0) {
                return m_febeStatusNotify.handle;
            }
        }
        
        // FE03 服务
        if (strcmp(serviceUUID, LeBoseQCEarbuds::FE03::SERVICE) == 0) {
            if (strcmp(charUUID, LeBoseQCEarbuds::FE03::DUPLEX_CHANNEL) == 0) {
                return m_fe03DuplexChannel.handle;
            }
            if (strcmp(charUUID, LeBoseQCEarbuds::FE03::CMD_CHANNEL_1) == 0) {
                return m_fe03CmdChannel1.handle;
            }
            if (strcmp(charUUID, LeBoseQCEarbuds::FE03::CMD_CHANNEL_2) == 0) {
                return m_fe03CmdChannel2.handle;
            }
            if (strcmp(charUUID, LeBoseQCEarbuds::FE03::CMD_CHANNEL_3) == 0) {
                return m_fe03CmdChannel3.handle;
            }
            if (strcmp(charUUID, LeBoseQCEarbuds::FE03::CMD_CHANNEL_4) == 0) {
                return m_fe03CmdChannel4.handle;
            }
            if (strcmp(charUUID, LeBoseQCEarbuds::FE03::STATUS_CHANNEL) == 0) {
                return m_fe03StatusChannel.handle;
            }
        }
        
        return 0;
    }
    
    void handleServiceDiscovery(esp_ble_gattc_cb_param_t* param) {
        uint16_t uuid16 = param->search_res.srvc_id.uuid.uuid.uuid16;
        esp_bt_uuid_t uuid128 = param->search_res.srvc_id.uuid.uuid;
        
        // 检查 FEBE 服务
        if (uuid16 == 0xFEBE) {
            m_febeService.startHandle = param->search_res.start_handle;
            m_febeService.endHandle = param->search_res.end_handle;
            ESP_LOGI(TAG, "FEBE service found, start=%d, end=%d", 
                     m_febeService.startHandle, m_febeService.endHandle);
            
            // 获取特征值
            esp_ble_gattc_get_all_char(m_gattcIf, m_connId, 
                                        m_febeService.startHandle, 
                                        m_febeService.endHandle);
        }
        
        // 检查 FE03 服务
        if (uuid16 == 0xFE03) {
            m_fe03Service.startHandle = param->search_res.start_handle;
            m_fe03Service.endHandle = param->search_res.end_handle;
            ESP_LOGI(TAG, "FE03 service found, start=%d, end=%d", 
                     m_fe03Service.startHandle, m_fe03Service.endHandle);
            
            // 获取特征值
            esp_ble_gattc_get_all_char(m_gattcIf, m_connId, 
                                        m_fe03Service.startHandle, 
                                        m_fe03Service.endHandle);
        }
    }
    
    void subscribeNotifications() {
        // 订阅 FEBE 服务通知
        enableNotify(LeBoseQCEarbuds::FEBE::SERVICE, 
                     LeBoseQCEarbuds::FEBE::STATUS_NOTIFY);
        enableNotify(LeBoseQCEarbuds::FEBE::SERVICE, 
                     LeBoseQCEarbuds::FEBE::MAIN_CONTROL);
        
        // 订阅 FE03 服务通知
        enableNotify(LeBoseQCEarbuds::FE03::SERVICE, 
                     LeBoseQCEarbuds::FE03::STATUS_CHANNEL);
        enableNotify(LeBoseQCEarbuds::FE03::SERVICE, 
                     LeBoseQCEarbuds::FE03::DUPLEX_CHANNEL);
    }
};

// =====================================================================
// SPP 管理器实现
// =====================================================================

class LeBoseQcSppManager : public SppManager {
private:
    bool m_connected;
    uint32_t m_sppHandle;
    DataCallback m_dataCallback;
    ConnectionCallback m_connCallback;

public:
    LeBoseQcSppManager() 
        : m_connected(false)
        , m_sppHandle(0)
        , m_dataCallback(nullptr)
        , m_connCallback(nullptr)
    {}
    
    bool initialize() override {
        ESP_LOGI(TAG, "Initializing SPP manager...");
        
        esp_err_t ret = esp_spp_init(ESP_SPP_MODE_CB);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPP init failed: %s", esp_err_to_name(ret));
            return false;
        }
        
        ESP_LOGI(TAG, "SPP manager initialized successfully");
        return true;
    }
    
    bool connect(const char* address) override {
        if (!address) {
            ESP_LOGE(TAG, "Invalid address");
            return false;
        }
        
        ESP_LOGI(TAG, "SPP connecting to %s", address);
        
        // TODO: 实现 SPP 连接
        return true;
    }
    
    void disconnect() override {
        if (m_connected && m_sppHandle != 0) {
            esp_spp_disconnect(m_sppHandle);
            m_connected = false;
            ESP_LOGI(TAG, "SPP disconnected");
        }
    }
    
    bool isConnected() override { 
        return m_connected; 
    }
    
    bool send(const uint8_t* data, size_t length) override {
        if (!m_connected || m_sppHandle == 0) {
            ESP_LOGE(TAG, "SPP not connected");
            return false;
        }
        
        ESP_LOGI(TAG, "SPP sending %d bytes", length);
        return esp_spp_write(m_sppHandle, length, data) == ESP_OK;
    }
    
    int receive(uint8_t* buffer, size_t maxLength) override {
        // SPP 数据通过回调接收
        return -1;
    }
    
    void setDataCallback(DataCallback callback) override {
        m_dataCallback = callback;
    }
    
    void setConnectionCallback(ConnectionCallback callback) override {
        m_connCallback = callback;
    }
    
    // SPP 事件回调处理
    void handleSppEvent(esp_spp_cb_event_t event, esp_spp_cb_param_t* param) {
        switch (event) {
            case ESP_SPP_INIT_EVT:
                ESP_LOGI(TAG, "SPP initialized, status=%d", param->init.status);
                break;
                
            case ESP_SPP_OPEN_EVT:
                m_sppHandle = param->open.handle;
                m_connected = true;
                ESP_LOGI(TAG, "SPP connection opened, handle=%d", m_sppHandle);
                if (m_connCallback) {
                    m_connCallback(true);
                }
                break;
                
            case ESP_SPP_CLOSE_EVT:
                m_connected = false;
                ESP_LOGI(TAG, "SPP connection closed");
                if (m_connCallback) {
                    m_connCallback(false);
                }
                break;
                
            case ESP_SPP_DATA_IND_EVT:
                ESP_LOGI(TAG, "SPP received %d bytes", param->data_ind.len);
                if (m_dataCallback) {
                    m_dataCallback(param->data_ind.data, param->data_ind.len);
                }
                break;
                
            default:
                break;
        }
    }
};

// =====================================================================
// 板级实现
// =====================================================================

class LeBoseQCEarbuds : public Board {
private:
    std::unique_ptr<LeBoseQcBleManager> m_bleManager;
    std::unique_ptr<LeBoseQcSppManager> m_sppManager;

public:
    LeBoseQCEarbuds() 
        : m_bleManager(std::make_unique<LeBoseQcBleManager>())
        , m_sppManager(std::make_unique<LeBoseQcSppManager>())
    {}

    void initialize() override {
        ESP_LOGI(TAG, "Initializing LE Bose QC Earbuds board...");
        
        // 1. 初始化蓝牙控制器（基类方法）
        if (!initializeBt()) {
            ESP_LOGE(TAG, "Failed to initialize Bluetooth controller");
            return;
        }
        
        // 2. 初始化 BLE 管理器
        if (m_bleManager) {
            if (!m_bleManager->initialize()) {
                ESP_LOGE(TAG, "Failed to initialize BLE manager");
            }
        }
        
        // 3. 初始化 SPP 管理器
        if (m_sppManager) {
            if (!m_sppManager->initialize()) {
                ESP_LOGE(TAG, "Failed to initialize SPP manager");
            }
        }
        
        // 4. 启动 UART 任务
        xTaskCreate([](void* param) {
            static_cast<Board*>(param)->UartCommandTask();
        }, "UartTask", 4096, this, tskIDLE_PRIORITY + 1, nullptr);
        
        ESP_LOGI(TAG, "Board initialized successfully");
    }

    BleManager* getBleManager() override { return m_bleManager.get(); }
    SppManager* getSppManager() override { return m_sppManager.get(); }

protected:
    void handleCommand(const char* command) override {
        ESP_LOGI(TAG, "Received command: %s", command);
        
        // 解析命令并执行相应操作
        if (strncmp(command, "ble_connect", 11) == 0) {
            // TODO: 解析地址并连接
            ESP_LOGI(TAG, "BLE connect command");
        }
        else if (strncmp(command, "ble_disconnect", 14) == 0) {
            m_bleManager->disconnect();
        }
        else if (strncmp(command, "spp_connect", 11) == 0) {
            // TODO: 解析地址并连接
            ESP_LOGI(TAG, "SPP connect command");
        }
        else if (strncmp(command, "spp_disconnect", 14) == 0) {
            m_sppManager->disconnect();
        }
        else if (strncmp(command, "status", 6) == 0) {
            ESP_LOGI(TAG, "BLE connected: %d, SPP connected: %d", 
                     m_bleManager->isConnected(), 
                     m_sppManager->isConnected());
        }
        else {
            ESP_LOGW(TAG, "Unknown command: %s", command);
        }
    }
};

DECLARE_BOARD(LeBoseQCEarbuds);