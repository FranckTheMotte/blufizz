#pragma once

/* Standard C++ header files. */
#include <string>
#include <vector>

struct Characteristic {
  std::string objectPath;
  std::string servicePath;
  std::string uuid;
};

struct Service {
  std::string objectPath;
  std::string uuid;
  std::vector<Characteristic> characteristics;
};

struct DeviceGattProfile {
  std::string devicePath;
  std::vector<Service> services;
};
