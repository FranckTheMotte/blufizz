#pragma once

#include <string>

namespace adapter {

/**
 * @brief Interface defining actions on top of bluetooth stack:
 * - switch bluetooth adapter on or off
 * - start or stop discovery session to scan BR/EDR and/or LE devices
 * - remove a device from bluetooth stack
 */
class IAdapterController
{
public:
  virtual ~IAdapterController() = default;

  /**
   * @brief Switch an adapter on.
   *
   * If the adapter is already on, it does nothing.
   */
  virtual void enable() = 0;

  /**
   * @brief Switch an adapter off.
   *
   * If the adapter is already off, it does nothing.
   */
  virtual void disable() = 0;

  /**
   * @brief Check the enable state of an adapter.
   *
   * @return
   * - @c true the adapter is on.
   * - @c false the adapter is off.
   */
  virtual bool isEnabled() const = 0;

  /**
   * @brief Remove a remote device from cache.
   *
   * @param objectPath Path to remote device.
   *
   * @return
   * - @c 0 on success.
   * - @c -1 otherwise.
   */
  virtual int removeDevice(std::string objectPath) = 0;

  /**
   * @brief Start the device discovery session.
   *
   * @return 0 if started, <0 otherwise.
   */
  virtual int startScan() = 0;

  /**
   * @brief Stop the device discovery session.
   *
   * @return 0 if stopped, <0 otherwise.
   */
  virtual int stopScan() = 0;
};

} // adapter namespace
