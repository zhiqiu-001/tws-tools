#pragma once

/**
 * @file ble_manager.h
 * @brief BLE 蓝牙管理器抽象接口
 */

#include <cstdint>
#include <functional>

/**
 * @brief BLE 蓝牙管理器抽象基类
 * 
 * 提供BLE连接、服务发现、数据收发等功能的抽象接口。
 * 具体板级实现需继承此类并实现所有纯虚函数。
 */
class BleManager {
public:
    virtual ~BleManager() = default;

    // ========== 初始化 ==========
    
    /**
     * @brief 初始化 BLE 管理器
     * 
     * 在 Board::initializeBt() 之后调用，注册 GATT 客户端应用。
     * 
     * @return 初始化成功返回 true
     */
    virtual bool initialize() = 0;

    // ========== 连接管理 ==========
    
    /**
     * @brief 连接BLE设备
     * @param address 目标设备地址，nullptr表示等待任意设备连接
     * @return 连接成功返回true
     */
    virtual bool connect(const char* address = nullptr) = 0;
    
    /**
     * @brief 断开BLE连接
     */
    virtual void disconnect() = 0;
    
    /**
     * @brief 检查是否已连接
     * @return 已连接返回true
     */
    virtual bool isConnected() = 0;

    // ========== 服务发现 ==========
    
    /**
     * @brief 发现所有服务
     * @return 发现成功返回true
     */
    virtual bool discoverServices() = 0;
    
    /**
     * @brief 检查是否存在指定服务
     * @param serviceUUID 服务UUID
     * @return 存在返回true
     */
    virtual bool hasService(const char* serviceUUID) = 0;

    // ========== 数据收发 ==========
    
    /**
     * @brief 写入特征值
     * @param serviceUUID 服务UUID
     * @param charUUID 特征值UUID
     * @param data 数据指针
     * @param length 数据长度
     * @return 写入成功返回true
     */
    virtual bool writeCharacteristic(const char* serviceUUID,
                                     const char* charUUID,
                                     const uint8_t* data,
                                     size_t length) = 0;
    
    /**
     * @brief 读取特征值
     * @param serviceUUID 服务UUID
     * @param charUUID 特征值UUID
     * @param buffer 数据缓冲区
     * @param length 输入缓冲区大小，输出实际读取长度
     * @return 读取成功返回true
     */
    virtual bool readCharacteristic(const char* serviceUUID,
                                    const char* charUUID,
                                    uint8_t* buffer,
                                    size_t* length) = 0;

    // ========== 通知订阅 ==========
    
    /**
     * @brief 启用特征值通知
     * @param serviceUUID 服务UUID
     * @param charUUID 特征值UUID
     * @return 启用成功返回true
     */
    virtual bool enableNotify(const char* serviceUUID,
                              const char* charUUID) = 0;
    
    /**
     * @brief 禁用特征值通知
     * @param serviceUUID 服务UUID
     * @param charUUID 特征值UUID
     * @return 禁用成功返回true
     */
    virtual bool disableNotify(const char* serviceUUID,
                               const char* charUUID) = 0;

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