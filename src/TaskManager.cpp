/* Standard C++ header files. */
#include <iostream>

/* Local header files. */
#include "TaskManager.h"
#include "Bluez_DeviceConnector.h"

namespace blufizz::tools {

TaskManager::TaskManager(device::IDeviceConnector &deviceConnector,
                         device::DevicesManager &devicesManager,
                         storage::Database &database)
  : deviceConnector_(deviceConnector),
    devicesManager_(devicesManager),
    database_(database),
    worker_([this](std::stop_token stop_token)
{
  run(stop_token);
}) {}

void TaskManager::push(Task task)
{
  {
    std::lock_guard lock(mutex_);
    tasks_.push_back(std::move(task));
  }

  cv_.notify_one();
}
/*
std::optional<device::Device> TaskManager::find_device(const std::string& address) const
{
  std::lock_guard lock(mutex_);

  auto it = devices_.find(address);
  if (it == devices_.end()) {
    return std::nullopt;
  }

  return it->second;
}
*/
void TaskManager::run(std::stop_token stopToken)
{
  while (!stopToken.stop_requested()) {
    std::optional<Task> task;

    {
      std::unique_lock lock(mutex_);

      cv_.wait(lock, stopToken, [this] {
        return !tasks_.empty();
      });

      if (tasks_.empty()) {
        continue;
      }

      task = std::move(tasks_.front());
      tasks_.pop_front();
    }

    std::visit([this](auto &&concreteTask) {
      handle(concreteTask);
    }, *task);
  }
}

void TaskManager::handle(const AddDeviceTask &task)
{
  std::shared_ptr<device::Device> device;

  {
    std::lock_guard lock(mutex_);
    device = task.device;

    /* if (device->objectPath() != "/org/bluez/hci0/dev_90_F1_57_D0_0A_C1")
    {
      std::cout << "Ignore " << device->objectPath() << std::endl;
      return ;
    }*/

    std::cout << "Add device : " + device->address() + " RSSI " + std::to_string(device->rssi().value_or(0)) << std::endl;
    devicesManager_.insert(device);
  }

  //deviceConnector_.connect(device->objectPath());
  deviceConnector_.fetchProfile(device);
  //deviceConnector_.disconnect(device->objectPath());
  std::cout << device->toString();
  database_.updateDevice(*device.get());
}

#if 0
void TaskManager::handle(const ConnectAndInspectDeviceTask &task)
{
  /*
    Important:
    Do the slow BlueZ work OUTSIDE the mutex.

    Example pseudo-flow:
      - create proxy for task.object_path
      - call Device1.Connect()
      - read Device1 properties
      - maybe inspect GATT services
  */



  Device inspected;
  inspected.address = task.address;
  inspected.object_path = task.object_path;

  // TODO: Replace this with real BlueZ calls.
  inspected.connected = true;
  inspected.name = "inspected-device";

  {
    std::lock_guard lock(mutex_);
    devices_.insert_or_assign(inspected.address, std::move(inspected));
  }
}
#endif

} // blufizz::tools
