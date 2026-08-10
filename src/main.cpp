#include "SmartHomeHub.h"
#include "DeviceFactory.h"
#include <iostream>
#include <limits>
#include <memory>
using namespace std;

// ANSI 颜色定义（WSL/类 Unix 终端支持）
const string RESET   = "\033[0m";
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string CYAN    = "\033[36m";
const string BOLD    = "\033[1m";

// 清屏函数（ANSI 转义码，适用于 WSL）
// \033[2J 清除当前可见屏幕，\033[3J 清除滚动缓冲区历史，\033[H 将光标移到左上角
static void clearScreen() {
    cout << "\033[2J\033[3J\033[H";
}

// 暂停等待回车
static void pause() {
    cout << YELLOW << "\n按 Enter 键继续..." << RESET;
    cin.get();
}

// 安全读取整数，带范围检查
static int getInt(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "❌ 输入无效，请输入数字！" << RESET << endl;
        } else if (value < minVal || value > maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "❌ 超出范围，请输入 " << minVal << "~" << maxVal << " 之间的数字！" << RESET << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

// 安全读取字符串，可选是否允许为空
static string getString(const string& prompt, bool allowEmpty = false) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (!allowEmpty && s.empty()) {
            cout << RED << "❌ 输入不能为空！" << RESET << endl;
        } else {
            return s;
        }
    }
}

// 读取 y/n 确认
static bool confirm(const string& prompt) {
    char c;
    while (true) {
        cout << prompt << " (y/n): ";
        cin >> c;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (c == 'y' || c == 'Y') return true;
        if (c == 'n' || c == 'N') return false;
        cout << RED << "❌ 请输入 y 或 n！" << RESET << endl;
    }
}

// 主菜单
static void showMenu() {
    cout << CYAN << BOLD << "\n========================================" << RESET << endl;
    cout << CYAN << BOLD << "    🏠 智能家居设备控制中心 v1.0" << RESET << endl;
    cout << CYAN << BOLD << "========================================" << RESET << endl;
    cout << " [1] 查看所有设备状态" << endl;
    cout << " [2] 添加新设备" << endl;
    cout << " [3] 控制单个设备" << endl;
    cout << " [4] 场景模式" << endl;
    cout << " [5] 保存配置到文件" << endl;
    cout << " [6] 从文件加载配置" << endl;
    cout << " [7] 系统日志报告" << endl;
    cout << " [8] 删除设备" << endl;
    cout << RED << " [0] 退出系统" << RESET << endl;
    cout << CYAN << BOLD << "========================================" << RESET << endl;
}

