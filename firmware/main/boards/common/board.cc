#include "board.h"

#include <driver/uart.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "Board"

Board::Board() 
    : m_uartPort(UART_NUM_0)
    , m_bufferIndex(0)
    , m_btInitialized(false)
{
}

// ========== 蓝牙控制器初始化 ==========

bool Board::initializeBt() {
    if (m_btInitialized) {
        ESP_LOGW(TAG, "Bluetooth already initialized");
        return true;
    }

    ESP_LOGI(TAG, "Initializing Bluetooth controller...");

    // 1. 初始化蓝牙控制器
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluetooth controller init failed: %s", esp_err_to_name(ret));
        return false;
    }
    ESP_LOGI(TAG, "Bluetooth controller init succeeded");

    // 2·. 启用蓝牙控制器（双模：BLE + 经典蓝牙）
    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
        // 尝试单独启用 BLE 模式
        ESP_LOGI(TAG, "Trying BLE-only mode...");
        ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "BLE-only mode also failed: %s", esp_err_to_name(ret));
            return false;
        }
        ESP_LOGW(TAG, "Enabled BLE-only mode (Classic BT disabled)");
    } else {
        ESP_LOGI(TAG, "Bluetooth controller enabled (Dual Mode)");
    }

    // 3·. 初始化 Bluedroid 协议栈
    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid init failed: %s", esp_err_to_name(ret));
        return false;
    }

    // 4·. 启用 Bluedroid
    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return false;
    }

    m_btInitialized = true;
    ESP_LOGI(TAG, "Bluetooth initialized successfully (Dual Mode: BLE + Classic)");
    return true;
}

// ========== UART 初始化和命令处理 ==========

void Board::UartCommandTask() {
    // UART 配置
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = 0,
    };

    esp_err_t ret = uart_param_config(m_uartPort, &uart_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure UART: %d", ret);
        return;
    }

    ret = uart_driver_install(m_uartPort, UART_BUFFER_SIZE * 2, 0, 0, nullptr, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install UART driver: %d", ret);
        return;
    }

    ESP_LOGI(TAG, "UART initialized, starting command loop...");

    // 命令处理循环
    while (true) {
        uint8_t data;
        int len = uart_read_bytes(m_uartPort, &data, 1, pdMS_TO_TICKS(10));

        if (len > 0) {
            if (data == '\n' || data == '\r') {
                if (m_bufferIndex > 0) {
                    m_buffer[m_bufferIndex] = '\0';
                    handleCommand((const char*)m_buffer);
                    m_bufferIndex = 0;
                }
            } else if (m_bufferIndex < UART_BUFFER_SIZE - 1) {
                m_buffer[m_bufferIndex++] = data;
            }
        }
    }
}

void Board::handleCommand(const char* command) {
    ESP_LOGI(TAG, "Received command: %s", command);
}