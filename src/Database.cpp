/* Standard C++ header files. */
#include <cstdio>
#include <cstdlib>
#include <iostream>

/* Local header files. */
#include "Database.h"
#include "Tools.h"

namespace storage {

Database::Database(const std::string &path)
{
  int rc = sqlite3_open_v2(
             path.c_str(),
             &db_,
             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
             nullptr
           );

  if (rc != SQLITE_OK) {
    std::string error = db_ ? sqlite3_errmsg(db_) : "unknown error";
    sqlite3_close(db_);
    throw std::runtime_error("Cannot open SQLite database: " + error);
  }

  initializeSchema();
}

Database::~Database()
{
  if (db_) {
    sqlite3_close(db_);
  }
}

void Database::exec(const char *sql)
{
  char *errMsg = nullptr;
  int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);

  if (rc != SQLITE_OK) {
    std::string error = errMsg ? errMsg : "unknown SQLite error";
    sqlite3_free(errMsg);
    throw std::runtime_error(error);
  }
}

void Database::initializeSchema()
{
  /* Table to store devices */
  exec(R"SQL(
        CREATE TABLE IF NOT EXISTS radio_devices (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            radio_type TEXT NOT NULL,
            -- values: 'bluetooth', 'wifi'
            stable_identifier TEXT NOT NULL,
            -- bluetooth: address + '/' + address_type
            -- wifi: bssid
            display_name TEXT,
            -- bluetooth: Name or Alias
            -- wifi: SSID if knowned
            first_seen_utc TEXT NOT NULL,
            last_seen_utc TEXT NOT NULL,
            UNIQUE(radio_type, stable_identifier)
        );
    )SQL");

  /* Table to store device's properties. */
  exec(R"SQL(
        CREATE TABLE IF NOT EXISTS bluetooth_details (
            device_id INTEGER PRIMARY KEY,
            bt_address TEXT NOT NULL,
            bt_address_type TEXT NOT NULL,

            last_name TEXT,
            last_rssi INTEGER,
            last_tx_power INTEGER,

            alias TEXT,
            icon TEXT,
            class INTEGER,
            appearance INTEGER,

            uuids_json TEXT,

            paired INTEGER,
            bonded INTEGER,
            connected INTEGER,
            trusted INTEGER,
            blocked INTEGER,

            legacy_pairing INTEGER,

            adapter TEXT,

            manufacturer_data_hex TEXT,
            service_data_hex TEXT,

            address_vendor TEXT,
            address_vendor_source TEXT,
            address_vendor_confidence TEXT,

            advertising_company_id INTEGER,
            advertising_company_name TEXT,

            updated_at_utc TEXT NOT NULL,

            FOREIGN KEY(device_id) REFERENCES radio_devices(id) ON DELETE CASCADE
        );
    )SQL");

  /* Table to store observations of a device. */
  exec(R"SQL(
        CREATE TABLE IF NOT EXISTS radio_observations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id INTEGER NOT NULL,
            radio_type TEXT NOT NULL,
            observed_at_utc TEXT NOT NULL,
            interface_name TEXT NOT NULL,
            signal_dbm INTEGER,
            gps_status TEXT NOT NULL,
            gps_latitude REAL,
            gps_longitude REAL,
            raw_json TEXT,
            created_at_utc TEXT NOT NULL,
            FOREIGN KEY(device_id) REFERENCES radio_devices(id)
        );
    )SQL");

  /* quick access by observed time */
  exec(R"SQL(
        CREATE INDEX IF NOT EXISTS idx_radio_observations_time
        ON radio_observations(observed_at_utc);
    )SQL");

  const char *CREATE_BLUETOOTH_DETAILS_INDEX = R"SQL(
        CREATE INDEX IF NOT EXISTS idx_bluetooth_details_address
        ON bluetooth_details(bt_address, bt_address_type);
    )SQL";
}

void Database::bindValue(sqlite3_stmt *stmt, int index, const std::string &value)
{
  sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
}

