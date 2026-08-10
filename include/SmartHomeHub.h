#ifndef SMART_HOME_HUB_H
#define SMART_HOME_HUB_H

#include "SmartDevice.h"
#include <vector>
#include <functional>

// 智能家居控制中心：统一管理所有设备
class SmartHomeHub {
    // 基类指针容器：存放不同子类对象，运行时多态调用的关键
    std::vector<SmartDevice*> devices;

public:
    SmartHomeHub();
    ~SmartHomeHub();

    // 运算符重载：hub + 新设备 即可把设备加入管理列表
    SmartHomeHub& operator+(SmartDevice* dev);

    void turnOnAll();
    void turnOffAll();
    void showAll() const;

    // 按设备 ID 查找并返回基类指针；找不到返回 nullptr
    SmartDevice* findDevice(const std::string& id) const;

    // 按设备 ID 删除设备，成功返回 true
    bool removeDevice(const std::string& id);

    // 场景模式：对所有设备执行传入的动作（通常用 Lambda 表达式描述）
    void sceneMode(const std::string& mode, std::function<void(SmartDevice*)> action);

    // 配置持久化：把设备状态写入文件 / 从文件恢复
    void saveConfig(const std::string& filename) const;
    void loadConfig(const std::string& filename);

    // 友元类：SystemLogger 可以直接访问私有的 devices 容器
    friend class SystemLogger;
};

// 系统日志类：作为友元生成设备报告，并记录日志文件
class SystemLogger {
public:
    static void printReport(const SmartHomeHub& hub);
    static void log(const std::string& msg);
};

#endif
