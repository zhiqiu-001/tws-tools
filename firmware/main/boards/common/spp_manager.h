#pragma once

/**
 * @file spp_manager.h
 * @brief SPP (Serial Port Profile) 蓝牙串口管理器抽象接口
 */

#include <cstdint>
#include <functional>

/**
 * @brief SPP 蓝牙串口管理器抽象基类
 * 
 * 提供SPP连接、数据收发等功能的抽象接口。
 * 具体板级实现需继承此类并实现所有纯虚函数。
 */
class SppManager {
public:
    virtual ~SppManager() = default;

    // ========== 初始化 ==========
    
    /**
     * @brief 初始化 SPP 管理器
     * 
     * 在 Board::initializeBt() 之后调用，注册 SPP 服务。
     * 
     * @return 初始化成功返回 true
     */
    virtual bool initialize() = 0;

    // ========== 连接管理 ==========
    
    /**
     * @brief 连接SPP设备
     * @param address 目标设备地址，nullptr表示等待任意设备连接
     * @return 连接成功返回true
     */
    virtual bool connect(const char* address = nullptr) = 0;
    
    /**
     * @brief 断开SPP连接
     */
    virtual void disconnect() = 0;
    
    /**
     * @brief 检查是否已连接
     * @return 已连接返回true
     */
    virtual bool isConnected() = 0;

    // ========== 数据收发 ==========
    
    /**
     * @brief 发送数据
     * @param data 数据指针
     * @param length 数据长度
     * @return 发送成功返回true
     */
    virtual bool send(const uint8_t* data, size_t length) = 0;
    
    /**
     * @brief 接收数据
     * @param buffer 数据缓冲区
     * @param maxLength 缓冲区最大长度
     * @return 实际接收的字节数，-1表示错误
     */
    virtual int receive(uint8_t* buffer, size_t maxLength) = 0;

    // ========== 回调注册 ==========
    
    using DataCallback = std::function<void(const uint8_t* data, size_t length)>;
    
    /**
     * @brief 设置数据接收回调
     * @param callback 回调函数
     */
    virtual void setDataCallback(DataCallback callback) = 0;
    
    /**
     * @brief 设置连接状态回调
     * @param callback 回调函数，参数为true表示已连接
     */
    using ConnectionCallback = std::function<void(bool connected)>;
    virtual void setConnectionCallback(ConnectionCallback callback) = 0;
};