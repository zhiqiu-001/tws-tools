#pragma once

/**
 * @file uuid_defs.h
 * @brief UUID 通用定义宏和类型
 * 
 * 本文件只包含通用的、与具体耳机无关的定义：
 * - UUID 定义宏
 * - 特征值属性标志
 * - 标准蓝牙描述符 UUID
 * 
 * 具体耳机的服务定义应放在各自的 ble_uuid.h 中。
 */

// UUID 定义宏
#define DEFINE_UUID(name, value) static constexpr const char* name = value

// BLE 特征值属性标志（通用）
enum class CharProperty : uint8_t {
    READ         = 0x01,
    WRITE        = 0x02,
    WRITE_NO_RSP = 0x04,
    NOTIFY       = 0x10,
    INDICATE     = 0x20
};

// 标准蓝牙描述符 UUID（通用）
namespace BleDescriptor {
    constexpr const char* CCCD = "00002902-0000-1000-8000-00805f9b34fb";
}

// 标准蓝牙 SIG 服务 UUID（所有蓝牙设备通用）
namespace BleSIGService {
    // Generic Access Profile
    constexpr const char* GAP = "00001800-0000-1000-8000-00805f9b34fb";
    
    // Generic Attribute Profile  
    constexpr const char* GATT = "00001801-0000-1000-8000-00805f9b34fb";
    
    // Device Information Service
    constexpr const char* DID = "0000180a-0000-1000-8000-00805f9b34fb";
}