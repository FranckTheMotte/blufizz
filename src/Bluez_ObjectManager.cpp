/* Standard C++ header files. */
#include <iostream>
#include <stdexcept>
#include <string>

/* External dependencies. */
#include <sdbus-c++/sdbus-c++.h>

/* Local header files. */
#include "Bluez_ObjectManager.h"
#include "Bluez_Dbus.h"

/**
 * @brief namespace for bluez related definitions.
 */
namespace bluetooth::bluez {

ObjectManager::ObjectManager(const std::shared_ptr<sdbus::IProxy> &objectManagerProxy, blufizz::tools::TaskManager &taskManager) :
  objectManagerProxy_(objectManagerProxy),
  taskManager_(taskManager)
{
  // Let's subscribe for the 'InterfacesAdded' signals
  sdbus::InterfaceName interfaceName{OBJECT_MANAGER_INTERFACE};
  sdbus::SignalName signalName{"InterfacesAdded"};
  objectManagerProxy->uponSignal(signalName).onInterface(interfaceName).call([this](
                                                                               const sdbus::ObjectPath & objectPath,
                                                                               const std::map <
                                                                               std::string,
                                                                               std::map<std::string, sdbus::Variant>
  > & interfacesAndProperties) {
    this->onInterfacesAdded(objectPath, interfacesAndProperties);
  });
}

void ObjectManager::onInterfacesAdded(
  const sdbus::ObjectPath &objectPath,
  const std::map <
  std::string,
  std::map<std::string, sdbus::Variant>
  > & interfacesAndProperties)
{
  /* (objectpath '/org/bluez/hci0/dev_30_B4_8A_B9_98_61',
     {'org.freedesktop.DBus.Introspectable': @a{sv} {},
      'org.bluez.Device1':
      {'Address': <'30:B4:8A:B9:98:61'>,
      'AddressType': <'random'>,
      'Alias': <'30-B4-8A-B9-98-61'>,
      'Paired': <false>,
      'Bonded': <false>,
      'Trusted': <false>,
      'Blocked': <false>,
      'LegacyPairing': <false>,
      'RSSI': <int16 -82>,
      'Connected': <false>,
      'UUIDs': <['0000fcf1-0000-1000-8000-00805f9b34fb']>,
      'Adapter': <objectpath '/org/bluez/hci0'>,
      'ServiceData': <{'0000fcf1-0000-1000-8000-00805f9b34fb':
      <[byte 0x04, 0x34, 0xf5, 0xa4, 0x56, 0x08, 0x21, 0x2e, 0x4f, 0x2e, 0x78, 0x25, 0x21, 0x81, 0xa4, 0x74, 0x72, 0xec, 0x43, 0x4a, 0x21]>}>,
      'ServicesResolved': <false>},
      'org.freedesktop.DBus.Properties': {}
      }
      )
  */

  /* Ignore service and characteristics */
  if (objectPath.contains("service")) {
    return;
  }

  std::cout << "Received signal with " << objectPath << std::endl;

  const auto &properties = interfacesAndProperties.at(B_DEVICE_INTERFACE);

  std::optional<int16_t> rssi;
  auto it = properties.find("RSSI");
  if (it == properties.end()) {
    rssi = it->second.get<int16_t>();
  }
  auto device = std::make_shared<device::Device>(
                  "",
                  properties.at("Address").get<std::string>(),
                  objectPath,
                  "noname",
                  false,
                  rssi.value_or(0));
  taskManager_.push(AddDeviceTask{
    .device = device
  });
}

} /* bluez namespace */
