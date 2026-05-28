#pragma once

/**
 * @file
 * @brief Manage gps functionnality based on gpsd.
 */

/* Standard C++ header files. */
#include <memory>

/* External dependencies. */
#include <libgpsmm.h>

namespace location {

const int READ_TIMEOUT = 2'000'000;
const int MAX_MODE_RETRY = 10;
const char GPSD_SERVER[] = "localhost";

/**
 * @brief A single gps point with at least latitude and longitude.
 *
 * @note In GPS/GNSS terminology, a fix means: a computed position solution from satellite signals.
 *
 */
struct GpsFix {
  double latitude;
  double longitude;
  double altitude;
  bool hasAltitude;
};

class GpsdClient
{
public:
  GpsdClient();

  /**
   * @brief Release of gps module.
   */
  ~GpsdClient();

  /**
   * @brief Initialization of gps module.
   *
   * @return 0 if success, < 0 otherwise.
   */

  int init(void);

  /**
   * @brief Read current gps coordinates.
   *
   * @param[out] If success contains the gps coordinates.
   *
   * @return 0 if success, < 0 otherwise.
   */

  int read(GpsFix &fix);

  /**
   * Getters and Setters
   * @{
   */
  bool available() const noexcept {
    return available_;
  }
  /**
   * @}
   */

private:

  /**
   * @brief Gps current state.
   */
  std::unique_ptr<gpsmm> gpsRec_;

  /**
   * @brief Gps current status.
   */
  bool available_ = false;
};
} /* location */