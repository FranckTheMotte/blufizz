#pragma once

/**
 * @file
 * @brief Manage bluetooth adapters.
 *
 * This class handles bluetooth adapters with bluez path.
 */

/* Standard C++ header files. */
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

/* Local header files. */
#include "Bluez_AdapterController.h"
#include "Bluez_ObjectManager.h"
#include "TaskManager.h"

namespace bluetooth::bluez {

class BluezAdapterManager
{
public:
  /**
   * @brief Initialize catching of bluez signals.
   *
   * @param connection  Dbus connection to system bus.
   * @param objectManagerProxy Dbus object manager proxy.
   * @param taskManager Manager for events from bluez signals.
   */
  BluezAdapterManager(const std::shared_ptr<sdbus::IConnection> &connection,
                      const std::shared_ptr<sdbus::IProxy> &objectManagerProxy,
                      blufizz::tools::TaskManager &taskManager);

  /**
  * @brief Release the resources.
  */
  ~BluezAdapterManager();

  /**
  * @brief Create or a get a bluez adapter from his object path.
  * TODO : get list of bluez adapters from dbus.
  *
  * @param object_path path to adapter (ex: /org/bluez/hci0).
  *
  * @return a supposed existing bluez adapter.
  */
  BluezAdapterController &get(std::string object_path);

  /**
  * @brief Find a bluez adapter from his object path.
  *
  * @param object_path path to adapter (ex: /org/bluez/hci0).
  *
  * @return a bluez adapter or a null if not found.
  */
  BluezAdapterController *find(std::string_view object_path) noexcept;

private:
  /**
   * @brief Share D-Bus connection.
   */
  std::shared_ptr<sdbus::IConnection> connection_;
  std::unordered_map<std::string, std::unique_ptr<BluezAdapterController>> adapters_;

  /**
   * @brief Object manager to catch new bluetooth devices with signals:
   * - InterfacesAdded
   */
  bluetooth::bluez::ObjectManager objectManager_;
};

} /* bluetooth namespace */