// 控制单个设备子菜单
static void controlSingleDevice(SmartHomeHub& hub) {
    cout << CYAN << BOLD << "\n--- 控制单个设备 ---" << RESET << endl;
    hub.showAll();
    string id = getString("\n请输入要控制的设备 ID (输入 0 返回): ", true);
    if (id == "0" || id.empty()) return;

    SmartDevice* dev = hub.findDevice(id);
    if (!dev) {
        cout << RED << "❌ 未找到 ID 为 " << id << " 的设备" << RESET << endl;
        return;
    }

    SmartLight* light = dynamic_cast<SmartLight*>(dev);
    SmartAC* ac = dynamic_cast<SmartAC*>(dev);
    SmartLock* lock = dynamic_cast<SmartLock*>(dev);

    int choice = -1;
    while (choice != 0) {
        cout << CYAN << BOLD << "\n--- 设备 [" << id << "] 控制菜单 ---" << RESET << endl;
        cout << " [1] 开启设备" << endl;
        cout << " [2] 关闭设备" << endl;
        cout << " [3] 查看当前状态" << endl;
        if (light) {
            cout << " [4] 调高亮度" << endl;
            cout << " [5] 调低亮度" << endl;
            cout << " [6] 设置亮度 (0~100)" << endl;
        } else if (ac) {
            cout << " [4] 设置温度 (16~30)" << endl;
        } else if (lock) {
            cout << " [4] 上锁" << endl;
            cout << " [5] 解锁" << endl;
        }
        cout << " [0] 返回" << endl;

        choice = getInt("请选择操作: ", 0, 6);

        switch (choice) {
            case 1:
                dev->turnOn();
                dev->showStatus();
                break;
            case 2:
                dev->turnOff();
                dev->showStatus();
                break;
            case 3:
                dev->showStatus();
                break;
            case 4:
                if (light) {
                    ++(*light);
                    light->showStatus();
                } else if (ac) {
                    int temp = getInt("请输入目标温度 (16~30): ", 16, 30);
                    try {
                        ac->setTemperature(temp);
                        ac->showStatus();
                    } catch (const DeviceException& e) {
                        cerr << RED << "❌ 操作失败: " << e.what() << RESET << endl;
                    }
                } else if (lock) {
                    lock->lock();
                    lock->showStatus();
                }
                break;
            case 5:
                if (light) {
                    --(*light);
                    light->showStatus();
                } else if (lock) {
                    string pwd = getString("请输入密码: ");
                    try {
                        lock->unlock(pwd);
                        lock->showStatus();
                    } catch (const SecurityException& e) {
                        cerr << RED << "❌ 操作失败: " << e.what() << RESET << endl;
                    }
                } else {
                    cout << RED << "❌ 无效选项" << RESET << endl;
                }
                break;
            case 6:
                if (light) {
                    int b = getInt("请输入亮度值 (0~100): ", 0, 100);
                    light->setBrightness(b);
                    light->showStatus();
                } else {
                    cout << RED << "❌ 无效选项" << RESET << endl;
                }
                break;
            case 0:
                break;
            default:
                cout << RED << "❌ 无效选项，请重新输入" << RESET << endl;
                break;
        }
    }
}

// 添加设备子菜单
static void addDeviceMenu(SmartHomeHub& hub) {
    cout << CYAN << BOLD << "\n--- 添加新设备 ---" << RESET << endl;
    cout << " [1] 智能灯" << endl;
    cout << " [2] 智能空调" << endl;
    cout << " [3] 智能门锁" << endl;
    cout << " [0] 返回" << endl;

    int type = getInt("请选择操作: ", 0, 3);
    if (type == 0) return;

    string id = getString("请输入设备ID (如 L003): ");
    if (id.find(' ') != string::npos) {
        cout << RED << "❌ ID 不能包含空格！" << RESET << endl;
        return;
    }

    string name = getString("请输入设备名称 (如 阳台灯): ");

    if (hub.findDevice(id) != nullptr) {
        cout << RED << "❌ 设备 ID " << id << " 已存在！" << RESET << endl;
        return;
    }

    switch (type) {
        case 1: {
            int brightness = getInt("请输入初始亮度 (0-100): ", 0, 100);
            hub + DeviceFactory::create("Light", id, name, brightness);
            cout << GREEN << "✅ 智能灯 [" << name << "] 添加成功！" << RESET << endl;
            break;
        }
        case 2: {
            int temp = getInt("请输入初始温度 (16-30): ", 16, 30);
            hub + DeviceFactory::create("AC", id, name, temp);
            cout << GREEN << "✅ 智能空调 [" << name << "] 添加成功！" << RESET << endl;
            break;
        }
        case 3: {
            string pwd = getString("请输入门锁密码: ", true);
            if (pwd.empty()) pwd = "123456";
            hub + DeviceFactory::create("Lock", id, name, 0, pwd);
            cout << GREEN << "✅ 智能门锁 [" << name << "] 添加成功！" << RESET << endl;
            break;
        }
        default:
            cout << RED << "❌ 无效选择！" << RESET << endl;
    }
}

// 删除设备子菜单
static void removeDeviceMenu(SmartHomeHub& hub) {
    cout << CYAN << BOLD << "\n--- 删除设备 ---" << RESET << endl;
    hub.showAll();
    string id = getString("\n请输入要删除的设备 ID (输入 0 返回): ", true);
    if (id == "0" || id.empty()) return;

    SmartDevice* dev = hub.findDevice(id);
    if (!dev) {
        cout << RED << "❌ 未找到 ID 为 " << id << " 的设备" << RESET << endl;
        return;
    }

    dev->showStatus();
    if (confirm("确定要删除该设备吗")) {
        if (hub.removeDevice(id)) {
            cout << GREEN << "✅ 设备 " << id << " 已删除" << RESET << endl;
        } else {
            cout << RED << "❌ 删除失败" << RESET << endl;
        }
    } else {
        cout << YELLOW << "已取消删除" << RESET << endl;
    }
}

