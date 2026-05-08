#pragma once

/* Standard C++ header files. */
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

/* External dependencies. */
#include <nlohmann/json.hpp>

/* Local header files. */
#include "Bluez_Gatt.h"

namespace device {

class Device
{
public:
  using ByteArray = std::vector<uint8_t>;
  using UuidList = std::vector<std::string>;

  using ManufacturerData = std::map<uint16_t, ByteArray>;
  using ServiceData = std::map<std::string, ByteArray>;

  /**
   * @brief Create a new device.
   *
   * @param firstDateTime UTC date time when the device is seen.
   * @param address Physical address.
   * @param objectPath Bluez path to the device.
   * @param name Device's name.
   * @param connected Connected state.
   * @param rssi Rssi.
   */
  Device(std::string firstDatetime, std::string address, std::string objectPath, std::string name, bool connected, int rssi);
  ~Device() = default;

  /**
   * Getters and Setters
   * @{
   */
  const int64_t id() const noexcept {
    return id_;
  }
  void setId(int64_t value) {
    id_ = value;
  }

  const std::string &objectPath() const noexcept {
    return objectPath_;
  }
  void setObjectPath(std::string value) {
    objectPath_ = std::move(value);
  }

  const std::string &address() const noexcept {
    return address_;
  }
  void setAddress(std::string value) {
    address_ = std::move(value);
  }

  const std::optional<std::string> &addressType() const noexcept {
    return addressType_;
  }
  void setAddressType(std::string value) {
    addressType_ = std::move(value);
  }

  const std::optional<std::string> &name() const noexcept {
    return name_;
  }
  void setName(std::string value) {
    name_ = std::move(value);
  }

  const std::optional<std::string> &alias() const noexcept {
    return alias_;
  }
  void setAlias(std::string value) {
    alias_ = std::move(value);
  }

  const std::optional<std::string> &icon() const noexcept {
    return icon_;
  }
  void setIcon(std::string value) {
    icon_ = std::move(value);
  }

  const std::optional<uint32_t> &deviceClass() const noexcept {
    return class_;
  }
  void setDeviceClass(uint32_t value) {
    class_ = value;
  }

  const std::optional<uint16_t> &appearance() const noexcept {
    return appearance_;
  }
  void setAppearance(uint16_t value) {
    appearance_ = value;
  }

  const UuidList &uuids() const noexcept {
    return uuids_;
  }
  const std::string uuidsJson() const noexcept {
    nlohmann::json j = uuids_;
    return j.dump();
  }
  void setUuids(UuidList value) {
    uuids_ = std::move(value);
  }

  bool paired() const noexcept {
    return paired_;
  }
  void setPaired(bool value) noexcept {
    paired_ = value;
  }

  bool bonded() const noexcept {
    return bonded_;
  }
  void setBonded(bool value) noexcept {
    bonded_ = value;
  }

  bool connected() const noexcept {
    return connected_;
  }
  void setConnected(bool value) noexcept {
    connected_ = value;
  }

  bool trusted() const noexcept {
    return trusted_;
  }
  void setTrusted(bool value) noexcept {
    trusted_ = value;
  }

  bool blocked() const noexcept {
    return blocked_;
  }
  void setBlocked(bool value) noexcept {
    blocked_ = value;
  }

  const std::optional<bool> &legacyPairing() const noexcept {
    return legacyPairing_;
  }
  void setLegacyPairing(bool value) noexcept {
    legacyPairing_ = value;
  }

  const std::optional<int16_t> &rssi() const noexcept {
    return rssi_;
  }
  void setRssi(int16_t value) noexcept {
    rssi_ = value;
  }

  const std::optional<int16_t> &txPower() const noexcept {
    return txPower_;
  }
  void setTxPower(int16_t value) noexcept {
    txPower_ = value;
  }

  const std::string &adapter() const noexcept {
    return adapter_;
  }
  void setAdapter(std::string value) {
    adapter_ = std::move(value);
  }

  const ManufacturerData &manufacturerData() const noexcept {
    return manufacturerData_;
  }

  /**
   * @brief Convert manufacturer data to a Json string.
   * For example: {"0x004C":"0215ACBB","0x0059":"010203"}
   */
  std::string manufacturerDataToJson() const;

  void setManufacturerData(ManufacturerData value) {
    manufacturerData_ = std::move(value);
  }

  const ServiceData &serviceData() const noexcept {
    return serviceData_;
  }

  /**
   * @brief Convert service data to a Json string.
   * For example: {"0000feaa-0000-1000-8000-00805f9b34fb":"102030FF"}
   */
  std::string serviceDataToJson() const;

  void setServiceData(ServiceData value) {
    serviceData_ = std::move(value);
  }

  bool servicesResolved() const noexcept {
    return servicesResolved_;
  }
  void setServicesResolved(bool value) noexcept {
    servicesResolved_ = value;
  }
  /** @} */


  /**
   * @brief get the stable identifier.
   *
   * @param A string with format : "AA:BB:CC:DD:EE:FF/<public|random>".
   */
  std::string stableIdentifier();

  std::string toString() const;

private:

  /**
   * @brief Unique device id.
   */
  int64_t id_;

  /**
   * @brief First datetime (YYYYMMDD-HH:MM:SS) when the device was discovered.
   */
  std::string firstDateTime_;

  std::string objectPath_;
  std::string address_;
  std::optional<std::string> addressType_;

  std::optional<std::string> name_;
  std::optional<std::string> alias_;
  std::optional<std::string> icon_;

  std::optional<uint32_t> class_;
  std::optional<uint16_t> appearance_;

  UuidList uuids_;

  bool paired_ = false;
  bool bonded_ = false;
  bool connected_ = false;
  bool trusted_ = false;
  bool blocked_ = false;

  std::optional<bool> legacyPairing_;
  std::optional<int16_t> rssi_;
  std::optional<int16_t> txPower_;  // experimental

  std::string adapter_;

  ManufacturerData manufacturerData_;
  ServiceData serviceData_;

  bool servicesResolved_ = false;

  /**
   * @brief Available services.
   */
  std::unordered_map<std::string, Service> services_;

  std::string companyIdToHex(uint16_t companyId) const;
};

} // device namespace
