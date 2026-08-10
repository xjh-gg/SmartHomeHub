#ifndef SMART_DEVICE_H
#define SMART_DEVICE_H

#include <iostream>
#include <fstream>
#include <string>
#include <exception>

/* ========== 异常类 ========== */
// 设备操作异常（如空调温度越界时抛出）
class DeviceException : public std::exception {
    std::string msg;
public:
    DeviceException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// 安全异常（如门锁密码连续输错时抛出）
class SecurityException : public std::exception {
    std::string msg;
public:
    SecurityException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

/* ========== 抽象基类 ========== */
// 所有智能设备的统一接口，含纯虚函数，不能直接实例化
class SmartDevice {
protected:
    const std::string deviceID;   // const 成员，只能在构造函数初始化列表中赋值
    std::string name;
    bool power;
    static int onlineCount;       // 静态成员：统计当前在线设备总数，所有对象共享

    void setPower(bool p);

public:
    SmartDevice(const std::string& id, const std::string& n);

    // 禁止拷贝与移动：设备对象通过指针管理，每个实例 ID 唯一
    SmartDevice(const SmartDevice&) = delete;
    SmartDevice& operator=(const SmartDevice&) = delete;
    SmartDevice(SmartDevice&&) = delete;
    SmartDevice& operator=(SmartDevice&&) = delete;

    virtual ~SmartDevice();       // 虚析构：保证通过基类指针 delete 时能调用子类析构

    // 纯虚函数：强制每个子类实现自己的版本（多态的核心）
    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual void showStatus() const = 0;
    virtual void save(std::ofstream& out) const = 0;

    std::string getID() const;
    bool isOn() const;
    static int getOnlineCount();
};

/* ========== 智能灯 ========== */
class SmartLight : public SmartDevice {
    int brightness;               // 亮度 0~100

public:
    SmartLight(const std::string& id, const std::string& n, int b = 50);

    void turnOn() override;
    void turnOff() override;
    void showStatus() const override;
    void save(std::ofstream& out) const override;

    // 运算符重载：++ 调亮、-- 调暗，每次 10%
    SmartLight& operator++();
    SmartLight& operator--();
    void setBrightness(int b);   // 直接设置亮度，越界自动裁剪到 0~100
    // 友元函数：让 cout << 灯对象 能直接输出灯的摘要信息
    friend std::ostream& operator<<(std::ostream& out, const SmartLight& L);
};

/* ========== 智能空调 ========== */
class SmartAC : public SmartDevice {
    int temperature;              // 设定温度，合法范围 16~30°C

public:
    SmartAC(const std::string& id, const std::string& n, int temp = 26);

    void turnOn() override;
    void turnOff() override;
    void showStatus() const override;
    void save(std::ofstream& out) const override;

    void setTemperature(int t);   // 温度越界时抛出 DeviceException
};

/* ========== 智能门锁 ========== */
class SmartLock : public SmartDevice {
    std::string password;
    bool locked;
    int failCount;                // 密码连续输错次数，达到 3 次触发安全锁定

public:
    SmartLock(const std::string& id, const std::string& n, const std::string& pwd = "123456");

    void turnOn() override;
    void turnOff() override;
    void showStatus() const override;
    void save(std::ofstream& out) const override;

    void unlock(const std::string& pwd);  // 密码错误或已锁定时抛出 SecurityException
    void lock();
};

#endif
