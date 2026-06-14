#pragma once

/**
 * @file ble_uuid.h
 * @brief LE-Bose QC Earbuds 蓝牙 UUID 定义
 * 
 * 基于 BLE 调试信息系统整理，包含完整的服务和特征值定义
 */

#include "uuid_defs.h"

namespace LeBoseQCEarbuds {

// =====================================================================
// 一、标准蓝牙SIG规范服务（通用BLE标配）
// =====================================================================

/**
 * @brief Generic Access (GAP) 通用接入服务
 * UUID: 00001800-0000-1000-8000-00805f9b34fb
 */
namespace GAP {
    DEFINE_UUID(SERVICE,     "00001800-0000-1000-8000-00805f9b34fb");
    DEFINE_UUID(DEVICE_NAME, "00002a00-0000-1000-8000-00805f9b34fb");  // READ: 设备名称
    DEFINE_UUID(APPEARANCE,  "00002a01-0000-1000-8000-00805f9b34fb");  // READ: 设备外观类型
}

/**
 * @brief Generic Attribute (GATT) 通用属性服务
 * UUID: 00001801-0000-1000-8000-00805f9b34fb
 */
namespace GATT {
    DEFINE_UUID(SERVICE,         "00001801-0000-1000-8000-00805f9b34fb");
    DEFINE_UUID(SERVICE_CHANGED, "00002a05-0000-1000-8000-00805f9b34fb");  // INDICATE: 服务变更通知
}

/**
 * @brief Device Information (DID) 设备信息服务
 * UUID: 0000180a-0000-1000-8000-00805f9b34fb
 */
namespace DID {
    DEFINE_UUID(SERVICE,            "0000180a-0000-1000-8000-00805f9b34fb");
    DEFINE_UUID(MANUFACTURER,       "00002a29-0000-1000-8000-00805f9b34fb");  // READ: 制造商名称
    DEFINE_UUID(MODEL_NUMBER,       "00002a24-0000-1000-8000-00805f9b34fb");  // READ: 型号
    DEFINE_UUID(HARDWARE_REVISION,  "00002a27-0000-1000-8000-00805f9b34fb");  // READ: 硬件版本
    DEFINE_UUID(FIRMWARE_REVISION,  "00002a26-0000-1000-8000-00805f9b34fb");  // READ: 固件版本
    DEFINE_UUID(SOFTWARE_REVISION,  "00002a28-0000-1000-8000-00805f9b34fb");  // READ: 软件版本
    DEFINE_UUID(SYSTEM_ID,          "00002a23-0000-1000-8000-00805f9b34fb");  // READ: 系统ID
    DEFINE_UUID(REG_CERT_DATA,      "00002a2a-0000-1000-8000-00805f9b34fb");  // READ: IEEE 11073认证数据
    DEFINE_UUID(PNP_ID,             "00002a50-0000-1000-8000-00805f9b34fb");  // READ: PnP ID
}

// =====================================================================
// 二、Bose私有自定义服务（LE-Bose QC Earbuds 特有）
// =====================================================================

/**
 * @brief FEBE服务 - Bose主控制服务
 * UUID: 0000febe-0000-1000-8000-00805f9b34fb
 * 
 * 功能：固件、配置、电量、按键控制
 */
namespace FEBE {
    DEFINE_UUID(SERVICE,           "0000febe-0000-1000-8000-00805f9b34fb");
    
    // 状态/反馈数据接收通道（READ + NOTIFY）
    DEFINE_UUID(STATUS_NOTIFY,     "9ec813b4-256b-4090-93a8-a4f0e9107733");
    
    // 双向数据交互通道（READ + WRITE + WRITE_NO_RESPONSE + NOTIFY）
    DEFINE_UUID(MAIN_CONTROL,      "d417c028-9818-4354-99d1-2ac09d074591");
    
    // 双向数据交互通道（READ + WRITE + WRITE_NO_RESPONSE + NOTIFY）
    DEFINE_UUID(BACKUP_CONTROL,    "c65b8f2f-aee2-4c89-b758-bc4892d6f2d8");
}

/**
 * @brief FE03服务 - Bose专用服务
 * UUID: 0000fe03-0000-1000-8000-00805f9b34fb
 * 
 * 功能：控制指令发送、状态接收
 */
namespace FE03 {
    DEFINE_UUID(SERVICE,           "0000fe03-0000-1000-8000-00805f9b34fb");
    
    // 双向数据交互通道（READ + WRITE + WRITE_NO_RESPONSE + NOTIFY）
    DEFINE_UUID(DUPLEX_CHANNEL,    "c65b8f2f-aee2-4c89-b758-bc4892d6f2d8");
    
    // 控制指令发送通道（WRITE）
    DEFINE_UUID(CMD_CHANNEL_1,     "234bfbd5-e3b3-4536-a3fe-723620d4b78d");
    
    // 控制指令发送通道（WRITE）
    DEFINE_UUID(CMD_CHANNEL_2,     "b8ec1fa5-f56b-4aad-8961-856b84fed4f8");
    
    // 控制指令发送通道（WRITE）
    DEFINE_UUID(CMD_CHANNEL_3,     "c1c449f8-34d7-4c61-ad8a-4fb364f10b27");
    
    // 控制指令发送通道（WRITE）
    DEFINE_UUID(CMD_CHANNEL_4,     "f04eb177-3005-43a7-ac61-a390ddf83076");
    
    // 状态/反馈数据接收通道（READ + NOTIFY）
    DEFINE_UUID(STATUS_CHANNEL,    "2beea05b-1879-4bb4-8a2f-72641f82420b");
}

// =====================================================================
// 三、广播包信息
// =====================================================================

/**
 * @brief 广播包解析信息
 */
namespace AdvData {
    // 设备名称
    constexpr const char* DEVICE_NAME = "LE-Bose QC Earbuds";
    
    // 设备类型：BR/EDR/LE（双模蓝牙）
    constexpr const char* DEVICE_TYPE = "BR/EDR/LE";
    
    // 广播类型：Legacy
    constexpr const char* ADV_TYPE = "Legacy";
    
    // 主要服务UUID（16-bit）
    constexpr uint16_t SERVICE_UUID_16BIT = 0xFEBE;
    
    // 厂商数据 Company ID
    constexpr uint16_t COMPANY_ID = 0x1D03;
}

} // namespace LeBoseQCEarbuds