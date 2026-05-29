/* Standard C++ header files. */
#include <map>

/* Local header files. */
#include "Bluez_AdapterController.h"
#include "Bluez_Dbus.h"

/**
 * @brief namespace for BlueTooth related definitions.
 */
namespace bluetooth::bluez {

BluezAdapterController::BluezAdapterController(const std::shared_ptr<sdbus::IConnection> &connection,
                                               sdbus::ServiceName destination,
                                               sdbus::ObjectPath objectPath)
  : destination_(std::move(destination)),
    objectPath_(std::move(objectPath)),
    proxy_(sdbus::createProxy(*connection, destination_, objectPath_))
{

  proxy_->uponSignal(PROPS_PROPERTIES_CHANGED)
  .onInterface("org.freedesktop.DBus.Properties")
  .call([this](
          const std::string & interfaceName,
          const std::map<std::string, sdbus::Variant> &changed,
  const std::vector<std::string> &invalidated) {
    if (interfaceName == B_ADAPTER_INTERFACE) {
      this->onAdapterPropertiesChanged(changed);
    }
  });
}

void BluezAdapterController::onAdapterPropertiesChanged(
  const std::map<std::string, sdbus::Variant> &changed)
{
  std::cout << "Properties changed" << std::endl;
}

void BluezAdapterController::enable()
{
  if (!isEnabled()) {
    setPowered(true);
  }
}

void BluezAdapterController::disable()
{
  setPowered(false);
}

bool BluezAdapterController::isEnabled() const
{
  return getPowered();
}

void BluezAdapterController::setPowered(bool value)
{
  try {
    proxy_->setProperty(B_ADAPTER_POWERED_PROP)
    .onInterface(B_ADAPTER_INTERFACE)
    .toValue(value);
  } catch (const sdbus::Error &e) {
    throw std::runtime_error(
      "Failed to set adapter power on " + getAdapterPath() +
      ": " + e.getName() + " - " + e.getMessage()
    );
  }
}

bool BluezAdapterController::getPowered() const
{
  try {
    bool powered{};
    sdbus::Variant value = proxy_->getProperty(B_ADAPTER_POWERED_PROP).onInterface(B_ADAPTER_INTERFACE);
    powered = value.get<bool>();
    return powered;
  } catch (const sdbus::Error &e) {
    throw std::runtime_error(
      "Failed to read adapter power on " + getAdapterPath() +
      ": " + e.getName() + " - " + e.getMessage()
    );
  }
}

int BluezAdapterController::startScan()
{
  try {
    std::map<std::string, sdbus::Variant> filters = {
      {
        // only LE device to catch more devices.
        // until distributing scan timing between BR/EDR and LE.
        B_DISCO_FILTER_TRANSPORT,
        sdbus::Variant{std::string{B_DISCO_FILTER_TRANSPORT_LE}}
      },
      {
        // better for continuous observation
        B_DISCO_FILTER_DUPLICATEDATA,
        sdbus::Variant{true}
      }
    };

    proxy_->callMethod(B_SET_DISCOVERY_FILTER_METHOD).onInterface(B_ADAPTER_INTERFACE).withArguments(filters);
    proxy_->callMethod(B_START_DISCOVERY_METHOD).onInterface(B_ADAPTER_INTERFACE);
    return 0;
  } catch (const sdbus::Error &e) {
    std::cout << "Failed to start discovery on " << getAdapterPath() << ": " <<
              e.getName() << " - " << e.getMessage() << std::endl;
    return -1;
  }
}

int BluezAdapterController::stopScan()
{
  try {
    proxy_->callMethod(B_STOP_DISCOVERY_METHOD).onInterface(B_ADAPTER_INTERFACE);
    return 0;
  } catch (const sdbus::Error &e) {
    std::cout << "Failed to stop discovery on " << getAdapterPath() << ": " <<
              e.getName() << " - " << e.getMessage() << std::endl;
    return -1;
  }
}

int BluezAdapterController::removeDevice(std::string objectPath)
{
  sdbus::ObjectPath removedObject(objectPath);
  try {
    proxy_->callMethod(B_REMOVE_DEVICE_METHOD).onInterface(B_ADAPTER_INTERFACE).withArguments(removedObject);
    return 0;
  } catch (const sdbus::Error &e) {
    std::cout << "Failed to remove device on " << getAdapterPath() << ": " <<
              e.getName() << " - " << e.getMessage() << std::endl;
    return -1;
  }
}

} /* bluetooth::bluez namespace */
