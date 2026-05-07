/* Standard C++ header files. */
#include <sstream>
#include <iomanip>

/* Local header files. */
#include "Device.h"
#include "Tools.h"

namespace device {

Device::Device(uint32_t id,
               std::string firstDatetime,
               std::string address,
               std::string objectPath,
               std::string name,
               bool connected,
               int rssi) :
  id_(id),
  firstDateTime_(firstDatetime),
  address_(address),
  objectPath_(objectPath),
  name_(name),
  connected_(connected),
  rssi_(rssi) {}

std::string Device::toString() const
{
  std::ostringstream oss;

  oss << "BluezDeviceProperties {\n";

  oss << "  object path: " << objectPath_ << "\n";
  oss << "  address: " << address_ << "\n";

  auto print_opt = [&](const char *name, const auto & opt) {
    if (opt) {
      oss << "  " << name << ": " << *opt << "\n";
    }
  };

  print_opt("address type", addressType_);
  print_opt("name", name_);
  print_opt("alias", alias_);
  print_opt("icon", icon_);
  print_opt("class", class_);
  print_opt("appearance", appearance_);
  print_opt("legacy pairing", legacyPairing_);
  print_opt("rssi", rssi_);

  if (!uuids_.empty()) {
    oss << "  uuids: [";
    for (size_t i = 0; i < uuids_.size(); ++i) {
      if (i != 0) {
        oss << ", ";
      }
      oss << uuids_[i];
    }
    oss << "]\n";
  }

  oss << "  paired: " << std::boolalpha << paired_ << "\n";
  oss << "  bonded: " << bonded_ << "\n";
  oss << "  connected: " << connected_ << "\n";
  oss << "  trusted: " << trusted_ << "\n";
  oss << "  blocked: " << blocked_ << "\n";
  oss << "  services resolved: " << servicesResolved_ << "\n";

  if (!adapter_.empty()) {
    oss << "  adapter: " << adapter_ << "\n";
  }

  if (!manufacturerData_.empty()) {
    oss << "  manufacturer data:\n";
    for (const auto& [id, data] : manufacturerData_) {
      oss << "    0x" << std::hex << std::setw(4) << std::setfill('0') << id
          << ": ";
      for (auto b : data) {
        oss << "0x" << std::setw(2) << static_cast<int>(b) << " ";
      }
      oss << std::dec << "\n";
    }
  }

  if (!serviceData_.empty()) {
    oss << "  service_data:\n";
    for (const auto& [uuid, data] : serviceData_) {
      oss << "    " << uuid << ": ";
      for (auto b : data) {
        oss << "0x" << std::hex << std::setw(2)
            << std::setfill('0') << static_cast<int>(b) << " ";
      }
      oss << std::dec << "\n";
    }
  }

  oss << "}\n";

  return oss.str();
}

std::string Device::companyIdToHex(uint16_t companyId) const
{
  std::ostringstream oss;

  oss << "0x"
      << std::hex
      << std::uppercase
      << std::setfill('0')
      << std::setw(4)
      << companyId;

  return oss.str();
}

std::string Device::manufacturerDataToJson() const
{
  nlohmann::json j;

  for (const auto& [companyId, payload] : manufacturerData_) {
    j[companyIdToHex(companyId)] = bytesToHex(payload);
  }

  return j.dump();
}

std::string Device::serviceDataToJson() const
{
  nlohmann::json j;

  for (const auto& [uuid, payload] : serviceData_) {
    j[uuid] = bytesToHex(payload);
  }

  return j.dump();
}

std::string Device::stableIdentifier()
{
  return address_ + std::string("/") + addressType_.value_or("unknown");
}

} // device namespace
