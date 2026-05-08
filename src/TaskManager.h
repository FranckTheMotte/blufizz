
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
  /**
   * @brief FIFO device task manager.
   * Available tasks:
   * - AddDevice
   * 
   * @param deviceConnector Tool to handle a device.
   * @param devicesManager Manager of all devices in the current session.
   * @param database Storage for devices.
   */
  TaskManager(device::IDeviceConnector &deviceConnector,
              device::DevicesManager &devicesManager,
              storage::Database &database);
  ~TaskManager() = default;

  TaskManager(const TaskManager &) = delete;
  TaskManager &operator=(const TaskManager &) = delete;

  /**
   * @brief Push a new task.
   * 
   * @param task New task.
   */
  void push(Task task);
private:
  void run(std::stop_token stop_token);
  void handle(const AddDeviceTask &task);

private:
  /**
   * @brief Protect against concurrent accesses.
   */
  mutable std::mutex mutex_;

  /**
   * @brief Condition to notify new tasks.
   */
  std::condition_variable_any cv_;

  /**
   * @brief List of tasks to execute.
   */
  std::deque<Task> tasks_;

  /**
   * @brief Worker executing the tasks.
   */
  std::jthread worker_;

  /**
   * @brief Tool to handle a device.
   */
  device::IDeviceConnector &deviceConnector_;

  /**
   * @brief Manager of all devices in the current session.
   */
  device::DevicesManager &devicesManager_;

  /**
   * @brief Storage for devices.
   */
  storage::Database &database_;
};

}
