#include "SmartHomeHub.h"
#include <algorithm>
#include <functional>
using namespace std;

SmartHomeHub::SmartHomeHub() {}

// 析构时逐一释放设备；因基类析构是虚函数，能正确调用各子类析构
SmartHomeHub::~SmartHomeHub() {
    for_each(devices.begin(), devices.end(), [](SmartDevice* dev) { delete dev; });
    devices.clear();
}

SmartHomeHub& SmartHomeHub::operator+(SmartDevice* dev) {
    if (dev) devices.push_back(dev);
    return *this;
}

void SmartHomeHub::turnOnAll() {
    cout << "\n=== 一键开启所有设备 ===" << endl;
    for_each(devices.begin(), devices.end(), mem_fn(&SmartDevice::turnOn));
}

void SmartHomeHub::turnOffAll() {
    cout << "\n=== 一键关闭所有设备 ===" << endl;
    for_each(devices.begin(), devices.end(), mem_fn(&SmartDevice::turnOff));
}

void SmartHomeHub::showAll() const {
    cout << "\n--- 设备状态总览 ---" << endl;
    for_each(devices.begin(), devices.end(), mem_fn(&SmartDevice::showStatus));
}

SmartDevice* SmartHomeHub::findDevice(const string& id) const {
    auto it = find_if(devices.begin(), devices.end(),
                      [&id](const SmartDevice* dev) { return dev->getID() == id; });
    return it != devices.end() ? *it : nullptr;
}

bool SmartHomeHub::removeDevice(const string& id) {
    auto it = find_if(devices.begin(), devices.end(),
                      [&id](SmartDevice* dev) { return dev->getID() == id; });
    if (it != devices.end()) {
        delete *it;
        devices.erase(it);
        return true;
    }
    return false;
}

// action 由调用方传入（通常是 Lambda），对每个设备执行同样的场景动作
void SmartHomeHub::sceneMode(const string& mode, function<void(SmartDevice*)> action) {
    cout << "\n=== 执行场景: " << mode << " ===" << endl;
    for_each(devices.begin(), devices.end(), action);
}

void SmartHomeHub::saveConfig(const string& filename) const {
    ofstream out(filename);
    if (!out) {
        cerr << "保存失败：无法创建文件" << endl;
        return;
    }
    for_each(devices.begin(), devices.end(), [&out](const SmartDevice* dev) { dev->save(out); });
    out.close();
    cout << "\n配置已保存至 " << filename << endl;
}

void SmartHomeHub::loadConfig(const string& filename) {
    ifstream in(filename);
    if (!in) {
        cout << "未找到配置文件，将使用默认配置" << endl;
        return;
    }

    // 清空当前设备
    for_each(devices.begin(), devices.end(), [](SmartDevice* dev) { delete dev; });
    devices.clear();

    string type, id, name;
    bool power;
    int value;   // 灯读作亮度、空调读作温度；门锁不使用该字段

    // 逐行解析配置，按类型重建对应的子类对象
    while (in >> type >> id >> name >> power >> value) {
        SmartDevice* dev = nullptr;
        if (type == "Light") {
            dev = new SmartLight(id, name, value);
        } else if (type == "AC") {
            dev = new SmartAC(id, name, value);
        } else if (type == "Lock") {
            dev = new SmartLock(id, name);
        }

        if (dev) {
            if (power) dev->turnOn();
            devices.push_back(dev);
        }
    }

    in.close();
    cout << "\n配置已从 " << filename << " 加载" << endl;
}

// 作为 SmartHomeHub 的友元，可直接访问其私有成员 devices
void SystemLogger::printReport(const SmartHomeHub& hub) {
    cout << "\n========== 系统日志报告 ==========" << endl;
    cout << "设备总数: " << hub.devices.size() << endl;
    cout << "在线设备: " << SmartDevice::getOnlineCount() << endl;
    cout << "----------------------------------" << endl;
    for_each(hub.devices.begin(), hub.devices.end(), mem_fn(&SmartDevice::showStatus));
    cout << "==================================" << endl;
}

// ios::app 表示追加写入，不会覆盖已有日志
void SystemLogger::log(const string& msg) {
    ofstream logfile("system.log", ios::app);
    logfile << msg << endl;
    logfile.close();
}
