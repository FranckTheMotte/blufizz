#pragma once

/**
 * @file
 * @brief Access to the BlueTooth controller.
 *
 * This class provides an abstraction layer on top of bluetooth stack for the following functions:
 * - switch bluetooth adapter on or off
 * - start or stop discovery session to scan BR/EDR and/or LE devices
 * - connect to or disconnect from a device
 * - query information on a device
 * with safe concurrent access.
 */

/* Standard C++ header files. */
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <memory>

/* External dependencies. */
#include <sdbus-c++/sdbus-c++.h>

/* Local header files. */
#include "IAdapterController.h"

/**
 * @brief namespace for bluez related definitions.
 */
namespace bluetooth::bluez {

class BluezAdapterController : adapter::IAdapterController
{
public:
  /**
   * @brief Create a bluez adapter with dbus proxy to control it.
   *
   * @param connection  D-bus system bus Connection.
   * @param destination D-bus destination service name.
   * @param objectPath  D-bus path to bluez adapter.
   */
  BluezAdapterController(const std::shared_ptr<sdbus::IConnection> &connection, sdbus::ServiceName destination, sdbus::ObjectPath objectPath);

  /* Copy and move operations are disabled. */
  BluezAdapterController(const BluezAdapterController &) = delete;
  BluezAdapterController &operator=(const BluezAdapterController &) = delete;
  BluezAdapterController(BluezAdapterController &&) = delete;
  BluezAdapterController &operator=(BluezAdapterController &&) = delete;

  /**
   * @brief Enable (power) of the adapter.
   */
  virtual void enable() override;

  /**
   * @brief Disable (power) of the adapter.
   */
  virtual void disable() override;

  /**
   * @brief Power status of the adapter.
   *
   * @return true if enable, false otherwise.
   */
  virtual bool isEnabled() const override;

  /**
   * @brief Start a scan.
   *
   * @return
   * - @c 0 on success.
   * - @c -1 otherwise.
   */
  virtual int startScan() override;

  /**
   * @brief Start a scan.
   *
   * @return
   * - @c 0 on success.
   * - @c -1 otherwise.
   */
  virtual int stopScan() override;

  /**
   * @brief Remove a remote device from cache.
   *
   * @param objectPath Path to remote device.
   *
   * @return
   * - @c 0 on success.
   * - @c -1 otherwise.
   */
  virtual int removeDevice(std::string objectPath) override;

  /**
   * @name getters
   * @{
   */
  const std::string &getAdapterPath() const noexcept {
    return adapterPath_;
  }

  const std::string &objectPath() const noexcept {
    return objectPath_;
  }
  /** @} */

private:

  /**
   * @brief Handler on org.freedesktop.Dbus.Properties.PropertiesChanged signal.
   *
   * @param changed Properties map.
   */
  void onAdapterPropertiesChanged(
    const std::map<std::string, sdbus::Variant> &changed);

  /**
   * @brief Setter of Powered property.
   *
   * @param value On/Off
   */
  void setPowered(bool value);

  /**
   * @brief Getter of Powered property.
   *
   * @return true/false.
   */
  bool getPowered() const;

  /**
   * @brief D-bus Service name.
   */
  sdbus::ServiceName destination_;

  /**
   * @ brief Path to bluetooth adapter.
   */
  std::string adapterPath_;

  /**
   * @brief D-bus object path.
   */
  sdbus::ObjectPath objectPath_;

  /**
   * @brief Proxy to call dbus methods.
   */
  std::unique_ptr<sdbus::IProxy> proxy_;
};

} /* bluetooth::bluez namespace */
