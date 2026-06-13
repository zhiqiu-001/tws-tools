#pragma once
/**
 * 需要实现uart 串口命令接受和处理
 * 子类需要继承和扩展
 * ble初始化
 */
#include <driver/uart.h>


#define UART_BUFFER_SIZE 256
#define UART_BAUD_RATE 115200


/**
 * @brief 创建板级实例的工厂函数声明
 * 
 * 由具体板级实现定义，返回对应板级类的实例指针
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
    Board(const Board&) = delete;            // 禁用拷贝构造函数
    Board& operator=(const Board&) = delete; // 禁用赋值操作

protected:
    /**
     * @brief 构造函数（受保护）
     */
    Board();

    uart_port_t m_uartPort;             // UART端口号
    uint8_t m_buffer[UART_BUFFER_SIZE]; // 接收缓冲区
    int m_bufferIndex;          // 缓冲区索引

public:
    /**
     * @brief 获取单例实例
     * 
     * @return Board& 板级实例引用
     */
    static Board& GetInstance() {
        static Board* instance = static_cast<Board*>(create_board());
        return *instance;
    }

    /**
     * @brief 析构函数（虚函数）
     */
    virtual ~Board() = default;

    void initialize();
    
    /**
     * @brief UART 命令任务（内部使用）
     */
    void UartCommandTask();
    
protected:
    /**
     * @brief 处理接收到的命令（子类可重写）
     * @param command 接收到的命令字符串
     */
    virtual void handleCommand(const char* command);
};

/**
 * @def DECLARE_BOARD(BOARD_CLASS_NAME)
 * @brief 板级类注册宏
 * 
 * 定义 create_board 函数，返回指定板级类的实例。
 * 
 * @param BOARD_CLASS_NAME 板级类名称
 */
#define DECLARE_BOARD(BOARD_CLASS_NAME) \
void* create_board() { \
    return new BOARD_CLASS_NAME(); \
}