void Database::bindValue(
  sqlite3_stmt *stmt,
  int index,
  const std::optional<std::string> &value
)
{
  if (value.has_value()) {
    sqlite3_bind_text(stmt, index, value->c_str(), -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_null(stmt, index);
  }
}

void Database::bindValue(
  sqlite3_stmt *stmt,
  int index,
  const std::optional<int> &value
)
{
  if (value.has_value()) {
    sqlite3_bind_int(stmt, index, *value);
  } else {
    sqlite3_bind_null(stmt, index);
  }
}

void Database::bindValue(
  sqlite3_stmt *stmt,
  int index,
  bool value
)
{
  sqlite3_bind_int(stmt, index, value ? 1 : 0);
}

void Database::bindValue(
  sqlite3_stmt *stmt,
  int index,
  int16_t value
)
{
  sqlite3_bind_int(stmt, index, value ? 1 : 0);
}

void Database::bindValue(
  sqlite3_stmt *stmt,
  int index,
  int64_t value
)
{
  sqlite3_bind_int64(stmt, index, static_cast<sqlite3_int64>(value));
}

void Database::updateDevice(device::Device &device)
{
  updateDevices(device);
  updateDeviceDetails(device);
  updateDeviceObservations(device);
}

void Database::updateDevices(device::Device &device)
{
  sqlite3_stmt *stmt = nullptr;

  auto deviceId = findRadioDeviceId(
                    "bluetooth",
                    device.stableIdentifier()
                  );

  // only update last seen
  if (deviceId.has_value()) {
    const char *sql = R"SQL(
        UPDATE radio_devices
        SET
            last_seen_utc = ?
        WHERE id = ?;
    )SQL";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
      throw std::runtime_error(sqlite3_errmsg(db_));
    }
    bindValue(stmt, 1, currentUtcTime());
    bindValue(stmt, 2, deviceId.value());

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
      std::string error = sqlite3_errmsg(db_);
      sqlite3_finalize(stmt);
      throw std::runtime_error(error);
    }

    if (sqlite3_changes(db_) == 0) {
      sqlite3_finalize(stmt);
      throw std::runtime_error("No radio_devices row updated");
    }

    sqlite3_finalize(stmt);
    return;
  }

  // A new entry
  const char *sql = R"SQL(
        INSERT INTO radio_devices (
            radio_type,
            stable_identifier,
            display_name,
            first_seen_utc,
            last_seen_utc
        )
        VALUES (?, ?, ?, ?, ?);
  )SQL";

  int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db_));
  }

  std::string utcTime = currentUtcTime();

  bindValue(stmt, 1, std::string("bluetooth"));
  bindValue(stmt, 2, device.stableIdentifier());
  bindValue(stmt, 3, device.name());

  bindValue(stmt, 4, utcTime); // first_seen_utc
  bindValue(stmt, 5, utcTime); // last_seen_utc

  rc = sqlite3_step(stmt);

  if (rc != SQLITE_DONE) {
    std::cout << "BBBB4 " << device.stableIdentifier() << " " << utcTime << "\n";

    std::string error = sqlite3_errmsg(db_);
    sqlite3_finalize(stmt);
    throw std::runtime_error(error);
  }

  sqlite3_finalize(stmt);

  device.setId(static_cast<int64_t>(sqlite3_last_insert_rowid(db_)));
}

