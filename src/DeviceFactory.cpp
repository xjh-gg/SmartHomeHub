#include "DeviceFactory.h"
#include <memory>

using namespace std;

unique_ptr<SmartDevice> DeviceFactory::create(const string& type,
                                               const string& id,
                                               const string& name,
                                               int value,
                                               const string& password) {
    if (type == "Light") {
        return make_unique<SmartLight>(id, name, value);
    } else if (type == "AC") {
        return make_unique<SmartAC>(id, name, value);
    } else if (type == "Lock") {
        return make_unique<SmartLock>(id, name, password.empty() ? "123456" : password);
    }
    return nullptr;
}
