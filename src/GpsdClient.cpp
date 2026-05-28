/* Standard C++ header files. */
#include <cmath>
#include <iostream>

/* Local header files. */
#include "GpsdClient.h"

namespace location {

GpsdClient::GpsdClient()
{
  gpsRec_ = std::make_unique<gpsmm>(GPSD_SERVER, DEFAULT_GPSD_PORT);
}

GpsdClient::~GpsdClient()
{
  available_ = false;
}

int GpsdClient::init()
{
  int result;

  // Subscribe to GPS reports
  if (NULL == gpsRec_->stream(WATCH_ENABLE | WATCH_JSON)) {
    std::cerr << "gps stream() failed with " << std::to_string(result);
    return -1;
  }

  available_ = true;
  return 0;
}

int GpsdClient::read(GpsFix &fix)
{
  int retries = 0;
  gps_data_t *gpsData = nullptr;

  do {
    if (!gpsRec_->waiting(READ_TIMEOUT)) {
      std::cerr << "Timeout when waiting for gps data.\n";
      return -1;
    }

    if ((gpsData = gpsRec_->read()) == nullptr) {
      std::cerr << "Failed to read gps data.\n";
      return -1;
    }
    retries++;
  } while ((gpsData->set & MODE_SET) != MODE_SET && retries < MAX_MODE_RETRY);

  if ((gpsData->set & MODE_SET) == 0) {
    std::cerr << "MODE is not set.\n";
    return -1;
  }

  if ((gpsData->set & LATLON_SET) == 0) {
    std::cerr << "LATLON is not set.\n";
    return -1;
  }

  if (gpsData->fix.mode < MODE_2D) {
    std::cerr << "Fix mode is too low.\n";
    return -1;
  }

  if (!std::isfinite(gpsData->fix.latitude) ||
      !std::isfinite(gpsData->fix.longitude)) {
    std::cerr << "Latitude or Longitude are not valid.\n";
    return -1;
  }

  fix.latitude = gpsData->fix.latitude;
  fix.longitude = gpsData->fix.longitude;

  // altitude
  if ((gpsData->set & ALTITUDE_SET) &&
      gpsData->fix.mode >= MODE_3D &&
      std::isfinite(gpsData->fix.altMSL)) {
    fix.hasAltitude = true;
    fix.altitude = gpsData->fix.altMSL;
  } else {
    fix.hasAltitude = false;
    fix.altitude = 0.0;
  }

  return 0;
}

} /* location namespace */
