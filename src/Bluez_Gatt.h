#pragma once

/* Standard C++ header files. */
#include <string>
#include <vector>

/**
 * @brief Bluez characteristic.
 */
struct Characteristic {
  std::string objectPath;
  std::string servicePath;
  std::string uuid;
};

/**
 * @brief Bluez service.
 */
struct Service {
  std::string objectPath;
  std::string uuid;
  std::vector<Characteristic> characteristics;
};

/**
 * @brief Bluez Gatt profile.
 */
struct DeviceGattProfile {
  std::string devicePath;
  std::vector<Service> services;
};
