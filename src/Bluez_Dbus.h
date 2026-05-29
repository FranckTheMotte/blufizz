#pragma once

/**
 * @file
 * @brief Bluez DBus related definitions.
 */

/**
 * @brief Bluez service.
 */
constexpr char B_SERVICE_NAME[] = "org.bluez";

/**
 * @brief Bluez adapter interface.
 */
constexpr char B_ADAPTER_INTERFACE[] = "org.bluez.Adapter1";

/**
 * @brief Adapter Powered properties.
 */
constexpr char B_ADAPTER_POWERED_PROP[] = "Powered";

/**
 * @brief Set Discovery Filter method.
 */
constexpr char B_SET_DISCOVERY_FILTER_METHOD[] = "SetDiscoveryFilter";

/**
 * @name Filter section.
 *
 * @{
 */
constexpr char B_DISCO_FILTER_TRANSPORT[] = "Transport";
constexpr char B_DISCO_FILTER_TRANSPORT_LE[] = "le";
constexpr char B_DISCO_FILTER_DUPLICATEDATA[] = "DuplicateData";

/** @} */

/**
 * @brief Start Discovery method.
 */
constexpr char B_START_DISCOVERY_METHOD[] = "StartDiscovery";

/**
 * @brief Stop Discovery method.
 */
constexpr char B_STOP_DISCOVERY_METHOD[] = "StopDiscovery";

/**
 * @brief Remove Device method.
 */
constexpr char B_REMOVE_DEVICE_METHOD[] = "RemoveDevice";

/**
 * @brief Bluez device interface.
 */
constexpr char B_DEVICE_INTERFACE[] = "org.bluez.Device1";

/**
 * @brief Connect device method.
 */
constexpr char B_DEVICE_CONNECT_METHOD[] = "Connect";

/**
 * @brief Disconnect device method.
 */
constexpr char B_DEVICE_DISCONNECT_METHOD[] = "Disconnect";

/**
 * @name Devices properties
 *
 * @{
 */
constexpr char B_DEVICE_PROP_ADDRESS[] = "Address";
constexpr char B_DEVICE_PROP_ADDRESS_TYPE[] = "AddressType";
constexpr char B_DEVICE_PROP_NAME[] = "Name";
constexpr char B_DEVICE_PROP_ICON[] = "Icon";
constexpr char B_DEVICE_PROP_CLASS[] = "Class";
constexpr char B_DEVICE_PROP_APPEREANCE[] = "Appearance";
constexpr char B_DEVICE_PROP_UUIDS[] = "UUIDs";
constexpr char B_DEVICE_PROP_PAIRED[] = "Paired";
constexpr char B_DEVICE_PROP_CONNECTED[] = "Connected";
constexpr char B_DEVICE_PROP_TRUSTED[] = "Trusted";
constexpr char B_DEVICE_PROP_BLOCKED[] = "Blocked";
constexpr char B_DEVICE_PROP_ALIAS[] = "Alias";
constexpr char B_DEVICE_PROP_ADAPTER[] = "Adapter";
constexpr char B_DEVICE_PROP_LEGACYPAIRING[] = "LegacyPairing";
constexpr char B_DEVICE_PROP_MODALIAS[] = "ModAlias";
constexpr char B_DEVICE_PROP_RSSI[] = "RSSI";
constexpr char B_DEVICE_PROP_TXPOWER[] = "TxPower";
constexpr char B_DEVICE_PROP_MANUFACTURERDATA[] = "ManufacturerData";
constexpr char B_DEVICE_PROP_SERVICEDATA[] = "ServiceData";
constexpr char B_DEVICE_PROP_GATTSERVICES[] = "GattServices";

/** @} */

/**
 * @name Device gatt services.
 *
 * @{
 */
constexpr char B_GATT_SERVICE_INTERFACE[] = "org.bluez.GattService1";
constexpr char B_CHARAC_READVALUE_METHOD[] = "ReadValue";

/** @} */

/**
 * @name Device gatt characteristics.
 *
 * @{
 */
constexpr char B_GATT_CHARAC_INTERFACE[] = "org.bluez.GattCharacteristic1";
constexpr char B_GATT_CHARAC_READVALUE_METHOD[] = "ReadValue";

/** @} */

/**
 * @brief org.freedesktop.DBus.ObjectManager
 */
constexpr char OBJECT_MANAGER_INTERFACE[] = "org.freedesktop.DBus.ObjectManager";
constexpr char OBJECT_MANAGER_INTERFACESADDED[] = "InterfacesAdded";
constexpr char OBJECT_MANAGER_INTERFACESREMOVED[] = "InterfacesRemoved";
constexpr char OBJECT_MANAGER_OBJECTS_GET_METHOD[] = "GetManagedObjects";
constexpr char OBJECT_MANAGER_PATH[] = "/";

/**
 * @brief org.freedesktop.Dbus.Properties
 */
constexpr char PROPS_INTERFACE[] = "org.freedesktop.DBus.Properties";
constexpr char PROPS_PROPERTIES_CHANGED[] = "PropertiesChanged";
