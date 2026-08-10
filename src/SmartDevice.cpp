#include "SmartDevice.h"
using namespace std;

/* ========== SmartDevice 基类实现 ========== */
// 静态成员必须在类外定义并初始化
int SmartDevice::onlineCount = 0;

SmartDevice::SmartDevice(const string& id, const string& n)
    : deviceID(id), name(n), power(false) {}

SmartDevice::~SmartDevice() {}

string SmartDevice::getID() const { return deviceID; }
bool SmartDevice::isOn() const { return power; }
int SmartDevice::getOnlineCount() { return onlineCount; }

// 统一在此维护在线计数：只有状态真正变化时才增减，避免重复计数
void SmartDevice::setPower(bool p) {
    if (p && !power) onlineCount++;
    if (!p && power) onlineCount--;
    power = p;
}

/* ========== SmartLight 实现 ========== */
SmartLight::SmartLight(const string& id, const string& n, int b)
    : SmartDevice(id, n), brightness(b) {}

void SmartLight::turnOn() {
    setPower(true);
    cout << "💡 " << name << " 已开启 | 亮度: " << brightness << "%" << endl;
}

void SmartLight::turnOff() {
    setPower(false);
    cout << "💡 " << name << " 已关闭" << endl;
}

void SmartLight::showStatus() const {
    cout << "[" << deviceID << "][灯] " << name
         << " | 状态:" << (power ? "开" : "关")
         << " | 亮度:" << brightness << "%" << endl;
}

void SmartLight::save(ofstream& out) const {
    out << "Light " << deviceID << " " << name << " " << power << " " << brightness << endl;
}

// ++ 调亮 10%，用 min 封顶 100，防止越界
SmartLight& SmartLight::operator++() {
    brightness = min(100, brightness + 10);
    return *this;
}

// -- 调暗 10%，用 max 保底 0
SmartLight& SmartLight::operator--() {
    brightness = max(0, brightness - 10);
    return *this;
}

void SmartLight::setBrightness(int b) {
    brightness = max(0, min(100, b));
}

ostream& operator<<(ostream& out, const SmartLight& L) {
    out << "💡 " << L.name << " 亮度:" << L.brightness << "%";
    return out;
}

/* ========== SmartAC 实现 ========== */
SmartAC::SmartAC(const string& id, const string& n, int temp)
    : SmartDevice(id, n), temperature(temp) {}

void SmartAC::turnOn() {
    setPower(true);
    cout << "❄️  " << name << " 已开启 | 温度: " << temperature << "°C" << endl;
}

void SmartAC::turnOff() {
    setPower(false);
    cout << "❄️  " << name << " 已关闭" << endl;
}

void SmartAC::showStatus() const {
    cout << "[" << deviceID << "][空调] " << name
         << " | 状态:" << (power ? "开" : "关")
         << " | 温度:" << temperature << "°C" << endl;
}

void SmartAC::save(ofstream& out) const {
    out << "AC " << deviceID << " " << name << " " << power << " " << temperature << endl;
}

void SmartAC::setTemperature(int t) {
    // 温度越界：抛出自定义异常，由调用方 try-catch 处理
    if (t < 16 || t > 30)
        throw DeviceException("温度设置异常：必须在 16-30°C 之间");
    temperature = t;
}

/* ========== SmartLock 实现 ========== */
SmartLock::SmartLock(const string& id, const string& n, const string& pwd)
    : SmartDevice(id, n), password(pwd), locked(true), failCount(0) {}

void SmartLock::turnOn() { unlock(password); }
void SmartLock::turnOff() { lock(); }

void SmartLock::showStatus() const {
    cout << "[" << deviceID << "][门锁] " << name
         << " | 状态:" << (power ? "开" : "关")
         << " | 锁定:" << (locked ? "是" : "否") << endl;
}

void SmartLock::save(ofstream& out) const {
    out << "Lock " << deviceID << " " << name << " " << power << " " << locked << endl;
}

void SmartLock::unlock(const string& pwd) {
    // 已连续输错 3 次：拒绝一切解锁尝试
    if (failCount >= 3)
        throw SecurityException("安全锁定：连续错误3次，已锁定");

    // 密码错误：累计次数并抛异常
    if (pwd != password) {
        failCount++;
        throw SecurityException("密码错误（第 " + to_string(failCount) + " 次）");
    }

    // 密码正确：解锁并清零错误计数
    locked = false;
    setPower(true);
    failCount = 0;
    cout << "🔓 " << name << " 已解锁" << endl;
}

void SmartLock::lock() {
    locked = true;
    setPower(false);
    cout << "🔒 " << name << " 已上锁" << endl;
}
