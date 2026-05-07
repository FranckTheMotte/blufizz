#pragma once

#include <mutex>
#include <string>

namespace bluetooth {

/**
 * @brief A remote bluetooth device.
 */
class BluetoothDevice
{
public:
  /**
   * @brief Create a device.
   *
   * @param[in] address The device's address.
   */
  BluetoothDevice(const std::string &address);

  /**
   * @brief Destroy a device.
   *
   * It calls finalize() if not done yet.
   */
  ~BluetoothDevice();

  /* Disable copy operations. */
  BluetoothDevice(const BluetoothDevice &) = delete;
  BluetoothDevice &operator=(const BluetoothDevice &) = delete;

  /* Disable move operations. */
  BluetoothDevice(BluetoothDevice &&) = delete;
  BluetoothDevice &operator=(BluetoothDevice &&) = delete;

private:

  /**
   * @brief BD ADDR of device.
   */
  std::string address_;

  /**
   * @brief Lock against concurrent accesses.
   */
  mutable std::mutex mutex_;
};

} // bluetooth namespace
