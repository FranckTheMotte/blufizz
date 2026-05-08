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

    std::cout << "Add device : " + device->address() + " RSSI " + std::to_string(device->rssi().value_or(0)) << std::endl;
    devicesManager_.insert(device);
  }

  //deviceConnector_.connect(device->objectPath());
  deviceConnector_.fetchProfile(device);
  //deviceConnector_.disconnect(device->objectPath());
  std::cout << device->toString();
  database_.updateDevice(*device.get());
}

} // blufizz::tools namespace