void Database::updateDeviceDetails(const device::Device &device)
{
  const char *sql = R"SQL(
        INSERT INTO bluetooth_details (
            device_id,
            bt_address,
            bt_address_type,
            last_name,
            last_rssi,
            last_tx_power,
            alias,
            icon,
            class,
            appearance,
            uuids_json,
            paired,
            bonded,
            connected,
            trusted,
            blocked,
            legacy_pairing,
            adapter,
            manufacturer_data_hex,
            service_data_hex,
            address_vendor,
            address_vendor_source,
            address_vendor_confidence,
            advertising_company_id,
            advertising_company_name,
            updated_at_utc
        )
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ON CONFLICT(device_id)
        DO UPDATE SET
            bt_address = excluded.bt_address,
            bt_address_type = excluded.bt_address_type,
            last_name = COALESCE(excluded.last_name, bluetooth_details.last_name),
            last_rssi = excluded.last_rssi,
            last_tx_power = excluded.last_tx_power,
            alias = excluded.alias,
            uuids_json = excluded.uuids_json,
            paired = excluded.paired,
            bonded = excluded.bonded,
            connected = excluded.connected,
            trusted = excluded.trusted,
            blocked = excluded.blocked,
            legacy_pairing = excluded.legacy_pairing,
            adapter = excluded.adapter,
            manufacturer_data_hex = excluded.manufacturer_data_hex,
            service_data_hex = excluded.service_data_hex,
            address_vendor = excluded.address_vendor,
            address_vendor_source = excluded.address_vendor_source,
            address_vendor_confidence = excluded.address_vendor_confidence,
            advertising_company_id = excluded.advertising_company_id,
            advertising_company_name = excluded.advertising_company_name,
            updated_at_utc = excluded.updated_at_utc;
    )SQL";

  sqlite3_stmt *stmt = nullptr;

  int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db_));
  }

  sqlite3_bind_int64(stmt, 1, device.id());

  bindValue(stmt, 2, device.address());
  bindValue(stmt, 3, device.addressType());

  bindValue(stmt, 4, device.name());
  bindValue(stmt, 5, device.rssi());
  bindValue(stmt, 6, device.txPower());


  bindValue(stmt, 7, device.alias());
  bindValue(stmt, 8, device.icon());
  bindValue(stmt, 9, device.deviceClass());
  bindValue(stmt, 10, device.appearance());

  bindValue(stmt, 11, device.uuidsJson());

  bindValue(stmt, 12, device.paired());
  bindValue(stmt, 13, device.bonded());
  bindValue(stmt, 14, device.connected());
  bindValue(stmt, 15, device.trusted());
  bindValue(stmt, 16, device.blocked());
  bindValue(stmt, 17, device.legacyPairing());

  bindValue(stmt, 18, device.adapter());

  bindValue(stmt, 19, device.manufacturerDataToJson());
  bindValue(stmt, 20, device.serviceDataToJson());

  bindValue(stmt, 21, std::string("TODO")); // address vendor
  bindValue(stmt, 22, std::string("TODO")); // address vendor source
  bindValue(stmt, 23, std::string("TODO")); // address vendor confidence

  bindValue(stmt, 24, static_cast<int16_t>(-1)); // advertising company id
  bindValue(stmt, 25, std::string("TODO")); // adverstising company name

  bindValue(stmt, 26, currentUtcTime());

  rc = sqlite3_step(stmt);

  if (rc != SQLITE_DONE) {
    std::string error = sqlite3_errmsg(db_);
    sqlite3_finalize(stmt);
    throw std::runtime_error(error);
  }

  sqlite3_finalize(stmt);
}

void Database::updateDeviceObservations(const device::Device &device)
{
}

std::optional<int64_t> Database::findRadioDeviceId(
  const std::string &radioType,
  const std::string &stableIdentifier
)
{
  const char *sql = R"SQL(
        SELECT id
        FROM radio_devices
        WHERE radio_type = ?
          AND stable_identifier = ?
        LIMIT 1;
    )SQL";

  sqlite3_stmt *stmt = nullptr;

  int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db_));
  }

  sqlite3_bind_text(stmt, 1, radioType.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, stableIdentifier.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);

  std::optional<int64_t> result;

  if (rc == SQLITE_ROW) {
    result = sqlite3_column_int64(stmt, 0);
  } else if (rc == SQLITE_DONE) {
    result = std::nullopt;
  } else {
    std::string error = sqlite3_errmsg(db_);
    sqlite3_finalize(stmt);
    throw std::runtime_error(error);
  }

  sqlite3_finalize(stmt);

  return result;
}

} // storage namespace
