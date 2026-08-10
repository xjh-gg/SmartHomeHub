#ifndef DEVICE_FACTORY_H
#define DEVICE_FACTORY_H

#include "SmartDevice.h"
#include <memory>
#include <string>

// 简单工厂：集中管理所有 SmartDevice 派生对象的创建逻辑
// 新增设备类型时，只需修改此处，无需改动 main.cpp 或 SmartHomeHub.cpp
class DeviceFactory {
public:
    // 根据类型字符串创建设备
    // type: "Light" / "AC" / "Lock"
    // value: Light=亮度, AC=温度, Lock 时忽略
    // password: 仅 Lock 使用，为空时使用默认密码
    static std::unique_ptr<SmartDevice> create(const std::string& type,
                                                const std::string& id,
                                                const std::string& name,
                                                int value = 0,
                                                const std::string& password = "");
};

#endif
