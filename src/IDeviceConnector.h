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
  virtual int connect(const std::string &objectPath) = 0;
  virtual int disconnect(const std::string &objectPath) = 0;
  virtual void fetchProfile(std::shared_ptr<device::Device> &device) = 0;
};

} // device namespace
