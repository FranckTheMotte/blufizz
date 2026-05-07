/* Local header files. */
#include "Bluez_DeviceConnector.h"

namespace bluetooth::bluez {

BluezDeviceConnector::BluezDeviceConnector(BluezDeviceClient &client) :
  client_(client) {}

int BluezDeviceConnector::connect(const std::string &objectPathStr)
{
  return client_.connect(objectPathStr);
}

int BluezDeviceConnector::disconnect(const std::string &objectPathStr)
{
  return client_.disconnect(objectPathStr);
}

void BluezDeviceConnector::fetchProfile(std::shared_ptr<device::Device> &device)
{
  return client_.fetchProfile(device);
}

} // bluetooth::bluez namespace
