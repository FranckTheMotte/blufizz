#pragma once

namespace device {

/**
 * @brief Interface defining actions on device:
 * - connect
 * - disconnect
 * - get properties
 */
class IDeviceConnector
{
public:
  virtual ~IDeviceConnector() = default;

  /**
   * @brief Connect synchronously to a remote device.
   *
   * @param objectPathStr  D-bus path to bluez device (ex /org/bluez/hci/dev_90_F1_57_D0_0A_C2).
   *
   * @return 0 if success, <0 otherwise.
   */
  virtual int connect(const std::string &objectPath) = 0;

  /**
   * @brief Disconnect synchronously from a remote device.
   *
   * @param objectPathStr  D-bus path to bluez device.
   *
   * @return 0 if success, <0 otherwise.
   */
  virtual int disconnect(const std::string &objectPath) = 0;

  /**
   * @brief Retrieve all properties from device.
   *
   * @param device Where to store the device properties.
   */
  virtual void fetchProfile(std::shared_ptr<device::Device> &device) = 0;
};

} // device namespace
