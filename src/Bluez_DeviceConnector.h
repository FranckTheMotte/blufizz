#pragma once

/* Local header files. */
#include "Bluez_DeviceClient.h"
#include "IDeviceConnector.h"

namespace bluetooth::bluez {
class BluezDeviceConnector : public device::IDeviceConnector
{
public:
  /**
   * @brief Create a bluez device connector.
   *
   * @param client Bluez client.
   */
  BluezDeviceConnector(BluezDeviceClient &client);
  virtual ~BluezDeviceConnector() = default;

  virtual int connect(const std::string &objectPath) override;
  virtual int disconnect(const std::string &objectPath) override;
  virtual void fetchProfile(std::shared_ptr<device::Device> &device) override;

private:
  /**
   * @brief Bluez client.
   */
  BluezDeviceClient &client_;
};

} // bluetooth::bluez namespace
