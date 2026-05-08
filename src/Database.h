/* Standard C++ header files. */
#include <optional>
#include <stdexcept>
#include <string>

/* External dependencies. */
#include <sqlite3.h>

/* Local headers. */
#include "Device.h"

namespace storage {
/**
 * @brief Class
 */
class Database
{
public:
  /**
   * @brief Initialiazation of database tables.
   *
   * @param path Path to stored database.
   */
  explicit Database(const std::string &path);

  ~Database();

  /**
   * @brief Sqlite3 db handle getter.
   *
   * @return Pointer to db handle.
   */
  sqlite3 *handle() {
    return db_;
  }

  /**
   * @brief Udpate the entry of a device (insert if not exist).
   *
   * @param device A remote device.
   */
  void updateDevice(device::Device &device);

private:
  /**
   * @brief SQLite db handle.
   */
  sqlite3 *db_ = nullptr;

  /**
   * @brief Execute a sql query with One-Step Query Execution Interface.
   *
   * @param sql Sql query.
   *
   * @exception std::runtime_error if failed.
   */
  void exec(const char *sql);

  /**
   * @brief Initialize sql tables.
   */
  void initializeSchema();

  /**
   * @brief Update device reference if it doesn't exist.
   * Id of the device will be updated if new.
   * 
   * @param device A device.
   */
  void updateDeviceReference(device::Device &device);

  /**
   * @brief Add or update device details.
   * 
   * @param device A device.
   */
  void updateDeviceDetails(const device::Device &device);

  /**
   * @brief Update device obvervations.
   * 
   * @param device A device.
   */
  void updateDeviceObservations(const device::Device &device);

  /**
   * @brief Bind text to prepared statements.
   *
   * @param stmt sqlite3 statement object.
   * @param index index of the SQL parameter to be set.
   * @param value value to bind.
   */
  void bindValue(sqlite3_stmt *stmt, int index, const std::string &value);

  /**
   * @brief Bind optional text to prepared statements.
   *
   * @param stmt sqlite3 statement object.
   * @param index index of the SQL parameter to be set.
   * @param value value to bind.
   */
  void bindValue(sqlite3_stmt *stmt,
                 int index,
                 const std::optional<std::string> &value);

  /**
   * @brief Bind optional int to prepared statements.
   *
   * @param stmt sqlite3 statement object.
   * @param index index of the SQL parameter to be set.
   * @param value value to bind.
   */
  void bindValue(sqlite3_stmt *stmt,
                 int index,
                 const std::optional<int> &value);

  /**
   * @brief Bind bool to prepared statements.
   *
   * @param stmt sqlite3 statement object.
   * @param index index of the SQL parameter to be set.
   * @param value value to bind.
   */
  void bindValue(sqlite3_stmt *stmt,
                 int index,
                 const bool value);

  /**
   * @brief Bind int16 to prepared statements.
   *
   * @param stmt sqlite3 statement object.
   * @param index index of the SQL parameter to be set.
   * @param value value to bind.
   */
  void bindValue(sqlite3_stmt *stmt,
                 int index,
                 int16_t value);

  /**
   * @brief Bind int64 to prepared statements.
   *
   * @param stmt sqlite3 statement object.
   * @param index index of the SQL parameter to be set.
   * @param value value to bind.
   */
  void bindValue(sqlite3_stmt *stmt,
                 int index,
                 int64_t value);

  /**
   * @brief Retrieve a device id in radio_devices table.
   *
   * @param radioType "bluetooth" or "wifi".
   * @param stableIdentifier Unique identifier.
   *
   * @return an id or nothing.
   */
  std::optional<int64_t> findRadioDeviceId(
    const std::string &radioType,
    const std::string &stableIdentifier
  );

};

} // storage namespace