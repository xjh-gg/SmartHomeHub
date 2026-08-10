#include "DeviceFactory.h"
#include <memory>

using namespace std;

unique_ptr<DeviceFactory> DeviceFactory::getFactory(const string& type) {
    if (type == "Light") return make_unique<LightFactory>();
    if (type == "AC")    return make_unique<ACFactory>();
    if (type == "Lock")  return make_unique<LockFactory>();
    return nullptr;
}

unique_ptr<SmartDevice> LightFactory::create(const string& id,
                                              const string& name,
                                              int value,
                                              const string& /*password*/) {
    return make_unique<SmartLight>(id, name, value);
}

unique_ptr<SmartDevice> ACFactory::create(const string& id,
                                           const string& name,
                                           int value,
                                           const string& /*password*/) {
    return make_unique<SmartAC>(id, name, value);
}

unique_ptr<SmartDevice> LockFactory::create(const string& id,
                                             const string& name,
                                             int /*value*/,
                                             const string& password) {
    return make_unique<SmartLock>(id, name, password.empty() ? "123456" : password);
}
