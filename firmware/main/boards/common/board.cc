#include "board.h"
#include <driver/uart.h>
#include <esp_log.h>
#include <esp_err.h>

#define TAG "Board"

Board::Board():m_uartPort(UART_NUM_0), m_bufferIndex(0) {

}

// 任务入口函数（静态成员）
static void UartCommandTaskEntry(void* param) {
    Board* board = static_cast<Board*>(param);
    board->UartCommandTask();
}

void Board::initialize() {
    ESP_LOGI(TAG, "Initializing UART Command...");
    
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
    
    ESP_LOGI(TAG, "UART Command initialized");

    // 使用静态入口函数，传递 this 指针
    xTaskCreate(UartCommandTaskEntry, "UartCommandTask", 4096, this, tskIDLE_PRIORITY + 1, nullptr);
}

void Board::UartCommandTask() {
    while (true) {
        uint8_t data;
        int len = uart_read_bytes(m_uartPort, &data, 1, pdMS_TO_TICKS(10));
        
        if (len > 0) {
            if (data == '\n' || data == '\r') {
                if (m_bufferIndex > 0) {
                    m_buffer[m_bufferIndex] = '\0';
                    handleCommand((const char*)m_buffer);  // 调用虚函数
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