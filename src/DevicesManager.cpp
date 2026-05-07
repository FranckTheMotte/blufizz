/* Local headers files. */
#include "DevicesManager.h"

namespace device {

void DevicesManager::insert(const std::shared_ptr<Device> device)
{
  devices_.insert_or_assign(device->address(), device);
}

} // device namespace