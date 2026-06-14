#pragma once

/**
 * @file board.h
 * @brief 板级抽象基类
 * 
 * 提供板级相关功能的抽象接口，包括：
 * - UART 串口命令处理
 * - BLE 蓝牙管理器
 * - SPP 蓝牙串口管理器
 * - 蓝牙控制器初始化（公共部分）
 * 
 * 子类需要继承并实现具体功能。
 */

#include <driver/uart.h>

#define UART_BUFFER_SIZE 256
#define UART_BAUD_RATE 115200

// 前向声明
class BleManager;
class SppManager;

/**
 * @brief 创建板级实例的工厂函数声明
 */
void* create_board();

/**
 * @brief 板级抽象基类
 * 
 * 采用单例模式，提供板级相关功能的抽象接口。
 * 具体板级实现通过 DECLARE_BOARD 宏注册。
 */
class Board {
private:
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

protected:
    Board();

    uart_port_t m_uartPort;
    uint8_t m_buffer[UART_BUFFER_SIZE];
    int m_bufferIndex;
    bool m_btInitialized;  // 蓝牙是否已初始化

public:
    static Board& GetInstance() {
        static Board* instance = static_cast<Board*>(create_board());
        return *instance;
    }

    virtual ~Board() = default;

    // ========== 初始化 ==========
    
    /**
     * @brief 初始化板级资源
     * 
     * 子类需实现此方法，调用 Board::initializeBt() 初始化蓝牙，
     * 然后初始化 BLE/SPP 管理器。
     */
    virtual void initialize() = 0;

    // ========== 蓝牙控制器初始化（公共部分） ==========
    
    /**
     * @brief 初始化蓝牙控制器和 Bluedroid 协议栈
     * 
     * 此方法只需调用一次，初始化双模蓝牙（BLE + 经典蓝牙）。
     * 子类在 initialize() 中应先调用此方法。
     * 
     * @return 初始化成功返回 true
     */
    bool initializeBt();

    // ========== UART 命令处理 ==========
    
    void UartCommandTask();

    // ========== BLE/SPP 管理器访问 ==========
    
    virtual BleManager* getBleManager() { return nullptr; }
    virtual SppManager* getSppManager() { return nullptr; }

protected:
    virtual void handleCommand(const char* command);
};

/**
 * @def DECLARE_BOARD(BOARD_CLASS_NAME)
 * @brief 板级类注册宏
 */
#define DECLARE_BOARD(BOARD_CLASS_NAME) \
void* create_board() { \
    return new BOARD_CLASS_NAME(); \
}