#pragma once

/* Standard C++ header files. */
#include <memory>
#include <string>
#include <unordered_map>

/* Local header files. */
#include "Device.h"

namespace device {

class DevicesManager
{
public:

  ~DevicesManager() = default;

  const auto &devices() {
    return devices_;
  }

  void insert(const std::shared_ptr<Device> device);

private:
  /**
   * @brief Devices by address.
   *
   * @{
   */
  struct DeviceAddressHash {
    std::size_t operator()(const std::string &p) const noexcept {
      return std::hash<std::string> {}(p);
    }
  };
  struct DeviceAddressEqual {
    bool operator()(const std::string &a,
                    const std::string &b) const noexcept {
      return a == b;
    }
  };
  std::unordered_map<std::string, std::shared_ptr<Device>, DeviceAddressHash, DeviceAddressEqual> devices_;

  /** @} */
};


} // device namespace