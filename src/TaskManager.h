
#pragma once

/* Standard C++ header files. */
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <variant>

/* Local header files. */
#include "Bluez_ObjectManager.h"
#include "Database.h"
#include "DevicesManager.h"
#include "IDeviceConnector.h"

struct AddDeviceTask {
  std::shared_ptr<device::Device> device;
};

namespace blufizz::tools {

using Task = std::variant<AddDeviceTask>;

class TaskManager
{
public:
  TaskManager(device::IDeviceConnector &deviceConnector,
              device::DevicesManager &devicesManager,
              storage::Database &database_);
  ~TaskManager() = default;

  TaskManager(const TaskManager &) = delete;
  TaskManager &operator=(const TaskManager &) = delete;

  void push(Task task);
private:
  void run(std::stop_token stop_token);
  void handle(const AddDeviceTask &task);

private:
  mutable std::mutex mutex_;
  std::condition_variable_any cv_;

  std::deque<Task> tasks_;

  std::jthread worker_;

  device::IDeviceConnector &deviceConnector_;

  device::DevicesManager &devicesManager_;

  storage::Database &database_;
};

}
