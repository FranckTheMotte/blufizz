#pragma once

/* Standard C++ header files. */
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <variant>

/* External dependencies. */
#include <sdbus-c++/sdbus-c++.h>

/* Local Headers. */
#include "Device.h"

namespace bluetooth::bluez {

using Properties = std::map<std::string, sdbus::Variant>;
using Interfaces = std::map<std::string, Properties>;
using ManagedObjects = std::map<sdbus::ObjectPath, Interfaces>;

class BluezDeviceClient
{
public:
  /**
   * @brief Create a bluez device client with dbus proxy to control it.
   *
   * @param connection  D-bus system bus Connection.
   * @param destination D-bus destination service name.
   * @param objectManagerProxy Dbus object manager proxy.
   */
  BluezDeviceClient(std::shared_ptr<sdbus::IConnection> &connection,
                    std::string destination,
                    const std::shared_ptr<sdbus::IProxy> &objectManagerProxy
                   );

  virtual ~BluezDeviceClient() = default;

  /* Disable copy operations. */
  BluezDeviceClient(const BluezDeviceClient &) = delete;
  BluezDeviceClient &operator=(const BluezDeviceClient &) = delete;

  /* Disable move operations. */
  BluezDeviceClient(BluezDeviceClient &&) = delete;
  BluezDeviceClient &operator=(BluezDeviceClient &&) = delete;

  /**
   * @brief Connect synchronously to a remote device.
   *
   * @param objectPathStr  D-bus path to bluez device (ex /org/bluez/hci/dev_90_F1_57_D0_0A_C2).
   *
   * @return 0 if success, <0 otherwise.
   */
  int connect(const std::string &objectPathStr);

  /**
   * @brief Disconnect synchronously from a remote device.
   *
   * @param objectPathStr  D-bus path to bluez device.
   *
   * @return 0 if success, <0 otherwise.
   */
  int disconnect(const std::string &objectPathStr);

  /**
   * @brief Retrieve all properties from device.
   *
   * @param device Where to store the device properties.
   */
  void fetchProfile(std::shared_ptr<device::Device> &device);

private:

  /**
   * @brief Get a property from device.
   *
   * @tparam T Type of the property.
   *
   * @param proxy Dbus proxy to the remote device.
   * @param propertyName Name of the property.
   *
   * @return a value if success, std::nullopt otherwise.
   */
  template<typename T> std::optional<T>
  getPropertyUnsafe(const std::shared_ptr<sdbus::IProxy> &proxy,
                    std::string propertyName);

  /**
   * @brief D-bus connection.
   */
  std::shared_ptr<sdbus::IConnection> &connection_;

  /**
   * @brief D-bus Service name.
   */
  sdbus::ServiceName destination_;

  /**
   * @brief Lock against concurrent accesses.
   */
  mutable std::mutex mutex_;

  /**
   * @brief Proxy to bluez signals.
   */
  const std::shared_ptr<sdbus::IProxy> &objectManagerProxy_;

  /**
   * @brief Wait for device's services to be resolved.
   * 
   * @param devicePath Bluez path to the device.
   * @param timeout Timeout in sec (default 10s).
   * 
   * @return True if resolved, false otherwise.
   */
  bool waitForServicesResolved(
    const std::string &devicePath,
    std::chrono::seconds timeout = std::chrono::seconds{10});
};

} // namespace bluetooth::bluez