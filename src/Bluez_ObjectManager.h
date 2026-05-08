#pragma once

/**
 * @file
 * @brief Object Manager to catch bluez events.
 *
 * This class catches the :
 * - device added
 * - device removed
 */

/* Standard C++ header files. */
#include <iostream>
#include <stdexcept>
#include <string>

/* External dependencies. */
#include <sdbus-c++/sdbus-c++.h>

/* Local header files. */
#include "TaskManager.h"

// forward declarations
namespace blufizz::tools {
class TaskManager;
}

/**
 * @brief namespace for bluez related definitions.
 */
namespace bluetooth::bluez {

class ObjectManager
{
public:
  /**
   * @brief Subscribe to bluez object manager signals.
   *
   * @param objectManagerProxy Dbus object manager proxy.
   * @param taskManager Manager for events from bluez signals.
   */
  ObjectManager(const std::shared_ptr<sdbus::IProxy> &objectManagerProxy,
                blufizz::tools::TaskManager &taskManager);

  /* Copy and move operations are disabled. */
  ObjectManager(const ObjectManager &) = delete;
  ObjectManager &operator=(const ObjectManager &) = delete;
  ObjectManager(ObjectManager &&) = delete;
  ObjectManager &operator=(ObjectManager &&) = delete;

private:
  void onInterfacesAdded(
    const sdbus::ObjectPath &objectPath,
    const std::map <
    std::string,
    std::map<std::string, sdbus::Variant>
    > & interfacesAndProperties);

  /**
   * @brief Proxy to bluez signals.
   */
  const std::shared_ptr<sdbus::IProxy> &objectManagerProxy_;

  /**
   * @brief The task manager.
   */
  blufizz::tools::TaskManager &taskManager_;

  uint32_t deviceId_ = 0;
};

} /* bluez namespace */
