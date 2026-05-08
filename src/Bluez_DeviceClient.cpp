/* Local Headers. */
#include "Bluez_DeviceClient.h"
#include "Bluez_Dbus.h"
#include "Bluez_Gatt.h"

using LockG = std::lock_guard<std::mutex>;

namespace bluetooth::bluez {

BluezDeviceClient::BluezDeviceClient(std::shared_ptr<sdbus::IConnection> &connection,
                                     std::string destination,
                                     const std::shared_ptr<sdbus::IProxy> &objectManagerProxy
                                    )
  : connection_(connection),
    destination_(destination),
    objectManagerProxy_(objectManagerProxy) {}

int BluezDeviceClient::connect(const std::string &objectPathStr)
{
  LockG autolock(mutex_);

  // create the proxy
  sdbus::ObjectPath objectPath{objectPathStr};
  std::unique_ptr<sdbus::IProxy> proxy = sdbus::createProxy(*connection_, destination_, objectPath);

  try {
    proxy->callMethod(B_DEVICE_CONNECT_METHOD).onInterface(B_DEVICE_INTERFACE);
    return 0;
  } catch (const sdbus::Error &e) {
    throw std::runtime_error(
      "Failed to connect to " + objectPath +
      ": " + e.getName() + " - " + e.getMessage()
    );
  }
}

int BluezDeviceClient::disconnect(const std::string &objectPathStr)
{
  LockG autolock(mutex_);

  // create the proxy
  sdbus::ObjectPath objectPath{objectPathStr};
  std::unique_ptr<sdbus::IProxy> proxy = sdbus::createProxy(*connection_, destination_, objectPath);

  try {
    proxy->callMethod(B_DEVICE_DISCONNECT_METHOD).onInterface(B_DEVICE_INTERFACE);
    return 0;
  } catch (const sdbus::Error &e) {
    throw std::runtime_error(
      "Failed to connect to " + objectPath +
      ": " + e.getName() + " - " + e.getMessage()
    );
  }
}

bool BluezDeviceClient::waitForServicesResolved(
  const std::string &devicePath,
  std::chrono::seconds timeout)
{
  sdbus::ObjectPath objectPath{devicePath};
  sdbus::ServiceName destination("org.bluez");

  auto proxy = sdbus::createProxy(*connection_, destination, objectPath);

  const auto deadline = std::chrono::steady_clock::now() + timeout;

  while (std::chrono::steady_clock::now() < deadline) {
    bool resolved = false;

    try {
      sdbus::Variant value = proxy->getProperty("ServicesResolved").onInterface(B_DEVICE_INTERFACE);
      resolved = value.get<bool>();

      if (resolved) {
        return true;
      }
    } catch (...) {
      return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{200});
  }

  return false;
}

void BluezDeviceClient::fetchProfile(std::shared_ptr<device::Device> &device)
{
  LockG autolock(mutex_);

  std::cout << "getProperties: " << device->objectPath() << " empty " << std::to_string(device->objectPath().empty()) << std::endl;
  if (device->objectPath().empty()) {
    return;
  }

  // create the proxy
  sdbus::ObjectPath objectPath{device->objectPath()};
  std::shared_ptr<sdbus::IProxy> proxy = sdbus::createProxy(*connection_, destination_, objectPath);

  auto address{getPropertyUnsafe<std::string>(proxy, B_DEVICE_PROP_ADDRESS)};
  device->setAddress(address.value_or("N/A"));

  auto addressType{getPropertyUnsafe<std::string>(proxy, B_DEVICE_PROP_ADDRESS_TYPE)};
  device->setAddressType(addressType.value_or("N/A"));

  auto name{getPropertyUnsafe<std::string>(proxy, B_DEVICE_PROP_NAME)};
  device->setName(name.value_or("N/A"));

  auto icon{getPropertyUnsafe<std::string>(proxy, B_DEVICE_PROP_ICON)};
  device->setIcon(icon.value_or("N/A"));

  auto deviceClass{getPropertyUnsafe<uint32_t>(proxy, B_DEVICE_PROP_CLASS)};
  device->setDeviceClass(deviceClass.value_or(0));

  auto appearance{getPropertyUnsafe<uint16_t>(proxy, B_DEVICE_PROP_APPEREANCE)};
  device->setAppearance(appearance.value_or(0));

  auto uuids{getPropertyUnsafe<std::vector<std::string>>(proxy, B_DEVICE_PROP_UUIDS)};
  if (uuids.has_value()) {
    device->setUuids(uuids.value());
  }

  auto paired{getPropertyUnsafe<bool>(proxy, B_DEVICE_PROP_PAIRED)};
  device->setPaired(paired.value_or(false));

  auto connected{getPropertyUnsafe<bool>(proxy, B_DEVICE_PROP_CONNECTED)};
  device->setConnected(connected.value_or(false));

  auto trusted{getPropertyUnsafe<bool>(proxy, B_DEVICE_PROP_TRUSTED)};
  device->setTrusted(trusted.value_or(false));

  auto blocked{getPropertyUnsafe<bool>(proxy, B_DEVICE_PROP_BLOCKED)};
  device->setBlocked(blocked.value_or(false));

  auto alias{getPropertyUnsafe<std::string>(proxy, B_DEVICE_PROP_ALIAS)};
  device->setAlias(alias.value_or("N/A"));

  auto adapter{getPropertyUnsafe<std::string>(proxy, B_DEVICE_PROP_ADAPTER)};
  if (adapter.has_value()) {
    device->setAdapter(adapter.value());
  }

  auto legacyPairing{getPropertyUnsafe<bool>(proxy, B_DEVICE_PROP_LEGACYPAIRING)};
  device->setLegacyPairing(legacyPairing.value_or(false));

  auto modalias{getPropertyUnsafe<std::string>(proxy, B_DEVICE_PROP_MODALIAS)};
  device->setAlias(modalias.value_or("N/A"));

  auto rssi{getPropertyUnsafe<int16_t>(proxy, B_DEVICE_PROP_RSSI)};
  device->setRssi(rssi.value_or(0));

  auto txPower{getPropertyUnsafe<int16_t>(proxy, B_DEVICE_PROP_TXPOWER)};
  device->setTxPower(txPower.value_or(0));

  auto manufacturerData{getPropertyUnsafe<device::Device::ManufacturerData>(proxy, B_DEVICE_PROP_MANUFACTURERDATA)};
  if (manufacturerData.has_value()) {
    device->setManufacturerData(manufacturerData.value());
  }
  auto serviceData{getPropertyUnsafe<device::Device::ServiceData>(proxy, B_DEVICE_PROP_SERVICEDATA)};
  if (serviceData.has_value()) {
    device->setServiceData(serviceData.value());
  }

  // retrieve Gatt services if device is connected
  if (device->connected()) {
    waitForServicesResolved(device->objectPath());
    // gatt profile
    ManagedObjects objects;
    objectManagerProxy_->callMethod(OBJECT_MANAGER_OBJECTS_GET_METHOD)
    .onInterface(OBJECT_MANAGER_INTERFACE)
    .storeResultsTo(objects);

    DeviceGattProfile profile;
    profile.devicePath = device->objectPath();

    std::unordered_map<std::string, std::size_t> service_index;

    // 3. Collect services for this device.
    for (const auto& [path, interfaces] : objects) {
      std::cout << "Path: " << path << std::endl;
      auto service_it = interfaces.find(B_GATT_SERVICE_INTERFACE);
      if (service_it == interfaces.end()) {
        continue;
      }

      const auto &props = service_it->second;

      auto deviceIt = props.find("Device");
      auto uuidIt = props.find("UUID");

      if (deviceIt == props.end() || uuidIt == props.end()) {
        continue;
      }

      auto service_device_path =
        static_cast<std::string>(deviceIt->second.get<sdbus::ObjectPath>());

      std::cout << "serviceDevicePath: " << service_device_path << std::endl;
      if (service_device_path != device->objectPath()) {
        continue;
      }

      Service service;
      service.objectPath = static_cast<std::string>(path);
      service.uuid = uuidIt->second.get<std::string>();

      service_index.emplace(service.objectPath, profile.services.size());
      profile.services.push_back(std::move(service));
    }

    // 4. Collect characteristics and attach them to their parent service.
    for (const auto& [path, interfaces] : objects) {
      auto char_it = interfaces.find(B_GATT_CHARAC_INTERFACE);
      if (char_it == interfaces.end()) {
        continue;
      }

      const auto &props = char_it->second;

      auto service_it = props.find("Service");
      auto uuidIt = props.find("UUID");

      if (service_it == props.end() || uuidIt == props.end()) {
        continue;
      }

      auto parentServicePath =
        static_cast<std::string>(service_it->second.get<sdbus::ObjectPath>());

      auto index_it = service_index.find(parentServicePath);
      if (index_it == service_index.end()) {
        continue;
      }

      Characteristic characteristic;
      characteristic.objectPath = static_cast<std::string>(path);
      characteristic.servicePath = parentServicePath;
      characteristic.uuid = uuidIt->second.get<std::string>();

      profile.services[index_it->second]
      .characteristics
      .push_back(std::move(characteristic));
    }

    // dump
    for (const auto &service : profile.services) {
      std::cout << "Service " << service.uuid << '\n';

      for (const auto &ch : service.characteristics) {
        std::cout << "  Characteristic " << ch.uuid << '\n';
      }
    }
  }
}

template<typename T> std::optional<T> BluezDeviceClient::getPropertyUnsafe(const std::shared_ptr<sdbus::IProxy> &proxy, std::string propertyName)
{
  try {
    sdbus::Variant value = proxy->getProperty(propertyName).onInterface(B_DEVICE_INTERFACE);
    return value.get<T>();
  } catch (const sdbus::Error &e) {
    //std::cout << "Failed to get " << propertyName << " : " + e.getName() + " - " + e.getMessage() << std::endl;
    return std::nullopt;
  }
}

} // bluetooth::bluez namespace