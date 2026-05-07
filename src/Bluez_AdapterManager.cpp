#include "Bluez_AdapterManager.h"
#include "Bluez_Dbus.h"

namespace bluetooth::bluez {

BluezAdapterManager::BluezAdapterManager(const std::shared_ptr<sdbus::IConnection> &connection,
                                         const std::shared_ptr<sdbus::IProxy> &objectManagerProxy,
                                         blufizz::tools::TaskManager &taskManager)
  : connection_(connection),
    objectManager_(objectManagerProxy, taskManager)
{
}

BluezAdapterManager::~BluezAdapterManager()
{
}

BluezAdapterController &BluezAdapterManager::get(std::string objectPathStr)
{
  auto it = adapters_.find(objectPathStr);
  if (it != adapters_.end()) {
    return *it->second;
  }

  sdbus::ServiceName destination{B_SERVICE_NAME};
  sdbus::ObjectPath objectPath{objectPathStr};

  auto adapter =
    std::make_unique<BluezAdapterController>(connection_, std::move(destination), std::move(objectPath));
  auto *raw_ptr = adapter.get();

  adapters_.emplace(raw_ptr->objectPath(), std::move(adapter));
  return *raw_ptr;
}

BluezAdapterController *BluezAdapterManager::find(std::string_view object_path) noexcept
{
  auto it = adapters_.find(std::string(object_path));
  if (it == adapters_.end()) {
    return nullptr;
  }
  return it->second.get();
}

} /* bluetooth namespace */
