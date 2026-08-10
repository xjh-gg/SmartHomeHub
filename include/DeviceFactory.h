#ifndef DEVICE_FACTORY_H
#define DEVICE_FACTORY_H

#include "SmartDevice.h"
#include <memory>
#include <string>

// 抽象工厂：定义设备创建的统一接口（对应课程第 11 章纯虚函数）
class DeviceFactory {
public:
    virtual ~DeviceFactory() = default;

    // 纯虚函数：由具体工厂实现对应的设备创建逻辑
    virtual std::unique_ptr<SmartDevice> create(const std::string& id,
                                                 const std::string& name,
                                                 int value = 0,
                                                 const std::string& password = "") = 0;

    // 静态工厂查找器：根据类型字符串返回对应的具体工厂
    static std::unique_ptr<DeviceFactory> getFactory(const std::string& type);
};

// 具体工厂：每个派生类负责创建一种设备（对应课程第 09/11 章继承 + 复写）
class LightFactory : public DeviceFactory {
public:
    std::unique_ptr<SmartDevice> create(const std::string& id,
                                         const std::string& name,
                                         int value = 0,
                                         const std::string& password = "") override;
};

class ACFactory : public DeviceFactory {
public:
    std::unique_ptr<SmartDevice> create(const std::string& id,
                                         const std::string& name,
                                         int value = 0,
                                         const std::string& password = "") override;
};

class LockFactory : public DeviceFactory {
public:
    std::unique_ptr<SmartDevice> create(const std::string& id,
                                         const std::string& name,
                                         int value = 0,
                                         const std::string& password = "") override;
};

#endif
