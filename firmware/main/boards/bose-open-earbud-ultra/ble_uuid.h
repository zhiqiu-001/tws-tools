#pragma once

/**
 * @file ble_uuid.h
 * @brief Bose Open Earbud Ultra 蓝牙 UUID 定义
 * 
 * 包含标准蓝牙SIG规范服务和Bose私有自定义服务的UUID定义
 */

#include "uuid_defs.h"

namespace BoseOpenEarbudUltra {

// =====================================================================
// 一、标准蓝牙SIG规范服务（通用BLE标配）
// =====================================================================

/**
 * @brief Generic Access (GAP) 通用接入服务
 */
namespace GAP {
    DEFINE_UUID(SERVICE,      "00001800-0000-1000-8000-00805f9b34fb");
    DEFINE_UUID(DEVICE_NAME,  "00002a00-0000-1000-8000-00805f9b34fb");  // READ: 设备名称
    DEFINE_UUID(APPEARANCE,   "00002a01-0000-1000-8000-00805f9b34fb");  // READ: 设备外观类型
}

/**
 * @brief Generic Attribute (GATT) 基础属性服务
 */
namespace GATT {
    DEFINE_UUID(SERVICE,          "00001801-0000-1000-8000-00805f9b34fb");
    DEFINE_UUID(SERVICE_CHANGED,  "00002a05-0000-1000-8000-00805f9b34fb");  // INDICATE: 服务变更通知
    DEFINE_UUID(RESERVED_RW,      "00002b29-0000-1000-8000-00805f9b34fb");  // READ+WRITE: 厂商预留读写
    DEFINE_UUID(RESERVED_RO_1,    "00002b2a-0000-1000-8000-00805f9b34fb");  // READ: 只读参数
    DEFINE_UUID(RESERVED_RO_2,    "00002b3a-0000-1000-8000-00805f9b34fb");  // READ: 只读参数
}

/**
 * @brief Device Information (DID) 设备信息服务
 */
namespace DID {
    DEFINE_UUID(SERVICE,            "0000180a-0000-1000-8000-00805f9b34fb");
    DEFINE_UUID(MANUFACTURER,       "00002a29-0000-1000-8000-00805f9b34fb");  // READ: 制造商
    DEFINE_UUID(MODEL_NUMBER,       "00002a24-0000-1000-8000-00805f9b34fb");  // READ: 型号
    DEFINE_UUID(SERIAL_NUMBER,      "00002a25-0000-1000-8000-00805f9b34fb");  // READ: 序列号SN
    DEFINE_UUID(HARDWARE_REVISION,  "00002a27-0000-1000-8000-00805f9b34fb");  // READ: 硬件版本
    DEFINE_UUID(FIRMWARE_REVISION,  "00002a26-0000-1000-8000-00805f9b34fb");  // READ: 固件版本
    DEFINE_UUID(SOFTWARE_REVISION,  "00002a28-0000-1000-8000-00805f9b34fb");  // READ: 软件版本
}

// =====================================================================
// 二、Bose私有自定义服务（Bose Open Earbud Ultra 特有）
// =====================================================================

/**
 * @brief FEBE服务 - Bose主控制服务
 * 
 * 功能：固件、配置、电量、按键
 */
namespace FEBE {
    DEFINE_UUID(SERVICE,         "0000febe-0000-1000-8000-00805f9b34fb");
    // 主控双向透传，带CCCD，耳机核心收发通道（电量/触控/状态上报）
    DEFINE_UUID(MAIN_CONTROL,    "d417c028-9818-4354-99d1-2ac09d074591");  // READ+WRITE+WRITE_NO_RSP+NOTIFY
    // 备用控制通道，带CCCD，固件升级/参数设置
    DEFINE_UUID(BACKUP_CONTROL,  "c65b8f2f-aee2-4c89-b758-bc4892d6f2d8");  // READ+WRITE+WRITE_NO_RSP+NOTIFY
}

/**
 * @brief FE2C服务 - Bose副控制服务
 * 
 * 功能：音效、降噪、EQ、配对指令
 */
namespace FE2C {
    DEFINE_UUID(SERVICE,         "0000fe2c-0000-1000-8000-00805f9b34fb");
    // 参数读取（降噪档位、EQ配置）
    DEFINE_UUID(PARAM_READ,      "fe2c1233-8366-4814-8eb0-01de32100bea");  // READ
    // 指令下发，带CCCD
    DEFINE_UUID(CMD_SEND,        "fe2c1234-8366-4814-8eb0-01de32100bea");  // WRITE+INDICATE
    // 配置写入，带CCCD
    DEFINE_UUID(CONFIG_WRITE,    "fe2c1235-8366-4814-8eb0-01de32100bea");  // WRITE+INDICATE
    // 单向写指令（配对重置、声道重置）
    DEFINE_UUID(CMD_ONEWAY,      "fe2c1236-8366-4814-8eb0-01de32100bea");  // WRITE
    // 状态应答，带CCCD
    DEFINE_UUID(STATUS_REPLY,     "fe2c1237-8366-4814-8eb0-01de32100bea");  // WRITE+INDICATE
    // 全功能双向通道，带CCCD
    DEFINE_UUID(FULL_DUPLEX,      "fe2c1238-8366-4814-8eb0-01de32100bea");  // READ+WRITE+NOTIFY
}

/**
 * @brief EB10服务 - 左右耳通讯服务
 * 
 * 功能：副耳同步、单耳补配专用
 */
namespace EB10 {
    DEFINE_UUID(SERVICE,       "0000eb10-d102-11e1-9b23-00025b00a5a5");
    // 左耳信息读取
    DEFINE_UUID(LEFT_INFO,     "0000eb11-d102-11e1-9b23-00025b00a5a5");  // READ
    // 右耳信息读取
    DEFINE_UUID(RIGHT_INFO,    "0000eb12-d102-11e1-9b23-00025b00a5a5");  // READ
    // 配对指令下发，带CCCD（主副耳绑定/重置）
    DEFINE_UUID(PAIRING_CMD,   "0000eb13-d102-11e1-9b23-00025b00a5a5");  // WRITE+INDICATE
    // 单耳补配写指令
    DEFINE_UUID(SINGLE_PAIR,   "0000eb14-d102-11e1-9b23-00025b00a5a5");  // WRITE
}

/**
 * @brief FD92服务 - TWS链路状态服务
 * 
 * 功能：左右耳链路上报、连接状态
 */
namespace FD92 {
    DEFINE_UUID(SERVICE,         "0000fd92-0000-1000-8000-00805f9b34fb");
    // 链路状态上报，带CCCD（左右耳在线/断开）
    DEFINE_UUID(LINK_STATUS,     "0000eb20-d102-11e1-9b23-00025b00a5a5");  // READ+NOTIFY
    // 链路控制指令
    DEFINE_UUID(LINK_CONTROL,    "0000eb21-d102-11e1-9b23-00025b00a5a5");  // WRITE
    // 硬件故障/电池异常上报，带CCCD
    DEFINE_UUID(FAULT_REPORT,    "0000eb22-d102-11e1-9b23-00025b00a5a5");  // READ+NOTIFY
}

} // namespace BoseOpenEarbudUltra