#pragma once

/* Standard C++ header files. */
#include <string>
#include <vector>

namespace device {

enum class KnownService {
  GenericAccess,
  GenericAttribute,
  Battery,
  DeviceInformation,
  HeartRate,
  Unknown
};

enum class KnownCharacteristic {
  DeviceName,
  Appearance,
  BatteryLevel,
  ManufacturerName,
  ModelNumber,
  Unknown
};

struct Characteristic {
  std::string uuid;
  std::string object_path;
  KnownCharacteristic known = KnownCharacteristic::Unknown;
};

struct Service {
  std::string uuid;
  std::string object_path;
  KnownService known = KnownService::Unknown;
  std::vector<Characteristic> characteristics;
};

} // device namespace