// 场景模式子菜单
static void sceneMenu(SmartHomeHub& hub) {
    cout << CYAN << BOLD << "\n--- 场景模式 ---" << RESET << endl;
    cout << " [1] 回家模式 (开灯开空调，大门解锁)" << endl;
    cout << " [2] 睡眠模式 (关灯，空调调 24°C)" << endl;
    cout << " [3] 离家模式 (关灯关空调，大门上锁)" << endl;
    cout << " [0] 返回" << endl;

    switch (getInt("请选择操作: ", 0, 3)) {
        case 1:
            hub.sceneMode("回家模式", [](SmartDevice* dev) {
                SmartLock* lock = dynamic_cast<SmartLock*>(dev);
                if (lock) {
                    try {
                        lock->unlock("888888");  // 默认主密码
                    } catch (...) {
                        // 密码错误时保持原状态，不中断其他设备
                    }
                } else {
                    dev->turnOn();
                }
            });
            break;
        case 2:
            hub.sceneMode("睡眠模式", [](SmartDevice* dev) {
                if (dynamic_cast<SmartLight*>(dev)) {
                    dev->turnOff();
                } else if (auto* ac = dynamic_cast<SmartAC*>(dev)) {
                    try {
                        ac->setTemperature(24);
                    } catch (...) {}
                    ac->turnOn();
                }
            });
            break;
        case 3:
            hub.sceneMode("离家模式", [](SmartDevice* dev) {
                SmartLock* lock = dynamic_cast<SmartLock*>(dev);
                if (lock) {
                    lock->lock();
                } else {
                    dev->turnOff();
                }
            });
            break;
        case 0:
            return;
        default:
            cout << RED << "❌ 无效选择！" << RESET << endl;
    }
}

int main() {
    SmartHomeHub hub;

    // 预置一些设备，避免空列表
    hub + DeviceFactory::create("Light", "L001", "客厅主灯", 80);
    hub + DeviceFactory::create("Light", "L002", "书房灯", 60);
    hub + DeviceFactory::create("AC", "AC01", "主卧空调", 26);
    hub + DeviceFactory::create("Lock", "LK01", "大门门锁", 0, "888888");

    // 若存在上次保存的配置文件，则覆盖默认设备
    hub.loadConfig("smart_home.cfg");

    cout << CYAN << BOLD << "========================================" << RESET << endl;
    cout << CYAN << BOLD << "  🏠 欢迎使用智能家居控制中心" << RESET << endl;
    cout << CYAN << BOLD << "========================================" << RESET << endl;
    pause();

    bool running = true;
    while (running) {
        clearScreen();
        showMenu();

        switch (getInt("请选择操作: ", 0, 8)) {
            case 1:
                hub.showAll();
                pause();
                break;
            case 2:
                addDeviceMenu(hub);
                pause();
                break;
            case 3:
                controlSingleDevice(hub);
                pause();
                break;
            case 4:
                sceneMenu(hub);
                pause();
                break;
            case 5:
                hub.saveConfig("smart_home.cfg");
                pause();
                break;
            case 6:
                hub.loadConfig("smart_home.cfg");
                pause();
                break;
            case 7:
                SystemLogger::printReport(hub);
                pause();
                break;
            case 8:
                removeDeviceMenu(hub);
                pause();
                break;
            case 0:
                if (confirm("确定要退出系统吗？配置将自动保存")) {
                    cout << CYAN << "\n💾 正在保存配置并退出..." << RESET << endl;
                    hub.saveConfig("smart_home.cfg");
                    running = false;
                }
                break;
            default:
                cout << RED << "❌ 无效选项，请重新输入！" << RESET << endl;
                pause();
        }
    }

    cout << GREEN << "系统已安全退出，所有资源已释放。" << RESET << endl;
    return 0;
}
